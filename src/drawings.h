// ============================================================================
// Component Tester PRO v3.0 — Funções Gráficas (CYD Edition)
// ============================================================================
#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================================
// INSTÂNCIA GLOBAL DO TFT (definida em main.cpp)
// ============================================================================
extern TFT_eSPI tft;

// ============================================================================
// FUNÇÕES DE UI MODERNAS
// ============================================================================
// Barra de status superior (versão, temperatura, ícone SD)
void draw_status_bar();

// Rodapé com dicas de navegação
void draw_footer_modern();

// Item do grid de menu (card com ícone e label)
void draw_grid_item(int x, int y, const char* label, int iconId, bool selected);

// Card moderno com título e borda colorida
void draw_modern_card(const char* title, uint16_t color);

// ============================================================================
// ÍCONES LEGADOS (retro-compatibilidade)
// ============================================================================
void draw_capacitor_icon(int x, int y);
void draw_resistor_icon(int x, int y);
void draw_diode_icon(int x, int y);
void draw_led_icon(int x, int y, uint16_t color);
void draw_transistor_icon(int x, int y);
void draw_inductor_icon(int x, int y);

// ============================================================================
// ÍCONES DINÂMICOS DE COMPONENTES (sistema de ícones vetoriais)
// ============================================================================
// Tipo extra para capacitor eletrolítico (visual diferenciado)
#define CAT_CAP_ELECTRO 100

// Desenha o ícone de um componente baseado no seu tipo (categoria do database)
void drawComponentIcon(uint8_t componentType, int x, int y, uint16_t color);

// Desenha ícone + label textual abaixo
void drawComponentWithLabel(uint8_t componentType, int x, int y, uint16_t color, const char* label);

// ============================================================================
// ÍCONES DO MENU PRINCIPAL
// ============================================================================
void draw_menu_icon(int x, int y, int iconId, uint16_t color);

#endif // DRAWINGS_H
