// ============================================================================
// Sondvolt v3.2 — Sistema de Menu Profissional
// ============================================================================

#include "menu.h"
#include "graphics.h"
#include "buttons.h"
#include "buzzer.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "globals.h"

MenuCard MAIN_MENU[] = {
    { "Automatico", STATE_MEASURE_GENERIC,   ICON_AUTO,          C_GREEN  },
    { "Resistor",  STATE_MEASURE_RESISTOR,  ICON_RESISTOR,      C_ORANGE },
    { "Capacitor", STATE_MEASURE_CAPACITOR, ICON_CAPACITOR,     C_BLUE   },
    { "Diodo",     STATE_MEASURE_DIODE,     ICON_DIODE,         C_RED    },
    { "LED",       STATE_MEASURE_LED,       ICON_LED,           C_GREEN  },
    { "Transistor", STATE_MEASURE_TRANSISTOR,ICON_TRANSISTOR_NPN, C_PURPLE },
    { "Indutor",   STATE_MEASURE_INDUCTOR,  ICON_INDUCTOR,      C_YELLOW },
    { "Multimetro", STATE_MULTIMETER,        ICON_MULTIMETER,    C_CYAN   },
    { "Termometro", STATE_THERMAL_PROBE,     ICON_TEMP,          C_ORANGE },
    { "Historico",  STATE_HISTORY,           ICON_HISTORY,       C_GREY   },
    { "Ajustes",    STATE_SETTINGS,          ICON_SETTINGS,      C_WHITE  },
    { "Sobre",      STATE_ABOUT,             ICON_ABOUT,         C_GREEN  }
};

const uint8_t MENU_COUNT = sizeof(MAIN_MENU) / sizeof(MenuCard);

// Estado Global do Menu
uint8_t currentMenuPage = 0;
static int8_t selectedIdx = 0;
static bool needsRedraw = true;

// Layout
const int16_t COLS = 3;
const int16_t ROWS = 2; 
const int16_t CARD_W = 95;
const int16_t CARD_H = 80;
const int16_t GAP = 8;

#include "fonts.h"

void draw_card(int16_t x, int16_t y, const MenuCard* card, bool selected) {
    uint16_t bg = selected ? C_CARD_SEL : COLOR_SURFACE;
    uint16_t border = selected ? COLOR_PRIMARY : 0x1082; // 0x1082 é um cinza muito escuro
    
    LOCK_TFT();
    // Sombra (subtil)
    if (!selected) {
        tft.fillRoundRect(x+2, y+2, CARD_W, CARD_H, 8, 0x0841); // Sombra quase preta
    }
    
    tft.fillRoundRect(x, y, CARD_W, CARD_H, 8, bg);
    tft.drawRoundRect(x, y, CARD_W, CARD_H, 8, border);
    
    // Ícone centralizado
    draw_bitmap_icon(card->icon, x + CARD_W/2 - 16, y + 15);
    
    // Rótulo com fundo contrastante se selecionado
    int16_t textLen = strlen(card->label) * 6;
    if (selected) {
        tft.fillRoundRect(x + 5, y + CARD_H - 18, CARD_W - 10, 12, 4, COLOR_PRIMARY);
        draw_text_5x7(tft, x + (CARD_W - textLen)/2, y + CARD_H - 15, card->label, TFT_BLACK, 1);
    } else {
        draw_text_5x7(tft, x + (CARD_W - textLen)/2, y + CARD_H - 15, card->label, TFT_WHITE, 1);
    }
    
    UNLOCK_TFT();
}

void menu_draw() {
    if (!needsRedraw) return;
    tft.fillScreen(C_BACKGROUND);
    graphics_draw_header("MENU PRINCIPAL");
    
    int8_t startIdx = currentMenuPage * (COLS * ROWS);
    
    for (int i = 0; i < (COLS * ROWS); i++) {
        int8_t idx = startIdx + i;
        if (idx >= MENU_COUNT) break;
        int16_t col = i % COLS;
        int16_t row = i / COLS;
        int16_t x = 10 + col * (CARD_W + GAP);
        int16_t y = 45 + row * (CARD_H + GAP);
        draw_card(x, y, &MAIN_MENU[idx], (idx == selectedIdx));
    }
    
    int8_t totalPages = (MENU_COUNT + (COLS * ROWS) - 1) / (COLS * ROWS);
    if (totalPages > 1) {
        LOCK_TFT();
        for(int p=0; p<totalPages; p++) {
            uint16_t c = (p == currentMenuPage) ? C_PRIMARY : C_DIVIDER;
            tft.fillCircle(SCREEN_WIDTH/2 - (totalPages*10)/2 + p*10, SCREEN_HEIGHT - 15, 3, c);
        }
        UNLOCK_TFT();
    }
    needsRedraw = false;
}

void menu_init() {
    selectedIdx = 0;
    currentMenuPage = 0;
    needsRedraw = true;
}

void menu_scroll(int8_t direction) {
    int8_t totalPages = (MENU_COUNT + (COLS * ROWS) - 1) / (COLS * ROWS);
    int16_t nextPage = (int16_t)currentMenuPage + direction;
    if (nextPage >= 0 && nextPage < totalPages) {
        currentMenuPage = (uint8_t)nextPage;
        needsRedraw = true;
    }
}

void menu_handle() {
    if (needsRedraw) menu_draw();
    
    bool changed = false;
    if (btn_just_pressed(BTN_RIGHT)) {
        selectedIdx++;
        if (selectedIdx >= MENU_COUNT) selectedIdx = 0;
        currentMenuPage = selectedIdx / (COLS * ROWS);
        changed = true;
    } 
    else if (btn_just_pressed(BTN_LEFT)) {
        selectedIdx--;
        if (selectedIdx < 0) selectedIdx = MENU_COUNT - 1;
        currentMenuPage = selectedIdx / (COLS * ROWS);
        changed = true;
    }
    else if (btn_just_pressed(BTN_OK)) {
        buzzer_click();
        previousAppState = currentAppState;
        currentAppState = MAIN_MENU[selectedIdx].targetState;
        needsRedraw = true;
    }
    
    if (changed) { buzzer_click(); needsRedraw = true; }
}

void menu_handle_touch(int16_t x, int16_t y) {
    int8_t startIdx = currentMenuPage * (COLS * ROWS);
    
    for (int i = 0; i < (COLS * ROWS); i++) {
        int8_t idx = startIdx + i;
        if (idx >= MENU_COUNT) break;
        
        int16_t col = i % COLS;
        int16_t row = i / COLS;
        int16_t cardX = 10 + col * (CARD_W + GAP);
        int16_t cardY = 45 + row * (CARD_H + GAP);
        
        if (x >= cardX && x <= (cardX + CARD_W) && y >= cardY && y <= (cardY + CARD_H)) {
            if (selectedIdx == idx) {
                // Clique duplo ou segundo clique: Executa
                buzzer_click();
                previousAppState = currentAppState;
                currentAppState = MAIN_MENU[idx].targetState;
                needsRedraw = true;
            } else {
                // Seleciona
                selectedIdx = idx;
                needsRedraw = true;
                buzzer_click();
            }
            return;
        }
    }
}

void menu_refresh() { needsRedraw = true; }
