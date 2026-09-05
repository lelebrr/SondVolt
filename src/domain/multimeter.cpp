// ============================================================================
// Sondvolt v4.0 - Multimetro AC/DC com True RMS
// ============================================================================
// Arquivo : multimeter.cpp
//
// Defeitos corrigidos nesta revisao
// ---------------------------------
//   1. FAIXA DC ERRADA POR PADRAO. A faixa inicial e RANGE_AUTO, mas o
//      codigo antigo caia no ramo "else" e multiplicava a leitura por
//      600/3.3 = 181. Uma bateria de 1.5 V aparecia como 272 V.
//   2. INA219 SEM PROTOCOLO. As leituras chamavam requestFrom() sem antes
//      escrever o ponteiro de registrador, e nao aplicavam os fatores de
//      escala (4 mV/bit no barramento, 10 uV/bit no shunt). Os numeros eram
//      aleatorios. Alem disso, o init do I2C rodava a cada leitura.
//   3. FILTRO COMPARTILHADO ENTRE MODOS. O mesmo buffer de media movel
//      atendia tensao, corrente e resistencia; ao trocar de modo a leitura
//      nova era contaminada pela anterior por varios ciclos.
//   4. RMS SEM REMOCAO DE OFFSET. O zero do ZMPT era assumido em 2048 fixo;
//      qualquer desvio do trimpot virava tensao fantasma. Agora o offset e
//      medido junto com o sinal.
// ============================================================================

#include "multimeter.h"
#include "hal.h"
#include "analysis.h"
#include "config.h"
#include "globals.h"
#include "safety.h"
#include "buzzer.h"
#include "leds.h"
#include <Wire.h>
#include <Preferences.h>
#include <math.h>

// ============================================================================
// REGISTRADORES DO INA219
// ============================================================================
static const uint8_t INA219_REG_CONFIG   = 0x00;
static const uint8_t INA219_REG_SHUNT    = 0x01;
static const uint8_t INA219_REG_BUS      = 0x02;
static const uint8_t INA219_REG_CALIB    = 0x05;

// 32 V, ganho /8 (+-320 mV no shunt), 12 bits com 128 amostras em media,
// modo continuo de shunt e barramento.
static const uint16_t INA219_CONFIG_32V_2A = 0x399F;

// Escalas do datasheet
static const float INA219_BUS_LSB_V    = 0.004f;    // 4 mV por bit
static const float INA219_SHUNT_LSB_V  = 0.00001f;  // 10 uV por bit

// ============================================================================
// ESTADO
// ============================================================================
static MultimeterMode   gMode       = MMODE_DC_VOLTAGE;
static MeasurementRange gRange      = RANGE_AUTO;
static MultimeterState  gMeterState = MSTATE_IDLE;

static MultimeterReading   gLastReading;
static MeasurementHistory  gHistory;

static uint32_t gLastUpdateMs = 0;

static float gZmptGain       = ZMPT_DEFAULT_GAIN;  // volts por conta RMS
static float gIna219Scale    = 1.0f;
static float gShuntOhms      = INA219_SHUNT_OHMS;
static float gDcDivider      = MULTI_DC_DIVIDER_11X;

static bool  gIna219Present  = false;
static float gLastPeakVolts  = 0.0f;

static Preferences gPrefs;

// ----------------------------------------------------------------------------
// Um filtro independente por modo: trocar de modo nao contamina a leitura.
// ----------------------------------------------------------------------------
struct ModeFilter {
    float   buffer[ZMPT_FILTER_SIZE];
    uint8_t index;
    uint8_t filled;
    float   sum;
    float   exponential;
    bool    primed;
};

static ModeFilter gFilters[6];   // um por MultimeterMode

static void filter_reset(ModeFilter& f) {
    memset(&f, 0, sizeof(ModeFilter));
}

static float filter_apply(ModeFilter& f, float value) {
    // Media movel para cortar ruido de banda larga...
    f.sum -= f.buffer[f.index];
    f.buffer[f.index] = value;
    f.sum += value;
    f.index = (uint8_t)((f.index + 1) % ZMPT_FILTER_SIZE);
    if (f.filled < ZMPT_FILTER_SIZE) f.filled++;

    float movingAvg = f.sum / (float)f.filled;

    // ...seguida de um filtro exponencial para dar estabilidade ao display.
    if (!f.primed) { f.exponential = movingAvg; f.primed = true; }
    else {
        f.exponential = FILTER_ALPHA_DEFAULT * movingAvg +
                        (1.0f - FILTER_ALPHA_DEFAULT) * f.exponential;
    }
    return f.exponential;
}

