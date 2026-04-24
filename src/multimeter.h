// ============================================================================
// Component Tester PRO v3.0 — Módulo Multímetro AC/DC (CYD Edition)
// ============================================================================
// NOVO módulo para medição de tensão AC (ZMPT101B) e corrente/tensão DC (INA219)
// ============================================================================
#ifndef MULTIMETER_H
#define MULTIMETER_H

#include <Arduino.h>

// ============================================================================
// ESTADOS INTERNOS DO MULTÍMETRO
// ============================================================================
enum MultimeterMode {
  MULTI_AC_VOLTAGE,   // Tensão AC via ZMPT101B (True RMS)
  MULTI_DC_VOLTAGE,   // Tensão DC via INA219 (bus voltage)
  MULTI_DC_CURRENT,   // Corrente DC via INA219 (shunt current)
  MULTI_DC_POWER,     // Potência DC via INA219 (P = V * I)
  MULTI_MODE_COUNT    // Número total de modos
};

// ============================================================================
// PROTÓTIPOS
// ============================================================================
// Inicializa o barramento I2C e o INA219
void multimeter_init();

// Desenha o menu de seleção do modo multímetro
void draw_multimeter_menu();

// Handler principal do modo multímetro (chamado no loop)
void multimeter_handle();

// Funções individuais de medição (bloqueantes, com UI)
void measure_ac_voltage();    // ZMPT101B — True RMS
void measure_dc_voltage();    // INA219 — Bus voltage
void measure_dc_current();    // INA219 — Shunt current
void measure_dc_power();      // INA219 — Power (V * I)

#endif // MULTIMETER_H
