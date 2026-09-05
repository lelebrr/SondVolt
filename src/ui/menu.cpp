// ============================================================================
// Sondvolt v3.2 — Sistema de Menu Profissional
// ============================================================================

#include "menu.h"
#include "graphics.h"
#include "buzzer.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "globals.h"
#include "visual.h"
#include "theme.h"
#include "safety.h"

MenuCard HOME_MENU[] = {
    { "Teste Auto", STATE_MEASURE_GENERIC,   ICON_AUTO,          V_NEON_GREEN    },
    { "Multimetro", STATE_MULTIMETER,        ICON_MULTIMETER,    V_CYAN_ELECTRIC },
    { "Trabalhos",  STATE_JOBS_LIST,         ICON_HISTORY,       V_WARNING       },
    { "Mais",       STATE_SUBMENU_MAIS,      ICON_SETTINGS,      V_PURE_WHITE    }
};

// Instrumentos que dependem da placa de expansao "Bancada".
MenuCard BANCADA_MENU[] = {
    { "Osciloscopio", STATE_SCOPE,        ICON_MULTIMETER,     V_CYAN_ELECTRIC },
    { "Curva I-V",    STATE_CURVE_TRACER, ICON_DIODE,          V_VIBRANT_PURPLE},
    { "Ripple",       STATE_RIPPLE,       ICON_CAPACITOR,      V_WARNING       },
    { "Gerador",      STATE_SIGGEN,       ICON_CONTINUITY,     V_NEON_GREEN    },
    { "Zener 12V",    STATE_ZENER,        ICON_DIODE,          V_ALERT         },
    { "Pareamento",   STATE_SORTING,      ICON_TRANSISTOR_NPN, V_CYAN_ELECTRIC }
};

MenuCard TEMP_MENU[] = {
    { "Termometro Contato", STATE_THERMAL_PROBE,  ICON_TEMP,       V_WARNING       },
    { "Camera Termica",     STATE_THERMAL_CAMERA, ICON_TEMP,       V_ALERT         }
};

MenuCard MAIS_MENU[] = {
    { "Bancada",    STATE_SUBMENU_BANCADA,   ICON_MULTIMETER,    V_CYAN_ELECTRIC },
    { "Rede WiFi",  STATE_NETWORK,           ICON_AUTO,          V_NEON_GREEN    },
    { "Resistor",   STATE_MEASURE_RESISTOR,  ICON_RESISTOR,      V_WARNING       },
    { "Capacitor",  STATE_MEASURE_CAPACITOR, ICON_CAPACITOR,     V_CYAN_ELECTRIC },
    { "Diodo",      STATE_MEASURE_DIODE,     ICON_DIODE,         V_ALERT         },
    { "LED",        STATE_MEASURE_LED,       ICON_LED,           V_NEON_GREEN    },
    { "Transistor", STATE_MEASURE_TRANSISTOR,ICON_TRANSISTOR_NPN, V_VIBRANT_PURPLE},
    { "Indutor",    STATE_MEASURE_INDUCTOR,  ICON_INDUCTOR,      V_WARNING       },
    { "CI / IC",    STATE_MEASURE_IC,        ICON_SETTINGS,      V_CYAN_ELECTRIC },
    { "Scanner",    STATE_SCANNER,           ICON_AUTO,          V_VIBRANT_PURPLE},
    { "Temperatura", STATE_SUBMENU_TEMP,      ICON_TEMP,          V_WARNING       },
    { "Calibrar",   STATE_CALIBRATION,       ICON_RESISTOR,      V_NEON_GREEN    },
    { "Historico",  STATE_HISTORY,           ICON_HISTORY,       V_TEXT_SUB      },
    { "Ajustes",    STATE_SETTINGS,          ICON_SETTINGS,      V_PURE_WHITE    },
    { "Diagnostico",STATE_STATS,             ICON_WARNING,       V_CYAN_ELECTRIC },
    { "Sobre",      STATE_ABOUT,             ICON_ABOUT,         V_NEON_GREEN    }
};

