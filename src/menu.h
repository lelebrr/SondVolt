#ifndef MENU_H
#define MENU_H

#include "globals.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

// Declaração da instância (definida em main.cpp)
extern Adafruit_ILI9341 tft;

// Estrutura para um item de menu
typedef struct {
  const char *text;
  AppState targetState; // Estado para onde o menu leva
} MenuItem;

// Funções para manipulação do menu
void menu_init();
void menu_handle();
void draw_menu();
void draw_footer();
void draw_settings_menu();
void handle_settings_menu();
void draw_about_screen();

#endif // MENU_H
