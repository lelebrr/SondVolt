// ============================================================================
// Sondvolt v4.0 — Sistema Visual Moderno (Header)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================

#ifndef VISUAL_H
#define VISUAL_H

#include <Arduino.h>

// ----------------------------------------------------------------------------
// 1. PALETA DE CORES DINAMICAS (RGB565) - Mapeadas para globals.h
// ----------------------------------------------------------------------------
extern uint16_t clr_back;
extern uint16_t clr_surf;
extern uint16_t clr_text;
extern uint16_t clr_dim;
extern uint16_t clr_primary;

#define V_BG_DARK          clr_back
#define V_BG_SURFACE       clr_surf
#define V_BG_HIGHLIGHT     clr_surf // Usando clr_surf para simplificar

#define V_CYAN_ELECTRIC    clr_primary
#define V_NEON_GREEN       0x3FE2  // #39FF14 - Verde Neon
#define V_VIBRANT_PURPLE   0xB85F  // #BC13FE - Roxo Vibrante
#define V_PURE_WHITE       clr_text
#define V_DEEP_BLUE        0x001F  // #0000FF - Azul Profundo

#define V_SUCCESS          0x3FE2  // Verde Neon para Sucesso
#define V_ALERT            0xF800  // Vermelho para Alerta
#define V_WARNING          0xFFE0  // Amarelo para Atenção
#define V_INFO             clr_primary

#define V_TEXT_MAIN        clr_text
#define V_TEXT_SUB         clr_dim
#define V_DIVIDER          clr_dim

// ----------------------------------------------------------------------------
// 2. COMPATIBILIDADE COM CODIGOS ANTIGOS (THEME_*)
// ----------------------------------------------------------------------------
#define THEME_CYAN          V_CYAN_ELECTRIC
#define THEME_NEON_GREEN    V_NEON_GREEN
#define THEME_DARK_BACK     V_BG_DARK
#define THEME_SURFACE       V_BG_SURFACE
#define THEME_SURFACE_LIGHT V_BG_SURFACE
#define THEME_DIVIDER       V_DIVIDER
#define THEME_TEXT_HIGH     V_PURE_WHITE
#define THEME_TEXT_MID      V_TEXT_MAIN
#define THEME_TEXT_LOW      V_TEXT_SUB
#define THEME_SUCCESS       V_SUCCESS
#define THEME_ERROR         V_ALERT
#define THEME_WARNING       V_WARNING

// ----------------------------------------------------------------------------
// 2. ESTILOS DE UI
// ----------------------------------------------------------------------------
#define V_RADIUS_LG        12      // Raio para cards grandes
#define V_RADIUS_MD        8       // Raio para botões/cards médios
#define V_RADIUS_SM        4       // Raio para pequenas boxes

#define V_HEADER_H         40      // Altura do Header
#define V_STATUS_H         24      // Altura da Barra de Status

// ----------------------------------------------------------------------------
// 3. MACROS DE AJUSTE DE COR (ILUMINAÇÃO/SOMBRA)
// ----------------------------------------------------------------------------
inline uint16_t color_mix(uint16_t c1, uint16_t c2, uint8_t alpha) {
    uint8_t r1 = (c1 >> 11) & 0x1F, g1 = (c1 >> 5) & 0x3F, b1 = c1 & 0x1F;
    uint8_t r2 = (c2 >> 11) & 0x1F, g2 = (c2 >> 5) & 0x3F, b2 = c2 & 0x1F;
    uint8_t r = (r1 * (255 - alpha) + r2 * alpha) >> 8;
    uint8_t g = (g1 * (255 - alpha) + g2 * alpha) >> 8;
    uint8_t b = (b1 * (255 - alpha) + b2 * alpha) >> 8;
    return (r << 11) | (g << 5) | b;
}

#endif // VISUAL_H
