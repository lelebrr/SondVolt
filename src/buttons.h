// ============================================================================
// Component Tester PRO v3.0 — Botões e Touchscreen (CYD Edition)
// ============================================================================
#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"
#include <Arduino.h>

// ============================================================================
// PROTÓTIPOS — Navegação unificada (touch + botões físicos opcionais)
// ============================================================================
void buttons_init();      // Inicializa touch (e botões físicos se conectados)
void buttons_update();    // Atualiza estado do touch e botões

// Funções de query de navegação (retornam true se ação detectada)
bool isUpPressed();
bool isDownPressed();
bool isLeftPressed();
bool isRightPressed();
bool isOkPressed();
bool isBackPressed();

// Calibração do Touchscreen
void calibrate_touch();

#endif // BUTTONS_H
