// ============================================================================
// Sondvolt v3.2 — Gráficos e Interface (Header)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: graphics.h
// Descrição: Protótipos de funções gráficas e ícones vetoriais
// ============================================================================

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "config.h"
#include "types.h"

// Protótipos de Funções Gráficas Gerais
void graphics_draw_header(const char* title);
void graphics_draw_button(int16_t x, int16_t y, int16_t w, int16_t h, const char* label, uint16_t color);
void graphics_draw_splash();
void graphics_draw_back_button();

// Função Central de Ícones
void draw_component_icon(IconType type, int16_t x, int16_t y, uint16_t color);
void draw_bitmap_icon(IconType type, int16_t x, int16_t y);
void draw_logo_full();
void draw_logo_small(int16_t x, int16_t y);

// Protótipos de Ícones Individuais
void draw_icon_resistor(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_capacitor(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_diode(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_led(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_transistor(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_inductor(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_multimeter(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_temp(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_history(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_settings(int16_t x, int16_t y, uint16_t size, uint16_t color);
void draw_icon_about(int16_t x, int16_t y, uint16_t size, uint16_t color);

#endif // GRAPHICS_H