// ============================================================================
// Component Tester PRO v3.0 — Funções Gráficas (CYD Edition)
// ============================================================================
// Migrado de Adafruit_ILI9341 para TFT_eSPI.
// As primitivas gráficas (fillRect, drawLine, drawCircle, etc.) são compatíveis.
// Diferenças principais:
//   - TFT_eSPI usa tft.width() e tft.height() da mesma forma
//   - Não existe ILI9341_CYAN etc., usamos nossas constantes UI_COLOR_*
//   - Fontes são ligeiramente diferentes, mas LOAD_GLCD mantém compatibilidade
// ============================================================================

#include <Arduino.h>
#include "drawings.h"
#include "config.h"
#include "globals.h"
#include "database.h"
#include <TFT_eSPI.h>
#include <math.h>
#include <string.h>

// ============================================================================
// BARRA DE STATUS SUPERIOR
// ============================================================================
// Mostra: título do firmware, temperatura atual, ícone do SD Card
void draw_status_bar() {
  tft.fillRect(0, 0, tft.width(), STATUS_BAR_HEIGHT, UI_COLOR_STATUS_BG);
  tft.drawLine(0, STATUS_BAR_HEIGHT - 1, tft.width(), STATUS_BAR_HEIGHT - 1, UI_COLOR_ACCENT);

  tft.setTextColor(UI_COLOR_TEXT);
  tft.setTextSize(1);

  // Título centralizado
  tft.setCursor(tft.width() / 2 - 50, 8);
  tft.print(F("CT PRO v3.0 CYD"));

  // Temperatura com ícone (canto superior direito)
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setCursor(tft.width() - 95, 8);
  tft.print(F("TEMP: "));
  tft.setTextColor(UI_COLOR_TEXT);
  tft.print(currentTemperature, 1);
  tft.print(F("C"));

  // Ícone do SD Card (Premium) — canto extremo direito
  int sdx = tft.width() - 18;
  tft.drawRoundRect(sdx, 6, 12, 16, 2, UI_COLOR_ACCENT);
  tft.drawLine(sdx + 3, 10, sdx + 9, 10, UI_COLOR_ACCENT);
  tft.drawLine(sdx + 3, 13, sdx + 9, 13, UI_COLOR_ACCENT);
}

// ============================================================================
// RODAPÉ MODERNO
// ============================================================================
// Mostra dicas de navegação (touch + botões)
void draw_footer_modern() {
  int y = tft.height() - FOOTER_HEIGHT;
  tft.fillRect(0, y, tft.width(), FOOTER_HEIGHT, UI_COLOR_BG);
  tft.drawLine(0, y, tft.width(), y, UI_COLOR_HILIGHT);

  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setCursor(10, y + 6);
  tft.print(F("[TOUCH] Sel   [OK] Ent   [<] Vol"));
}

// ============================================================================
// ITEM DO GRID DE MENU
// ============================================================================
// Desenha um card do menu principal com ícone e label
void draw_grid_item(int x, int y, const char* label, int iconId, bool selected) {
  int w = 150;
  int h = 54;

  if (selected) {
    // Efeito de brilho externo (borda cyan)
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 10, UI_COLOR_ACCENT);
    tft.fillRoundRect(x, y, w, h, 10, UI_COLOR_HILIGHT);
    tft.setTextColor(UI_COLOR_TEXT);
  } else {
    // Card inativo (navy escuro com borda sutil)
    tft.fillRoundRect(x, y, w, h, 10, UI_COLOR_DARK);
    tft.drawRoundRect(x, y, w, h, 10, UI_COLOR_BORDER);
    tft.setTextColor(UI_COLOR_GREY);
  }

  // Ícone do menu (à esquerda do card)
  draw_menu_icon(x + 15, y + 12, iconId, selected ? UI_COLOR_ACCENT : UI_COLOR_GREY);

  // Label do item (à direita do ícone)
  tft.setCursor(x + 55, y + 22);
  tft.setTextSize(1);
  tft.print(label);
}

