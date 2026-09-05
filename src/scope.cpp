// ============================================================================
// Sondvolt v5.0 - Osciloscopio, Curva I-V, Ripple, Gerador e Zener
// ============================================================================

#include "scope.h"
#include "hal.h"
#include "expander.h"
#include "config.h"
#include "globals.h"
#include <driver/i2s.h>
#include <driver/adc.h>
#include <math.h>

// ============================================================================
// ESTADO DO OSCILOSCOPIO
// ============================================================================
static bool         gActive       = false;
static uint32_t     gRate         = 100000;
static ScopeTrigger gTrigger      = SCOPE_TRIG_AUTO;
static float        gTriggerLevel = 1.65f;
static bool         gAttenuator   = false;

static const i2s_port_t kI2sPort = I2S_NUM_0;

// ============================================================================
// OSCILOSCOPIO
// ============================================================================

void scope_init() {
    gActive       = false;
    gRate         = 100000;
    gTrigger      = SCOPE_TRIG_AUTO;
    gTriggerLevel = 1.65f;
    gAttenuator   = false;
}

bool scope_begin(uint32_t sampleRateHz) {
    if (gActive) return true;

    if (sampleRateHz < SCOPE_MIN_RATE_HZ) sampleRateHz = SCOPE_MIN_RATE_HZ;
    if (sampleRateHz > SCOPE_MAX_RATE_HZ) sampleRateHz = SCOPE_MAX_RATE_HZ;
    gRate = sampleRateHz;

    // O I2S no modo ADC assume o controle do ADC1 inteiro. Enquanto isso
    // durar, nenhuma outra medicao pode ler o conversor - por isso
    // scope_end() e obrigatorio ao sair da tela.
    i2s_config_t cfg = {};
    cfg.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX |
                                            I2S_MODE_ADC_BUILT_IN);
    cfg.sample_rate          = gRate;
    cfg.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format       = I2S_CHANNEL_FMT_ONLY_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
    cfg.dma_buf_count        = 4;
    cfg.dma_buf_len          = SCOPE_BUFFER_SIZE;
    cfg.use_apll             = false;

    if (i2s_driver_install(kI2sPort, &cfg, 0, nullptr) != ESP_OK) {
        LOG_SERIAL_F("[SCP] Falha ao instalar o driver I2S");
        return false;
    }

    if (i2s_set_adc_mode(ADC_UNIT_1, (adc1_channel_t)SCOPE_ADC_CHANNEL) != ESP_OK) {
        i2s_driver_uninstall(kI2sPort);
        LOG_SERIAL_F("[SCP] Falha ao ligar o I2S ao ADC1");
        return false;
    }

    adc1_config_channel_atten((adc1_channel_t)SCOPE_ADC_CHANNEL, ADC_ATTEN_DB_11);
    i2s_adc_enable(kI2sPort);

    gActive = true;
    LOG_SERIAL_FMT("[SCP] Osciloscopio ativo a %lu amostras/s\n",
                   (unsigned long)gRate);
    return true;
}

void scope_end() {
    if (!gActive) return;

    i2s_adc_disable(kI2sPort);
    i2s_driver_uninstall(kI2sPort);
    gActive = false;

    // Devolve o ADC ao estado que o resto do firmware espera.
    analogReadResolution(ADC_RESOLUTION_BITS);
    LOG_SERIAL_F("[SCP] Osciloscopio encerrado, ADC devolvido");
}

bool scope_active() { return gActive; }

void scope_set_rate(uint32_t hz) {
    if (hz < SCOPE_MIN_RATE_HZ) hz = SCOPE_MIN_RATE_HZ;
    if (hz > SCOPE_MAX_RATE_HZ) hz = SCOPE_MAX_RATE_HZ;
    if (hz == gRate) return;

    // Mudar a taxa exige reinstalar o driver.
    bool wasActive = gActive;
    if (wasActive) scope_end();
    gRate = hz;
    if (wasActive) scope_begin(gRate);
}

uint32_t     scope_get_rate()          { return gRate; }
ScopeTrigger scope_get_trigger()       { return gTrigger; }
float        scope_get_trigger_level() { return gTriggerLevel; }

