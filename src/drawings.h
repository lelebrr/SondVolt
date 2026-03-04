#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// Declaração da instância do TFT_eSPI (definida em main.cpp)
extern TFT_eSPI tft;

// Funções de desenho
void draw_pixel_art_icon(int x, int y, int width, int height, const uint16_t* bitmap);
void draw_splash_screen();

#endif // DRAWINGS_H