// ============================================================================
// ÍCONES DO MENU PRINCIPAL
// ============================================================================
void draw_menu_icon(int x, int y, int iconId, uint16_t color) {
  switch (iconId) {
    case 0: // Medir (Transistor/Probe)
      tft.drawLine(x + 15, y, x + 15, y + 10, color);
      tft.drawLine(x + 5, y + 25, x + 15, y + 10, color);
      tft.drawLine(x + 25, y + 25, x + 15, y + 10, color);
      tft.fillRect(x + 10, y + 8, 10, 4, color);
      break;

    case 1: // Térmica (Termômetro)
      tft.drawCircle(x + 15, y + 20, 5, color);
      tft.drawRoundRect(x + 13, y, 5, 18, 2, color);
      tft.fillCircle(x + 15, y + 20, 3, color);
      break;

    case 2: // Scanner (Lupa)
      tft.drawCircle(x + 12, y + 12, 10, color);
      tft.drawLine(x + 18, y + 18, x + 28, y + 28, color);
      break;

    case 3: // Histórico (Lista)
      tft.drawRect(x + 5, y + 5, 20, 24, color);
      tft.drawLine(x + 8, y + 10, x + 22, y + 10, color);
      tft.drawLine(x + 8, y + 17, x + 22, y + 17, color);
      tft.drawLine(x + 8, y + 24, x + 22, y + 24, color);
      break;

    case 4: // Estatísticas (Gráfico de barras)
      tft.drawLine(x + 5, y + 25, x + 25, y + 25, color);
      tft.drawLine(x + 5, y + 25, x + 5, y + 5, color);
      tft.fillRect(x + 8, y + 15, 4, 10, color);
      tft.fillRect(x + 14, y + 8, 4, 17, color);
      tft.fillRect(x + 20, y + 12, 4, 13, color);
      break;

    case 5: // Settings (Engrenagem)
      tft.drawCircle(x + 15, y + 15, 8, color);
      tft.drawCircle(x + 15, y + 15, 3, color);
      for (int i = 0; i < 8; i++) {
        float angle = i * 45 * 3.14159f / 180.0f;
        int x1 = x + 15 + (int)(cos(angle) * 8);
        int y1 = y + 15 + (int)(sin(angle) * 8);
        int x2 = x + 15 + (int)(cos(angle) * 11);
        int y2 = y + 15 + (int)(sin(angle) * 11);
        tft.drawLine(x1, y1, x2, y2, color);
      }
      break;

    case 6: // Multímetro (Voltímetro — NOVO)
      tft.drawCircle(x + 15, y + 15, 12, color);
      tft.setCursor(x + 12, y + 10);
      tft.setTextColor(color);
      tft.setTextSize(1);
      tft.print(F("V"));
      // Agulha do voltímetro
      tft.drawLine(x + 15, y + 22, x + 22, y + 8, color);
      break;

    case 7: // About (Info)
      tft.drawCircle(x + 15, y + 15, 12, color);
      tft.setCursor(x + 13, y + 8);
      tft.setTextColor(color);
      tft.print(F("i"));
      break;
  }
}

// ============================================================================
// CARD MODERNO (para telas internas)
// ============================================================================
void draw_modern_card(const char* title, uint16_t color) {
  // Sombra suave
  tft.fillRoundRect(13, 43, tft.width() - 26, tft.height() - 76, 12, 0x0005);
  // Card Principal
  tft.fillRoundRect(10, 40, tft.width() - 20, tft.height() - 70, 12, UI_COLOR_BG);
  tft.drawRoundRect(10, 40, tft.width() - 20, tft.height() - 70, 12, color);

  // Header do Card (pill com título)
  int titleLen = strlen(title);
  int pillW = titleLen * 6 + 20; // Largura proporcional ao texto
  if (pillW > tft.width() - 40) pillW = tft.width() - 40;
  tft.fillRoundRect(20, 32, pillW, 18, 4, color);
  tft.setCursor(25, 37);
  tft.setTextColor(UI_COLOR_BG);
  tft.setTextSize(1);
  tft.print(title);
}

// ============================================================================
// ÍCONES LEGADOS (retro-compatibilidade)
// ============================================================================
void draw_capacitor_icon(int x, int y) {
  tft.fillRect(x, y, 4, 20, UI_COLOR_ACCENT);
  tft.fillRect(x + 10, y, 4, 20, UI_COLOR_ACCENT);
}

void draw_resistor_icon(int x, int y) {
  tft.drawRect(x, y + 8, 20, 6, UI_COLOR_ORANGE);
  tft.drawLine(x - 5, y + 11, x, y + 11, UI_COLOR_TEXT);
  tft.drawLine(x + 20, y + 11, x + 25, y + 11, UI_COLOR_TEXT);
}

void draw_diode_icon(int x, int y) {
  tft.fillRect(x, y + 5, 15, 10, UI_COLOR_TEXT);
  tft.fillRect(x + 15, y, 3, 20, UI_COLOR_TEXT);
}

void draw_led_icon(int x, int y, uint16_t color) {
  tft.fillRect(x + 5, y + 5, 10, 10, color);
  tft.drawLine(x + 5, y - 5, x + 10, y - 10, color);
}

