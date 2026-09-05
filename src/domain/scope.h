// ============================================================================
// Sondvolt v5.0 - Osciloscopio, Tracador de Curva e Medidor de Ripple
// ============================================================================
// Arquivo : scope.h
//
// Por que I2S e nao analogRead()
// ------------------------------
// Um laco com analogRead() + delayMicroseconds() tem jitter de dezenas de
// microssegundos: o intervalo real entre amostras varia conforme o
// escalonador do FreeRTOS, cache e interrupcoes. Para medir um valor DC isso
// nao importa; para reconstruir uma forma de onda, arruina tudo.
//
// O periferico I2S do ESP32 sabe ler o ADC1 direto para a memoria por DMA,
// com o espacamento entre amostras definido por hardware. E o unico jeito de
// ter uma base de tempo confiavel nesta placa.
//
// Limite pratico: cerca de 200 kSPS. Acima disso o ADC do ESP32 comeca a
// perder linearidade. Com 512 amostras isso da uma janela minima de ~2,5 ms,
// suficiente para ver PWM de fonte chaveada (20 a 150 kHz), ondulacao de
// retificador (100 ou 120 Hz) e sinal de audio.
//
// O que este modulo NAO e
// -----------------------
// Nao substitui um osciloscopio de bancada. A entrada e limitada a 0-3,3 V
// (ou 0-33 V com a ponta atenuadora 10x), tem uma unica via, e o ADC do
// ESP32 tem cerca de 9 bits efetivos de resolucao real. Serve para
// diagnostico, nao para medicao de laboratorio.
// ============================================================================

#ifndef SCOPE_H
#define SCOPE_H

#include <Arduino.h>
#include "pins.h"

// ----------------------------------------------------------------------------
// Gatilho
// ----------------------------------------------------------------------------
enum ScopeTrigger {
    SCOPE_TRIG_AUTO = 0,    // dispara sozinho, mostre o que houver
    SCOPE_TRIG_RISING,      // espera a borda de subida
    SCOPE_TRIG_FALLING,     // espera a borda de descida
    SCOPE_TRIG_SINGLE       // uma captura e congela
};

// ----------------------------------------------------------------------------
// Resultado de uma captura
// ----------------------------------------------------------------------------
struct ScopeCapture {
    uint16_t samples[SCOPE_BUFFER_SIZE];  // contas brutas do ADC
    uint16_t count;
    uint32_t sampleRateHz;                // taxa efetivamente usada
    bool     triggered;                   // o gatilho encontrou a borda
    bool     valid;

    // Medidas calculadas automaticamente sobre a captura
    float    vMin;
    float    vMax;
    float    vPeakToPeak;
    float    vAverage;
    float    vRms;
    float    frequencyHz;                 // 0 se nao houver periodicidade
    float    dutyPercent;
    uint16_t triggerIndex;                // onda alinhada a partir daqui
};

// ============================================================================
// OSCILOSCOPIO
// ============================================================================

// Prepara o I2S. Nao aloca DMA ainda: so ao entrar no modo osciloscopio.
void scope_init();

// Aloca os buffers de DMA e assume o controle do ADC1.
// Enquanto o osciloscopio estiver ativo, as demais medicoes ficam suspensas -
// o periferico I2S monopoliza o conversor.
bool scope_begin(uint32_t sampleRateHz);

// Libera o I2S e devolve o ADC ao uso normal. SEMPRE chamar ao sair da tela.
void scope_end();

// Verdadeiro enquanto o osciloscopio detem o ADC.
bool scope_active();

// Captura um quadro. Bloqueia por ate timeoutMs esperando o gatilho.
bool scope_capture(ScopeCapture* out, uint32_t timeoutMs = 500);

// Configuracao
void scope_set_rate(uint32_t hz);
uint32_t scope_get_rate();
void scope_set_trigger(ScopeTrigger mode, float levelVolts);
ScopeTrigger scope_get_trigger();
float scope_get_trigger_level();

