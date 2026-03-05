#include "drawings.h"

// Função para desenhar um ícone pixel art (RGB565)
void draw_pixel_art_icon(int x, int y, int width, int height,
                         const uint16_t *bitmap) {
  tft.drawRGBBitmap(x, y, bitmap, width, height);
}

// Função para exibir a splash screen
void draw_splash_screen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Component Tester PRO v2.0"));
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println(F("- Leandro -"));

  // Desenha um ícone simples
  tft.drawRect(100, 50, 40, 40, ILI9341_WHITE);
  tft.drawLine(110, 60, 130, 60, ILI9341_WHITE);
  tft.drawLine(110, 70, 130, 70, ILI9341_WHITE);
  tft.drawLine(110, 80, 130, 80, ILI9341_WHITE);

  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.println(F("Initializing..."));
}

// Função para desenhar ícone de capacitor
void draw_capacitor_icon(int x, int y) {
  tft.fillRect(x, y, 4, 20, ILI9341_CYAN);
  tft.fillRect(x + 6, y, 4, 20, ILI9341_CYAN);
  tft.fillRect(x, y + 8, 10, 4, ILI9341_CYAN);
}

// Função para desenhar ícone de diodo
void draw_diode_icon(int x, int y) {
  tft.drawLine(x, y, x + 10, y + 10, ILI9341_WHITE);
  tft.drawLine(x, y + 20, x + 10, y + 10, ILI9341_WHITE);
  tft.drawLine(x + 10, y + 5, x + 10, y + 15, ILI9341_WHITE);
}

// Função para desenhar ícone de LED
void draw_led_icon(int x, int y, uint16_t color) {
  tft.fillCircle(x + 10, y + 10, 8, color);
  tft.drawLine(x + 18, y + 10, x + 25, y + 5, color);
  tft.drawLine(x + 18, y + 10, x + 25, y + 15, color);
}
