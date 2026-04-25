// ============================================================================
// Sondvolt v3.5 — Sistema de Temas e Estilos
// ============================================================================
#ifndef THEME_H
#define THEME_H

#include <Arduino.h>

// Cores Profissionais (Paleta Premium)
#define THEME_CYAN          0x07FF  // #00FFFF
#define THEME_NEON_GREEN    0x07E0  // #00FF00
#define THEME_DARK_BACK     0x0000  // #000000
#define THEME_SURFACE       0x0841  // #101010 (Cinza profundo)
#define THEME_SURFACE_LIGHT 0x2104  // #202020
#define THEME_DIVIDER       0x3186  // #303030
#define THEME_TEXT_HIGH     0xFFFF  // Branco
#define THEME_TEXT_MID      0xAD55  // Cinza médio
#define THEME_TEXT_LOW      0x632C  // Cinza escuro

// Cores de Status
#define THEME_SUCCESS       0x07E0
#define THEME_ERROR         0xF800
#define THEME_WARNING       0xFDA0
#define THEME_MEASURING     0x001F
#define THEME_THERMAL       0x780F

// Gradientes (Simulados por passos ou usados em funções)
#define THEME_GRAD_START    0x0410
#define THEME_GRAD_END      0x0000

// Tamanhos e Espaçamentos
#define THEME_RADIUS        8
#define THEME_MARGIN        10
#define THEME_HEADER_H      40
#define THEME_FOOTER_H      25

// Fontes (Escalonamento)
#define FONT_SIZE_SMALL     1
#define FONT_SIZE_MEDIUM    2
#define FONT_SIZE_LARGE     3
#define FONT_SIZE_HUGE      5

// Efeitos
#define THEME_SHADOW_COLOR  0x0000
#define THEME_HIGHLIGHT     0xFFFF

#endif // THEME_H
