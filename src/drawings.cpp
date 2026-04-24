#include "drawings.h"
#include "config.h"
#include "globals.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <SD.h>

void draw_status_bar() {
  tft.fillRect(0, 0, tft.width(), STATUS_BAR_HEIGHT, 0x0014); // Very dark navy
  tft.drawLine(0, STATUS_BAR_HEIGHT-1, tft.width(), STATUS_BAR_HEIGHT-1, UI_COLOR_ACCENT);
  
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setTextSize(1);
  
  // Título Centralizado
  tft.setCursor(tft.width()/2 - 40, 8);
  tft.print(F("CT PRO v2.1"));
  
  // Temperatura com ícone
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setCursor(tft.width() - 85, 8);
  tft.print(F("TEMP: "));
  tft.setTextColor(UI_COLOR_TEXT);
  tft.print(currentTemperature, 1);
  tft.print(F("C"));
  
  // Ícone SD (Premium)
  int sdx = tft.width() - 18;
  tft.drawRoundRect(sdx, 6, 12, 16, 2, UI_COLOR_ACCENT);
  tft.drawLine(sdx+3, 10, sdx+9, 10, UI_COLOR_ACCENT);
  tft.drawLine(sdx+3, 13, sdx+9, 13, UI_COLOR_ACCENT);
}

void draw_footer_modern() {
  int y = tft.height() - 20;
  tft.fillRect(0, y, tft.width(), 20, UI_COLOR_BG);
  tft.drawLine(0, y, tft.width(), y, UI_COLOR_HILIGHT);
  
  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setCursor(10, y + 6);
  tft.print(F("[NAV] Sel   [OK] Ent   [<] Vol"));
}

void draw_grid_item(int x, int y, const char* label, int iconId, bool selected) {
  int w = 150;
  int h = 54;
  
  if (selected) {
    // Efeito de brilho externo
    tft.drawRoundRect(x-1, y-1, w+2, h+2, 10, UI_COLOR_ACCENT);
    tft.fillRoundRect(x, y, w, h, 10, UI_COLOR_HILIGHT);
    tft.setTextColor(UI_COLOR_TEXT);
  } else {
    tft.fillRoundRect(x, y, w, h, 10, 0x0008); // Darker navy
    tft.drawRoundRect(x, y, w, h, 10, 0x0841); // Dark grey/blue border
    tft.setTextColor(UI_COLOR_GREY);
  }
  
  draw_menu_icon(x + 15, y + 12, iconId, selected ? UI_COLOR_ACCENT : UI_COLOR_GREY);
  
  tft.setCursor(x + 55, y + 22);
  tft.setTextSize(1);
  tft.print(label);
}

void draw_menu_icon(int x, int y, int iconId, uint16_t color) {
  switch (iconId) {
    case 0: // Medir (Transistor)
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
    case 3: // Histórico (Livro/Lista)
      tft.drawRect(x + 5, y + 5, 20, 24, color);
      tft.drawLine(x + 8, y + 10, x + 22, y + 10, color);
      tft.drawLine(x + 8, y + 17, x + 22, y + 17, color);
      tft.drawLine(x + 8, y + 24, x + 22, y + 24, color);
      break;
    case 4: // Stats (Gráfico)
      tft.drawLine(x + 5, y + 25, x + 25, y + 25, color);
      tft.drawLine(x + 5, y + 25, x + 5, y + 5, color);
      tft.fillRect(x + 8, y + 15, 4, 10, color);
      tft.fillRect(x + 14, y + 8, 4, 17, color);
      tft.fillRect(x + 20, y + 12, 4, 13, color);
      break;
    case 5: // Settings (Engrenagem)
      tft.drawCircle(x + 15, y + 15, 8, color);
      tft.drawCircle(x + 15, y + 15, 3, color);
      for(int i=0; i<8; i++) {
        float angle = i * 45 * 3.14 / 180;
        tft.drawLine(x+15 + cos(angle)*8, y+15 + sin(angle)*8, x+15 + cos(angle)*11, y+15 + sin(angle)*11, color);
      }
      break;
    case 6: // About (Info)
      tft.drawCircle(x + 15, y + 15, 12, color);
      tft.setCursor(x + 13, y + 8);
      tft.setTextColor(color);
      tft.print(F("i"));
      break;
  }
}

void draw_modern_card(const char* title, uint16_t color) {
  // Sombra suave
  tft.fillRoundRect(13, 43, tft.width() - 26, tft.height() - 76, 12, 0x0005);
  // Card Principal
  tft.fillRoundRect(10, 40, tft.width() - 20, tft.height() - 70, 12, UI_COLOR_BG);
  tft.drawRoundRect(10, 40, tft.width() - 20, tft.height() - 70, 12, color);
  
  // Header do Card
  tft.fillRoundRect(20, 32, 100, 18, 4, color);
  tft.setCursor(25, 37);
  tft.setTextColor(UI_COLOR_BG);
  tft.setTextSize(1);
  tft.print(title);
}

// Retro-compatibilidade com ícones antigos se necessário
void draw_capacitor_icon(int x, int y) {
  tft.fillRect(x, y, 4, 20, ILI9341_CYAN);
  tft.fillRect(x + 10, y, 4, 20, ILI9341_CYAN);
}

void draw_resistor_icon(int x, int y) {
  tft.drawRect(x, y + 8, 20, 6, 0x8800); // Maroon
  tft.drawLine(x - 5, y + 11, x, y + 11, ILI9341_WHITE);
  tft.drawLine(x + 20, y + 11, x + 25, y + 11, ILI9341_WHITE);
}

void draw_diode_icon(int x, int y) {
  tft.fillRect(x, y + 5, 15, 10, ILI9341_WHITE);
  tft.fillRect(x + 15, y, 3, 20, ILI9341_WHITE);
}

void draw_led_icon(int x, int y, uint16_t color) {
  tft.fillRect(x + 5, y + 5, 10, 10, color);
  tft.drawLine(x + 5, y - 5, x + 10, y - 10, color);
}

void draw_transistor_icon(int x, int y) {
  tft.fillRect(x + 5, y + 5, 10, 10, 0x7BEF); // Dark grey
  tft.drawLine(x + 10, y, x + 10, y - 5, ILI9341_WHITE);
}

void draw_inductor_icon(int x, int y) {
  for (int i = 0; i < 3; i++) {
    tft.drawRect(x + 5 + (i * 8), y + 10, 5, 5, 0xFD20); // Orange
  }
}
