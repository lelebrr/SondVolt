// ============================================================================
// Sondvolt v4.0 - Motor de Medicao e Analise (implementacao)
// ============================================================================

#include "analysis.h"
#include "hal.h"
#include "config.h"
#include <math.h>

// ----------------------------------------------------------------------------
// Estado
// ----------------------------------------------------------------------------
static float gResistanceOffset  = 0.0f;   // ohms das pontas em curto
static float gCapacitanceOffset = 0.0f;   // farads parasitas dos cabos

void analysis_init() {
    gResistanceOffset  = 0.0f;
    gCapacitanceOffset = 0.0f;
}

void analysis_set_offsets(float r, float c) {
    // Rejeita offsets absurdos que so poderiam vir de uma calibracao ruim.
    gResistanceOffset  = (r >= 0.0f && r < 100.0f)  ? r : 0.0f;
    gCapacitanceOffset = (c >= 0.0f && c < 1e-7f)   ? c : 0.0f;
}

float analysis_get_resistance_offset()  { return gResistanceOffset; }
float analysis_get_capacitance_offset() { return gCapacitanceOffset; }

// ----------------------------------------------------------------------------
// Controle do circuito de excitacao
// ----------------------------------------------------------------------------

// Coloca todos os pinos de excitacao em alta impedancia.
static void drive_idle() {
    pinMode(PIN_PROBE_DRIVE, INPUT);
    pinMode(PIN_PROBE_DRIVE_LOW, INPUT);
    pinMode(PIN_CAP_DISCHARGE, OUTPUT);
    digitalWrite(PIN_CAP_DISCHARGE, LOW);
}

// Energiza a ponta 1 atraves do resistor de referencia escolhido.
static void drive_high(ProbeRange range) {
    if (range == PROBE_RANGE_LOW) {
        pinMode(PIN_PROBE_DRIVE, INPUT);
        pinMode(PIN_PROBE_DRIVE_LOW, OUTPUT);
        digitalWrite(PIN_PROBE_DRIVE_LOW, HIGH);
    } else {
        pinMode(PIN_PROBE_DRIVE_LOW, INPUT);
        pinMode(PIN_PROBE_DRIVE, OUTPUT);
        digitalWrite(PIN_PROBE_DRIVE, HIGH);
    }
}

static float range_resistor(ProbeRange range) {
    return (range == PROBE_RANGE_LOW) ? PROBE_REF_RESISTOR_LOW
                                      : PROBE_REF_RESISTOR;
}

// Aterra a ponta 1 pelo MOSFET de descarga ate a tensao cair abaixo do limite.
static void discharge_probe(uint32_t maxMs = 1500) {
    if (!hal_bus_acquire(HAL_BUS_DISCHARGE, 200)) return;

    pinMode(PIN_CAP_DISCHARGE, OUTPUT);
    digitalWrite(PIN_CAP_DISCHARGE, HIGH);

    uint32_t start = millis();
    while ((millis() - start) < maxMs) {
        if (hal_adc_read_avg(PIN_ADC_PROBE1, 4) < 40) break;
        delay(5);
    }
    digitalWrite(PIN_CAP_DISCHARGE, LOW);
    hal_bus_release(HAL_BUS_DISCHARGE);
}

// ============================================================================
// 2. MEDICOES PRIMARIAS
// ============================================================================

// Mede a resistencia em uma faixa fixa, sem auto-range.
static float measure_resistance_fixed(ProbeRange range) {
    const float rRef = range_resistor(range);
    const float vcc  = hal_adc_vref();

    drive_high(range);
    delayMicroseconds(500);                 // acomodacao do divisor

    float v = hal_adc_read_volts(PIN_ADC_PROBE1, 24);
    drive_idle();

    // Pontas abertas: o divisor nao carrega, a ponta fica no potencial da fonte
    if (v >= (vcc - 0.04f)) return ANALYSIS_OPEN;
    // Curto franco: a ponta fica presa no terra
    if (v <= 0.012f)        return 0.0f;

    // Divisor de tensao: Rx = Rref * V / (Vcc - V)
    float rx = rRef * v / (vcc - v);

    rx -= gResistanceOffset;                // desconta a resistencia dos cabos
    if (rx < 0.0f) rx = 0.0f;
    return rx;
}

