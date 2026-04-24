// ============================================================================
// Sondvolt v3.x — Multímetro AC/DC com True RMS
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: multimeter.cpp
// Descricao: Implementacao completa do multimento AC/DC com ZMPT101B e INA219
// ============================================================================

#include "multimeter.h"
#include "config.h"
#include "globals.h"
#include "pins.h"
#include "safety.h"
#include "graphics.h"
#include "buzzer.h"
#include "display_globals.h"
#include <Wire.h>

// ============================================================================
// VARIAVEIS GLOBAIS
// ============================================================================

static MultimeterMode currentMode = MMODE_DC_VOLTAGE;
static MeasurementRange currentRange = RANGE_AUTO;
static MultimeterState meterState = MSTATE_IDLE;

static MultimeterReading lastReading;
static MeasurementHistory history;

static unsigned long lastUpdateMs = 0;
static unsigned long measureStartMs = 0;

static float zmptCalibration = ZMPT_CALIBRATION;
static float ina219Calibration = INA_CALIBRATION;
static float shuntResistance = INA219_SHUNT;

static float filterBuffer[ZMPT_FILTER_SIZE];
static uint8_t filterIndex = 0;
static float filterSum = 0;

static bool highVoltageAlert = false;
static bool shortCircuitAlert = true;
static bool soundEnabled = true;

static TaskHandle_t acMeasureTaskHandle = nullptr;

// ============================================================================
// INICIALIZACAO
// ============================================================================

void multimeter_init(bool calibrate) {
    memset(&lastReading, 0, sizeof(MultimeterReading));
    memset(&history, 0, sizeof(MeasurementHistory));

    // Inicialização ADC básica (sempre funciona)
    multimeter_adc_init();

    // Tenta inicializar INA219 (opcional)
    bool ina219Ok = multimeter_ina219_init();
    if (!ina219Ok) {
        DBG("[MULTI] INA219 não detectado, usando modo ADC apenas");
    }

    // Carrega ou calibra
    if(calibrate) {
        multimeter_reset_calibration();
    } else {
        multimeter_load_calibration();
    }

    if(soundEnabled) {
        buzzer_beep(BUZZER_FREQ_OK, 50);
    }
}

void multimeter_adc_init() {
    pinMode(PIN_ADC_PROBE1, INPUT);
    pinMode(PIN_ADC_PROBE2, INPUT);

    analogReadResolution(12);
    // adc1_config_width and adc1_config_channel_atten are legacy ESP-IDF
}

bool multimeter_ina219_init() {
    Wire.begin(PIN_INA_SDA, PIN_INA_SCL);
    Wire.setClock(400000);

    Wire.beginTransmission(INA_I2C_ADDR);
    uint8_t error = Wire.endTransmission();

    if(error == 0) {
        return true;
    }
    return false;
}

void multimeter_shutdown() {
    if(acMeasureTaskHandle != nullptr) {
        vTaskDelete(acMeasureTaskHandle);
        acMeasureTaskHandle = nullptr;
    }

    multimeter_save_calibration();
}

// ============================================================================
// LEITURAS
// ============================================================================

float multimeter_read_dc_voltage() {
    uint16_t adcValue = analogRead(PIN_ADC_PROBE1);

    float voltage = (adcValue * ADC_REF_VOLTAGE / ADC_MAX_VALUE);

    if(currentRange == RANGE_LOW) {
        voltage *= MULTI_DC_RANGE_20V / 3.3f;
    } else if(currentRange == RANGE_MED) {
        voltage *= MULTI_DC_RANGE_200V / 3.3f;
    } else {
        voltage *= MULTI_DC_RANGE_600V / 3.3f;
    }

    return multimeter_apply_filters(voltage);
}

float multimeter_read_dc_current() {
    if(!multimeter_ina219_init()) {
        return 0.0f;
    }

    Wire.requestFrom(INA_I2C_ADDR, (uint8_t)4);
    uint16_t busVoltage = Wire.read() << 8;
    busVoltage |= Wire.read();

    Wire.requestFrom(INA_I2C_ADDR, (uint8_t)2);
    uint16_t shuntVoltage = Wire.read() << 8;
    shuntVoltage |= Wire.read();

    float current = (shuntVoltage / 1000.0f) / shuntResistance;

    return multimeter_apply_filters(current);
}