// ============================================================================
// I2C DE BAIXO NIVEL
// ============================================================================

static bool ina219_write_reg(uint8_t reg, uint16_t value) {
    Wire.beginTransmission(INA219_ADDR);
    Wire.write(reg);
    Wire.write((uint8_t)(value >> 8));
    Wire.write((uint8_t)(value & 0xFF));
    return (Wire.endTransmission() == 0);
}

// Le um registrador de 16 bits. Devolve false se o dispositivo nao responder.
static bool ina219_read_reg(uint8_t reg, uint16_t* out) {
    Wire.beginTransmission(INA219_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;   // repeated start

    if (Wire.requestFrom((uint8_t)INA219_ADDR, (uint8_t)2) != 2) return false;

    uint16_t value = ((uint16_t)Wire.read() << 8);
    value |= (uint16_t)Wire.read();
    if (out) *out = value;
    return true;
}

// ============================================================================
// INICIALIZACAO
// ============================================================================

bool multimeter_ina219_init() {
    // O barramento I2C divide pinos com o drive das pontas na Rev A.
    if (!hal_bus_acquire(HAL_BUS_I2C, 200)) return false;

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FREQ_HZ);

    Wire.beginTransmission(INA219_ADDR);
    bool present = (Wire.endTransmission() == 0);

    if (present) {
        // Se a configuracao nao entrar, o chip fica nos padroes de fabrica e
        // todas as leituras saem com escala errada - sem nenhum aviso.
        bool cfgOk = ina219_write_reg(INA219_REG_CONFIG, INA219_CONFIG_32V_2A);

        // Calibracao para LSB de corrente de 100 uA com shunt de 0.1 ohm:
        //   cal = 0.04096 / (current_LSB * Rshunt)
        uint16_t cal = (uint16_t)(0.04096f / (0.0001f * gShuntOhms));
        bool calOk = ina219_write_reg(INA219_REG_CALIB, cal);

        if (!cfgOk || !calOk) {
            LOG_SERIAL_F("[MUL] INA219 respondeu mas recusou a configuracao");
            present = false;
        }
    }

    hal_bus_release(HAL_BUS_I2C);
    gIna219Present = present;
    return present;
}

void multimeter_adc_init() {
    // Ja feito por hal_init(); mantido para compatibilidade de chamadas.
}

void multimeter_init(bool calibrate) {
    memset(&gLastReading, 0, sizeof(gLastReading));
    memset(&gHistory, 0, sizeof(gHistory));
    for (uint8_t i = 0; i < 6; i++) filter_reset(gFilters[i]);

    gMode  = MMODE_DC_VOLTAGE;
    gRange = RANGE_AUTO;

    if (calibrate) multimeter_reset_calibration();
    else           multimeter_load_calibration();

    if (multimeter_ina219_init()) {
        LOG_SERIAL_F("[MUL] INA219 detectado e calibrado");
    } else {
        LOG_SERIAL_F("[MUL] INA219 ausente: corrente e potencia desativadas");
    }
}

void multimeter_shutdown() {
    multimeter_save_calibration();
    multimeter_clear_alerts();
}

bool multimeter_ina219_present() { return gIna219Present; }

// ============================================================================
// LEITURAS
// ============================================================================

float multimeter_read_dc_voltage() {
    // A ponta 1 recebe a tensao atraves de um divisor externo. gDcDivider
    // guarda a razao real desse divisor (1.0 se a medida for direta).
    float v = hal_adc_read_volts(PIN_ADC_PROBE1, 24) * gDcDivider;

    // Auto-range: se o divisor de 11x estiver deixando a leitura muito baixa,
    // avisa que o usuario pode passar para a entrada direta e ganhar
    // resolucao. Nao ha rele para trocar sozinho.
    if (gRange == RANGE_AUTO) {
        gRange = (v < MULTI_DC_RANGE_3V) ? RANGE_LOW : RANGE_HIGH;
    }

    return filter_apply(gFilters[MMODE_DC_VOLTAGE], v);
}

