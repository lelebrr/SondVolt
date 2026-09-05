// ============================================================================
// Sondvolt v4.0 - Motor de Medicao e Analise de Componentes
// ============================================================================
// Arquivo : analysis.h
// Objetivo: Todas as medicoes eletricas de componentes passivos e ativos,
//           a identificacao automatica e as funcoes auxiliares de engenharia
//           (codigo de cores, series E, notacao de engenharia).
//
// Topologia assumida (veja docs/WIRING.md):
//
//   +3V3 ---[ PIN_PROBE_DRIVE ]---[ R_REF 10k 1% ]---+--- PONTA 1 (ADC GPIO35)
//                                                    |
//   +3V3 ---[ PIN_PROBE_DRIVE_LOW ]---[ 470R 1% ]----+
//                                                    |
//                                              COMPONENTE
//                                                    |
//   [ PIN_CAP_DISCHARGE ]--(dreno MOSFET)------------+--- PONTA 2 (ADC GPIO34)
//                                                    |
//                                                   GND
// ============================================================================

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <Arduino.h>
#include "types.h"

// ----------------------------------------------------------------------------
// Faixa usada na medicao (auto-range)
// ----------------------------------------------------------------------------
enum ProbeRange {
    PROBE_RANGE_LOW = 0,   // resistor de 470 ohms  - 0.5 a 2k
    PROBE_RANGE_HIGH,      // resistor de 10k ohms  - 1k a 2M
    PROBE_RANGE_AUTO
};

// ----------------------------------------------------------------------------
// Resultado completo de uma identificacao automatica
// ----------------------------------------------------------------------------
struct AnalysisResult {
    ComponentType   type;
    ComponentStatus status;

    float resistance;      // ohms
    float capacitance;     // farads
    float inductance;      // henries
    float esr;             // ohms
    float forwardVoltage;  // volts (diodo/LED)
    float gain;            // hFE (transistor)
    float leakage;         // ohms equivalente de fuga

    char  label[24];       // "Resistor", "Diodo Schottky", ...
    char  valueText[24];   // "4.70 kOhm" ja formatado
    char  detail[40];      // observacao livre ("Vf 0.62 V", "hFE 214")
    bool  valid;
    uint32_t timestampMs;
};

// ============================================================================
// 1. INICIALIZACAO
// ============================================================================
void  analysis_init();

// Aplica os offsets salvos pela calibracao (resistencia de contato das pontas
// e capacitancia parasita dos cabos).
void  analysis_set_offsets(float resistanceOffset, float capacitanceOffset);
float analysis_get_resistance_offset();
float analysis_get_capacitance_offset();

// ============================================================================
// 2. MEDICOES PRIMARIAS
// ============================================================================

// Resistencia em ohms. Devolve ANALYSIS_OPEN se as pontas estiverem abertas
// e 0.0 em curto franco. Faz auto-range entre os dois resistores de
// referencia quando range == PROBE_RANGE_AUTO.
float analysis_measure_resistance(ProbeRange range = PROBE_RANGE_AUTO);

// Capacitancia em farads pelo metodo da constante de tempo RC (63.2 %).
// Faixa util aproximada: 1 nF a 4700 uF.
float analysis_measure_capacitance();

// Resistencia serie equivalente de um capacitor, em ohms. Usa um pulso
// curto de corrente e mede a queda instantanea antes da carga comecar.
float analysis_measure_esr();

// Tensao direta de juncao em volts. 0 se nao houver juncao.
float analysis_measure_forward_voltage();

// Tensao de um diodo Zener em polarizacao reversa (precisa da fonte auxiliar
// de 12 V no conector P3; devolve 0 se ela nao estiver presente).
float analysis_measure_zener_voltage();

// Ganho de corrente hFE. type recebe COMP_TRANSISTOR_NPN ou _PNP conforme
// detectado; escreva nullptr se nao quiser o tipo.
float analysis_measure_hfe(ComponentType* detectedType);

// Indutancia em henries, estimada pela constante de tempo L/R.
// Precisao limitada: valida de ~100 uH a ~100 mH.
float analysis_measure_inductance();

