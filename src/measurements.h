// ============================================================================
// Sondvolt v3.0 — Medições de Componentes (Header)
// ============================================================================
#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino.h>
#include <stdint.h>
#include <TFT_eSPI.h>

// ============================================================================
// CONSTANTES
// ============================================================================

// Modos de medição
#define MODE_RESISTOR    0
#define MODE_CAPACITOR  1
#define MODE_DIODE     2
#define MODE_TRANSISTOR 3
#define MODE_INDUCTOR 4
#define MODE_AC       5
#define MODE_GENERIC 99

// Tipos de componentes
#define COMP_RESISTOR   0
#define COMP_CAPACITOR  1
#define COMP_DIODE      2
#define COMP_LED        3
#define COMP_TRANSISTOR 4
#define COMP_INDUCTOR   5
#define COMP_OPEN      98
#define COMP_SHORT     99
#define COMP_GENERIC   100

// Status de componente
#define STATUS_GOOD    0
#define STATUS_SUSPECT 1
#define STATUS_BAD   2
#define STATUS_OPEN 3
#define STATUS_SHORT 4

// Limiares
#define THRESHOLD_SHORT    1.0f        // 1Ω
#define THRESHOLD_OPEN   10000000.0f  // 10MΩ

// ============================================================================
// ESTRUTURA DE RESULTADO
// ============================================================================

typedef struct {
    float value;
    const char* unit;
    const char* name;
    uint8_t type;
    uint8_t status;
    float temp;
    char valueStr[24];
} ComponentResult;

// Variáveis externas
extern float probeOffsetR;
extern float probeOffsetC;
extern uint8_t currentAppState;
extern unsigned long lastActivityMs;

// ============================================================================
// FUNÇÕES DE MEDIÇÃO
// ============================================================================

// Leitura ADC rápida (inline)
inline uint16_t read_adc_fast();
inline uint16_t read_adc_filtered();
inline uint16_t read_adc_avg(uint8_t n);

// Conversão ADC
inline float adc_to_v(uint16_t raw);
inline float v_to_r(float vout, float rref, float vin);

// Medição de resistência
float measure_resistance_raw();
float measure_resistance();
float measure_resistance_low();

// Medição de capacitância
float measure_capacitance();

// Medição de diodo
float measure_diode_vf();

// Medição de tensão AC (ZMPT)
float measure_ac_voltage();

// Auto-detecção
uint8_t auto_detect();

// Teste de continuidade
bool test_continuity();

// Formatador de valor
void format_value(float value, const char* unit, char* out, uint8_t maxLen);

// ============================================================================
// UI / CONTROLE
// ============================================================================

void measurement_start(uint8_t mode);
void measurement_stop();
void measurement_draw();
void measurement_update();
void measurements_loop();

// Banco de dados de componentes
uint8_t db_judge(float value, uint8_t type);

#endif