float multimeter_read_resistance() {
    pinMode(PIN_ADC_PROBE2, OUTPUT);
    digitalWrite(PIN_ADC_PROBE2, HIGH);

    delayMicroseconds(100);

    uint16_t adcValue = analogRead(PIN_ADC_PROBE1);

    pinMode(PIN_ADC_PROBE2, INPUT);

    float voltage = adcValue * ADC_REF_VOLTAGE / ADC_MAX_VALUE;
    float resistor = 10000.0f * voltage / (3.3f - voltage);

    if(voltage < 0.1f) {
        return RESISTANCE_MAX;
    }

    return multimeter_apply_filters(resistor);
}

float multimeter_read_ac_voltage_rms() {
    static int16_t samples[ZMPT_NUM_SAMPLES];
    uint32_t sampleCount = 0;

    uint32_t startTime = micros();

    while(sampleCount < ZMPT_NUM_SAMPLES) {
        uint16_t adcValue = analogRead(PIN_ADC_ZMPT);

        int16_t sample = (int16_t)adcValue - ZMPT_ZERO_POINT;
        samples[sampleCount++] = sample;

        delayMicroseconds(ZMPT_SAMPLE_RATE_US);
    }

    uint32_t elapsed = micros() - startTime;

    float rms = multimeter_calculate_rms(samples, sampleCount);

    float voltage = rms * ZMPT_SCALE_FACTOR * zmptCalibration / 2048.0f;

    return multimeter_apply_filters(voltage);
}

float multimeter_calculate_rms(const int16_t* samples, uint16_t count) {
    if(count == 0) return 0.0f;

    float sumSquares = 0.0f;

    for(uint16_t i = 0; i < count; i++) {
        float sample = (float)samples[i];
        sumSquares += sample * sample;
    }

    float rms = sqrtf(sumSquares / (float)count);

    return rms;
}

bool multimeter_test_short() {
    float resistance = multimeter_read_resistance();
    return (resistance < SHORT_CIRCUIT_OHMS);
}

MultimeterReading multimeter_read() {
    MultimeterReading reading;
    memset(&reading, 0, sizeof(MultimeterReading));

    reading.mode = currentMode;
    reading.range = currentRange;
    reading.timestamp = millis();

    switch(currentMode) {
        case MMODE_DC_VOLTAGE:
            reading.value = multimeter_read_dc_voltage();
            reading.unit = "V";
            reading.unitAbbrev = "V";
            break;

        case MMODE_AC_VOLTAGE:
            reading.value = multimeter_read_ac_voltage_rms();
            reading.unit = "V";
            reading.unitAbbrev = "V";
            break;

        case MMODE_DC_CURRENT:
            reading.value = multimeter_read_dc_current();
            reading.unit = "A";
            reading.unitAbbrev = "A";
            break;

        case MMODE_RESISTANCE:
            reading.value = multimeter_read_resistance();
            reading.unit = "ohm";
            reading.unitAbbrev = "O";
            break;

        case MMODE_CONTINUITY:
            reading.value = multimeter_read_resistance();
            reading.valid = (reading.value < SHORT_CIRCUIT_OHMS);
            reading.unit = reading.valid ? "OK" : "OL";
            reading.unitAbbrev = reading.valid ? "OK" : "OL";
            break;

        default:
            reading.state = MSTATE_ERROR;
            reading.valid = false;
            return reading;
    }

    if(reading.value < 0) {
        reading.value = 0;
    }

    if(currentMode == MMODE_AC_VOLTAGE && reading.value > SAFETY_WARNING_VOLTAGE) {
        reading.state = MSTATE_HIGH_VOLTAGE;
        reading.statusColor = C_ERROR;
    } else if(currentMode == MMODE_RESISTANCE && reading.value < SHORT_CIRCUIT_OHMS) {
        reading.state = MSTATE_SHORT;
        reading.statusColor = C_WARNING;
    } else {
        reading.state = MSTATE_MEASURING;
        reading.statusColor = C_SUCCESS;
    }

    reading.valid = true;

    if(highVoltageAlert && currentMode == MMODE_AC_VOLTAGE) {
        safety_check_voltage(reading.value);
    }

    lastReading = reading;

    return reading;
}