// Corrente de fuga equivalente (resistencia paralela) de um capacitor.
float analysis_measure_leakage();

// Resistencia interna de uma bateria, em ohms, a partir da queda de tensao
// sob carga conhecida.
float analysis_measure_battery_resistance(float openVolts, float loadedVolts,
                                          float loadOhms);

// ============================================================================
// 3. TESTES LOGICOS
// ============================================================================

// Continuidade: true se a resistencia estiver abaixo do limiar.
bool analysis_test_continuity(float thresholdOhms = 30.0f);

// Curto-circuito franco entre as pontas.
bool analysis_test_short();

// Pontas em aberto (nada conectado).
bool analysis_test_open();

// Deteccao de MOSFET: aplica tensao no gate e observa a conducao.
bool analysis_detect_mosfet(ComponentType* channelType);

// Verifica se um fusivel esta integro.
bool analysis_test_fuse();

// ============================================================================
// 4. IDENTIFICACAO AUTOMATICA
// ============================================================================

// Roda a bateria completa de testes e devolve o melhor palpite.
AnalysisResult analysis_identify();

// Classifica um diodo pela tensao direta.
ComponentType analysis_classify_diode(float vf);

// Nome legivel de um tipo de componente.
const char* analysis_type_name(ComponentType type);

// Simbolo da unidade principal de um tipo de componente.
const char* analysis_type_unit(ComponentType type);

// ============================================================================
// 5. ENGENHARIA - CODIGO DE CORES E SERIES E
// ============================================================================

// Nomes das faixas de cor de um resistor de 4 faixas.
// Preenche band[0..3] com ponteiros para strings constantes.
bool analysis_resistor_color_bands(float ohms, const char* band[4]);

// Cor RGB565 correspondente ao nome de uma faixa (para desenhar na tela).
uint16_t analysis_color_band_rgb(const char* bandName);

// Converte indices de cor (0=preto .. 9=branco) em valor de resistencia.
float analysis_resistor_from_bands(uint8_t d1, uint8_t d2, uint8_t mult);

// Valor comercial mais proximo dentro de uma serie E.
// seriesSize aceita 6, 12, 24, 48 ou 96.
float analysis_nearest_e_series(float value, uint8_t seriesSize = 24);

// Desvio percentual em relacao ao valor comercial mais proximo.
float analysis_e_series_deviation(float value, uint8_t seriesSize = 24);

// Faixa de tolerancia comercial sugerida (1, 2, 5, 10 ou 20 %).
uint8_t analysis_suggest_tolerance(float value, uint8_t seriesSize = 24);

// ============================================================================
// 6. FORMATACAO
// ============================================================================

// Notacao de engenharia com prefixo SI: 4700 -> "4.70 k".
// O sufixo da unidade NAO e incluido, para poder reaproveitar em qualquer
// grandeza. Devolve o proprio buffer.
char* analysis_format_eng(float value, char* buf, size_t bufLen,
                          uint8_t decimals = 2);

// Igual a anterior, mas ja concatena a unidade: "4.70 kOhm".
char* analysis_format_value(float value, const char* unit,
                            char* buf, size_t bufLen);

// Formata um intervalo de tempo em texto curto ("2 min", "3 h").
char* analysis_format_duration(uint32_t ms, char* buf, size_t bufLen);

// ============================================================================
// 7. SINAIS
// ============================================================================

// Frequencia de um sinal digital na ponta 1, em Hz. 0 se nao houver sinal.
float analysis_measure_frequency(uint32_t timeoutMs = 250);

// Ciclo de trabalho de um sinal digital, de 0 a 100 %.
float analysis_measure_duty_cycle(uint32_t timeoutMs = 250);

// Tensao de pico a pico de um sinal na ponta 1.
float analysis_measure_peak_to_peak(uint16_t samples = 256);

// ============================================================================
// CONSTANTES PUBLICAS
// ============================================================================
#define ANALYSIS_OPEN          9.9e6f    // pontas em aberto
#define ANALYSIS_SHORT_OHMS    1.5f      // limiar de curto franco
#define ANALYSIS_VF_MIN        0.15f     // juncao minima detectavel

#endif // ANALYSIS_H
