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
#include "visual.h"
#include "safety.h"

MenuCard HOME_MENU[] = {
    { "Teste Auto", STATE_MEASURE_GENERIC,   ICON_AUTO,          V_NEON_GREEN    },
    { "Multimetro", STATE_MULTIMETER,        ICON_MULTIMETER,    V_CYAN_ELECTRIC },
    { "Comparar",   STATE_COMPARATOR,        ICON_TRANSISTOR_NPN,V_VIBRANT_PURPLE},
    { "Mais",       STATE_SUBMENU_MAIS,      ICON_SETTINGS,      V_PURE_WHITE    }
};

MenuCard TEMP_MENU[] = {
    { "Toque",       STATE_THERMAL_PROBE,     ICON_TEMP,          V_WARNING       },
    { "Camera",      STATE_THERMAL_CAMERA,    ICON_TEMP,          V_ALERT         }
};

MenuCard MAIS_MENU[] = {
    { "Resistor",   STATE_MEASURE_RESISTOR,  ICON_RESISTOR,      V_WARNING       },
    { "Capacitor",  STATE_MEASURE_CAPACITOR, ICON_CAPACITOR,     V_CYAN_ELECTRIC },
    { "Diodo",      STATE_MEASURE_DIODE,     ICON_DIODE,         V_ALERT         },
    { "LED",        STATE_MEASURE_LED,       ICON_LED,           V_NEON_GREEN    },
    { "Transistor", STATE_MEASURE_TRANSISTOR,ICON_TRANSISTOR_NPN, V_VIBRANT_PURPLE},
    { "Indutor",    STATE_MEASURE_INDUCTOR,  ICON_INDUCTOR,      V_WARNING       },
    { "Temp",       STATE_SUBMENU_TEMP,      ICON_TEMP,          V_WARNING       },
    { "Calibrar",   STATE_CALIBRATION,       ICON_RESISTOR,      V_NEON_GREEN    },
    { "Historico",  STATE_HISTORY,           ICON_HISTORY,       V_TEXT_SUB      },
    { "Ajustes",    STATE_SETTINGS,          ICON_SETTINGS,      V_PURE_WHITE    },
    { "Sobre",      STATE_ABOUT,             ICON_ABOUT,         V_NEON_GREEN    }
};

static MenuCard* get_current_menu(uint8_t* count) {
    if (currentAppState == STATE_SUBMENU_TEMP) {
        if (count) *count = 2;
        return TEMP_MENU;
    } else if (currentAppState == STATE_SUBMENU_MAIS) {
        if (count) *count = 11;
        return MAIS_MENU;
    } else {
        if (count) *count = 4;
        return HOME_MENU;
    }
}

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

void draw_card(int16_t x, int16_t y, const MenuCard* card, bool selected, int16_t w, int16_t h) {
    uint16_t bg = selected ? color_mix(card->color, V_BG_DARK, 220) : V_BG_SURFACE;
    uint16_t border = selected ? card->color : V_DIVIDER; 
    
    LOCK_TFT();
    // Efeito de sombra/brilho Neon
    if (selected) {
        // Brilho Neon Externo (Multi-camada)
        for(int i=1; i<3; i++) {
            tft.drawRoundRect(x-i, y-i, w+i*2, h+i*2, V_RADIUS_LG, color_mix(card->color, V_BG_DARK, 180 + i*20));
        }
    } else {
        // Sombra suave
        tft.fillRoundRect(x+2, y+2, w, h, V_RADIUS_MD, 0x0000);
    }
    
    // Corpo do Card
    tft.fillRoundRect(x, y, w, h, V_RADIUS_MD, bg);
    tft.drawRoundRect(x, y, w, h, V_RADIUS_MD, border);
    
    // Ícone centralizado
    int16_t iconX = x + w/2 - 16;
    int16_t iconY = y + h/2 - 20;
    draw_bitmap_icon(card->icon, iconX, iconY);
    
    // Rótulo com fundo colorido se selecionado
    int16_t textLen = strlen(card->label) * 6;
    if (selected) {
        tft.fillRoundRect(x + 4, y + h - 18, w - 8, 14, 4, card->color);
        draw_text_5x7(tft, x + (w - textLen)/2, y + h - 14, card->label, V_BG_DARK, 1);
    } else {
        draw_text_5x7(tft, x + (w - textLen)/2, y + h - 14, card->label, V_TEXT_MAIN, 1);
    }
    
    UNLOCK_TFT();
}

