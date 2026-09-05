// ============================================================================
// Sondvolt v3.x — Multímetro AC/DC com True RMS
// Descrição: Sistema completo de medição elétrica com ZMPT101B e INA219
// Versão: CYD Edition para ESP32-2432S028R
// ============================================================================
#ifndef MULTIMETER_H
#define MULTIMETER_H

#include <stdint.h>
#include <Arduino.h>
#include "types.h"
#include "config.h"

// ============================================================================
// NOTA SOBRE CONSTANTES
// ============================================================================
// Toda a pinagem vive em pins.h e todos os limites de medicao vivem em
// config.h. Este cabecalho nao redefine nada: a versao anterior duplicava
// dezenas de macros (ZMPT_*, INA219_*, HISTORY_SIZE) com valores diferentes
// dos de config.h, e qual valor prevalecia dependia da ordem dos includes.
// ============================================================================

// Faixas nominais de rede, usadas apenas para rotular a tela.
#define AC_VOLTAGE_RANGE_127    127.0f
#define AC_VOLTAGE_RANGE_220    220.0f

// ============================================================================
// RESULTADO DE MEDICAO
// ============================================================================

struct MultimeterReading {
    float value;                  // Valor medido (RMS)
    float peakValue;              // Valor de pico detectado
    float minValue;               // Valor mínimo (para estatísticas)
    float maxValue;               // Valor máximo
    float peakToPeak;             // Valor pico a pico (para AC)
    const char* unit;             // String da unidade
    const char* unitAbbrev;       // Abreviação da unidade
    MultimeterMode mode;          // Modo atual
    MultimeterState state;        // Estado do multímetro
    MeasurementRange range;       // Range utilizado
    bool valid;                   // Leitura válida
    bool surgeDetected;           // Se houve detecção de surto (pico repentino)
    unsigned long timestamp;      // Timestamp da leitura
    uint16_t statusColor;         // Cor do status (para UI)
};

// ============================================================================
// HISTORICO DE MEDICOES  (HISTORY_SIZE vem de config.h)
// ============================================================================

struct MeasurementHistoryEntry {
    float value;               // Valor medido
    MultimeterMode mode;       // Modo usado
    unsigned long timestamp;   // Quando foi medido
    bool valid;             // Se foi válido
};

struct MeasurementHistory {
    MeasurementHistoryEntry entries[HISTORY_SIZE];
    uint8_t count;          // Número de entradas
    uint8_t index;         // Índice atual (circular)
};

// ============================================================================
// ESTRUTURA DE CONFIGURAÇÃO/ESTADO
// ============================================================================

struct MultimeterConfig {
    // Calibração
    float zmptCalibration;      // Fator de calibração do ZMPT
    float ina219Calibration;  // Fator de calibração do INA219
    float shuntResistance;    // Resistência real do shunt
    
    // Filtros
    uint8_t filterSamples;   // Amostras para filtro (média)
    float filterAlpha;      // Fator alpha do filtro exponencial (0-1)
    
    // Display
    uint16_t updateIntervalMs; // Intervalo de update (200ms)
    
    // Segurança
    bool highVoltageAlert;  // Alerta de tensão alta ativado
    bool shortCircuitAlert; // Alerta de curto ativado
    bool soundEnabled;     // Som ativado
    
    // Range
    MeasurementRange currentRange; // Range atual
};

struct MultimeterStatus {
    bool zmptConnected;    // ZMPT101B conectado?
    bool ina219Connected; // INA219 conectado?
    uint8_t errors;        // Contador de erros
    unsigned long uptimeMs; // Tempo de operação
};

// ============================================================================
// PROTÓTIPOS — INICIALIZAÇÃO E CONTROLE
// ============================================================================

// Inicializa o sistema de multímetro
void multimeter_init(bool calibrate = false);

// Inicializa o ADC para medições
void multimeter_adc_init();

// Inicializa o INA219
bool multimeter_ina219_init();

// Desliga/limpa o sistema
void multimeter_shutdown();

// ============================================================================
// PROTÓTIPOS — LEITURAS
// ============================================================================

// Lê tensão DC (usando INA219 ou ADC)
float multimeter_read_dc_voltage();

// Lê tensão AC com True RMS real (ZMPT101B)
// Esta função roda no core secundário do ESP32
void multimeter_read_ac_voltage_task(void* param);

