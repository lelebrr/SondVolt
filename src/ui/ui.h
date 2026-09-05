#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include "globals.h"

// Inicialização e Ciclo de Vida
// ui_boot_begin() desenha o logo e acende o backlight suavemente; deve ser
// chamada ANTES do autoteste, para que ui_boot_progress() possa reportar o
// andamento real de cada verificacao.
void ui_boot_begin();
void ui_boot_progress(uint8_t percent, const char* message);

void ui_init();
void ui_update();

// Toque e Interação
bool ui_handle_touch(uint16_t x, uint16_t y);
void ui_reset_touch_state();

// Calibração
void ui_calibration_update_progress(uint8_t progress, const char* msg);
void ui_calibration_show_result(bool success, const char* msg);

#endif