void draw_transistor_icon(int x, int y) {
  tft.fillRect(x + 5, y + 5, 10, 10, UI_COLOR_GREY);
  tft.drawLine(x + 10, y, x + 10, y - 5, UI_COLOR_TEXT);
}

void draw_inductor_icon(int x, int y) {
  for (int i = 0; i < 3; i++) {
    tft.drawRect(x + 5 + (i * 8), y + 10, 5, 5, UI_COLOR_ORANGE);
  }
}

// ============================================================================
// SISTEMA DE ÍCONES DINÂMICOS DE COMPONENTES
// ============================================================================
// Desenha um ícone vetorial (~40x40 pixels) representando o tipo de componente
void drawComponentIcon(uint8_t componentType, int x, int y, uint16_t color) {
  switch (componentType) {
    case 10: // Resistor (Zigzag)
      tft.drawLine(x, y + 20, x + 5, y + 20, color);
      tft.drawLine(x + 5, y + 20, x + 10, y + 10, color);
      tft.drawLine(x + 10, y + 10, x + 15, y + 30, color);
      tft.drawLine(x + 15, y + 30, x + 20, y + 10, color);
      tft.drawLine(x + 20, y + 10, x + 25, y + 30, color);
      tft.drawLine(x + 25, y + 30, x + 30, y + 20, color);
      tft.drawLine(x + 30, y + 20, x + 35, y + 20, color);
      break;

    case 9: // Capacitor Cerâmico
      tft.drawLine(x + 5, y + 20, x + 15, y + 20, color);
      tft.drawLine(x + 15, y + 5, x + 15, y + 35, color);
      tft.drawLine(x + 20, y + 5, x + 20, y + 35, color);
      tft.drawLine(x + 20, y + 20, x + 30, y + 20, color);
      break;

    case 100: // Capacitor Eletrolítico (CAT_CAP_ELECTRO)
      tft.drawRect(x + 12, y + 5, 12, 25, color);
      tft.fillRect(x + 12, y + 5, 12, 5, color);
      tft.drawLine(x + 18, y, x + 18, y + 5, color);
      tft.drawLine(x + 18, y + 30, x + 18, y + 35, color);
      tft.setTextSize(1);
      tft.setTextColor(color);
      tft.setCursor(x + 26, y + 5);  tft.print("+");
      tft.setCursor(x + 26, y + 25); tft.print("-");
      break;

    case 5: // Diodo
    case 7: // Schottky
      tft.drawTriangle(x + 10, y + 10, x + 10, y + 30, x + 25, y + 20, color);
      tft.drawLine(x + 25, y + 10, x + 25, y + 30, color);
      tft.drawLine(x + 5, y + 20, x + 10, y + 20, color);
      tft.drawLine(x + 25, y + 20, x + 30, y + 20, color);
      break;

    case 6: // Zener
      tft.drawTriangle(x + 10, y + 10, x + 10, y + 30, x + 25, y + 20, color);
      tft.drawLine(x + 25, y + 10, x + 25, y + 30, color);
      tft.drawLine(x + 25, y + 10, x + 30, y + 5, color);
      tft.drawLine(x + 25, y + 30, x + 20, y + 35, color);
      tft.drawLine(x + 5, y + 20, x + 10, y + 20, color);
      tft.drawLine(x + 25, y + 20, x + 30, y + 20, color);
      break;

    case 8: // LED
      tft.drawCircle(x + 18, y + 20, 15, color);
      tft.drawTriangle(x + 12, y + 15, x + 12, y + 25, x + 22, y + 20, color);
      tft.drawLine(x + 22, y + 15, x + 22, y + 25, color);
      // Setas de luz
      tft.drawLine(x + 25, y + 10, x + 35, y, color);
      tft.drawLine(x + 35, y, x + 30, y, color);
      tft.drawLine(x + 35, y, x + 35, y + 5, color);
      break;

    case 1: // NPN
      tft.drawCircle(x + 20, y + 20, 18, color);
      tft.drawLine(x + 12, y + 10, x + 12, y + 30, color);
      tft.drawLine(x + 5, y + 20, x + 12, y + 20, color);
      tft.drawLine(x + 12, y + 15, x + 25, y + 5, color);
      tft.drawLine(x + 12, y + 25, x + 25, y + 35, color);
      tft.drawTriangle(x + 22, y + 32, x + 28, y + 38, x + 20, y + 38, color);
      break;

    case 2: // PNP
      tft.drawCircle(x + 20, y + 20, 18, color);
      tft.drawLine(x + 12, y + 10, x + 12, y + 30, color);
      tft.drawLine(x + 5, y + 20, x + 12, y + 20, color);
      tft.drawLine(x + 12, y + 15, x + 25, y + 5, color);
      tft.drawLine(x + 12, y + 25, x + 25, y + 35, color);
      tft.drawTriangle(x + 13, y + 26, x + 18, y + 21, x + 18, y + 31, color);
      break;

    case 3: // MOSFET N
    case 4: // MOSFET P
      tft.drawCircle(x + 20, y + 20, 18, color);
      tft.drawLine(x + 12, y + 10, x + 12, y + 30, color);
      tft.drawLine(x + 15, y + 10, x + 15, y + 15, color);
      tft.drawLine(x + 15, y + 18, x + 15, y + 22, color);
      tft.drawLine(x + 15, y + 25, x + 15, y + 30, color);
      tft.drawLine(x + 5, y + 20, x + 12, y + 20, color);
      tft.drawLine(x + 15, y + 12, x + 25, y + 12, color);
      tft.drawLine(x + 15, y + 28, x + 25, y + 28, color);
      break;
    
    case 12: // Optoacoplador (CAT_OPTOCOUPLER)
      tft.drawRect(x + 5, y + 5, 30, 30, color);
      tft.drawCircle(x + 13, y + 15, 4, color); // Lado LED
      tft.drawLine(x + 17, y + 15, x + 25, y + 15, color); // Seta de acoplamento
      tft.drawLine(x + 22, y + 12, x + 25, y + 15, color);
      tft.drawLine(x + 22, y + 18, x + 25, y + 15, color);
      tft.drawLine(x + 28, y + 10, x + 28, y + 30, color); // Lado Transistor
      break;

    case 11: // Indutor
      for (int i = 0; i < 4; i++) {
        tft.drawCircle(x + 8 + (i * 8), y + 20, 5, color);
      }
      tft.fillRect(x, y + 20, 40, 10, UI_COLOR_BG);
      break;

    case 14: // Potenciômetro
      tft.drawRect(x + 5, y + 15, 25, 10, color);
      tft.drawLine(x, y + 20, x + 5, y + 20, color);
      tft.drawLine(x + 30, y + 20, x + 35, y + 20, color);
      tft.drawLine(x + 17, y + 5, x + 17, y + 15, color);
      tft.drawTriangle(x + 14, y + 12, x + 20, y + 12, x + 17, y + 16, color);
      break;

    case 13: // Cristal
      tft.drawRect(x + 10, y + 10, 20, 20, color);
      tft.drawLine(x + 5, y + 10, x + 5, y + 30, color);
      tft.drawLine(x + 35, y + 10, x + 35, y + 30, color);
      tft.drawLine(x, y + 20, x + 5, y + 20, color);
      tft.drawLine(x + 35, y + 20, x + 40, y + 20, color);
      break;

    case 15: // Fusível
      tft.drawRect(x + 5, y + 15, 30, 10, color);
      tft.drawLine(x + 5, y + 20, x + 35, y + 20, color);
      tft.drawLine(x + 15, y + 15, x + 15, y + 25, color);
      tft.drawLine(x, y + 20, x + 5, y + 20, color);
      tft.drawLine(x + 35, y + 20, x + 40, y + 20, color);
      break;

    case 16: // Varistor
      tft.drawRect(x + 5, y + 15, 25, 10, color);
      tft.drawLine(x, y + 35, x + 35, y + 5, color);
      tft.drawLine(x, y + 20, x + 5, y + 20, color);
      tft.drawLine(x + 30, y + 20, x + 35, y + 20, color);
      break;

    case 18: // TRIAC
    case 19: // SCR
      tft.drawTriangle(x + 10, y + 10, x + 10, y + 30, x + 25, y + 20, color);
      tft.drawLine(x + 25, y + 10, x + 25, y + 30, color);
      tft.drawLine(x + 20, y + 24, x + 30, y + 34, color);
      tft.drawLine(x + 5, y + 20, x + 10, y + 20, color);
      tft.drawLine(x + 25, y + 20, x + 30, y + 20, color);
      break;

    default: // Componente desconhecido
      tft.drawRect(x + 5, y + 5, 30, 30, color);
      tft.setCursor(x + 15, y + 12);
      tft.setTextSize(3);
      tft.setTextColor(color);
      tft.print("?");
      break;
  }
}

// ============================================================================
// ÍCONE COM LABEL
// ============================================================================
void drawComponentWithLabel(uint8_t componentType, int x, int y, uint16_t color, const char* label) {
  drawComponentIcon(componentType, x, y, color);
  tft.setCursor(x - 10, y + 45);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setTextSize(1);
  tft.print(label);
}