void scope_set_trigger(ScopeTrigger mode, float levelVolts) {
    gTrigger = mode;
    if (levelVolts >= 0.0f && levelVolts <= 3.3f) gTriggerLevel = levelVolts;
}

bool scope_set_attenuator(bool tenX) {
    if (!expander_present()) return false;
    if (!expander_scope_attenuator(tenX)) return false;
    gAttenuator = tenX;
    return true;
}

bool scope_get_attenuator() { return gAttenuator; }

float scope_sample_to_volts(uint16_t raw) {
    float v = hal_adc_to_volts(raw);
    return gAttenuator ? (v * SCOPE_ATTEN_RATIO) : v;
}

float scope_window_ms() {
    if (gRate == 0) return 0.0f;
    return ((float)SCOPE_BUFFER_SIZE / (float)gRate) * 1000.0f;
}

const char* scope_timebase_text() {
    static char buf[20];
    // Dez divisoes horizontais, como num osciloscopio de verdade.
    float perDivMs = scope_window_ms() / 10.0f;

    if (perDivMs < 0.001f)      snprintf(buf, sizeof(buf), "%.0f ns/div", perDivMs * 1e6f);
    else if (perDivMs < 1.0f)   snprintf(buf, sizeof(buf), "%.0f us/div", perDivMs * 1000.0f);
    else                        snprintf(buf, sizeof(buf), "%.2f ms/div", perDivMs);
    return buf;
}

// ----------------------------------------------------------------------------
// Analise da forma de onda capturada
// ----------------------------------------------------------------------------

static void analyze_capture(ScopeCapture* c) {
    if (!c || c->count == 0) return;

    uint16_t rawMin = 0xFFFF, rawMax = 0;
    double   sum = 0.0, sumSq = 0.0;

    for (uint16_t i = 0; i < c->count; i++) {
        uint16_t s = c->samples[i];
        if (s < rawMin) rawMin = s;
        if (s > rawMax) rawMax = s;
        sum += s;
    }

    c->vMin        = scope_sample_to_volts(rawMin);
    c->vMax        = scope_sample_to_volts(rawMax);
    c->vPeakToPeak = c->vMax - c->vMin;

    float meanRaw = (float)(sum / c->count);
    c->vAverage = scope_sample_to_volts((uint16_t)meanRaw);

    // RMS calculado sobre o desvio em relacao a media (componente AC).
    for (uint16_t i = 0; i < c->count; i++) {
        double d = (double)c->samples[i] - meanRaw;
        sumSq += d * d;
    }
    float rmsRaw = (float)sqrt(sumSq / c->count);
    // Converte a amplitude, nao o nivel absoluto: por isso a diferenca.
    c->vRms = scope_sample_to_volts((uint16_t)(meanRaw + rmsRaw)) - c->vAverage;
    if (c->vRms < 0.0f) c->vRms = -c->vRms;

    // --- Frequencia por cruzamento da media, com histerese ------------------
    // A histerese de 10 % da amplitude evita contar ruido como ciclo.
    float hyst = (float)(rawMax - rawMin) * 0.10f;
    if (hyst < 20.0f) hyst = 20.0f;

    uint16_t crossings = 0;
    uint32_t firstCross = 0, lastCross = 0;
    uint32_t highSamples = 0;
    bool above = ((float)c->samples[0] > meanRaw);

    for (uint16_t i = 1; i < c->count; i++) {
        float s = (float)c->samples[i];
        if (s > meanRaw) highSamples++;

        if (above && s < (meanRaw - hyst)) {
            above = false;
            if (crossings == 0) firstCross = i;
            lastCross = i;
            crossings++;
        } else if (!above && s > (meanRaw + hyst)) {
            above = true;
            if (crossings == 0) firstCross = i;
            lastCross = i;
            crossings++;
        }
    }

    if (crossings >= 2 && lastCross > firstCross) {
        // Cada ciclo completo tem dois cruzamentos.
        float cycles  = (float)(crossings - 1) / 2.0f;
        float seconds = (float)(lastCross - firstCross) / (float)c->sampleRateHz;
        c->frequencyHz = (seconds > 0.0f) ? (cycles / seconds) : 0.0f;
    } else {
        c->frequencyHz = 0.0f;   // sinal DC ou lento demais para esta janela
    }

    c->dutyPercent = ((float)highSamples / (float)c->count) * 100.0f;
}

