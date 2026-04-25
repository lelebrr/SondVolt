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
#include "display_mutex.h"
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
static float shuntResistance = INA_SHUNT_OHMS;

static float filterBuffer[ZMPT_FILTER_SIZE];
static uint8_t filterIndex = 0;
static float filterSum = 0;

static bool highVoltageAlert = false;
static bool shortCircuitAlert = true;
static bool soundEnabled = true;

static TaskHandle_t acMeasureTaskHandle = nullptr;

// Helper para indicadores de status
#define MSTATUS_GOOD 0
#define MSTATUS_WARNING 1
#define MSTATUS_BAD 2

static void draw_status_indicator(uint8_t type, uint16_t x, uint16_t y, uint16_t size) {
    uint16_t color = (type == MSTATUS_GOOD) ? COLOR_GOOD : (type == MSTATUS_WARNING ? COLOR_WARNING : COLOR_BAD);
    LOCK_TFT();
    tft.fillCircle(x, y, size/2, color);
    UNLOCK_TFT();
}

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
        LOG_SERIAL_F("[MULTI] INA219 não detectado, usando modo ADC apenas");
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

    float voltage = (adcValue * ADC_REF_VOLT / ADC_MAX_VAL);

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

    if (Wire.requestFrom(INA_I2C_ADDR, (uint8_t)4) == 4) {
        uint16_t busVoltage = (uint16_t)Wire.read() << 8;
        busVoltage |= (uint16_t)Wire.read();
    }

    if (Wire.requestFrom(INA_I2C_ADDR, (uint8_t)2) == 2) {
        uint16_t shuntVoltage = (uint16_t)Wire.read() << 8;
        shuntVoltage |= (uint16_t)Wire.read();
        float current = ((float)(int16_t)shuntVoltage / 1000.0f) / shuntResistance;
        return multimeter_apply_filters(current);
    }
    return 0.0f;
}

float multimeter_read_resistance() {
    pinMode(PIN_ADC_PROBE2, OUTPUT);
    digitalWrite(PIN_ADC_PROBE2, HIGH);

    delayMicroseconds(100);

    uint16_t adcValue = analogRead(PIN_ADC_PROBE1);

    pinMode(PIN_ADC_PROBE2, INPUT);

    float voltage = adcValue * ADC_REF_VOLT / ADC_MAX_VAL;
    if (voltage >= 3.29f) return RESISTANCE_MAX;
    float resistor = 10000.0f * voltage / (3.3f - voltage);

    if(voltage < 0.01f) {
        return 0.0f;
    }

    return multimeter_apply_filters(resistor);
}

static float lastPeakValue = 0.0f;

float multimeter_read_ac_voltage_rms() {
    int16_t samples[ZMPT_NUM_SAMPLES];
    uint32_t sampleCount = 0;
    int16_t maxSample = 0;

    for(int i = 0; i < ZMPT_NUM_SAMPLES; i++) {
        uint16_t adcValue = analogRead(PIN_ADC_ZMPT);
        int16_t sample = (int16_t)adcValue - ZMPT_ZERO_POINT;
        samples[sampleCount++] = sample;
        
        int16_t absSample = (sample < 0) ? -sample : sample;
        if (absSample > maxSample) maxSample = absSample;

        delayMicroseconds(ZMPT_SAMPLE_RATE_US);
    }

    float rms = multimeter_calculate_rms(samples, sampleCount);
    float voltage = rms * ZMPT_SCALE_FACTOR * zmptCalibration / 2048.0f;
    
    // Calcula valor de pico real em Volts
    lastPeakValue = (maxSample * ZMPT_SCALE_FACTOR * zmptCalibration / 2048.0f);

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
        case MMODE_AC_VOLTAGE:
            reading.value = multimeter_read_ac_voltage_rms();
            reading.peakValue = lastPeakValue;
            reading.unit = "V AC";
            reading.unitAbbrev = "V";
            
            // Detecção de surto (se o pico for > 20% maior que o RMS * 1.414)
            if (reading.peakValue > (reading.value * 1.7f)) {
                reading.surgeDetected = true;
            }
            break;
            
        case MMODE_DC_VOLTAGE:
            reading.value = multimeter_read_dc_voltage();
            reading.unit = "V DC";
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

        case MMODE_POWER:
            reading.value = multimeter_read_dc_voltage() * multimeter_read_dc_current();
            reading.unit = "W";
            reading.unitAbbrev = "W";
            break;

        default:
            reading.state = MSTATE_ERROR;
            reading.valid = false;
            return reading;
    }

    if(reading.value < 0) {
        reading.value = 0;
    }

    if(currentMode == MMODE_AC_VOLTAGE && reading.value > HIGH_VOLTAGE_THRESHOLD) {
        reading.state = MSTATE_HIGH_VOLTAGE;
        reading.statusColor = COLOR_BAD;
    } else if(currentMode == MMODE_RESISTANCE && reading.value < SHORT_CIRCUIT_OHMS) {
        reading.state = MSTATE_SHORT;
        reading.statusColor = COLOR_WARNING;
    } else {
        reading.state = MSTATE_MEASURING;
        reading.statusColor = COLOR_GOOD;
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
    shuntResistance = INA_SHUNT_OHMS;
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
        // multimeter_update_display(); // Desabilitado para evitar conflito com UI principal
        lastUpdateMs = now;
    }
}