float multimeter_read_dc_current() {
    if (!gIna219Present) return 0.0f;

    // Sob contencao, devolver gLastReading.value entregaria o valor do MODO
    // ANTERIOR (uma tensao, por exemplo) como se fosse corrente. Zero e
    // honesto: o filtro ignora e a tela mostra a leitura anterior do proprio
    // modo de corrente.
    if (!hal_bus_acquire(HAL_BUS_I2C, 100)) return 0.0f;

    uint16_t raw = 0;
    bool ok = ina219_read_reg(INA219_REG_SHUNT, &raw);
    hal_bus_release(HAL_BUS_I2C);

    if (!ok) { gIna219Present = false; return 0.0f; }

    // O registrador de shunt e um inteiro com sinal em complemento de dois.
    int16_t signedRaw = (int16_t)raw;
    float shuntVolts  = (float)signedRaw * INA219_SHUNT_LSB_V;
    float amps        = (shuntVolts / gShuntOhms) * gIna219Scale;

    return filter_apply(gFilters[MMODE_DC_CURRENT], amps);
}

// Tensao do barramento medida pelo proprio INA219 (entrada V+).
float multimeter_read_bus_voltage() {
    if (!gIna219Present) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_I2C, 100)) return 0.0f;

    uint16_t raw = 0;
    bool ok = ina219_read_reg(INA219_REG_BUS, &raw);
    hal_bus_release(HAL_BUS_I2C);

    if (!ok) { gIna219Present = false; return 0.0f; }

    // Bits 15:3 carregam o valor; bit 0 sinaliza estouro de escala.
    if (raw & 0x0001) { gMeterState = MSTATE_OVERLOAD; }
    return (float)(raw >> 3) * INA219_BUS_LSB_V;
}

float multimeter_read_resistance() {
    // A medicao de resistencia usa o circuito de pontas, nao a entrada do
    // multimetro. Delegar evita duas implementacoes divergentes.
    float r = analysis_measure_resistance(PROBE_RANGE_AUTO);
    return filter_apply(gFilters[MMODE_RESISTANCE], r);
}

float multimeter_calculate_rms(const int16_t* samples, uint16_t count) {
    if (!samples || count == 0) return 0.0f;

    // Soma em double: 256 amostras de ate 2048 ao quadrado passam de 1e9 e
    // perderiam precisao em float.
    double sumSquares = 0.0;
    for (uint16_t i = 0; i < count; i++) {
        double s = (double)samples[i];
        sumSquares += s * s;
    }
    return (float)sqrt(sumSquares / (double)count);
}

float multimeter_read_ac_voltage_rms() {
    static int16_t samples[ZMPT_NUM_SAMPLES];

    // Passo 1: coleta bruta com intervalo fixo.
    int32_t sum = 0;
    uint16_t rawMin = ADC_MAX_COUNT, rawMax = 0;

    for (uint16_t i = 0; i < ZMPT_NUM_SAMPLES; i++) {
        uint16_t raw = hal_adc_read(PIN_ADC_ZMPT);
        samples[i] = (int16_t)raw;
        sum += raw;
        if (raw < rawMin) rawMin = raw;
        if (raw > rawMax) rawMax = raw;
        delayMicroseconds(ZMPT_SAMPLE_RATE_US);
    }

    // Passo 2: o zero e a MEDIA das amostras, nao um 2048 fixo. Assim um
    // trimpot fora do centro nao vira tensao fantasma.
    const int16_t zero = (int16_t)(sum / ZMPT_NUM_SAMPLES);
    for (uint16_t i = 0; i < ZMPT_NUM_SAMPLES; i++) {
        samples[i] = (int16_t)(samples[i] - zero);
    }

    // Passo 3: RMS em contas de ADC, depois convertido pela calibracao.
    float rmsCounts = multimeter_calculate_rms(samples, ZMPT_NUM_SAMPLES);
    float volts = rmsCounts * gZmptGain;

    // Piso de ruido: abaixo de ~2 contas RMS nao ha sinal, so ruido do ADC.
    if (rmsCounts < 2.0f) volts = 0.0f;

    // Pico real, util para detectar surtos e formas de onda distorcidas.
    int16_t peakCounts = (int16_t)max((int)(rawMax - zero), (int)(zero - rawMin));
    gLastPeakVolts = (float)peakCounts * gZmptGain;

    return filter_apply(gFilters[MMODE_AC_VOLTAGE], volts);
}

bool multimeter_test_short() {
    return (multimeter_read_resistance() < SHORT_CIRCUIT_OHMS);
}

// ============================================================================
// LEITURA COMPLETA
// ============================================================================