void multimeter_read_async_start() {
    if(acMeasureTaskHandle != nullptr) {
        vTaskDelete(acMeasureTaskHandle);
    }

    xTaskCreatePinnedToCore(
        multimeter_read_ac_voltage_task,
        "AC_Measure",
        4096,
        nullptr,
        1,
        &acMeasureTaskHandle,
        0
    );
}

void multimeter_read_ac_voltage_task(void* param) {
    while(true) {
        if(currentMode == MMODE_AC_VOLTAGE) {
            lastReading.value = multimeter_read_ac_voltage_rms();
            lastReading.mode = currentMode;
            lastReading.timestamp = millis();
            lastReading.valid = true;
        }

        vTaskDelay(pdMS_TO_TICKS(TIME_REFRESH_MEAS));
    }
}

MultimeterReading multimeter_get_last_reading() {
    return lastReading;
}

// ============================================================================
// MODO E RANGE
// ============================================================================

void multimeter_set_mode(MultimeterMode mode) {
    currentMode = mode;

    if(soundEnabled) {
        buzzer_beep(BUZZER_FREQ_BTN, 30);
    }
}

MultimeterMode multimeter_get_mode() {
    return currentMode;
}

void multimeter_set_range(MeasurementRange range) {
    currentRange = range;

    if(soundEnabled) {
        buzzer_beep(BUZZER_FREQ_BTN, 30);
    }
}

MeasurementRange multimeter_get_range() {
    return currentRange;
}

void multimeter_auto_range() {
    MeasurementRange suggested = multimeter_suggest_range(lastReading.value);
    if(suggested != currentRange) {
        currentRange = suggested;
    }
}

MeasurementRange multimeter_suggest_range(float value) {
    if(value < 0.2f) return RANGE_LOW;
    if(value < 2.0f) return RANGE_MED;
    return RANGE_HIGH;
}

// ============================================================================
// CALIBRACAO
// ============================================================================

void multimeter_calibrate_zmpt(float realVoltage) {
    float currentVoltage = multimeter_read_ac_voltage_rms();

    if(currentVoltage > 0.1f) {
        zmptCalibration = realVoltage / currentVoltage;
    }
}

void multimeter_calibrate_ina219(float realVoltage, float realCurrent) {
    float measuredCurrent = multimeter_read_dc_current();

    if(measuredCurrent > 0.01f) {
        ina219Calibration = realCurrent / measuredCurrent;
    }
}

void multimeter_save_calibration() {
    // NVS write would go here
}

void multimeter_load_calibration() {
    // NVS read would go here
}

void multimeter_reset_calibration() {
    zmptCalibration = ZMPT_CALIBRATION;
    ina219Calibration = INA_CALIBRATION;
    shuntResistance = INA219_SHUNT;
}

// ============================================================================
// FILTROS
// ============================================================================

float multimeter_filter_moving_average(float newValue) {
    filterSum -= filterBuffer[filterIndex];
    filterBuffer[filterIndex] = newValue;
    filterSum += newValue;

    filterIndex = (filterIndex + 1) % ZMPT_FILTER_SIZE;

    return filterSum / ZMPT_FILTER_SIZE;
}

float multimeter_filter_exponential(float newValue) {
    static float lastFiltered = 0;
    float alpha = 0.2f;

    float filtered = alpha * newValue + (1.0f - alpha) * lastFiltered;
    lastFiltered = filtered;

    return filtered;
}

float multimeter_apply_filters(float rawValue) {
    float filtered = multimeter_filter_moving_average(rawValue);
    filtered = multimeter_filter_exponential(filtered);

    return filtered;
}

// ============================================================================
// UI
// ============================================================================

void multimeter_handle() {
    unsigned long now = millis();

    if((now - lastUpdateMs) >= TIME_REFRESH_MEAS) {
        lastReading = multimeter_read();
        multimeter_update_display();
        lastUpdateMs = now;
    }
}