// Atenuador de entrada: false = 1x (0-3,3 V), true = 10x (0-33 V).
// Exige a placa de expansao; devolve false se ela nao estiver presente.
bool scope_set_attenuator(bool tenX);
bool scope_get_attenuator();

// Converte uma amostra bruta em volts, ja considerando o atenuador.
float scope_sample_to_volts(uint16_t raw);

// Janela de tempo total da captura, em milissegundos.
float scope_window_ms();

// Texto da base de tempo por divisao ("50 us/div"), para a tela.
const char* scope_timebase_text();

// ============================================================================
// MEDIDOR DE RIPPLE
// ============================================================================
// Mede a ondulacao AC que anda em cima de um trilho DC. E o teste que
// denuncia capacitor de filtro ressecado numa fonte - o sintoma aparece no
// ripple muito antes de a tensao media sair da faixa.

struct RippleResult {
    float dcVolts;          // nivel medio do trilho
    float rippleVpp;        // ondulacao pico a pico
    float rippleRms;
    float ripplePercent;    // ondulacao em relacao ao nivel DC
    float frequencyHz;      // 100/120 Hz = rede; kHz = fonte chaveada
    bool  valid;
    char  verdict[40];      // avaliacao legivel
};

// Mede o ripple na ponta 2, com o capacitor de acoplamento inserido.
// Exige a placa de expansao.
RippleResult ripple_measure();

// Verdadeiro se o hardware de acoplamento AC esta disponivel.
bool ripple_available();

// ============================================================================
// TRACADOR DE CURVA I-V
// ============================================================================
// Varre a tensao aplicada e mede a corrente resultante, ponto a ponto.
// Revela o joelho do diodo, a saturacao do transistor e a regiao de
// conducao do LED - coisas que um numero isolado nao mostra.

struct CurvePoint {
    float volts;
    float amps;
};

struct CurveTrace {
    CurvePoint points[CURVE_STEPS];
    uint8_t    count;
    float      maxVolts;
    float      maxAmps;
    bool       valid;
    char       interpretation[48];   // o que a curva sugere
};

// Executa uma varredura completa. Leva cerca de 1 segundo.
bool curve_trace(CurveTrace* out);

// Verdadeiro se o shunt do tracador esta disponivel.
bool curve_available();

// ============================================================================
// GERADOR DE SINAL
// ============================================================================
// Onda quadrada com frequencia e ciclo de trabalho ajustaveis, saindo pela
// ponta 1. Serve para injetar sinal em estagio de audio, testar entrada
// logica e alimentar um circuito de clock.
//
// Onda senoidal exigiria um DAC. Os dois DACs do ESP32 (GPIO25 e GPIO26)
// estao ocupados pelo clock do touch e pelo buzzer, e nao ha pino livre para
// remaneja-los. Quadrada e o que cabe nesta placa.

// Liga o gerador. A saida so chega na ponta se a placa de expansao estiver
// presente para fechar o caminho.
bool siggen_start(uint32_t frequencyHz, uint8_t dutyPercent);

// Desliga e devolve o pino a excitacao das pontas.
void siggen_stop();

bool     siggen_active();
uint32_t siggen_frequency();
uint8_t  siggen_duty();

// Ajusta em tempo real, sem parar a saida.
bool siggen_set_frequency(uint32_t hz);
bool siggen_set_duty(uint8_t percent);

// Frequencia efetivamente gerada. O LEDC nao consegue qualquer valor: ele
// divide o clock base, entao 1 kHz pedido pode virar 1000,2 Hz.
float siggen_actual_frequency();

// ============================================================================
// TESTE DE ZENER (usa a fonte auxiliar de 12 V)
// ============================================================================

struct ZenerResult {
    float zenerVolts;      // tensao de joelho medida
    float testCurrentMa;
    bool  valid;
    char  detail[40];
};

// Polariza o Zener em reverso pela fonte de 12 V e mede o joelho.
// Exige a placa de expansao com o conversor boost.
ZenerResult zener_measure();
bool zener_available();

#endif // SCOPE_H