MultimeterReading multimeter_read() {
    MultimeterReading r;
    memset(&r, 0, sizeof(r));
    r.mode      = gMode;
    r.range     = gRange;
    r.timestamp = millis();
    r.valid     = true;

    switch (gMode) {
        case MMODE_AC_VOLTAGE:
            r.value     = multimeter_read_ac_voltage_rms();
            r.peakValue = gLastPeakVolts;
            r.peakToPeak = gLastPeakVolts * 2.0f;
            r.unit      = "V AC";
            r.unitAbbrev = "V";
            // Numa senoide limpa o pico e 1.414 vezes o RMS. Bem acima disso
            // ha um transitorio ou uma forma de onda muito distorcida.
            if (r.value > 5.0f && r.peakValue > (r.value * 1.75f)) {
                r.surgeDetected = true;
            }
            break;

        case MMODE_DC_VOLTAGE:
            // Se o INA219 estiver presente ele mede melhor que o ADC direto.
            r.value = gIna219Present ? multimeter_read_bus_voltage()
                                     : multimeter_read_dc_voltage();
            r.unit      = "V DC";
            r.unitAbbrev = "V";
            break;

        case MMODE_DC_CURRENT:
            if (!gIna219Present) {
                r.valid = false;
                r.state = MSTATE_ERROR;
                r.unit  = "sem INA219";
                r.unitAbbrev = "A";
                gLastReading = r;
                return r;
            }
            r.value      = multimeter_read_dc_current();
            r.unit       = "A";
            r.unitAbbrev = "A";
            break;

        case MMODE_RESISTANCE:
            r.value      = multimeter_read_resistance();
            r.unit       = "Ohm";
            r.unitAbbrev = "R";
            break;

        case MMODE_CONTINUITY: {
            float res = multimeter_read_resistance();
            r.value      = res;
            bool closed  = (res < SHORT_CIRCUIT_OHMS);
            r.unit       = closed ? "FECHADO" : "ABERTO";
            r.unitAbbrev = closed ? "OK" : "OL";
            // Apito continuo enquanto houver continuidade, como num
            // multimetro de bancada.
            if (closed) buzzer_beep(2000, 120);
            break;
        }

        case MMODE_POWER: {
            if (!gIna219Present) {
                r.valid = false;
                r.state = MSTATE_ERROR;
                r.unit  = "sem INA219";
                gLastReading = r;
                return r;
            }
            float v = multimeter_read_bus_voltage();
            float i = multimeter_read_dc_current();
            r.value      = v * i;
            r.unit       = "W";
            r.unitAbbrev = "W";
            break;
        }

        default:
            r.valid = false;
            r.state = MSTATE_ERROR;
            gLastReading = r;
            return r;
    }

    // Corrente pode ser negativa (sentido invertido); as demais grandezas nao.
    if (gMode != MMODE_DC_CURRENT && r.value < 0.0f) r.value = 0.0f;

    // --- Estado e cor -------------------------------------------------------
    if (gMode == MMODE_AC_VOLTAGE && r.value > HIGH_VOLTAGE_THRESHOLD) {
        r.state       = MSTATE_HIGH_VOLTAGE;
        r.statusColor = COLOR_BAD;
        safety_check_voltage(r.value);
    } else if (gMode == MMODE_AC_VOLTAGE && r.value > MAX_VOLTAGE_AC) {
        r.state       = MSTATE_OVERLOAD;
        r.statusColor = COLOR_BAD;
    } else if ((gMode == MMODE_RESISTANCE || gMode == MMODE_CONTINUITY) &&
               r.value < SHORT_CIRCUIT_OHMS) {
        r.state       = MSTATE_SHORT;
        r.statusColor = COLOR_SUSPECT;
    } else {
        r.state       = MSTATE_MEASURING;
        r.statusColor = COLOR_GOOD;
    }

    gMeterState  = r.state;
    gLastReading = r;
    return r;
}

MultimeterReading multimeter_get_last_reading() { return gLastReading; }

void multimeter_handle() {
    uint32_t now = millis();
    if ((now - gLastUpdateMs) < TIME_REFRESH_MEAS) return;
    gLastUpdateMs = now;

    MultimeterReading r = multimeter_read();

    // Alimenta as variaveis globais usadas pelas telas.
    if (r.valid) {
        switch (r.mode) {
            case MMODE_AC_VOLTAGE:
            case MMODE_DC_VOLTAGE: lastVoltage = r.value; break;
            case MMODE_DC_CURRENT: lastCurrent = r.value; break;
            case MMODE_RESISTANCE:
            case MMODE_CONTINUITY: lastResistance = r.value; break;
            default: break;
        }
        multimeter_history_add(r.value, r.mode);
    }
}