// Lê corrente DC (INA219)
float multimeter_read_dc_current();

// Lê resistência (método de tensão conhecida)
float multimeter_read_resistance();

// teste de continuidade (curto-circuito)
bool multimeter_test_short();

// Calcula RMS de um array de amostras
float multimeter_calculate_rms(const int16_t* samples, uint16_t count);

// Lê todos os valores (modo atual)
MultimeterReading multimeter_read();

// Versão assíncrona (para usar com FreeRTOS)
void multimeter_read_async_start();
MultimeterReading multimeter_get_last_reading();

// ============================================================================
// PROTÓTIPOS — MODO/RANGE
// ============================================================================

// Define o modo de medição
void multimeter_set_mode(MultimeterMode mode);
MultimeterMode multimeter_get_mode();

// Define o range (auto ou manual)
void multimeter_set_range(MeasurementRange range);
MeasurementRange multimeter_get_range();

// Auto-ranging
void multimeter_auto_range();
MeasurementRange multimeter_suggest_range(float value);

// ============================================================================
// PROTÓTIPOS — CALIBRAÇÃO
// ============================================================================

// Calibra o ZMPT com tensão conhecida
void multimeter_calibrate_zmpt(float realVoltage);

// Calibra o INA219 com tensão conhecida
void multimeter_calibrate_ina219(float realVoltage, float realCurrent);

// Salva calibração na NVS
void multimeter_save_calibration();

// Carrega calibração da NVS
void multimeter_load_calibration();

// Reseta calibração para padrão
void multimeter_reset_calibration();

// ============================================================================
// PROTÓTIPOS — FILTROS
// ============================================================================

// Filtro de média móvel
float multimeter_filter_moving_average(float newValue);

// Filtro exponencial
float multimeter_filter_exponential(float newValue);

// Aplica filtros e retorna valor estabilizado
float multimeter_apply_filters(float rawValue);

// ============================================================================
// PROTÓTIPOS — UI / DISPLAY
// ============================================================================

// Loop principal (chamar no loop do app)
void multimeter_handle();

// ============================================================================
// PROTÓTIPOS — PROTEÇÃO / SEGURANÇA
// ============================================================================

// Verifica se tensão é muito alta e alerta
void multimeter_check_high_voltage(float voltage);

// Verifica curto-circuito e alerta
void multimeter_check_short_circuit(float resistance);

// Ativa alerta sonoro
void multimeter_alert_sound(uint8_t type);

// Ativa alerta visual (LED)
void multimeter_alert_led(uint8_t type);

// Desliga todos os alertas
void multimeter_clear_alerts();

// ============================================================================
// PROTÓTIPOS — HISTÓRICO
// ============================================================================

// Adiciona entrada ao histórico
void multimeter_history_add(float value, MultimeterMode mode);

// Limpa histórico
void multimeter_history_clear();

// Pega histórico completo
MeasurementHistory* multimeter_get_history();

// ============================================================================
// PROTÓTIPOS — UTILITÁRIOS
// ============================================================================

// Formata valor para string
void multimeter_format_value(float value, char* buffer, uint8_t maxLen);

// Detecta se a tensao medida corresponde a uma rede de 220 V.
bool multimeter_detect_voltage_type(float voltage);

// ============================================================================
// NOVIDADES DA v4.0
// ============================================================================

// Le a tensao do barramento pelo proprio INA219 (mais preciso que o ADC).
float multimeter_read_bus_voltage();

// Le a tensao AC eficaz do ZMPT101B com remocao automatica de offset.
float multimeter_read_ac_voltage_rms();

// Verdadeiro se o INA219 respondeu no barramento I2C.
bool multimeter_ina219_present();

// Estado interno do instrumento.
MultimeterState multimeter_get_state();

// Nomes legiveis para a interface.
const char* multimeter_mode_name(MultimeterMode mode);
const char* multimeter_state_name(MultimeterState state);

// Razao do divisor externo de tensao DC (1.0 = medida direta, 11.0 = 10:1).
void  multimeter_set_dc_divider(float ratio);
float multimeter_get_dc_divider();

// Ganho de calibracao do ZMPT101B, em volts por conta de ADC.
float multimeter_get_zmpt_gain();

#endif // MULTIMETER_H