void multimeter_update_display() {
    LOCK_TFT();
    // Desenha caixa de valor
    tft.fillRoundRect(20, 50, SCREEN_WIDTH - 40, 90, 8, COLOR_SURFACE);
    tft.drawRoundRect(20, 50, SCREEN_WIDTH - 40, 90, 8, COLOR_PRIMARY);
    
    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setTextSize(1);
    tft.setCursor(30, 55);
    tft.print("TRUE RMS");
    
    // Valor RMS Principal
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(3);
    char valBuf[16];
    dtostrf(lastReading.value, 6, 2, valBuf);
    tft.setCursor(40, 75);
    tft.print(valBuf);
    tft.setTextSize(2);
    tft.print(" ");
    tft.print(lastReading.unitAbbrev);
    
    // Valor de Pico (Subordinado)
    if (lastReading.mode == MMODE_AC_VOLTAGE) {
        tft.setTextColor(COLOR_WARNING);
        tft.setTextSize(1);
        tft.setCursor(30, 115);
        tft.print("PICO:");
        
        tft.setTextSize(2);
        char peakBuf[16];
        dtostrf(lastReading.peakValue, 6, 1, peakBuf);
        tft.setCursor(80, 110);
        tft.print(peakBuf);
        tft.print(" Vp");
        
        if (lastReading.surgeDetected) {
            tft.setTextColor(COLOR_BAD);
            tft.setCursor(SCREEN_WIDTH - 110, 110);
            tft.print("[SURGE!]");
        }
    }
    
    UNLOCK_TFT();

    draw_status_indicator(
        lastReading.state == MSTATE_MEASURING ? MSTATUS_GOOD :
        lastReading.state == MSTATE_HIGH_VOLTAGE ? MSTATUS_BAD : MSTATUS_WARNING,
        SCREEN_WIDTH - 40, 65, 24
    );
}

void multimeter_draw(const MultimeterReading* reading) {
    char valueStr[32];
    multimeter_format_value(reading->value, valueStr, sizeof(valueStr));

    char fullStr[48];
    snprintf(fullStr, sizeof(fullStr), "%s %s", valueStr, reading->unit);

    LOCK_TFT();
    tft.setTextColor(COLOR_PRIMARY);
    tft.setTextSize(3);
    tft.setCursor(SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2);
    tft.print(fullStr);
    UNLOCK_TFT();
}

void multimeter_draw_value(float value, const char* unit, uint16_t color) {
    char valueStr[32];
    multimeter_format_value(value, valueStr, sizeof(valueStr));

    char fullStr[48];
    snprintf(fullStr, sizeof(fullStr), "%s %s", valueStr, unit);

    LOCK_TFT();
    tft.fillRect(0, CONTENT_Y, SCREEN_WIDTH, CONTENT_H - 40, COLOR_BACKGROUND);
    tft.setTextColor(color);
    tft.setTextSize(4);
    tft.setCursor(SCREEN_WIDTH/2 - 60, CONTENT_Y + CONTENT_H/2 - 20);
    tft.print(fullStr);
    UNLOCK_TFT();
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

    LOCK_TFT();
    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setTextSize(1);
    tft.setCursor(10, SCREEN_HEIGHT - FOOTER_H - 10);
    tft.print(modeStr);

    tft.setCursor(SCREEN_WIDTH - 40, SCREEN_HEIGHT - FOOTER_H - 10);
    tft.print(rangeStr);
    UNLOCK_TFT();
}

void multimeter_draw_alert(const char* message, uint16_t color) {
    LOCK_TFT();
    tft.fillRect(SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, COLOR_BACKGROUND);

    tft.setTextColor(color);
    tft.setTextSize(2);
    tft.setCursor(SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/4 + 20);
    tft.print("ALERTA");

    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2);
    tft.print(message);
    UNLOCK_TFT();
}
void multimeter_draw_history(const MeasurementHistory* hist) {
    uint16_t yStart = CONTENT_Y + 20;
    uint16_t barHeight = CONTENT_H - 40;
    uint16_t xStart = 20;
    uint16_t barWidth = SCREEN_WIDTH - 40;

    LOCK_TFT();
    tft.fillRect(xStart, yStart, barWidth, barHeight, COLOR_BACKGROUND);

    uint8_t displayCount = min(hist->count, (uint8_t)10);

    if(displayCount == 0) {
        UNLOCK_TFT();
        return;
    }

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

        tft.fillRect(x + 1, y, stepX - 2, barH, COLOR_PRIMARY);
    }
    UNLOCK_TFT();
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


// End of file