void menu_draw() {
    if (!needsRedraw) return;
    
    uint8_t count = 0;
    MenuCard* menu = get_current_menu(&count);
    
    tft.fillScreen(V_BG_DARK);
    
    const char* header = "MENU PRINCIPAL";
    int cols = 3, rows = 2;
    int cardW = 95, cardH = 80;
    int gap = 8;
    int startX = 10;
    int startY = 45;

    if (currentAppState == STATE_MENU) {
        header = "SONDVOLT HOME";
        cols = 2; rows = 2;
        cardW = 145; cardH = 55;
        gap = 10;
        startX = 10;
        startY = 40;
    } else if (currentAppState == STATE_SUBMENU_TEMP) {
        header = "TEMPERATURA";
        cols = 1; rows = 2;
        cardW = 280; cardH = 70;
        gap = 15;
        startX = 20;
        startY = 60;
    } else if (currentAppState == STATE_SUBMENU_MAIS) {
        header = "MAIS FUNCOES";
    }

    graphics_draw_header(header);
    
    int8_t startIdx = currentMenuPage * (cols * rows);
    
    for (int i = 0; i < (cols * rows); i++) {
        int8_t idx = startIdx + i;
        if (idx >= count) break;
        int16_t col = i % cols;
        int16_t row = i / cols;
        int16_t x = startX + col * (cardW + gap);
        int16_t y = startY + row * (cardH + gap);
        
        draw_card(x, y, &menu[idx], (idx == selectedIdx), cardW, cardH);
    }

    // SEÇÃO DE COMPONENTES RECENTES (Requisito 1)
    if (currentAppState == STATE_MENU) {
        LOCK_TFT();
        draw_text_5x7(tft, 12, 178, "RECENTES", V_NEON_GREEN, 1);
        tft.drawLine(10, 188, 310, 188, V_DIVIDER);
        
        for (int i = 0; i < 6; i++) {
            int16_t x = 12 + i * 51;
            int16_t y = 194;
            
            // Corpo sutil
            tft.fillRoundRect(x, y, 46, 44, 6, V_BG_SURFACE);
            tft.drawRoundRect(x, y, 46, 44, 6, V_BG_HIGHLIGHT);
            
            if (recentTests[i].timestamp > 0) {
                IconType icon = ICON_ABOUT;
                if (strstr(recentTests[i].componentName, "Resistor")) icon = ICON_RESISTOR;
                else if (strstr(recentTests[i].componentName, "Capacitor")) icon = ICON_CAPACITOR;
                else if (strstr(recentTests[i].componentName, "Diodo")) icon = ICON_DIODE;
                else if (strstr(recentTests[i].componentName, "Transistor")) icon = ICON_TRANSISTOR_NPN;
                
                draw_bitmap_icon(icon, x + 7, y + 6);
            } else {
                draw_text_5x7(tft, x + 20, y + 18, "+", V_TEXT_SUB, 1);
            }
        }
        UNLOCK_TFT();
    }
    
    int8_t totalPages = (count + (cols * rows) - 1) / (cols * rows);
    if (totalPages > 1) {
        LOCK_TFT();
        for(int p=0; p<totalPages; p++) {
            uint16_t c = (p == currentMenuPage) ? V_CYAN_ELECTRIC : V_BG_HIGHLIGHT;
            tft.fillCircle(SCREEN_WIDTH/2 - (totalPages*12)/2 + p*12, SCREEN_HEIGHT - 12, 3, c);
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
    uint8_t count = 0;
    get_current_menu(&count);
    int8_t totalPages = (count + (COLS * ROWS) - 1) / (COLS * ROWS);
    int16_t nextPage = (int16_t)currentMenuPage + direction;
    if (nextPage >= 0 && nextPage < totalPages) {
        currentMenuPage = (uint8_t)nextPage;
        needsRedraw = true;
    }
}

void menu_handle() {
    uint8_t count = 0;
    MenuCard* menu = get_current_menu(&count);
    if (needsRedraw) menu_draw();
    
    bool changed = false;
    if (btn_just_pressed(BTN_RIGHT)) {
        selectedIdx++;
        if (selectedIdx >= count) selectedIdx = 0;
        currentMenuPage = selectedIdx / (COLS * ROWS);
        changed = true;
    } 
    else if (btn_just_pressed(BTN_LEFT)) {
        selectedIdx--;
        if (selectedIdx < 0) selectedIdx = count - 1;
        currentMenuPage = selectedIdx / (COLS * ROWS);
        changed = true;
    }
    else if (btn_just_pressed(BTN_OK)) {
        buzzer_click();
        
        // Se estiver entrando no modo Multímetro, exige confirmação de segurança
        if (menu[selectedIdx].targetState == STATE_MULTIMETER) {
            if (!safety_confirm_electrical_measurement()) {
                needsRedraw = true;
                return; // Cancela entrada se não confirmar
            }
        }
        
        previousAppState = currentAppState;
        currentAppState = menu[selectedIdx].targetState;
        needsRedraw = true;
    }
    
    if (changed) { buzzer_click(); needsRedraw = true; }
}

void menu_handle_touch(int16_t x, int16_t y) {
    uint8_t count = 0;
    MenuCard* menu = get_current_menu(&count);
    
    int cols = 3, rows = 2;
    int cardW = 95, cardH = 80;
    int gap = 8;
    int startX = 10;
    int startY = 45;

    if (currentAppState == STATE_MENU) {
        cols = 2; rows = 2;
        cardW = 145; cardH = 55;
        gap = 10;
        startX = 10;
        startY = 40;
    } else if (currentAppState == STATE_SUBMENU_TEMP) {
        cols = 1; rows = 2;
        cardW = 280; cardH = 70;
        gap = 15;
        startX = 20;
        startY = 60;
    }

    int8_t startIdx = currentMenuPage * (cols * rows);
    
    for (int i = 0; i < (cols * rows); i++) {
        int8_t idx = startIdx + i;
        if (idx >= count) break;
        
        int16_t col = i % cols;
        int16_t row = i / cols;
        int16_t cardX = startX + col * (cardW + gap);
        int16_t cardY = startY + row * (cardH + gap);
        
        if (x >= cardX && x <= (cardX + cardW) && y >= cardY && y <= (cardY + cardH)) {
            if (selectedIdx == idx) {
                buzzer_click();
                previousAppState = currentAppState;
                currentAppState = menu[idx].targetState;
                needsRedraw = true;
            } else {
                selectedIdx = idx;
                needsRedraw = true;
                buzzer_click();
            }
            return;
        }
    }

    // Clique em Componentes Recentes
    if (currentAppState == STATE_MENU && y > 180) {
        for (int i = 0; i < 6; i++) {
            int16_t cardX = 10 + i * 52;
            if (x >= cardX && x <= (cardX + 45)) {
                if (recentTests[i].timestamp > 0) {
                    buzzer_click();
                    // Define o estado baseado no que foi clicado
                    if (strstr(recentTests[i].componentName, "Resistor")) currentAppState = STATE_MEASURE_RESISTOR;
                    else if (strstr(recentTests[i].componentName, "Capacitor")) currentAppState = STATE_MEASURE_CAPACITOR;
                    else if (strstr(recentTests[i].componentName, "Diodo")) currentAppState = STATE_MEASURE_DIODE;
                    else if (strstr(recentTests[i].componentName, "Transistor")) currentAppState = STATE_MEASURE_TRANSISTOR;
                    else currentAppState = STATE_MEASURE_GENERIC;
                    
                    needsRedraw = true;
                    return;
                }
            }
        }
    }
}

void menu_refresh() { needsRedraw = true; }