float analysis_measure_resistance(ProbeRange range) {
    if (!hal_probe_available()) return ANALYSIS_OPEN;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 200)) return ANALYSIS_OPEN;

    float result;
    if (range != PROBE_RANGE_AUTO) {
        result = measure_resistance_fixed(range);
    } else {
        // Comeca pela faixa alta, que e segura para qualquer valor.
        result = measure_resistance_fixed(PROBE_RANGE_HIGH);

        // Abaixo de ~2k o resistor de 10k perde resolucao: repete no de 470.
        if (result < 2000.0f && result > 0.0f) {
            float low = measure_resistance_fixed(PROBE_RANGE_LOW);
            if (low > 0.0f && low < ANALYSIS_OPEN) result = low;
        }
    }

    hal_bus_release(HAL_BUS_PROBE_DRIVE);
    return result;
}

float analysis_measure_capacitance() {
    if (!hal_probe_available()) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 400)) return 0.0f;

    discharge_probe();

    // Limiar de 63.2 % da alimentacao: nesse ponto t = R * C.
    const float vcc      = hal_adc_vref();
    const float vTarget  = vcc * 0.632f;
    const uint16_t rawTarget = (uint16_t)((vTarget / vcc) * ADC_MAX_COUNT);

    // Comeca pelo resistor grande (bom para capacitores pequenos).
    ProbeRange range = PROBE_RANGE_HIGH;
    float capacitance = 0.0f;

    for (uint8_t attempt = 0; attempt < 2; attempt++) {
        discharge_probe(600);

        const float rRef = range_resistor(range);
        drive_high(range);

        uint32_t t0 = micros();
        uint32_t elapsed = 0;
        bool reached = false;
        const uint32_t timeoutUs = 3000000UL;   // 3 s

        while ((elapsed = micros() - t0) < timeoutUs) {
            if (hal_adc_read(PIN_ADC_PROBE1) >= rawTarget) { reached = true; break; }
        }
        drive_idle();

        if (!reached) { capacitance = 0.0f; break; }

        // Menos de 60 us e ruido de comutacao, nao um capacitor real.
        if (elapsed < 60) {
            if (range == PROBE_RANGE_HIGH) { range = PROBE_RANGE_LOW; continue; }
            capacitance = 0.0f;
            break;
        }

        // C = t / R   (t em segundos)
        capacitance = ((float)elapsed * 1e-6f) / rRef;

        // Se o capacitor for grande demais para o resistor atual a medicao
        // demora demais: troca para o resistor menor e refaz.
        if (elapsed > 1500000UL && range == PROBE_RANGE_HIGH) {
            range = PROBE_RANGE_LOW;
            continue;
        }
        break;
    }

    discharge_probe(400);
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    capacitance -= gCapacitanceOffset;
    if (capacitance < 0.0f) capacitance = 0.0f;
    return capacitance;
}

float analysis_measure_esr() {
    if (!hal_probe_available()) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 300)) return 0.0f;

    discharge_probe(800);

    // Um capacitor descarregado se comporta como um curto no primeiro instante.
    // A tensao que sobra imediatamente apos o pulso e a queda na ESR.
    const float rRef = PROBE_REF_RESISTOR_LOW;
    const float vcc  = hal_adc_vref();

    pinMode(PIN_PROBE_DRIVE_LOW, OUTPUT);
    digitalWrite(PIN_PROBE_DRIVE_LOW, HIGH);
    delayMicroseconds(25);                        // pulso curto e controlado
    uint16_t raw = hal_adc_read(PIN_ADC_PROBE1);  // leitura unica, sem media
    digitalWrite(PIN_PROBE_DRIVE_LOW, LOW);
    pinMode(PIN_PROBE_DRIVE_LOW, INPUT);

    discharge_probe(400);
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    float v = hal_adc_to_volts(raw);
    if (v <= 0.0f || v >= vcc) return 0.0f;

    // Mesmo divisor da resistencia, mas medido no primeiro microssegundo.
    float esr = rRef * v / (vcc - v);
    esr -= gResistanceOffset;
    if (esr < 0.0f) esr = 0.0f;
    if (esr > 200.0f) return 0.0f;    // acima disso nao e um capacitor
    return esr;
}

float analysis_measure_forward_voltage() {
    if (!hal_probe_available()) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 200)) return 0.0f;

    // Com o resistor de 470 ohms circula cerca de 5 mA, corrente tipica de
    // teste para uma juncao de silicio.
    drive_high(PROBE_RANGE_LOW);
    delay(2);
    float v = hal_adc_read_volts(PIN_ADC_PROBE1, 24);
    drive_idle();

    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    if (v < ANALYSIS_VF_MIN) return 0.0f;      // curto, nao juncao
    if (v > 3.1f)            return 0.0f;      // aberto
    return v;
}

