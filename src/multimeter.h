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

// ============================================================================
// PINOS E CONFIGURAÇÕES DE HARDWARE
// ============================================================================

// Pinos do ADC (ESP32)
#define MULTIMETER_ADC_PIN       34      // ZMPT101B conectado aqui
#define MULTIMETER_ADC_CHANNEL    ADC1_CHANNEL_6

// Pinos de controle
#define MULTIMETER_BUZZER_PIN     26      // Buzzer para alertas
#define MULTIMETER_LED_GREEN      16      // LED verde (OK)
#define MULTIMETER_LED_RED         17      // LED vermelho (alerta)

// Pinos do relé de medição (opcional)
#define MULTIMETER_RELAY_PIN      27      // Relé para isolar

// ============================================================================
// CONSTANTES DO ZMPT101B (Transformador de Tensão)
// ============================================================================

#define ZMPT_SAMPLES_PER_CYCLE   64      // Amostras por ciclo (60Hz)
#define ZMPT_ZERO_VOLTAGE        2048    // Zero ideal do ADC (12-bit)
#define ZMPT_VOLTS_PER_COUNT      0.146f  // Fator de conversão (220V / 1507 contagens)

// ============================================================================
// CONSTANTES DO INA219 (Sensor de Corrente/Tensão)
// ============================================================================

#define INA219_I2C_ADDR          0x40    // Endereço I2C padrão
#define INA219_SHUNT_OHMS        0.1f    // Resistência do shunt
#define INA219_MAX_AMPS          3.2f    // Corrente máxima
#define INA219_MAX_VOLTS        26.0f   // Tensão máxima (32V - 0.4V)
#ifndef INA219_CALIBRATION
#define INA219_CALIBRATION        4096    // Valor de calibração
#endif

// ============================================================================
// CONSTANTES DE MEDIÇÃO
// ============================================================================

// Faixas de tensão DC
#define DC_VOLTAGE_RANGE_LOW      0.0f    // 0-5V
#define DC_VOLTAGE_RANGE_MED     5.0f    // 0-15V  
#define DC_VOLTAGE_RANGE_HIGH    15.0f   // 0-26V

// Faixas de tensão AC
#define AC_VOLTAGE_RANGE_110    110.0f  // 110V nominal
#define AC_VOLTAGE_RANGE_220   220.0f  // 220V nominal
#define AC_VOLTAGE_MAX        250.0f  // Máximo aceito (com margem)

// Faixas de corrente
#define CURRENT_RANGE_MA       1000.0f // 0-1A
#define CURRENT_RANGE_A         3.2f    // 0-3.2A

// Resistência
// Limiares de segurança (já definidos em config.h se necessário)
// #define HIGH_VOLTAGE_THRESHOLD 50.0f   // Alerta de tensão alta (AC)

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
// HISTÓRICO DE MEDIÇÕES
// ============================================================================

#define HISTORY_SIZE              20      // Número de medições salvas

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

// Atualiza display (chamar periodicamente)
void multimeter_update_display();

// Desenha tela completa do multímetro
void multimeter_draw(const MultimeterReading* reading);

// Desenha leitura grande (valor principal)
void multimeter_draw_value(float value, const char* unit, uint16_t color);

// Desenha indicadores de modo e range
void multimeter_draw_indicators(MultimeterMode mode, MeasurementRange range);

// Desenha alerta de segurança
void multimeter_draw_alert(const char* message, uint16_t color);

// Desenha histórico (gráfico ou lista)
void multimeter_draw_history(const MeasurementHistory* history);

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

// Detecta tipo de tensão (110V ou 220V)
bool multimeter_detect_voltage_type(float voltage);

#endif // MULTIMETER_H