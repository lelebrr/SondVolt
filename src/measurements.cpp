// ============================================================================
// Sondvolt v3.0 — Medições de Componentes (Otimizado)
// ============================================================================

#include "measurements.h"
#include "config.h"
#include "utils.h"
#include "thermal.h"
#include "buzzer.h"
#include "leds.h"

// ============================================================================
// CONSTANTES DE HARDWARE
// ============================================================================

// Resistores de referência internos (divisor resistivo)
static const float R_REFERENCE = 10000.0f;    // 10kΩ pull-up
static const float R_LOW = 1000.0f;             // 1kΩ para medições de baixa resistência

// Capacitor de referência para medição de indutores
static const float C_REF = 0.000001f;        // 1µF

// thresholds de detecção
static const float VOLT_THRESHOLD_LOW = 0.05f;    // 50mV
static const float VOLT_THRESHOLD_HIGH = 3.0f;     // 3.0V
static const float DIODE_VF_MIN = 0.1f;           // 100mV
static const float DIODE_VF_MAX = 1.5f;            // 1.5V

// ============================================================================
// ESTADO GLOBAL (static para não poluir escopo)
// ============================================================================

static uint8_t measureMode = 0;
static bool measuring = false;
static unsigned long measureStart = 0;
static ComponentResult result;
static MovingAverage<8> adcFilter;     // Média móvel de 8 amostras
static LowPass voltFilter;            // Filtro passa-baixa

// ============================================================================
// ADC — Leituras rápidas e filtradas
// ============================================================================

// Lê ADC com média móvel (inline para velocidade)
inline uint16_t read_adc_fast() {
    return analogRead(PIN_PROBE_1);
}

// Lê ADC com filtro passa-baixa
inline uint16_t read_adc_filtered() {
    uint16_t raw = analogRead(PIN_PROBE_1);
    float val = (float)raw;
    val = voltFilter.update(val);
    return (uint16_t)val;
}

// Lê múltiplas amostras e retorna média
inline uint16_t read_adc_avg(uint8_t n) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < n; i++) {
        sum += analogRead(PIN_PROBE_1);
    }
    return (uint16_t)(sum / n);
}

// Converte contagem ADC para volts
inline float adc_to_v(uint16_t raw) {
    return (float)raw * ADC_SCALE;
}

// Converte tensão para resistência (lei de Ohm)
// R = (Vout * Rref) / (Vin - Vout)
inline float v_to_r(float vout, float rref, float vin) {
    if (vin <= vout || vout <= 0.001f) return 0.0f;
    return rref * vout / (vin - vout);
}

// ============================================================================
// MEDIÇÃO DE RESISTOR
// ============================================================================

float measure_resistance_raw() {
    uint16_t raw = read_adc_avg(10);
    float vout = adc_to_v(raw);
    return v_to_r(vout, R_REFERENCE, ADC_REF_VOLTAGE);
}

float measure_resistance() {
    float r = measure_resistance_raw();
    
    // Aplica offset de calibração
    r -= probeOffsetR;
    if (r < 0.0f) r = 0.0f;
    
    return r;
}

// Medição de resistor de baixa resistência
float measure_resistance_low() {
    uint16_t raw = read_adc_avg(10);
    float vout = adc_to_v(raw);
    return v_to_r(vout, R_LOW, ADC_REF_VOLTAGE);
}

// ============================================================================
// MEDIÇÃO DE CAPACITOR
// ============================================================================

float measure_capacitance() {
    const float R_CHARGE = 10000.0f;     // 10kΩ
    const float V_TH = 0.632f * ADC_REF_VOLTAGE;  // 63.2% de Vcc
    
    // Descarga capacitor
    pinMode(PIN_PROBE_2, OUTPUT);
    digitalWrite(PIN_PROBE_2, LOW);
    delayMicroseconds(1000);
    
    // Inicia carga
    pinMode(PIN_PROBE_2, INPUT);
    unsigned long start = micros();
    
    // Espera atingir V_TH
    uint16_t raw;
    do {
        raw = analogRead(PIN_PROBE_1);
    } while (adc_to_v(raw) < V_TH && (micros() - start) < 5000000UL);
    
    unsigned long elapsed = micros() - start;
    
    // C = τ / R = (tempo_us) / R
    float c = (float)elapsed / R_CHARGE;
    
    // Converte para microfarads
    c *= 1000000.0f;
    
    // Aplica offset
    c -= probeOffsetC;
    if (c < 0.0f) c = 0.0f;
    
    return c;
}

// ============================================================================
// MEDIÇÃO DE DIODO
// ============================================================================

float measure_diode_vf() {
    uint16_t raw = read_adc_avg(5);
    return adc_to_v(raw);
}

// ============================================================================
// MEDIÇÃO DE TENSÃO AC (ZMPT)
// ============================================================================

float measure_ac_voltage() {
    uint16_t raw = analogRead(PIN_ZMPT_AC);
    int32_t diff = (int32_t)raw - ZMPT_ZERO_POINT;
    return (float)abs(diff) * ZMPT_SCALE / 4096.0f;
}

// ============================================================================
// AUTO-DETECÇÃO DE COMPONENTE
// ============================================================================

uint8_t auto_detect() {
    uint16_t raw = read_adc_avg(5);
    float v = adc_to_v(raw);
    
    // Curto: tensão muito baixa
    if (v < VOLT_THRESHOLD_LOW) return COMP_SHORT;
    
    // Aberto: tensão muito alta
    if (v > VOLT_THRESHOLD_HIGH) return COMP_OPEN;
    
    // Diodo: tensão entre 0.1V e 1.5V
    if (v > DIODE_VF_MIN && v < DIODE_VF_MAX) return COMP_DIODE;
    
    // Resistor: tensão intermediária
    return COMP_RESISTOR;
}