void multimeter_update_display() {
    // Desenha caixa de valor
    tft.fillRoundRect(20, 60, SCREEN_W - 40, 60, 6, C_DARK_GREY);
    tft.drawRoundRect(20, 60, SCREEN_W - 40, 60, 6, C_PRIMARY);
    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setFreeFont(FONT_SMALL);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Value", 30, 65);
    tft.setTextColor(C_WHITE);
    tft.setFreeFont(FONT_VALUE);
    tft.setTextDatum(MC_DATUM);
    char valBuf[16];
    dtostrf(lastReading.value, 6, 2, valBuf);
    tft.drawString(valBuf, SCREEN_W/2 - 20, 95);
    tft.setFreeFont(FONT_NORMAL);
    tft.drawString(lastReading.unit, SCREEN_W - 60, 95);

    draw_status_indicator(
        lastReading.state == MSTATE_MEASURING ? STATUS_GOOD :
        lastReading.state == MSTATE_HIGH_VOLTAGE ? STATUS_BAD : STATUS_WARNING,
        SCREEN_W - 40, 70, 24
    );
}

void multimeter_draw(const MultimeterReading* reading) {
    char valueStr[32];
    multimeter_format_value(reading->value, valueStr, sizeof(valueStr));

    char fullStr[48];
    snprintf(fullStr, sizeof(fullStr), "%s %s", valueStr, reading->unit);

    tft.setTextColor(C_PRIMARY);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FONT_VALUE);
    tft.drawString(fullStr, SCREEN_W/2, SCREEN_H/2);
}

void multimeter_draw_value(float value, const char* unit, uint16_t color) {
    char valueStr[32];
    multimeter_format_value(value, valueStr, sizeof(valueStr));

    char fullStr[48];
    snprintf(fullStr, sizeof(fullStr), "%s %s", valueStr, unit);

    tft.fillRect(0, CONTENT_Y, SCREEN_W, CONTENT_H - 40, C_BLACK);
    tft.setTextColor(color);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FONT_HEADER);
    tft.drawString(fullStr, SCREEN_W/2, CONTENT_Y + CONTENT_H/2 - 20);
}

void multimeter_draw_indicators(MultimeterMode mode, MeasurementRange range) {
    const char* modeStr = "";
    switch(mode) {
        case MMODE_DC_VOLTAGE: modeStr = "VCC"; break;
        case MMODE_AC_VOLTAGE: modeStr = "VAC"; break;
        case MMODE_DC_CURRENT: modeStr = "A"; break;
        case MMODE_RESISTANCE: modeStr = "OHM"; break;
        case MMODE_CONTINUITY: modeStr = "CONT"; break;
        default: modeStr = "---";
    }

    const char* rangeStr = "";
    switch(range) {
        case RANGE_AUTO: rangeStr = "AUTO"; break;
        case RANGE_LOW: rangeStr = "LOW"; break;
        case RANGE_MED: rangeStr = "MED"; break;
        case RANGE_HIGH: rangeStr = "HIGH"; break;
        default: rangeStr = "---";
    }

    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setTextDatum(TL_DATUM);
    tft.setFreeFont(FONT_SMALL);
    tft.drawString(modeStr, 10, SCREEN_H - FOOTER_H - 10);

    tft.setTextDatum(TR_DATUM);
    tft.drawString(rangeStr, SCREEN_W - 10, SCREEN_H - FOOTER_H - 10);
}