float analysis_measure_zener_voltage() {
    // Sem a fonte auxiliar de 12 V nao ha como polarizar o Zener no joelho.
    // Preferimos devolver 0 a inventar um numero.
    return 0.0f;
}

float analysis_measure_hfe(ComponentType* detectedType) {
    if (detectedType) *detectedType = COMP_UNKNOWN;
    if (!hal_probe_available()) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 300)) return 0.0f;

    const float vcc = hal_adc_vref();
    float hfe = 0.0f;

    // --- Tentativa NPN -----------------------------------------------------
    // Base via 10k (Ib ~ 0.26 mA), coletor via 470 (Ic limitado a ~7 mA).
    pinMode(PIN_PROBE_DRIVE, OUTPUT);      // base, atraves de 10k
    pinMode(PIN_PROBE_DRIVE_LOW, OUTPUT);  // coletor, atraves de 470
    digitalWrite(PIN_PROBE_DRIVE, HIGH);
    digitalWrite(PIN_PROBE_DRIVE_LOW, HIGH);
    delay(3);

    float vCollector = hal_adc_read_volts(PIN_ADC_PROBE1, 16);

    digitalWrite(PIN_PROBE_DRIVE, LOW);
    delay(3);
    float vCollectorOff = hal_adc_read_volts(PIN_ADC_PROBE1, 16);

    drive_idle();
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    // Um transistor bom conduz com a base ativa e corta sem ela. A diferenca
    // entre os dois estados e a assinatura de um dispositivo com ganho.
    float swing = vCollectorOff - vCollector;
    if (swing > 0.4f) {
        float ib = (vcc - 0.7f) / PROBE_REF_RESISTOR;               // amperes
        float ic = (vcc - vCollector) / PROBE_REF_RESISTOR_LOW;     // amperes
        if (ib > 1e-9f) hfe = ic / ib;
        if (detectedType) *detectedType = COMP_TRANSISTOR_NPN;
    }

    if (hfe < 1.0f)    hfe = 0.0f;
    if (hfe > 2000.0f) hfe = 2000.0f;   // acima disso e ruido, nao ganho
    return hfe;
}

float analysis_measure_inductance() {
    if (!hal_probe_available()) return 0.0f;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 300)) return 0.0f;

    // Constante de tempo L/R: mede quanto tempo a tensao no indutor leva para
    // cair a 37 % apos o degrau. Valido de ~100 uH a ~100 mH; fora disso o
    // tempo fica curto ou longo demais para o ADC acompanhar.
    const float rRef = PROBE_REF_RESISTOR_LOW;

    drive_high(PROBE_RANGE_LOW);
    delayMicroseconds(200);

    uint16_t peak = hal_adc_read(PIN_ADC_PROBE1);
    uint16_t threshold = (uint16_t)(peak * 0.37f);

    uint32_t t0 = micros();
    uint32_t elapsed = 0;
    bool decayed = false;
    while ((elapsed = micros() - t0) < 200000UL) {
        if (hal_adc_read(PIN_ADC_PROBE1) <= threshold) { decayed = true; break; }
    }
    drive_idle();
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    if (!decayed || elapsed < 5) return 0.0f;

    // tau = L / R  ->  L = tau * R
    float inductance = ((float)elapsed * 1e-6f) * rRef;
    if (inductance < 1e-6f || inductance > 1.0f) return 0.0f;
    return inductance;
}

float analysis_measure_leakage() {
    // Depois de carregado, um capacitor bom mantem a carga. A velocidade da
    // queda revela a resistencia de fuga em paralelo.
    if (!hal_probe_available()) return ANALYSIS_OPEN;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 400)) return ANALYSIS_OPEN;

    drive_high(PROBE_RANGE_HIGH);
    delay(120);                                   // carrega
    float v0 = hal_adc_read_volts(PIN_ADC_PROBE1, 8);
    drive_idle();                                 // solta em alta impedancia

    delay(500);
    float v1 = hal_adc_read_volts(PIN_ADC_PROBE1, 8);

    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    if (v0 <= 0.1f) return ANALYSIS_OPEN;
    float drop = v0 - v1;
    if (drop <= 0.01f) return ANALYSIS_OPEN;      // nao vazou nada: otimo

    // Aproximacao de primeira ordem da descarga exponencial.
    float ratio = v1 / v0;
    if (ratio <= 0.0f || ratio >= 1.0f) return ANALYSIS_OPEN;
    float tau = 0.5f / (-logf(ratio));
    float capacitance = analysis_get_capacitance_offset() + 1e-9f;
    float rLeak = tau / capacitance;
    if (rLeak > ANALYSIS_OPEN) rLeak = ANALYSIS_OPEN;
    return rLeak;
}