// ============================================================================
// MODO E FAIXA
// ============================================================================

void multimeter_set_mode(MultimeterMode mode) {
    if (mode == gMode) return;
    gMode = mode;
    // Zera o filtro do modo novo para nao herdar leituras do modo anterior.
    if (mode < 6) filter_reset(gFilters[mode]);
    gRange = RANGE_AUTO;
    buzzer_click();
}

MultimeterMode   multimeter_get_mode()  { return gMode; }
void             multimeter_set_range(MeasurementRange r) { gRange = r; buzzer_click(); }
MeasurementRange multimeter_get_range() { return gRange; }
MultimeterState  multimeter_get_state() { return gMeterState; }

void multimeter_auto_range() {
    gRange = multimeter_suggest_range(gLastReading.value);
}

MeasurementRange multimeter_suggest_range(float value) {
    float v = fabsf(value);
    if (v < 3.0f)   return RANGE_LOW;
    if (v < 30.0f)  return RANGE_MED;
    return RANGE_HIGH;
}

const char* multimeter_mode_name(MultimeterMode mode) {
    switch (mode) {
        case MMODE_DC_VOLTAGE: return "TENSAO DC";
        case MMODE_AC_VOLTAGE: return "TENSAO AC";
        case MMODE_DC_CURRENT: return "CORRENTE DC";
        case MMODE_RESISTANCE: return "RESISTENCIA";
        case MMODE_CONTINUITY: return "CONTINUIDADE";
        case MMODE_POWER:      return "POTENCIA";
        default:               return "?";
    }
}

const char* multimeter_state_name(MultimeterState state) {
    switch (state) {
        case MSTATE_IDLE:         return "EM ESPERA";
        case MSTATE_MEASURING:    return "MEDINDO";
        case MSTATE_ERROR:        return "ERRO";
        case MSTATE_OVERLOAD:     return "SOBRECARGA";
        case MSTATE_SHORT:        return "CURTO";
        case MSTATE_HIGH_VOLTAGE: return "TENSAO ALTA";
        default:                  return "?";
    }
}

// ============================================================================
// CALIBRACAO (persistida na NVS)
// ============================================================================

