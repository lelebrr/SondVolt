#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "globals.h"

// Declaração da instância do TFT_eSPI (definida em main.cpp)
extern TFT_eSPI tft;

// Estrutura para um item de menu
typedef struct {
    const char* text;
    AppState targetState; // Estado para onde o menu leva
} MenuItem;

// Funções para manipulação do menu
void menu_init();
void menu_handle();
void draw_menu();
void draw_footer();

#endif // MENU_H