float analysis_measure_battery_resistance(float openVolts, float loadedVolts,
                                          float loadOhms) {
    if (loadOhms <= 0.0f)            return 0.0f;
    if (loadedVolts <= 0.0f)         return 0.0f;
    if (openVolts <= loadedVolts)    return 0.0f;

    // Rint = (Vaberto - Vcarga) / Icarga, com Icarga = Vcarga / Rcarga
    float current = loadedVolts / loadOhms;
    if (current < 1e-6f) return 0.0f;
    return (openVolts - loadedVolts) / current;
}

// ============================================================================
// 3. TESTES LOGICOS
// ============================================================================

bool analysis_test_continuity(float thresholdOhms) {
    float r = analysis_measure_resistance(PROBE_RANGE_LOW);
    return (r < thresholdOhms);
}

bool analysis_test_short() {
    float r = analysis_measure_resistance(PROBE_RANGE_LOW);
    return (r < ANALYSIS_SHORT_OHMS);
}

bool analysis_test_open() {
    float r = analysis_measure_resistance(PROBE_RANGE_HIGH);
    return (r >= ANALYSIS_OPEN * 0.9f);
}

bool analysis_detect_mosfet(ComponentType* channelType) {
    if (channelType) *channelType = COMP_UNKNOWN;
    if (!hal_probe_available()) return false;
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 300)) return false;

    // Gate em alta impedancia: um MOSFET de canal N deve estar cortado.
    pinMode(PIN_PROBE_DRIVE, INPUT);
    pinMode(PIN_PROBE_DRIVE_LOW, OUTPUT);
    digitalWrite(PIN_PROBE_DRIVE_LOW, HIGH);
    delay(2);
    float vOff = hal_adc_read_volts(PIN_ADC_PROBE1, 12);

    // Carrega o gate: o canal deve conduzir e derrubar a tensao.
    pinMode(PIN_PROBE_DRIVE, OUTPUT);
    digitalWrite(PIN_PROBE_DRIVE, HIGH);
    delay(4);
    float vOn = hal_adc_read_volts(PIN_ADC_PROBE1, 12);

    drive_idle();
    hal_bus_release(HAL_BUS_PROBE_DRIVE);

    // Um MOSFET real mantem o estado por causa da capacitancia do gate, o que
    // o distingue de um transistor bipolar (que precisa de corrente continua).
    bool conducts = (vOff - vOn) > 0.5f;
    if (conducts && channelType) *channelType = COMP_MOSFET_N;
    return conducts;
}

bool analysis_test_fuse() {
    float r = analysis_measure_resistance(PROBE_RANGE_LOW);
    return (r < 5.0f);   // fusivel bom e praticamente um curto
}

// ============================================================================
// 4. IDENTIFICACAO AUTOMATICA
// ============================================================================

ComponentType analysis_classify_diode(float vf) {
    if (vf <= 0.0f)   return COMP_UNKNOWN;
    if (vf < 0.20f)   return COMP_UNKNOWN;   // curto, nao juncao
    if (vf < 0.40f)   return COMP_DIODE;     // Schottky (0.2 a 0.4 V)
    if (vf < 0.80f)   return COMP_DIODE;     // silicio comum (0.6 a 0.75 V)
    if (vf < 2.60f)   return COMP_LED;       // LED vermelho/amarelo/verde
    return COMP_LED;                          // LED azul/branco (2.6 a 3.4 V)
}

// Nome mais especifico do diodo, usado no campo detail.
static const char* diode_family(float vf) {
    if (vf < 0.40f) return "Schottky";
    if (vf < 0.80f) return "Silicio";
    if (vf < 2.10f) return "LED vermelho";
    if (vf < 2.50f) return "LED verde";
    return "LED azul/branco";
}

