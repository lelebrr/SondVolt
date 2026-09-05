// ============================================================================
// Sondvolt v4.0 - Sonda Termica (DS18B20)
// ============================================================================
// Arquivo : thermal.cpp
//
// Na revisao A da placa o barramento OneWire divide o GPIO4 com o LED
// vermelho. Sem arbitragem, um LED aceso mantem o pino em nivel alto e
// impede o DS18B20 de responder - o sensor "some" justamente quando um
// alerta esta ativo. A HAL empresta o pino, faz a leitura e devolve o LED
// ao estado anterior.
//
// A conversao de 12 bits do DS18B20 leva 750 ms. Bloquear a tarefa de
// medicao por todo esse tempo travaria a interface, entao a leitura e
// assincrona: pedimos a conversao, saimos, e coletamos o resultado no ciclo
// seguinte.
// ============================================================================

#include "thermal.h"
#include "hal.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

static OneWire           oneWire(PIN_ONEWIRE);
static DallasTemperature sensors(&oneWire);

static float    gLastTemp        = THERMAL_INVALID_TEMP;
static bool     gPresent         = false;
static bool     gConversionBusy  = false;
static uint32_t gConversionStart = 0;
static uint8_t  gDeviceCount     = 0;

// Tempo de conversao do DS18B20 em 12 bits, com folga.
static const uint32_t kConversionMs = 800;

void thermal_init() {
    if (!hal_bus_acquire(HAL_BUS_ONEWIRE, 500)) {
        LOG_SERIAL_F("[TMP] Barramento OneWire ocupado na inicializacao");
        return;
    }

    sensors.begin();
    gDeviceCount = sensors.getDeviceCount();
    gPresent     = (gDeviceCount > 0);

    if (gPresent) {
        sensors.setResolution(12);
        // Nao esperar pela conversao e o que permite a leitura assincrona.
        sensors.setWaitForConversion(false);
        LOG_SERIAL_FMT("[TMP] %u sensor(es) DS18B20 detectado(s)\n",
                       (unsigned)gDeviceCount);
    } else {
        LOG_SERIAL_F("[TMP] Nenhum DS18B20 no barramento");
    }

    hal_bus_release(HAL_BUS_ONEWIRE);
}

bool thermal_present() { return gPresent; }
uint8_t thermal_device_count() { return gDeviceCount; }

// Leitura sincrona: usada pelo autoteste e pela calibracao, onde esperar
// 800 ms nao atrapalha ninguem.
float thermal_read() {
    if (!gPresent) {
        // Tenta redetectar: o usuario pode ter plugado a sonda agora.
        static uint32_t lastProbe = 0;
        if ((millis() - lastProbe) < 3000) return THERMAL_INVALID_TEMP;
        lastProbe = millis();
        thermal_init();
        if (!gPresent) return THERMAL_INVALID_TEMP;
    }

    if (!hal_bus_acquire(HAL_BUS_ONEWIRE, 300)) return gLastTemp;

    sensors.setWaitForConversion(true);
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    sensors.setWaitForConversion(false);

    hal_bus_release(HAL_BUS_ONEWIRE);

    if (t <= DEVICE_DISCONNECTED_C || t < -55.0f || t > 125.0f) {
        gPresent = false;
        return THERMAL_INVALID_TEMP;
    }

    gLastTemp = t;
    return t;
}

// Leitura assincrona: chamar repetidamente a partir da tarefa de medicao.
// Devolve true quando um valor novo ficou disponivel.
bool thermal_update() {
    if (!gPresent) return false;

    if (!gConversionBusy) {
        if (!hal_bus_acquire(HAL_BUS_ONEWIRE, 50)) return false;
        sensors.requestTemperatures();
        hal_bus_release(HAL_BUS_ONEWIRE);

        gConversionBusy  = true;
        gConversionStart = millis();
        return false;
    }

    if ((millis() - gConversionStart) < kConversionMs) return false;

    if (!hal_bus_acquire(HAL_BUS_ONEWIRE, 50)) return false;
    float t = sensors.getTempCByIndex(0);
    hal_bus_release(HAL_BUS_ONEWIRE);

    gConversionBusy = false;

    if (t <= DEVICE_DISCONNECTED_C || t < -55.0f || t > 125.0f) {
        gPresent = false;
        gLastTemp = THERMAL_INVALID_TEMP;
        return false;
    }

    gLastTemp = t;
    return true;
}

float thermal_get_last() { return gLastTemp; }

bool thermal_is_valid() {
    return gPresent && (gLastTemp > THERMAL_INVALID_TEMP);
}

bool thermal_is_warning() {
    return thermal_is_valid() && (gLastTemp > THERMAL_WARNING_TEMP);
}

bool thermal_is_critical() {
    return thermal_is_valid() && (gLastTemp > THERMAL_CRITICAL_TEMP);
}

const char* thermal_status_text() {
    if (!thermal_is_valid())   return "SONDA AUSENTE";
    if (thermal_is_critical()) return "CRITICO";
    if (thermal_is_warning())  return "QUENTE";
    if (gLastTemp < 5.0f)      return "FRIO";
    return "NORMAL";
}

uint16_t thermal_status_color() {
    if (!thermal_is_valid())   return COLOR_TEXT_DIM;
    if (thermal_is_critical()) return COLOR_BAD;
    if (thermal_is_warning())  return COLOR_SUSPECT;
    return COLOR_GOOD;
}
