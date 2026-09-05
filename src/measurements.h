// ============================================================================
// Sondvolt v4.0 - Camada de Medicao (Header)
// ============================================================================
// Arquivo : measurements.h
// Objetivo: Interface simples entre a maquina de estados da interface e o
//           motor de analise. Quem precisa de detalhe usa analysis.h.
// ============================================================================

#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino.h>
#include "types.h"
#include "analysis.h"

// Inicializa o motor de analise. Exige hal_init() executado antes.
void measurements_init();

// Descarrega o capacitor conectado, acompanhando a tensao real e cedendo o
// processador entre os passos. Atualiza isDischarging e dischargeProgress.
void measurements_discharge_capacitor();

// Medicoes individuais (delegam para analysis.cpp).
float measurements_get_raw_resistance();
float measurements_get_raw_capacitance();
float measurements_get_esr();
float measurements_get_inductance();
float measurements_read_ac_rms();

// Identificacao automatica continua, chamada pelo laco de medicao.
void measurements_update();

// Ultimo valor e status calculados por measurements_update().
float           measurements_get_last_value();
ComponentStatus measurements_get_last_status();
AnalysisResult  measurements_get_last_result();

// Teste completo sob demanda, com retorno sonoro e visual e contabilizacao
// nas estatisticas de uso.
AnalysisResult measurements_run_full_test();

#endif // MEASUREMENTS_H
