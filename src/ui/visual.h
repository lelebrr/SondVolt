// ============================================================================
// Sondvolt v5.0 - Ponte entre o codigo antigo e o sistema de design
// ============================================================================
// Arquivo : visual.h
//
// Ate a v4.0 este arquivo definia a paleta inteira, e theme.h definia OUTRA
// paleta com os mesmos nomes e cores diferentes. Qual delas valia dependia da
// ordem dos includes de cada .cpp - a mesma cor aparecia diferente em telas
// diferentes, e o aviso de redefinicao ficava escondido pela flag -w.
//
// Agora o dono da aparencia e theme.h, e este arquivo apenas traduz os nomes
// V_* usados pelas telas existentes para os tokens novos. Codigo novo deve
// usar theme.h direto.
// ============================================================================

#ifndef VISUAL_H
#define VISUAL_H

#include <Arduino.h>
#include "theme.h"

// ----------------------------------------------------------------------------
// Superficies
// ----------------------------------------------------------------------------
#define V_BG_DARK          th_bg0
#define V_BG_SURFACE       th_bg1
#define V_BG_HIGHLIGHT     th_bg2
#define V_DIVIDER          th_bg3

// ----------------------------------------------------------------------------
// Texto
// ----------------------------------------------------------------------------
#define V_TEXT_MAIN        th_txHi
#define V_TEXT_SUB         th_txLo
#define V_PURE_WHITE       th_txHi

// ----------------------------------------------------------------------------
// Acento e semantica
// ----------------------------------------------------------------------------
#define V_CYAN_ELECTRIC    th_accent
#define V_NEON_GREEN       TH_SUCCESS
#define V_VIBRANT_PURPLE   TH_SPECIAL
#define V_DEEP_BLUE        TH_INFO

#define V_SUCCESS          TH_SUCCESS
#define V_ALERT            TH_DANGER
#define V_WARNING          TH_WARNING
#define V_INFO             TH_INFO

// ----------------------------------------------------------------------------
// Medidas
// ----------------------------------------------------------------------------
#define V_RADIUS_LG        TH_RADIUS_LG
#define V_RADIUS_MD        TH_RADIUS_MD
#define V_RADIUS_SM        TH_RADIUS_SM
#define V_HEADER_H         TH_HEADER_H
#define V_STATUS_H         TH_STATUSBAR_H

// ----------------------------------------------------------------------------
// Nomes THEME_* do codigo antigo
// ----------------------------------------------------------------------------
#define THEME_CYAN          th_accent
#define THEME_NEON_GREEN    TH_SUCCESS
#define THEME_DARK_BACK     th_bg0
#define THEME_SURFACE       th_bg1
#define THEME_SURFACE_LIGHT th_bg2
#define THEME_DIVIDER       th_bg3
#define THEME_TEXT_HIGH     th_txHi
#define THEME_TEXT_MID      th_txMd
#define THEME_TEXT_LOW      th_txLo
#define THEME_SUCCESS       TH_SUCCESS
#define THEME_ERROR         TH_DANGER
#define THEME_WARNING       TH_WARNING

// ----------------------------------------------------------------------------
// Mistura de cor (nome antigo de th_mix)
// ----------------------------------------------------------------------------
inline uint16_t color_mix(uint16_t c1, uint16_t c2, uint8_t alpha) {
    return th_mix(c1, c2, alpha);
}

#endif // VISUAL_H