// ============================================================================
// TESTE DE CONTINUIDADE
// ============================================================================

bool test_continuity() {
    float r = measure_resistance();
    return (r < THRESHOLD_SHORT);
}

// ============================================================================
// FORMATAR VALOR PARA STRING (sem String class)
// ============================================================================

void format_value(float value, const char* unit, char* out, uint8_t maxLen) {
    if (value <= 0.0f || isnan(value) || isinf(value)) {
        snprintf(out, maxLen, "---");
        return;
    }
    
    if (unit[0] == 'Ω') {
        if (value >= 1000000.0f) {
            snprintf(out, maxLen, "%.2fM", value / 1000000.0f);
        } else if (value >= 1000.0f) {
            snprintf(out, maxLen, "%.1fK", value / 1000.0f);
        } else {
            snprintf(out, maxLen, "%.0f", value);
        }
    } else if (unit[0] == 'µ' || (unit[0] == 'u' && unit[1] == 'F')) {
        if (value >= 1000.0f) {
            snprintf(out, maxLen, "%.2fmF", value / 1000.0f);
        } else if (value >= 1.0f) {
            snprintf(out, maxLen, "%.2fµF", value);
        } else if (value >= 0.001f) {
            snprintf(out, maxLen, "%.2fnF", value * 1000.0f);
        } else {
            snprintf(out, maxLen, "%.0fpF", value * 1000000.0f);
        }
    } else if (unit[0] == 'V') {
        snprintf(out, maxLen, "%.3fV", value);
    } else {
        dtostrf(value, 6, 2, out);
    }
}

// ============================================================================
// ATUALIZAÇÃO DE ESTADO
// ============================================================================

void measurement_update() {
    lastActivityMs = millis();
    
    float value = 0.0f;
    const char* unit = "?";
    const char* name = "Desconhecido";
    uint8_t type = COMP_GENERIC;
    
    switch (measureMode) {
        case MODE_RESISTOR:
            value = measure_resistance();
            unit = "Ω";
            name = "Resistor";
            type = COMP_RESISTOR;
            break;
            
        case MODE_CAPACITOR:
            value = measure_capacitance();
            unit = "µF";
            name = "Capacitor";
            type = COMP_CAPACITOR;
            break;
            
        case MODE_DIODE:
            value = measure_diode_vf();
            unit = "V";
            name = "Diodo";
            type = COMP_DIODE;
            break;
            
        case MODE_AC:
            value = measure_ac_voltage();
            unit = "V";
            name = "Tensão AC";
            type = COMP_GENERIC;
            break;
            
        default:
            break;
    }
    
    // Atualiza resultado
    result.value = value;
    result.unit = unit;
    result.type = type;
    result.status = db_judge(value, type);
    
    format_value(value, unit, result.valueStr, sizeof(result.valueStr));
    result.name = name;
    result.temp = thermal_read();
}

// ============================================================================
// UI DE MEDIÇÃO
// ============================================================================

void measurement_start(uint8_t mode) {
    measureMode = mode;
    measuring = true;
    measureStart = millis();
    adcFilter.reset();
    voltFilter.reset(2048.0f);
    
    led_set_rgb(0, 0, 50);
    buzzer_beep(100);
}

void measurement_stop() {
    measuring = false;
    led_off();
}

void measurement_draw() {
    tft.fillScreen(C_BLACK);
    
    // Barra de status
    draw_status_bar();
    
    // Card de medição central
    int16_t cx = SCREEN_W / 2;
    int16_t cy = SCREEN_H / 2;
    
    // Fundo do card
    tft.fillRoundRect(cx - 140, cy - 80, 280, 160, 10, C_DARK_GREY);
    
    // Borda conforme status
    uint16_t borderC = (result.status == STATUS_GOOD) ? C_GREEN : 
                    (result.status == STATUS_BAD) ? C_RED : C_YELLOW;
    tft.drawRoundRect(cx - 140, cy - 80, 280, 160, 10, borderC);
    
    // Nome do componente
    tft.setTextDatum(TC_DATUM);
    tft.setTextSize(2);
    tft.setTextColor(C_LIGHT_GREY);
    tft.setCursor(cx, cy - 65);
    tft.print(result.name);
    
    // Valor principal
    tft.setTextSize(5);
    tft.setTextColor(C_WHITE);
    tft.setCursor(cx, cy - 25);
    tft.print(result.valueStr);
    
    // Unidade
    tft.setTextSize(2);
    tft.setTextColor(C_GREEN);
    tft.setCursor(cx, cy + 20);
    tft.print(result.unit);
    
    // Temperatura
    tft.setTextSize(1);
    tft.setTextColor(C_GREY);
    tft.setCursor(cx, cy + 50);
    char tempStr[16];
    snprintf(tempStr, sizeof(tempStr), "Temp: %.1f°C", result.temp);
    tft.print(tempStr);
    
    tft.setTextDatum(TL_DATUM);
    draw_footer();
}

// ============================================================================
// LOOP PRINCIPAL DE MEDIÇÃO
// ============================================================================

void measurements_loop() {
    if (!measuring) return;
    
    // Atualiza medição
    measurement_update();
    
    // Desenha resultado
    static unsigned long lastDraw = 0;
    if (millis() - lastDraw >= UPDATE_DISP) {
        measurement_draw();
        lastDraw = millis();
    }
    
    // Feedback visual (LED contínuo)
    led_set_rgb(0, 50, 0);
    
    // Verifica timeout de inatividade
    if (has_elapsed(lastActivityMs, AUTOOFF_TIME)) {
        measurement_stop();
    }
    
    delayMicroseconds(UPDATE_MEAS);
}