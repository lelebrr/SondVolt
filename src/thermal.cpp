// ============================================================================
// Sondvolt v3.0 — Sonda Termica DS18B20 (Implementacao)
// Descricao: Leitura da sonda de temperatura OneWire
// ============================================================================

#include "thermal.h"
#include "config.h"
#include "pins.h"
#include "globals.h"
#include "buzzer.h"
#include "leds.h"
#include "ui.h"
#include "graphics.h"
#include "display_globals.h"

#include <OneWire.h>
#include <DallasTemperature.h>

// ============================================================================
// VARIAVEIS
// ============================================================================

static OneWire* gOneWire = nullptr;
static DallasTemperature* gSensors = nullptr;
static DeviceAddress gTempSensor;
static float gLastTemp = 25.0f;
static bool gThermalInitialized = false;
static bool gAlertActive = false;

// Limiares de temperatura
#define TEMP_WARNING     70.0f  // Celsius
#define TEMP_CRITICAL    90.0f  // Celsius

// ============================================================================
// INICIALIZACAO
// ============================================================================

void thermal_init() {
    if (gThermalInitialized) {
        return;
    }

    // Inicializa OneWire
    gOneWire = new OneWire(PIN_ONEWIRE);
    gSensors = new DallasTemperature(gOneWire);

    // Inicializa sensor
    gSensors->begin();

    if (gSensors->getDeviceCount() > 0) {
        // Usa primeiro sensor encontrado
        gSensors->getAddress(gTempSensor, 0);
        gSensors->setResolution(gTempSensor, 12);
        gThermalInitialized = true;
        DBG("[TEMP] Sonda termica inicializada");
    } else {
        DBG("[TEMP] Nenhum sensor encontrado, usando temperatura padrão");
        gThermalInitialized = false; // Indica que não tem sensor
    }
}

// ============================================================================
// LEITURA
// ============================================================================

float thermal_read() {
    if (!gThermalInitialized) {
        thermal_init();
        return 25.0f;
    }

    gSensors->requestTemperatures();
    float temp = gSensors->getTempC(gTempSensor);

    // Verifica leitura valida
    if (temp == DEVICE_DISCONNECTED_C || temp < -127.0f || temp > 125.0f) {
        return gLastTemp;
    }

    gLastTemp = temp;
    return temp;
}

float thermal_read_async() {
    if (!gThermalInitialized) {
        return 25.0f;
    }

    // Nao bloqueia
    float temp = gSensors->getTempC(gTempSensor);

    if (temp != DEVICE_DISCONNECTED_C) {
        gLastTemp = temp;
    }

    return gLastTemp;
}

float thermal_get_last() {
    return gLastTemp;
}

// ============================================================================
// ALERTAS
// ============================================================================

bool thermal_has_alert() {
    return thermal_is_warning() || thermal_is_critical();
}

bool thermal_is_warning() {
    return gLastTemp > TEMP_WARNING;
}

bool thermal_is_critical() {
    return gLastTemp > TEMP_CRITICAL;
}

void thermal_alert_beep() {
    if (!deviceSettings.silentMode) {
        if (thermal_is_critical()) {
            buzzer_alert();
        } else if (thermal_is_warning()) {
            buzzer_beep(BUZZER_FREQ_WARNING, BUZZER_DURATION_WARNING);
        }
    }
}

// ============================================================================
// HANDLE (loop)
// ============================================================================

void thermal_handle() {
    if (!gThermalInitialized) {
        return;
    }

    float temp = thermal_read_async();

    // Verifica alertas
    if (thermal_has_alert()) {
        if (!gAlertActive) {
            gAlertActive = true;
            thermal_alert_beep();

            // LED de alerta
            if (thermal_is_critical()) {
                flash_alert();
            } else {
                flash_error();
            }
        }
    } else {
        if (gAlertActive) {
            gAlertActive = false;
            led_off();
        }
    }
}

// ============================================================================
// DESENHO
// ============================================================================

void thermal_draw() {
    float temp = thermal_get_last();

    // Fundo
    tft.fillScreen(C_BLACK);

    // Titulo
    draw_status_bar("Temperatura", "");

    // Valor principal
    char tempStr[16];
    snprintf(tempStr, sizeof(tempStr), "%.1f", temp);

    uint16_t color;
    if (thermal_is_critical()) {
        color = C_ERROR;
    } else if (thermal_is_warning()) {
        color = C_WARNING;
    } else {
        color = C_PRIMARY;
    }

    tft.setTextColor(color);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FONT_VALUE);
    tft.drawString(tempStr, SCREEN_W / 2, SCREEN_H / 2);

    // Unidade
    tft.setTextSize(2);
    tft.setTextColor(C_TEXT);
    tft.drawString("C", SCREEN_W / 2 + 60, SCREEN_H / 2);

    // Icone
    draw_icon_temp(SCREEN_W / 2 - 60, SCREEN_H / 2 - 40, 32, color);

    // Status
    const char* status;
    if (thermal_is_critical()) {
        status = "CRITICO!";
    } else if (thermal_is_warning()) {
        status = "QUENTE";
    } else {
        status = "NORMAL";
    }

    tft.setTextColor(color);
    tft.setFreeFont(FONT_NORMAL);
    tft.drawString(status, SCREEN_W / 2, SCREEN_H / 2 + 50);

    draw_footer("OK", "VOLTAR");
}

// ============================================================================
// RESOLUCAO
// ============================================================================

void thermal_set_resolution(uint8_t resolution) {
    if (!gThermalInitialized || resolution < 9 || resolution > 12) {
        return;
    }

    gSensors->setResolution(gTempSensor, resolution);
}