// ============================================================================
// Sondvolt v3.2 — Sistema de Menu e Navegação (Header)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: menu.h
// Descrição: Definições de estados de navegação e protótipos de interface
// ============================================================================

#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "globals.h" // Para IconType e AppState
#include "graphics.h"

struct MenuCard {
    const char* label;
    AppState targetState;
    IconType icon;
    uint16_t color;
};

// Estados da Interface
enum UIState {
    UI_STATE_SPLASH,
    UI_STATE_MAIN_MENU,
    UI_STATE_COMPONENT_TESTER,
    UI_STATE_MULTIMETER,
    UI_STATE_THERMAL,
    UI_STATE_LOGS,
    UI_STATE_CALIBRATION,
    UI_STATE_ABOUT
};

// Estrutura de Botões do Menu
struct MenuButton {
    int16_t x, y, w, h;
    const char* label;
    uint16_t color;
    void (*callback)();
};

// Funções do Sistema de Menu
void menu_init();
void menu_handle_touch(int16_t x, int16_t y);
void menu_draw();
void menu_handle();
void menu_refresh();
void menu_scroll(int8_t direction);

// Sub-telas de renderização
void draw_calibration_screen();

#endif // MENU_H