void multimeter_draw_alert(const char* message, uint16_t color) {
    tft.fillRect(SCREEN_W/4, SCREEN_H/4, SCREEN_W/2, SCREEN_H/2, C_BLACK);

    tft.setTextColor(color);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FONT_NORMAL);
    tft.drawString("ALERTA", SCREEN_W/2, SCREEN_H/4 + 20);

    tft.setTextColor(C_TEXT);
    tft.drawString(message, SCREEN_W/2, SCREEN_H/2);
}
void multimeter_draw_history(const MeasurementHistory* hist) {
    uint16_t yStart = CONTENT_Y + 20;
    uint16_t barHeight = CONTENT_H - 40;
    uint16_t xStart = 20;
    uint16_t barWidth = SCREEN_W - 40;

    tft.fillRect(xStart, yStart, barWidth, barHeight, C_BLACK);

    uint8_t displayCount = min(hist->count, (uint8_t)10);

    if(displayCount == 0) return;

    float maxVal = 0;
    for(uint8_t i = 0; i < displayCount; i++) {
        if(hist->entries[i].value > maxVal) {
            maxVal = hist->entries[i].value;
        }
    }

    if(maxVal == 0) maxVal = 1;

    uint16_t stepX = barWidth / displayCount;

    for(uint8_t i = 0; i < displayCount; i++) {
        uint16_t barH = (uint16_t)(hist->entries[i].value * barHeight / maxVal);
        uint16_t x = xStart + i * stepX;
        uint16_t y = yStart + barHeight - barH;

        tft.fillRect(x + 1, y, stepX - 2, barH, C_PRIMARY);
    }
}

// ============================================================================
// PROTECAO / SEGURANCA
// ============================================================================

void multimeter_check_high_voltage(float voltage) {
    if(voltage > HIGH_VOLTAGE_THRESHOLD) {
        highVoltageAlert = true;
        meterState = MSTATE_HIGH_VOLTAGE;

        if(soundEnabled) {
            buzzer_alert();
        }

        digitalWrite(PIN_LED_RED, HIGH);
        digitalWrite(PIN_LED_GREEN, LOW);
    } else {
        highVoltageAlert = false;
        digitalWrite(PIN_LED_RED, LOW);
    }
}

void multimeter_check_short_circuit(float resistance) {
    if(resistance < SHORT_CIRCUIT_OHMS && shortCircuitAlert) {
        meterState = MSTATE_SHORT;

        if(soundEnabled) {
            buzzer_beep(BUZZER_FREQ_WARNING, BUZZER_DURATION_WARNING);
        }
    }
}

void multimeter_alert_sound(uint8_t type) {
    if(!soundEnabled) return;

    switch(type) {
        case 0:
            buzzer_beep(BUZZER_FREQ_OK, BUZZER_DURATION_OK);
            break;
        case 1:
            buzzer_beep(BUZZER_FREQ_WARNING, BUZZER_DURATION_WARNING);
            break;
        case 2:
            buzzer_beep(BUZZER_FREQ_ERROR, BUZZER_DURATION_ERROR);
            break;
    }
}

void multimeter_alert_led(uint8_t type) {
    switch(type) {
        case 0:
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_RED, LOW);
            break;
        case 1:
            digitalWrite(PIN_LED_GREEN, LOW);
            digitalWrite(PIN_LED_RED, HIGH);
            break;
        case 2:
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_RED, HIGH);
            break;
    }
}

void multimeter_clear_alerts() {
    highVoltageAlert = false;
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
}

// ============================================================================
// HISTORICO
// ============================================================================

void multimeter_history_add(float value, MultimeterMode mode) {
    uint8_t idx = history.index;
    history.entries[idx].value = value;
    history.entries[idx].mode = mode;
    history.entries[idx].timestamp = millis();
    history.entries[idx].valid = true;

    history.index = (history.index + 1) % HISTORY_SIZE;
    if(history.count < HISTORY_SIZE) {
        history.count++;
    }
}

void multimeter_history_clear() {
    memset(&history, 0, sizeof(MeasurementHistory));
}

MeasurementHistory* multimeter_get_history() {
    return &history;
}

// ============================================================================
// UTILITARIOS
// ============================================================================

void multimeter_format_value(float value, char* buffer, uint8_t maxLen) {
    if(value < 0.01f) {
        snprintf(buffer, maxLen, "0.00");
    } else if(value < 1.0f) {
        snprintf(buffer, maxLen, "%.3f", value);
    } else if(value < 100.0f) {
        snprintf(buffer, maxLen, "%.2f", value);
    } else if(value < 10000.0f) {
        snprintf(buffer, maxLen, "%.1f", value);
    } else {
        snprintf(buffer, maxLen, "%.0f", value);
    }
}

bool multimeter_detect_voltage_type(float voltage) {
    if(voltage > 200.0f) {
        return true;
    }
    return false;
}