void multimeter_calibrate_zmpt(float realVoltage) {
    // O filtro exponencial guarda varias leituras anteriores. Calibrar sem
    // zera-lo mede uma media que inclui o estado antigo, e o ganho sai
    // deslocado. Zerar e tomar algumas leituras de aquecimento resolve.
    filter_reset(gFilters[MMODE_AC_VOLTAGE]);
    for (uint8_t i = 0; i < ZMPT_FILTER_SIZE; i++) {
        multimeter_read_ac_voltage_rms();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    float measured = multimeter_read_ac_voltage_rms();
    if (measured > 0.5f && realVoltage > 0.5f) {
        gZmptGain *= (realVoltage / measured);
        deviceSettings.zmptScaleFactor = gZmptGain;
        multimeter_save_calibration();
        LOG_SERIAL_FMT("[MUL] Ganho do ZMPT ajustado para %.5f V/conta\n", gZmptGain);
    }
}

void multimeter_calibrate_ina219(float realVoltage, float realCurrent) {
    (void)realVoltage;
    float measured = multimeter_read_dc_current();
    if (fabsf(measured) > 0.005f && fabsf(realCurrent) > 0.005f) {
        gIna219Scale *= (realCurrent / measured);
        multimeter_save_calibration();
    }
}

// Ajusta a razao do divisor externo de tensao DC.
void multimeter_set_dc_divider(float ratio) {
    if (ratio >= 1.0f && ratio <= 100.0f) {
        gDcDivider = ratio;
        multimeter_save_calibration();
    }
}

float multimeter_get_dc_divider() { return gDcDivider; }
float multimeter_get_zmpt_gain()  { return gZmptGain; }

void multimeter_save_calibration() {
    if (!gPrefs.begin("mmcal", false)) return;
    gPrefs.putFloat("zmpt",  gZmptGain);
    gPrefs.putFloat("inasc", gIna219Scale);
    gPrefs.putFloat("shunt", gShuntOhms);
    gPrefs.putFloat("divdc", gDcDivider);
    gPrefs.end();
}

void multimeter_load_calibration() {
    if (!gPrefs.begin("mmcal", true)) { multimeter_reset_calibration(); return; }
    gZmptGain    = gPrefs.getFloat("zmpt",  ZMPT_DEFAULT_GAIN);
    gIna219Scale = gPrefs.getFloat("inasc", 1.0f);
    gShuntOhms   = gPrefs.getFloat("shunt", INA219_SHUNT_OHMS);
    gDcDivider   = gPrefs.getFloat("divdc", MULTI_DC_DIVIDER_11X);
    gPrefs.end();

    // Rejeita valores absurdos vindos de uma gravacao corrompida.
    if (gZmptGain    <= 0.0f || gZmptGain    > 10.0f)  gZmptGain    = ZMPT_DEFAULT_GAIN;
    if (gIna219Scale <= 0.0f || gIna219Scale > 10.0f)  gIna219Scale = 1.0f;
    if (gShuntOhms   <= 0.0f || gShuntOhms   > 10.0f)  gShuntOhms   = INA219_SHUNT_OHMS;
    if (gDcDivider   <  1.0f || gDcDivider   > 100.0f) gDcDivider   = MULTI_DC_DIVIDER_11X;
}

void multimeter_reset_calibration() {
    gZmptGain    = ZMPT_DEFAULT_GAIN;
    gIna219Scale = 1.0f;
    gShuntOhms   = INA219_SHUNT_OHMS;
    gDcDivider   = MULTI_DC_DIVIDER_11X;
    multimeter_save_calibration();
}

// ============================================================================
// FILTROS (interface publica mantida por compatibilidade)
// ============================================================================

float multimeter_filter_moving_average(float v) {
    return filter_apply(gFilters[gMode], v);
}

float multimeter_filter_exponential(float v) {
    return filter_apply(gFilters[gMode], v);
}

float multimeter_apply_filters(float v) {
    return filter_apply(gFilters[gMode], v);
}

// ============================================================================
// PROTECAO
// ============================================================================

void multimeter_check_high_voltage(float voltage) {
    if (voltage > HIGH_VOLTAGE_THRESHOLD) {
        gMeterState = MSTATE_HIGH_VOLTAGE;
        led_status_danger();
        buzzer_alert();
    } else {
        led_off();
    }
}

void multimeter_check_short_circuit(float resistance) {
    if (resistance < SHORT_CIRCUIT_OHMS) {
        gMeterState = MSTATE_SHORT;
        buzzer_beep(BUZZER_FREQ_WARNING, BUZZER_DURATION_WARNING);
    }
}

void multimeter_alert_sound(uint8_t type) {
    switch (type) {
        case 0: buzzer_ok();    break;
        case 1: buzzer_alert(); break;
        case 2: buzzer_error(); break;
        default: break;
    }
}

void multimeter_alert_led(uint8_t type) {
    switch (type) {
        case 0: led_status_good();   break;
        case 1: led_status_bad();    break;
        case 2: led_status_danger(); break;
        default: led_off();          break;
    }
}

void multimeter_clear_alerts() {
    led_off();
    buzzer_no_tone();
    gMeterState = MSTATE_IDLE;
}

// ============================================================================
// HISTORICO
// ============================================================================

void multimeter_history_add(float value, MultimeterMode mode) {
    MeasurementHistoryEntry& e = gHistory.entries[gHistory.index];
    e.value     = value;
    e.mode      = mode;
    e.timestamp = millis();
    e.valid     = true;

    gHistory.index = (uint8_t)((gHistory.index + 1) % HISTORY_SIZE);
    if (gHistory.count < HISTORY_SIZE) gHistory.count++;
}

void multimeter_history_clear() { memset(&gHistory, 0, sizeof(gHistory)); }

MeasurementHistory* multimeter_get_history() { return &gHistory; }

// ============================================================================
// UTILITARIOS
// ============================================================================

void multimeter_format_value(float value, char* buffer, uint8_t maxLen) {
    if (!buffer || maxLen == 0) return;

    float v = fabsf(value);
    if (v < 0.001f)      snprintf(buffer, maxLen, "%.4f", value);
    else if (v < 1.0f)   snprintf(buffer, maxLen, "%.3f", value);
    else if (v < 100.0f) snprintf(buffer, maxLen, "%.2f", value);
    else if (v < 10000.0f) snprintf(buffer, maxLen, "%.1f", value);
    else                 snprintf(buffer, maxLen, "%.0f", value);
}

bool multimeter_detect_voltage_type(float voltage) {
    // true = rede de 220 V, false = 127 V ou tensao baixa
    return (voltage > 170.0f);
}