// Procura a borda de gatilho e devolve o indice onde a onda deve comecar.
static uint16_t find_trigger(const ScopeCapture* c, uint16_t levelRaw,
                             bool rising, bool* found) {
    *found = false;
    if (!c || c->count < 4) return 0;

    // Deixa um quarto do buffer antes do gatilho, para o usuario ver o que
    // aconteceu ANTES da borda - o mesmo que um osciloscopio faz.
    uint16_t start = c->count / 4;

    for (uint16_t i = start; i < c->count - 1; i++) {
        uint16_t a = c->samples[i];
        uint16_t b = c->samples[i + 1];

        if (rising  && a <  levelRaw && b >= levelRaw) { *found = true; return i; }
        if (!rising && a >= levelRaw && b <  levelRaw) { *found = true; return i; }
    }
    return 0;
}

bool scope_capture(ScopeCapture* out, uint32_t timeoutMs) {
    if (!out) return false;
    memset(out, 0, sizeof(ScopeCapture));
    out->sampleRateHz = gRate;

    if (!gActive) return false;

    // Buffer temporario: o I2S entrega 16 bits por amostra, onde os 12 bits
    // do ADC ficam nos bits baixos e os 4 altos carregam o numero do canal.
    static uint16_t raw[SCOPE_BUFFER_SIZE];
    size_t bytesRead = 0;

    uint32_t deadline = millis() + timeoutMs;
    uint8_t  attempts = 0;

    while (millis() < deadline && attempts < 8) {
        attempts++;

        esp_err_t err = i2s_read(kI2sPort, raw, sizeof(raw), &bytesRead,
                                 pdMS_TO_TICKS(120));
        if (err != ESP_OK || bytesRead == 0) continue;

        uint16_t n = (uint16_t)(bytesRead / sizeof(uint16_t));
        if (n > SCOPE_BUFFER_SIZE) n = SCOPE_BUFFER_SIZE;

        for (uint16_t i = 0; i < n; i++) {
            out->samples[i] = raw[i] & 0x0FFF;   // descarta o id do canal
        }
        out->count = n;

        if (gTrigger == SCOPE_TRIG_AUTO) {
            out->triggered    = true;
            out->triggerIndex = 0;
            break;
        }

        // Converte o nivel de gatilho de volts para conta de ADC.
        float level = gAttenuator ? (gTriggerLevel / SCOPE_ATTEN_RATIO)
                                  : gTriggerLevel;
        uint16_t levelRaw = (uint16_t)((level / hal_adc_vref()) * ADC_MAX_COUNT);

        bool found = false;
        uint16_t idx = find_trigger(out, levelRaw,
                                    gTrigger != SCOPE_TRIG_FALLING, &found);
        if (found) {
            out->triggered    = true;
            out->triggerIndex = idx;
            break;
        }
        // Sem borda nesta janela: tenta outra captura ate o tempo acabar.
    }

    if (out->count == 0) return false;

    // Se o gatilho nao achou nada, mostramos assim mesmo e sinalizamos - e
    // mais util que uma tela em branco.
    out->valid = true;
    analyze_capture(out);
    return true;
}

// ============================================================================
// MEDIDOR DE RIPPLE
// ============================================================================

bool ripple_available() { return expander_present(); }

