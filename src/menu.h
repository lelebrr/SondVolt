// ============================================================================
// Component Tester PRO v3.0 — Menu Principal (CYD Edition)
// ============================================================================
#ifndef MENU_H
#define MENU_H

#include "globals.h"
#include <TFT_eSPI.h>
#include <Arduino.h>

extern TFT_eSPI tft;

// ============================================================================
// ESTRUTURA DE ITEM DO MENU
// ============================================================================
typedef struct {
  const char *text;         // Texto do item
  AppState targetState;     // Estado alvo ao selecionar
} MenuItem;

// ============================================================================
// PROTÓTIPOS
// ============================================================================
void menu_init();
void menu_handle();
void draw_menu();
void draw_footer();
void draw_settings_menu();
void handle_settings_menu();
void draw_settings_menu_with_highlights(int highlighted);
void draw_about_screen();
void draw_history();
void handle_history();
void draw_scanner();
void handle_scanner();
void draw_stats();
void handle_stats();

#endif // MENU_H