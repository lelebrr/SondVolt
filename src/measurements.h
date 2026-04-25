// ============================================================================
// Sondvolt v3.2 — Medições e Julgamento (Header)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: measurements.h
// Descrição: Protótipos de funções de medição e análise
// ============================================================================

#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino.h>
#include "database.h"

// Modos de Medição
#define MODE_RESISTOR    0
#define MODE_CAPACITOR   1
#define MODE_DIODE       2
#define MODE_TRANSISTOR  3
#define MODE_INDUCTOR    4
#define MODE_AC          5

// Funções do Sistema de Medição
void measurements_init();
void measurements_update();
float measurements_get_last_value();
ComponentStatus measurements_get_last_status();
float measurements_get_raw_resistance();
float measurements_get_raw_capacitance();
void measurements_discharge_capacitor();

// Medições Específicas
float measurements_read_ac_rms();

#endif // MEASUREMENTS_H