RippleResult ripple_measure() {
    RippleResult r;
    memset(&r, 0, sizeof(r));
    strncpy(r.verdict, "hardware ausente", sizeof(r.verdict) - 1);

    if (!expander_present()) return r;

    // --- 1) Nivel DC, com a entrada acoplada diretamente --------------------
    expander_ripple_coupling(false);
    vTaskDelay(pdMS_TO_TICKS(30));
    r.dcVolts = hal_adc_read_volts(PIN_RIPPLE_INPUT, 32);

    // --- 2) Ondulacao, com o capacitor de acoplamento inserido --------------
    // Com o capacitor, a componente DC e bloqueada e o que sobra oscila em
    // torno do ponto de polarizacao de 1,65 V.
    expander_ripple_coupling(true);

    const uint16_t kSamples = 512;
    uint16_t rawMin = ADC_MAX_COUNT, rawMax = 0;
    double sum = 0.0, sumSq = 0.0;
    uint16_t crossings = 0;
    bool above = false;
    uint32_t firstCross = 0, lastCross = 0;

    // Amostra por 40 ms: cobre dois ciclos completos de 50 Hz com folga.
    for (uint16_t i = 0; i < kSamples; i++) {
        uint16_t v = hal_adc_read(PIN_RIPPLE_INPUT);
        if (v < rawMin) rawMin = v;
        if (v > rawMax) rawMax = v;
        sum += v;
        delayMicroseconds(78);
    }

    float mean = (float)(sum / kSamples);

    // Segunda passada para o RMS e a frequencia, agora com a media conhecida.
    for (uint16_t i = 0; i < kSamples; i++) {
        uint16_t v = hal_adc_read(PIN_RIPPLE_INPUT);
        double d = (double)v - mean;
        sumSq += d * d;

        if (above && (float)v < mean - 15.0f) {
            above = false;
            if (crossings == 0) firstCross = i;
            lastCross = i; crossings++;
        } else if (!above && (float)v > mean + 15.0f) {
            above = true;
            if (crossings == 0) firstCross = i;
            lastCross = i; crossings++;
        }
        delayMicroseconds(78);
    }

    expander_ripple_coupling(false);

    r.rippleVpp = hal_adc_to_volts(rawMax) - hal_adc_to_volts(rawMin);
    float rmsRaw = (float)sqrt(sumSq / kSamples);
    r.rippleRms = (rmsRaw / (float)ADC_MAX_COUNT) * hal_adc_vref();

    if (crossings >= 2 && lastCross > firstCross) {
        float cycles  = (float)(crossings - 1) / 2.0f;
        float seconds = (float)(lastCross - firstCross) * 78e-6f;
        r.frequencyHz = (seconds > 0.0f) ? (cycles / seconds) : 0.0f;
    }

    r.ripplePercent = (r.dcVolts > 0.2f)
                    ? (r.rippleVpp / r.dcVolts) * 100.0f : 0.0f;
    r.valid = true;

    // --- 3) Veredito -------------------------------------------------------
    // Os limiares vem da pratica de bancada: acima de 10 % de ondulacao o
    // capacitor de filtro esta claramente ruim; abaixo de 1 % esta bom.
    if (r.dcVolts < 0.3f) {
        strncpy(r.verdict, "sem tensao na entrada", sizeof(r.verdict) - 1);
        r.valid = false;
    } else if (r.ripplePercent > 10.0f) {
        strncpy(r.verdict, "ondulacao alta: filtro ruim",
                sizeof(r.verdict) - 1);
    } else if (r.ripplePercent > 3.0f) {
        strncpy(r.verdict, "ondulacao elevada: suspeito",
                sizeof(r.verdict) - 1);
    } else if (r.frequencyHz > 1000.0f) {
        strncpy(r.verdict, "fonte chaveada, ondulacao ok",
                sizeof(r.verdict) - 1);
    } else {
        strncpy(r.verdict, "ondulacao normal", sizeof(r.verdict) - 1);
    }
    return r;
}

// ============================================================================
// TRACADOR DE CURVA I-V
// ============================================================================

bool curve_available() {
    return expander_present() && hal_probe_available();
}

