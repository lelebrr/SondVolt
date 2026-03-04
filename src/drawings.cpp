#include "drawings.h"

// Função para desenhar um ícone pixel art (RGB565)
void draw_pixel_art_icon(int x, int y, int width, int height, const uint16_t* bitmap) {
    tft.pushImage(x, y, width, height, bitmap);
}

// Função para exibir a splash screen (já implementada em main.cpp, mas pode ser movida para cá)
void draw_splash_screen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println(F("Component Tester PRO v2.0"));
    tft.setTextSize(1);
    tft.setCursor(10, 30);
    tft.println(F("- Leandro -"));
}
