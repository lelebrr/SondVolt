// ============================================================================
// Sondvolt v4.0 - Camada de Medicao
// ============================================================================
// Arquivo : measurements.cpp
//
// Este arquivo virou uma fachada fina sobre analysis.cpp. A versao anterior
// tinha tres defeitos graves que valem registro:
//
//   1. Fazia pinMode(GPIO35, OUTPUT) para carregar o capacitor. GPIO34-39 do
//      ESP32 sao entrada apenas: a chamada nao tinha efeito nenhum e a
//      "capacitancia" medida era o tempo de leitura do ADC.
//   2. Lia o ZMPT (rede eletrica) para julgar componentes, misturando a
//      entrada de 220 V com a de bancada.
//   3. measurements_discharge_capacitor() usava delay(100) onze vezes dentro
//      de uma tarefa FreeRTOS, travando a medicao por mais de um segundo.
//
// Agora todas as leituras passam pela HAL e pelo motor de analise, que
// conhecem a topologia real do circuito.
// ============================================================================

#include "measurements.h"
#include "analysis.h"
#include "hal.h"
#include "buzzer.h"
#include "leds.h"
#include "config.h"
#include "database.h"
#include "globals.h"
#include "diagnostics.h"

static float           gLastValue  = 0.0f;
static ComponentStatus gLastStatus = STATUS_UNKNOWN;
static AnalysisResult  gLastResult;

void measurements_init() {
    // A configuracao de pinos e do ADC ja foi feita por hal_init().
    analysis_init();
    memset(&gLastResult, 0, sizeof(gLastResult));
    gLastResult.type   = COMP_UNKNOWN;
    gLastResult.status = STATUS_UNKNOWN;

    if (!hal_probe_available()) {
        LOG_SERIAL_F("[MED] Circuito de excitacao ausente: medicoes de "
                     "componente desativadas");
    }
}

// ----------------------------------------------------------------------------
// Descarga de capacitor
// ----------------------------------------------------------------------------
// Executa em passos curtos, cedendo o processador entre eles, e acompanha a
// tensao real em vez de esperar um tempo fixo.

void measurements_discharge_capacitor() {
    if (!hal_bus_acquire(HAL_BUS_DISCHARGE, 500)) {
        LOG_SERIAL_F("[MED] Descarga ocupada");
        return;
    }

    isDischarging    = true;
    dischargeProgress = 0.0f;
    led_status_working();

    pinMode(PIN_CAP_DISCHARGE, OUTPUT);
    digitalWrite(PIN_CAP_DISCHARGE, HIGH);

    const uint16_t startRaw = hal_adc_read_avg(PIN_ADC_PROBE1, 4);
    const uint32_t start    = millis();

    while ((millis() - start) < CAP_DISCHARGE_MAX_MS) {
        uint16_t raw = hal_adc_read_avg(PIN_ADC_PROBE1, 4);

        // Progresso real: quanto ja caiu em relacao ao ponto de partida.
        if (startRaw > 40) {
            float done = 1.0f - ((float)raw / (float)startRaw);
            dischargeProgress = (done < 0.0f) ? 0.0f : (done > 1.0f ? 1.0f : done);
        }
        if (raw < 40) break;                 // praticamente zero volt
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    digitalWrite(PIN_CAP_DISCHARGE, LOW);
    hal_bus_release(HAL_BUS_DISCHARGE);

    dischargeProgress = 1.0f;
    isDischarging     = false;
    led_off();
    buzzer_success();
}

// ----------------------------------------------------------------------------
// Medicoes individuais
// ----------------------------------------------------------------------------

float measurements_get_raw_resistance() {
    float r = analysis_measure_resistance(PROBE_RANGE_AUTO);
    lastResistance = r;
    return r;
}

float measurements_get_raw_capacitance() {
    float c = analysis_measure_capacitance();
    lastCapacitance = c;
    return c;
}

float measurements_get_esr() {
    return analysis_measure_esr();
}

float measurements_get_inductance() {
    float l = analysis_measure_inductance();
    lastInductance = l;
    return l;
}

float measurements_read_ac_rms() {
    // Mantido por compatibilidade: a implementacao real vive em multimeter.cpp,
    // que aplica a calibracao do ZMPT101B.
    extern float multimeter_read_ac_voltage_rms();
    return multimeter_read_ac_voltage_rms();
}

// ----------------------------------------------------------------------------
// Identificacao automatica
// ----------------------------------------------------------------------------

void measurements_update() {
    if (!hal_probe_available()) {
        gLastValue  = 0.0f;
        gLastStatus = STATUS_INVALID;
        return;
    }

    gLastResult = analysis_identify();
    gLastStatus = gLastResult.status;

    // O "valor" exposto depende do que foi identificado.
    switch (gLastResult.type) {
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO:
            gLastValue      = gLastResult.capacitance;
            lastCapacitance = gLastResult.capacitance;
            break;
        case COMP_INDUCTOR:
            gLastValue     = gLastResult.inductance;
            lastInductance = gLastResult.inductance;
            break;
        case COMP_DIODE:
        case COMP_LED:
        case COMP_ZENER:
            gLastValue = gLastResult.forwardVoltage;
            break;
        case COMP_TRANSISTOR_NPN:
        case COMP_TRANSISTOR_PNP:
            gLastValue = gLastResult.gain;
            break;
        default:
            gLastValue     = gLastResult.resistance;
            lastResistance = gLastResult.resistance;
            break;
    }
}

float           measurements_get_last_value()  { return gLastValue; }
ComponentStatus measurements_get_last_status() { return gLastStatus; }

AnalysisResult measurements_get_last_result() { return gLastResult; }

// Faz uma identificacao completa, registra nas estatisticas e devolve o
// resultado. Usada pelo botao de teste automatico.
AnalysisResult measurements_run_full_test() {
    buzzer_measure_start();
    led_status_working();

    AnalysisResult r = analysis_identify();
    gLastResult = r;
    gLastStatus = r.status;
    gLastValue  = r.resistance;

    diag_count_measurement(r.status);

    if (r.status == STATUS_GOOD)      { led_status_good(); buzzer_ok(); }
    else if (r.status == STATUS_BAD ||
             r.status == STATUS_SHORT) { led_status_bad();  buzzer_error(); }
    else                               { led_off();         buzzer_measure_end(); }

    return r;
}