bool curve_trace(CurveTrace* out) {
    if (!out) return false;
    memset(out, 0, sizeof(CurveTrace));
    strncpy(out->interpretation, "hardware ausente",
            sizeof(out->interpretation) - 1);

    if (!curve_available()) return false;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 500)) return false;

    expander_curve_sense(true);

    // Varre o duty do PWM de 0 a 100 %: a tensao media aplicada sobe junto,
    // e o shunt de 100 R nos da a corrente.
    hal_pwm_attach(PIN_PROBE_DRIVE, LEDC_CH_CURVE, 20000, 8);

    for (uint8_t step = 0; step < CURVE_STEPS; step++) {
        uint32_t duty = (uint32_t)((step * 255UL) / (CURVE_STEPS - 1));
        hal_pwm_write(PIN_PROBE_DRIVE, LEDC_CH_CURVE, duty);
        delayMicroseconds(800);          // acomodacao do filtro RC

        float vComponent = hal_adc_read_volts(PIN_ADC_PROBE1, 8);
        float vShunt     = hal_adc_read_volts(PIN_ADC_PROBE2, 8);

        out->points[step].volts = vComponent;
        out->points[step].amps  = vShunt / CURVE_SENSE_RESISTOR;

        if (vComponent > out->maxVolts) out->maxVolts = vComponent;
        if (out->points[step].amps > out->maxAmps)
            out->maxAmps = out->points[step].amps;

        out->count++;
    }

    hal_pwm_stop(PIN_PROBE_DRIVE, LEDC_CH_CURVE);
    pinMode(PIN_PROBE_DRIVE, INPUT);
    expander_curve_sense(false);
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    out->valid = (out->count > 4);
    if (!out->valid) return false;

    // --- Interpretacao da forma da curva ------------------------------------
    // Reta que passa pela origem  -> resistor
    // Joelho abrupto              -> juncao (diodo ou LED)
    // Corrente quase nula         -> aberto
    if (out->maxAmps < 1e-5f) {
        strncpy(out->interpretation, "sem conducao: aberto",
                sizeof(out->interpretation) - 1);
        return true;
    }

    // Procura o joelho: primeiro ponto que passa de 10 % da corrente maxima.
    float kneeVolts = 0.0f;
    for (uint8_t i = 0; i < out->count; i++) {
        if (out->points[i].amps > (out->maxAmps * 0.10f)) {
            kneeVolts = out->points[i].volts;
            break;
        }
    }

    // Linearidade: compara a inclinacao no primeiro e no ultimo terco.
    uint8_t a = out->count / 3;
    uint8_t b = (out->count * 2) / 3;
    float slope1 = (out->points[a].volts > 0.001f)
                 ? out->points[a].amps / out->points[a].volts : 0.0f;
    float slope2 = (out->points[b].volts > 0.001f)
                 ? out->points[b].amps / out->points[b].volts : 0.0f;
    bool linear = (slope1 > 0.0f) &&
                  (fabsf(slope2 - slope1) / slope1 < 0.30f);

    if (linear) {
        float ohms = (slope2 > 0.0f) ? (1.0f / slope2) : 0.0f;
        snprintf(out->interpretation, sizeof(out->interpretation),
                 "Resistiva, cerca de %.0f Ohm", ohms);
    } else if (kneeVolts > 0.15f && kneeVolts < 1.0f) {
        snprintf(out->interpretation, sizeof(out->interpretation),
                 "Juncao de silicio, joelho %.2f V", kneeVolts);
    } else if (kneeVolts >= 1.0f) {
        snprintf(out->interpretation, sizeof(out->interpretation),
                 "LED ou juncao alta, joelho %.2f V", kneeVolts);
    } else {
        strncpy(out->interpretation, "Curva nao linear",
                sizeof(out->interpretation) - 1);
    }
    return true;
}

// ============================================================================
// GERADOR DE SINAL
// ============================================================================

static bool     gSigActive = false;
static uint32_t gSigFreq   = 1000;
static uint8_t  gSigDuty   = 50;

bool siggen_start(uint32_t frequencyHz, uint8_t dutyPercent) {
    if (frequencyHz < SIGGEN_MIN_HZ) frequencyHz = SIGGEN_MIN_HZ;
    if (frequencyHz > SIGGEN_MAX_HZ) frequencyHz = SIGGEN_MAX_HZ;
    if (dutyPercent > 99) dutyPercent = 99;
    if (dutyPercent < 1)  dutyPercent = 1;

    // O pino do gerador e o mesmo da excitacao de baixa impedancia.
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 300)) return false;

    gSigFreq = frequencyHz;
    gSigDuty = dutyPercent;

    hal_pwm_attach(PIN_SIGGEN_OUT, LEDC_CH_SIGGEN, gSigFreq, SIGGEN_PWM_BITS);
    uint32_t maxDuty = (1UL << SIGGEN_PWM_BITS) - 1;
    hal_pwm_write(PIN_SIGGEN_OUT, LEDC_CH_SIGGEN,
                  (maxDuty * gSigDuty) / 100UL);

    // Fecha o caminho ate a ponta, se a placa de expansao estiver presente.
    expander_siggen_output(true);

    gSigActive = true;
    LOG_SERIAL_FMT("[GER] Gerador ligado: %lu Hz, %u%%\n",
                   (unsigned long)gSigFreq, (unsigned)gSigDuty);
    return true;
}