static MenuCard* get_current_menu(uint8_t* count) {
    if (currentAppState == STATE_SUBMENU_TEMP) {
        if (count) *count = 2;
        return TEMP_MENU;
    } else if (currentAppState == STATE_SUBMENU_BANCADA) {
        if (count) *count = 6;
        return BANCADA_MENU;
    } else if (currentAppState == STATE_SUBMENU_MAIS) {
        if (count) *count = 16;
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

static void sync_menu_state_for_current_context() {
    uint8_t count = 0;
    get_current_menu(&count);

    int cols = 3;
    int rows = 2;
    if (currentAppState == STATE_MENU) {
        cols = 2;
        rows = 2;
    } else if (currentAppState == STATE_SUBMENU_TEMP) {
        cols = 1;
        rows = 2;
    }

    const int itemsPerPage = cols * rows;
    const int totalPages = (count + itemsPerPage - 1) / itemsPerPage;

    if (count == 0) {
        selectedIdx = 0;
        currentMenuPage = 0;
        return;
    }

    if (selectedIdx < 0 || selectedIdx >= count) {
        selectedIdx = 0;
    }

    if (currentMenuPage >= totalPages) {
        currentMenuPage = 0;
    }

    const int firstIdxOnPage = currentMenuPage * itemsPerPage;
    const int lastIdxOnPage = firstIdxOnPage + itemsPerPage - 1;
    if (selectedIdx < firstIdxOnPage || selectedIdx > lastIdxOnPage) {
        selectedIdx = firstIdxOnPage;
        if (selectedIdx >= count) {
            selectedIdx = count - 1;
        }
    }
}

#include "fonts.h"

void draw_card(int16_t x, int16_t y, const MenuCard* card, bool selected,
               int16_t w, int16_t h) {
    const uint16_t accent = card->color;

    // O cartao selecionado ganha fundo tingido do proprio acento e uma borda
    // cheia; os demais ficam na superficie neutra com borda discreta. Isso
    // separa os dois estados sem precisar de brilho falso em volta - o efeito
    // de "neon" da versao anterior sujava a tela em vez de destacar.
    const uint16_t fill   = selected ? th_tint(accent, th_bg1) : th_bg1;
    const uint16_t border = selected ? accent : th_bg3;

    LOCK_TFT();
    tft.drawRoundRect(x + 1, y + 2, w, h, TH_RADIUS_MD, th_shade(th_bg0, 90));
    tft.fillRoundRect(x, y, w, h, TH_RADIUS_MD, fill);
    tft.drawRoundRect(x, y, w, h, TH_RADIUS_MD, border);

    // Barra de acento na lateral esquerda do cartao selecionado: indica a
    // selecao sem competir com o icone pela atencao.
    if (selected) {
        tft.fillRoundRect(x + 3, y + TH_SP_2, 3, h - TH_SP_4, 1, accent);
    }
    UNLOCK_TFT();

    // Icone centralizado, com espaco para o rotulo embaixo.
    const int16_t iconX = x + (w - 32) / 2;
    const int16_t iconY = y + (h - 32) / 2 - 6;
    draw_bitmap_icon(card->icon, iconX, iconY);

    // Rotulo na base, na cor do acento quando selecionado.
    LOCK_TFT();
    const int16_t tw = TH_TEXT_W(card->label, TH_FONT_CAPTION);
    draw_text_5x7(tft, x + (w - tw) / 2, y + h - 12, card->label,
                  selected ? accent : th_txMd, TH_FONT_CAPTION);
    UNLOCK_TFT();
}

void menu_draw() {
    if (!needsRedraw) return;

    sync_menu_state_for_current_context();
    
    uint8_t count = 0;
    MenuCard* menu = get_current_menu(&count);

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    
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
    } else if (currentAppState == STATE_SUBMENU_BANCADA) {
        header = "INSTRUMENTOS DE BANCADA";
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

// menu_handle() foi removida na v5.0.
//
// Ela lia botoes fisicos (btn_just_pressed) que a CYD nao tem: a placa vem
// sem botoes soldados, HAS_PHYSICAL_BUTTONS valia 0, e nada no firmware
// chamava esta funcao. Toda a navegacao acontece por menu_handle_touch().
// Junto com ela saiu buttons.cpp, que existia so para alimenta-la.

void menu_handle_touch(int16_t x, int16_t y) {
    sync_menu_state_for_current_context();

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
