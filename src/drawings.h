#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

// Declaração da instância (definida em main.cpp)
extern Adafruit_ILI9341 tft;

// Funções de desenho
void draw_pixel_art_icon(int x, int y, int width, int height,
                         const uint16_t *bitmap);
void draw_splash_screen();

#endif // DRAWINGS_H