void siggen_stop() {
    if (!gSigActive) return;

    expander_siggen_output(false);
    hal_pwm_stop(PIN_SIGGEN_OUT, LEDC_CH_SIGGEN);
    pinMode(PIN_SIGGEN_OUT, INPUT);

    gSigActive = false;
    hal_bus_release(HAL_BUS_PROBE_DRIVE);
    LOG_SERIAL_F("[GER] Gerador desligado");
}

bool     siggen_active()    { return gSigActive; }
uint32_t siggen_frequency() { return gSigFreq; }
uint8_t  siggen_duty()      { return gSigDuty; }

bool siggen_set_frequency(uint32_t hz) {
    if (!gSigActive) return false;
    if (hz < SIGGEN_MIN_HZ || hz > SIGGEN_MAX_HZ) return false;

    gSigFreq = hz;
    hal_pwm_attach(PIN_SIGGEN_OUT, LEDC_CH_SIGGEN, gSigFreq, SIGGEN_PWM_BITS);
    return siggen_set_duty(gSigDuty);
}

bool siggen_set_duty(uint8_t percent) {
    if (!gSigActive) return false;
    if (percent < 1 || percent > 99) return false;

    gSigDuty = percent;
    uint32_t maxDuty = (1UL << SIGGEN_PWM_BITS) - 1;
    hal_pwm_write(PIN_SIGGEN_OUT, LEDC_CH_SIGGEN,
                  (maxDuty * gSigDuty) / 100UL);
    return true;
}

float siggen_actual_frequency() {
    // O LEDC divide o clock de 80 MHz por um inteiro. Com 8 bits de
    // resolucao, a frequencia realmente gerada e o clock dividido pelo
    // divisor inteiro mais proximo - por isso ela raramente bate exatamente
    // com o valor pedido.
    const float base = 80000000.0f;
    const float steps = (float)(1UL << SIGGEN_PWM_BITS);
    float divider = base / (steps * (float)gSigFreq);
    if (divider < 1.0f) divider = 1.0f;

    float rounded = floorf(divider + 0.5f);
    return base / (steps * rounded);
}

// ============================================================================
// TESTE DE ZENER
// ============================================================================

bool zener_available() {
    return expander_present() && hal_probe_available();
}

ZenerResult zener_measure() {
    ZenerResult z;
    memset(&z, 0, sizeof(z));
    strncpy(z.detail, "fonte de 12V ausente", sizeof(z.detail) - 1);

    if (!zener_available()) return z;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 500)) return z;

    // Liga a fonte auxiliar. O Zener e polarizado em reverso atraves do
    // resistor de 4k7, que limita a corrente de teste a cerca de 2 mA.
    if (!expander_boost_enable(true)) {
        hal_bus_release(HAL_BUS_PROBE_DRIVE);
        return z;
    }

    vTaskDelay(pdMS_TO_TICKS(40));   // acomodacao do joelho

    // A tensao no Zener e lida pelo divisor de 11:1 da entrada do multimetro,
    // porque 12 V nao cabem direto no ADC de 3,3 V.
    float measured = hal_adc_read_volts(PIN_ADC_PROBE1, 32) * MULTI_DC_DIVIDER_11X;

    expander_boost_enable(false);
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    z.zenerVolts    = measured;
    z.testCurrentMa = ((BOOST_OUTPUT_VOLTS - measured) / ZENER_SERIES_RESISTOR)
                    * 1000.0f;

    if (measured < 1.5f) {
        strncpy(z.detail, "curto ou polaridade invertida",
                sizeof(z.detail) - 1);
        z.valid = false;
    } else if (measured > ZENER_MAX_VOLTS) {
        // Sem joelho ate 11 V: ou o Zener e de tensao maior que a fonte
        // consegue atingir, ou esta aberto. Nao da para distinguir.
        strncpy(z.detail, "acima de 11V ou aberto", sizeof(z.detail) - 1);
        z.valid = false;
    } else {
        snprintf(z.detail, sizeof(z.detail), "Iz %.2f mA", z.testCurrentMa);
        z.valid = true;
    }
    return z;
}
