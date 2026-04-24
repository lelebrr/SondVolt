// ============================================================================
// Sondvolt v3.0 — Botões e Touchscreen
// Descrição: Sistema de entrada unificado (touch XPT2046 + botões físicos opcionais)
// ============================================================================
#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"
#include <stdint.h>

// ============================================================================
// ENTRADA DO TOUCH
// ============================================================================
// Estrutura de ponto capturada do touchscreen
typedef struct {
    int16_t x;  // Coordenada X (já mapeada para a tela)
    int16_t y;  // Coordenada Y (já mapeada para a tela)
    int16_t z;  // Pressão Z (0 = sem toque)
} TouchPoint;

// ============================================================================
// BOTÕES FÍSICOS (opcionais — conectores da CYD)
// A CYD não tem botões físicos soldados de fábrica.
// Se você soldou botões nos GPIOs livres, defina-os aqui.
// Caso contrário, use apenas o touchscreen.
// ============================================================================
#define HAS_PHYSICAL_BUTTONS 0   // Mude para 1 se soldou botões

#if HAS_PHYSICAL_BUTTONS
#define BTN_UP     0      // Botão "para cima"
#define BTN_DOWN   35     // Botão "para baixo"
#define BTN_LEFT   2     // Botão "esquerda"
#define BTN_RIGHT  26     // Botão "direita"
#define BTN_OK     32    // Botão "OK / CENTER"
#define BTN_BACK   33    // Botão "voltar"
#endif

// Estados dos botões (para detecção de borda)
typedef enum {
    BTN_STATE_IDLE = 0,   // Repouso (não pressionado)
    BTN_STATE_PRESSED = 1, // Pressionado (borda de descida)
    BTN_STATE_HOLD = 2     // Segurando (após debounce)
} ButtonState;

// ============================================================================
// PROTÓTIPOS
// ============================================================================

// Inicialização do sistema de entrada
void buttons_init();

// Atualização dos estados (chamar no loop principal)
void buttons_update();

// Estado do touchscreen
bool touch_is_pressed();
TouchPoint touch_get_point();
TouchPoint touch_get_raw_point();

// Query de navegação (true = ação detectada neste frame)
bool btn_just_pressed(int btnId);
bool btn_is_pressed(int btnId);
bool btn_long_pressed(int btnId);
bool btn_long_pressed_any();

// Constantes dos botões
#define BTN_UP     0
#define BTN_DOWN   1
#define BTN_LEFT   2
#define BTN_RIGHT  3
#define BTN_CENTER 4
#define BTN_OK     4   // Alias (OK = CENTER)
#define BTN_BACK   5
#define BTN_TOUCH  6   // Toque na tela
#define BTN_TOTAL  7

// Tempo de debounce (ms)
#define DEBOUNCE_MS  50

// Tempo para "long press" (ms)
#define LONG_PRESS_MS 800

#endif // BUTTONS_H