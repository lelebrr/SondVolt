// ============================================================================
// Sondvolt v4.0 - Tokens de Estilo
// ============================================================================
// Arquivo : theme.h
//
// Este arquivo definia as mesmas macros THEME_* que visual.h, com VALORES
// DIFERENTES. Qual delas valia dependia da ordem dos includes de cada .cpp,
// entao a mesma cor aparecia diferente em telas diferentes. O aviso de
// redefinicao existia desde sempre, mas a flag -w do platformio.ini o
// escondia.
//
// Agora a paleta tem um dono unico: visual.h. Aqui ficam apenas as medidas
// de layout, que visual.h nao define.
// ============================================================================

#ifndef THEME_H
#define THEME_H

#include "visual.h"

// Espacamentos e raios (em pixels)
#define THEME_RADIUS        V_RADIUS_MD
#define THEME_MARGIN        10
#define THEME_HEADER_H      V_HEADER_H
#define THEME_FOOTER_H      25

// Escalas de fonte da fonte bitmap 5x7
#define FONT_SIZE_SMALL     1
#define FONT_SIZE_MEDIUM    2
#define FONT_SIZE_LARGE     3
#define FONT_SIZE_HUGE      5

// Cores auxiliares que visual.h nao cobre
#define THEME_MEASURING     V_DEEP_BLUE
#define THEME_THERMAL       V_VIBRANT_PURPLE
#define THEME_SHADOW_COLOR  0x0000
#define THEME_HIGHLIGHT     0xFFFF

#endif // THEME_H