AnalysisResult analysis_identify() {
    AnalysisResult r;
    memset(&r, 0, sizeof(r));
    r.type        = COMP_UNKNOWN;
    r.status      = STATUS_UNKNOWN;
    r.timestampMs = millis();
    strncpy(r.label, "Desconhecido", sizeof(r.label) - 1);

    if (!hal_probe_available()) {
        strncpy(r.label,  "Sem hardware", sizeof(r.label) - 1);
        strncpy(r.detail, "Circuito de pontas ausente", sizeof(r.detail) - 1);
        r.status = STATUS_INVALID;
        return r;
    }

    // --- 1) Nada conectado? -------------------------------------------------
    float resistance = analysis_measure_resistance(PROBE_RANGE_AUTO);
    r.resistance = resistance;

    if (resistance >= ANALYSIS_OPEN * 0.9f) {
        // Pode ser um capacitor (que em DC parece aberto) ou nada.
        float c = analysis_measure_capacitance();
        if (c > 5e-10f) {                    // acima de 500 pF e capacitor
            r.type        = (c > 1e-6f) ? COMP_CAPACITOR_ELECTRO
                                        : COMP_CAPACITOR_CERAMIC;
            r.capacitance = c;
            r.esr         = analysis_measure_esr();
            r.valid       = true;
            strncpy(r.label, (c > 1e-6f) ? "Capacitor eletrolitico"
                                         : "Capacitor ceramico",
                    sizeof(r.label) - 1);
            analysis_format_value(c, "F", r.valueText, sizeof(r.valueText));

            // ESR alta e o sintoma classico de eletrolitico ressecado.
            if (r.esr > 8.0f && c > 1e-6f) {
                r.status = STATUS_SUSPECT;
                snprintf(r.detail, sizeof(r.detail), "ESR alta: %.1f Ohm", r.esr);
            } else {
                r.status = STATUS_GOOD;
                snprintf(r.detail, sizeof(r.detail), "ESR %.2f Ohm", r.esr);
            }
            return r;
        }

        strncpy(r.label,  "Aberto", sizeof(r.label) - 1);
        strncpy(r.detail, "Nada conectado nas pontas", sizeof(r.detail) - 1);
        r.type   = COMP_NONE;
        r.status = STATUS_OPEN;
        return r;
    }

    // --- 2) Curto franco ----------------------------------------------------
    if (resistance < ANALYSIS_SHORT_OHMS) {
        r.type   = COMP_GENERIC;
        r.status = STATUS_SHORT;
        r.valid  = true;
        strncpy(r.label, "Curto-circuito", sizeof(r.label) - 1);
        analysis_format_value(resistance, "Ohm", r.valueText, sizeof(r.valueText));
        strncpy(r.detail, "Fio, jumper ou fusivel bom", sizeof(r.detail) - 1);
        return r;
    }

    // --- 3) Existe juncao? --------------------------------------------------
    float vf = analysis_measure_forward_voltage();
    if (vf >= ANALYSIS_VF_MIN && vf <= 3.2f && resistance > 300.0f) {
        ComponentType hfeType = COMP_UNKNOWN;
        float hfe = analysis_measure_hfe(&hfeType);

        if (hfe > 8.0f) {
            // Ganho consistente: e um transistor, nao um diodo simples.
            r.type   = hfeType;
            r.gain   = hfe;
            r.forwardVoltage = vf;
            r.valid  = true;
            r.status = (hfe > 20.0f) ? STATUS_GOOD : STATUS_SUSPECT;
            strncpy(r.label, "Transistor NPN", sizeof(r.label) - 1);
            snprintf(r.valueText, sizeof(r.valueText), "hFE %.0f", hfe);
            snprintf(r.detail, sizeof(r.detail), "Vbe %.2f V", vf);
            return r;
        }

        ComponentType channel = COMP_UNKNOWN;
        if (analysis_detect_mosfet(&channel)) {
            r.type   = channel;
            r.valid  = true;
            r.status = STATUS_GOOD;
            strncpy(r.label, "MOSFET canal N", sizeof(r.label) - 1);
            snprintf(r.valueText, sizeof(r.valueText), "Vgs %.2f V", vf);
            strncpy(r.detail, "Gate responde ao acionamento", sizeof(r.detail) - 1);
            return r;
        }

        r.type           = analysis_classify_diode(vf);
        r.forwardVoltage = vf;
        r.valid          = true;
        r.status         = STATUS_GOOD;
        snprintf(r.label, sizeof(r.label), "%s",
                 (r.type == COMP_LED) ? "LED" : "Diodo");
        snprintf(r.valueText, sizeof(r.valueText), "Vf %.2f V", vf);
        snprintf(r.detail, sizeof(r.detail), "%s", diode_family(vf));
        return r;
    }

    // --- 4) Indutor? --------------------------------------------------------
    if (resistance < 100.0f) {
        float l = analysis_measure_inductance();
        if (l > 1e-5f) {
            r.type       = COMP_INDUCTOR;
            r.inductance = l;
            r.resistance = resistance;
            r.valid      = true;
            r.status     = STATUS_GOOD;
            strncpy(r.label, "Indutor", sizeof(r.label) - 1);
            analysis_format_value(l, "H", r.valueText, sizeof(r.valueText));
            snprintf(r.detail, sizeof(r.detail), "DCR %.2f Ohm", resistance);
            return r;
        }
    }

    // --- 5) Resistor ---------------------------------------------------------
    r.type   = COMP_RESISTOR;
    r.valid  = true;
    strncpy(r.label, "Resistor", sizeof(r.label) - 1);
    analysis_format_value(resistance, "Ohm", r.valueText, sizeof(r.valueText));

    float nearest = analysis_nearest_e_series(resistance, 24);
    float deviation = analysis_e_series_deviation(resistance, 24);
    char nearestTxt[16];
    analysis_format_eng(nearest, nearestTxt, sizeof(nearestTxt), 2);
    snprintf(r.detail, sizeof(r.detail), "E24 %s (%+.1f%%)", nearestTxt, deviation);

    // Mais de 10 % fora de qualquer valor comercial e sinal de componente
    // degradado ou de leitura ruim.
    r.status = (fabsf(deviation) <= 10.0f) ? STATUS_GOOD : STATUS_SUSPECT;
    return r;
}

