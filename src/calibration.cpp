// ============================================================================
// Sondvolt v4.0 - Calibracao das Pontas de Prova
// ============================================================================
// Arquivo : calibration.cpp
//
// A calibracao mede dois erros sistematicos que nenhum software adivinha:
//   1. A resistencia dos proprios cabos e contatos (tipicamente 0.2 a 2 ohms)
//   2. A capacitancia parasita entre os cabos (tipicamente 20 a 200 pF)
//
// Correcoes desta revisao:
//   - calibration_init() nunca era chamado, entao os offsets gravados na NVS
//     jamais eram carregados: cada boot comecava sem calibracao.
//   - A ordem das instrucoes estava trocada (pedia curto, media, e so depois
//     mandava abrir as pontas), medindo capacitancia com as pontas em curto.
//   - Nao havia validacao: um offset absurdo era salvo e passava a estragar
//     todas as medicoes seguintes.
//   - Nao existia checksum de verdade, so um campo zerado.
// ============================================================================

#include "calibration.h"
#include "analysis.h"
#include "hal.h"
#include "ui.h"
#include "globals.h"
#include "buzzer.h"
#include "config.h"
#include <Preferences.h>

CalibrationData currentCal;
static Preferences gPrefs;

// Limites de sanidade: fora disso a montagem esta errada, nao descalibrada.
static const float kMaxResistanceOffset  = 20.0f;    // ohms
static const float kMaxCapacitanceOffset = 5e-9f;    // 5 nF

// Checksum simples sobre os bytes uteis da estrutura.
static uint32_t calc_checksum(const CalibrationData& c) {
    uint32_t sum = 0x5A5A5A5A;
    const uint8_t* p = (const uint8_t*)&c;
    const size_t n = sizeof(CalibrationData) - sizeof(c.checksum);
    for (size_t i = 0; i < n; i++) {
        sum = (sum << 3) ^ (sum >> 29) ^ p[i];
    }
    return sum;
}

void calibration_reset() {
    currentCal.resistance_offset  = 0.0f;
    currentCal.capacitance_offset = 0.0f;
    currentCal.voltage_scale      = 1.0f;
    currentCal.checksum           = calc_checksum(currentCal);

    analysis_set_offsets(0.0f, 0.0f);
    deviceSettings.calibrated = false;
}

void calibration_load() {
    if (!gPrefs.begin("calib", true)) { calibration_reset(); return; }

    size_t got = gPrefs.getBytes("data", &currentCal, sizeof(CalibrationData));
    gPrefs.end();

    if (got != sizeof(CalibrationData)) {
        LOG_SERIAL_F("[CAL] Nenhuma calibracao gravada");
        calibration_reset();
        return;
    }

    if (currentCal.checksum != calc_checksum(currentCal)) {
        LOG_SERIAL_F("[CAL] Calibracao corrompida, descartada");
        calibration_reset();
        return;
    }

    // Mesmo com checksum valido, recusamos valores fisicamente impossiveis.
    if (currentCal.resistance_offset  < 0.0f ||
        currentCal.resistance_offset  > kMaxResistanceOffset ||
        currentCal.capacitance_offset < 0.0f ||
        currentCal.capacitance_offset > kMaxCapacitanceOffset) {
        LOG_SERIAL_F("[CAL] Offsets fora de faixa, descartados");
        calibration_reset();
        return;
    }

    analysis_set_offsets(currentCal.resistance_offset,
                         currentCal.capacitance_offset);
    deviceSettings.calibrated = true;

    LOG_SERIAL_FMT("[CAL] Offsets carregados: R=%.3f Ohm, C=%.1f pF\n",
                   currentCal.resistance_offset,
                   currentCal.capacitance_offset * 1e12f);
}

void calibration_save() {
    currentCal.checksum = calc_checksum(currentCal);

    if (!gPrefs.begin("calib", false)) return;
    gPrefs.putBytes("data", &currentCal, sizeof(CalibrationData));
    gPrefs.end();

    deviceSettings.calibrated = true;
    LOG_SERIAL_F("[CAL] Calibracao gravada na NVS");
}

void calibration_init() {
    calibration_load();
}

bool calibration_is_valid() {
    return deviceSettings.calibrated &&
           currentCal.checksum == calc_checksum(currentCal);
}

// ============================================================================
// ROTINA AUTOMATICA
// ============================================================================

bool calibration_run_auto() {
    if (!hal_probe_available()) {
        ui_calibration_show_result(false, "Circuito de pontas ausente");
        return false;
    }

    // --- Etapa 1: pontas em CURTO -> resistencia dos cabos -------------------
    ui_calibration_update_progress(5, "Encoste as pontas uma na outra");
    vTaskDelay(pdMS_TO_TICKS(3500));

    ui_calibration_update_progress(20, "Medindo resistencia dos cabos...");

    // Zera o offset atual para medir o valor bruto, senao calibrariamos
    // sobre uma calibracao anterior.
    analysis_set_offsets(0.0f, 0.0f);

    float rSum = 0.0f;
    uint8_t rValid = 0;
    for (uint8_t i = 0; i < 12; i++) {
        float r = analysis_measure_resistance(PROBE_RANGE_LOW);
        if (r >= 0.0f && r < kMaxResistanceOffset) { rSum += r; rValid++; }
        ui_calibration_update_progress(20 + i * 2, "Medindo resistencia...");
        vTaskDelay(pdMS_TO_TICKS(80));
    }

    if (rValid < 6) {
        ui_calibration_show_result(false, "Pontas nao estao em curto");
        calibration_load();          // restaura o que havia antes
        return false;
    }

    float rOffset = rSum / rValid;

    // --- Etapa 2: pontas ABERTAS -> capacitancia parasita --------------------
    ui_calibration_update_progress(50, "Agora AFASTE as pontas");
    buzzer_beep(1200, 120);
    vTaskDelay(pdMS_TO_TICKS(4000));

    ui_calibration_update_progress(65, "Medindo capacitancia dos cabos...");

    float cSum = 0.0f;
    uint8_t cValid = 0;
    for (uint8_t i = 0; i < 8; i++) {
        float c = analysis_measure_capacitance();
        if (c >= 0.0f && c < kMaxCapacitanceOffset) { cSum += c; cValid++; }
        ui_calibration_update_progress(65 + i * 3, "Medindo capacitancia...");
        vTaskDelay(pdMS_TO_TICKS(80));
    }

    float cOffset = (cValid > 0) ? (cSum / cValid) : 0.0f;

    // --- Etapa 3: validacao --------------------------------------------------
    ui_calibration_update_progress(92, "Validando resultados...");
    vTaskDelay(pdMS_TO_TICKS(300));

    if (rOffset > kMaxResistanceOffset) {
        ui_calibration_show_result(false, "Resistencia dos cabos alta demais");
        calibration_load();
        return false;
    }

    // --- Etapa 4: gravacao ---------------------------------------------------
    currentCal.resistance_offset  = rOffset;
    currentCal.capacitance_offset = cOffset;
    currentCal.voltage_scale      = 1.0f;

    analysis_set_offsets(rOffset, cOffset);
    calibration_save();

    ui_calibration_update_progress(100, "Concluido");

    char msg[48];
    snprintf(msg, sizeof(msg), "R %.2f Ohm  C %.0f pF",
             rOffset, cOffset * 1e12f);
    ui_calibration_show_result(true, msg);
    buzzer_completion();
    return true;
}
