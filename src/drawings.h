#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

// Declaração da instância (definida em main.cpp)
extern Adafruit_ILI9341 tft;

// Funções de UI Modernas
void draw_status_bar();
void draw_footer_modern();
void draw_grid_item(int x, int y, const char* label, int iconId, bool selected);
void draw_modern_card(const char* title, uint16_t color);

// Funções de Ícones de Componentes
void draw_capacitor_icon(int x, int y);
void draw_resistor_icon(int x, int y);
void draw_diode_icon(int x, int y);
void draw_led_icon(int x, int y, uint16_t color);
void draw_transistor_icon(int x, int y);
void draw_inductor_icon(int x, int y);

// Funções de Ícones do Menu
void draw_menu_icon(int x, int y, int iconId, uint16_t color);

#endif // DRAWINGS_H