const char* analysis_type_name(ComponentType type) {
    switch (type) {
        case COMP_RESISTOR:           return "Resistor";
        case COMP_CAPACITOR:          return "Capacitor";
        case COMP_CAPACITOR_CERAMIC:  return "Cap. ceramico";
        case COMP_CAPACITOR_ELECTRO:  return "Cap. eletrolitico";
        case COMP_DIODE:              return "Diodo";
        case COMP_LED:                return "LED";
        case COMP_ZENER:              return "Zener";
        case COMP_TRANSISTOR:         return "Transistor";
        case COMP_TRANSISTOR_NPN:     return "Transistor NPN";
        case COMP_TRANSISTOR_PNP:     return "Transistor PNP";
        case COMP_MOSFET:             return "MOSFET";
        case COMP_MOSFET_N:           return "MOSFET canal N";
        case COMP_MOSFET_P:           return "MOSFET canal P";
        case COMP_INDUCTOR:           return "Indutor";
        case COMP_CRYSTAL:            return "Cristal";
        case COMP_FUSE:               return "Fusivel";
        case COMP_VARISTOR:           return "Varistor";
        case COMP_POTENTIOMETER:      return "Potenciometro";
        case COMP_OPTOCOUPLER:        return "Optoacoplador";
        case COMP_RELAY:              return "Rele";
        case COMP_IC:                 return "Circuito integrado";
        case COMP_COIL:               return "Bobina";
        case COMP_GENERIC:            return "Componente";
        case COMP_NONE:               return "Nenhum";
        default:                      return "Desconhecido";
    }
}

const char* analysis_type_unit(ComponentType type) {
    switch (type) {
        case COMP_RESISTOR:
        case COMP_POTENTIOMETER:      return "Ohm";
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO:  return "F";
        case COMP_INDUCTOR:
        case COMP_COIL:               return "H";
        case COMP_DIODE:
        case COMP_LED:
        case COMP_ZENER:              return "V";
        case COMP_TRANSISTOR:
        case COMP_TRANSISTOR_NPN:
        case COMP_TRANSISTOR_PNP:     return "hFE";
        default:                      return "";
    }
}

// ============================================================================
// 5. CODIGO DE CORES E SERIES E
// ============================================================================

static const char* kBandNames[10] = {
    "Preto", "Marrom", "Vermelho", "Laranja", "Amarelo",
    "Verde", "Azul", "Violeta", "Cinza", "Branco"
};

static const uint16_t kBandColors[10] = {
    0x0000,  // Preto
    0x6980,  // Marrom
    0xF800,  // Vermelho
    0xFD20,  // Laranja
    0xFFE0,  // Amarelo
    0x07E0,  // Verde
    0x001F,  // Azul
    0x780F,  // Violeta
    0x8410,  // Cinza
    0xFFFF   // Branco
};

bool analysis_resistor_color_bands(float ohms, const char* band[4]) {
    if (ohms < 1.0f || ohms > 99e6f) return false;

    // Normaliza para dois digitos significativos e um multiplicador.
    int exponent = 0;
    float v = ohms;
    while (v >= 100.0f) { v /= 10.0f; exponent++; }
    while (v < 10.0f)   { v *= 10.0f; exponent--; }

    int digits = (int)(v + 0.5f);
    if (digits > 99) { digits = 99; }
    if (exponent < 0 || exponent > 9) return false;

    band[0] = kBandNames[digits / 10];
    band[1] = kBandNames[digits % 10];
    band[2] = kBandNames[exponent];
    band[3] = "Dourado";              // tolerancia de 5 %, a mais comum
    return true;
}

uint16_t analysis_color_band_rgb(const char* bandName) {
    if (!bandName) return 0x0000;
    for (uint8_t i = 0; i < 10; i++) {
        if (strcmp(bandName, kBandNames[i]) == 0) return kBandColors[i];
    }
    if (strcmp(bandName, "Dourado") == 0) return 0xC580;
    if (strcmp(bandName, "Prata")   == 0) return 0xC618;
    return 0x8410;
}

float analysis_resistor_from_bands(uint8_t d1, uint8_t d2, uint8_t mult) {
    if (d1 > 9 || d2 > 9 || mult > 9) return 0.0f;
    return (float)(d1 * 10 + d2) * powf(10.0f, (float)mult);
}

// Mantissas normalizadas das series comerciais (x100).
static const uint16_t kE6[]  = { 100, 150, 220, 330, 470, 680 };
static const uint16_t kE12[] = { 100, 120, 150, 180, 220, 270,
                                 330, 390, 470, 560, 680, 820 };
static const uint16_t kE24[] = { 100, 110, 120, 130, 150, 160, 180, 200,
                                 220, 240, 270, 300, 330, 360, 390, 430,
                                 470, 510, 560, 620, 680, 750, 820, 910 };

static const uint16_t* e_series_table(uint8_t size, uint8_t* count) {
    switch (size) {
        case 6:  *count = 6;  return kE6;
        case 12: *count = 12; return kE12;
        default: *count = 24; return kE24;
    }
}

float analysis_nearest_e_series(float value, uint8_t seriesSize) {
    if (value <= 0.0f) return 0.0f;

    uint8_t count = 0;
    const uint16_t* table = e_series_table(seriesSize, &count);

    // Separa mantissa (100 a 999) do expoente decimal.
    int exponent = 0;
    float mantissa = value;
    while (mantissa >= 1000.0f) { mantissa /= 10.0f; exponent++; }
    while (mantissa < 100.0f)   { mantissa *= 10.0f; exponent--; }

    float best = (float)table[0];
    float bestErr = fabsf(mantissa - best);
    for (uint8_t i = 1; i < count; i++) {
        float err = fabsf(mantissa - (float)table[i]);
        if (err < bestErr) { bestErr = err; best = (float)table[i]; }
    }
    // A serie e ciclica: 910 pode estar mais perto de 1000 do proximo ciclo.
    float wrapErr = fabsf(mantissa - 1000.0f);
    if (wrapErr < bestErr) best = 1000.0f;

    return best * powf(10.0f, (float)exponent);
}

float analysis_e_series_deviation(float value, uint8_t seriesSize) {
    float nearest = analysis_nearest_e_series(value, seriesSize);
    if (nearest <= 0.0f) return 0.0f;
    return ((value - nearest) / nearest) * 100.0f;
}

uint8_t analysis_suggest_tolerance(float value, uint8_t seriesSize) {
    float dev = fabsf(analysis_e_series_deviation(value, seriesSize));
    if (dev <= 1.0f)  return 1;
    if (dev <= 2.0f)  return 2;
    if (dev <= 5.0f)  return 5;
    if (dev <= 10.0f) return 10;
    return 20;
}

// ============================================================================
// 6. FORMATACAO
// ============================================================================

char* analysis_format_eng(float value, char* buf, size_t bufLen,
                          uint8_t decimals) {
    if (!buf || bufLen == 0) return buf;

    if (isnan(value) || isinf(value)) { snprintf(buf, bufLen, "---"); return buf; }

    bool negative = (value < 0.0f);
    float v = negative ? -value : value;

    if (v == 0.0f) { snprintf(buf, bufLen, "0"); return buf; }

    static const char* kPrefix[] = { "p", "n", "u", "m", "", "k", "M", "G" };
    const int zeroIndex = 4;           // posicao do prefixo vazio
    int index = zeroIndex;

    while (v >= 1000.0f && index < 7) { v /= 1000.0f; index++; }
    while (v <  1.0f    && index > 0) { v *= 1000.0f; index--; }

    // Mantem tres algarismos significativos independentemente da magnitude.
    uint8_t dec = decimals;
    if (v >= 100.0f)     dec = (decimals > 1) ? 1 : decimals;
    else if (v >= 10.0f) dec = (decimals > 2) ? 2 : decimals;

    snprintf(buf, bufLen, "%s%.*f %s", negative ? "-" : "", dec, v, kPrefix[index]);
    return buf;
}

char* analysis_format_value(float value, const char* unit,
                            char* buf, size_t bufLen) {
    if (!buf || bufLen == 0) return buf;

    char eng[20];
    analysis_format_eng(value, eng, sizeof(eng), 2);

    // format_eng ja termina com o prefixo (ou com um espaco solto).
    snprintf(buf, bufLen, "%s%s", eng, unit ? unit : "");
    return buf;
}

char* analysis_format_duration(uint32_t ms, char* buf, size_t bufLen) {
    if (!buf || bufLen == 0) return buf;

    uint32_t seconds = ms / 1000UL;
    if (seconds < 60)      snprintf(buf, bufLen, "%lus", (unsigned long)seconds);
    else if (seconds < 3600)
        snprintf(buf, bufLen, "%lumin", (unsigned long)(seconds / 60));
    else if (seconds < 86400)
        snprintf(buf, bufLen, "%luh %lumin",
                 (unsigned long)(seconds / 3600),
                 (unsigned long)((seconds % 3600) / 60));
    else
        snprintf(buf, bufLen, "%lud %luh",
                 (unsigned long)(seconds / 86400),
                 (unsigned long)((seconds % 86400) / 3600));
    return buf;
}

// ============================================================================
// 7. SINAIS
// ============================================================================

float analysis_measure_frequency(uint32_t timeoutMs) {
    // Conta cruzamentos de meia escala em uma janela de tempo. Suficiente para
    // sinais logicos e PWM de fontes chaveadas ate alguns kHz.
    const uint16_t midpoint = ADC_MAX_COUNT / 2;
    const uint16_t hysteresis = 250;

    uint32_t start = millis();
    uint32_t crossings = 0;
    bool above = (hal_adc_read(PIN_ADC_PROBE1) > midpoint);

    while ((millis() - start) < timeoutMs) {
        uint16_t v = hal_adc_read(PIN_ADC_PROBE1);
        if (above && v < (midpoint - hysteresis))      { above = false; crossings++; }
        else if (!above && v > (midpoint + hysteresis)) { above = true;  crossings++; }
    }

    uint32_t elapsed = millis() - start;
    if (elapsed == 0 || crossings < 2) return 0.0f;

    // Dois cruzamentos formam um ciclo completo.
    return ((float)crossings / 2.0f) * (1000.0f / (float)elapsed);
}

float analysis_measure_duty_cycle(uint32_t timeoutMs) {
    const uint16_t midpoint = ADC_MAX_COUNT / 2;

    uint32_t start = millis();
    uint32_t total = 0, high = 0;

    while ((millis() - start) < timeoutMs) {
        if (hal_adc_read(PIN_ADC_PROBE1) > midpoint) high++;
        total++;
    }
    if (total == 0) return 0.0f;
    return ((float)high / (float)total) * 100.0f;
}

float analysis_measure_peak_to_peak(uint16_t samples) {
    if (samples < 8) samples = 8;

    uint16_t vmin = ADC_MAX_COUNT, vmax = 0;
    for (uint16_t i = 0; i < samples; i++) {
        uint16_t v = hal_adc_read(PIN_ADC_PROBE1);
        if (v < vmin) vmin = v;
        if (v > vmax) vmax = v;
        delayMicroseconds(100);
    }
    return hal_adc_to_volts(vmax) - hal_adc_to_volts(vmin);
}
