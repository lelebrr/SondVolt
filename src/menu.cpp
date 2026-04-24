// ============================================================================
// Sondvolt v3.1 — Menu Navigation System
// Complete implementation with modern UI and button navigation
// ============================================================================

#include "menu.h"
#include "graphics.h"
#include "globals.h"
#include "buttons.h"
#include "leds.h"
#include "buzzer.h"
#include "multimeter.h"
#include "display_globals.h"
#include "safety.h"

// ============================================================================
// MENU CONFIGURATION
// ============================================================================

const MenuCard MAIN_MENU_CARDS[] = {
    { "Resistor",   STATE_MEASURE_RESISTOR,   ICON_RESISTOR,       C_ORANGE,   MENU_FLAG_NONE },
    { "Capacitor", STATE_MEASURE_CAPACITOR,  ICON_CAPACITOR,    C_BLUE,    MENU_FLAG_NONE },
    { "Diodo",   STATE_MEASURE_DIODE,     ICON_DIODE,       C_RED,    MENU_FLAG_NONE },
    { "LED",     STATE_MEASURE_GENERIC,   ICON_LED,        C_GREEN,  MENU_FLAG_NONE },
    { "Transist",STATE_MEASURE_TRANSISTOR,ICON_TRANSISTOR_NPN,C_PURPLE,MENU_FLAG_NONE },
    { "Indutor", STATE_MEASURE_INDUCTOR,   ICON_INDUCTOR,    C_YELLOW, MENU_FLAG_NONE },
    { "Multimetro",STATE_MULTIMETER,     ICON_MULTIMETER, C_CYAN,   MENU_FLAG_NONE },
    { "Temperat",STATE_THERMAL_PROBE,     ICON_TEMP,        C_ORANGE,  MENU_FLAG_NONE },
    { "Historico",STATE_HISTORY,          ICON_HISTORY,     C_GREY,   MENU_FLAG_NONE },
    { "Calibrar",STATE_CALIBRATION,       ICON_SETTINGS,    C_YELLOW,  MENU_FLAG_NONE },
    { "Config",  STATE_SETTINGS,           ICON_SETTINGS,    C_DARK_GREY, MENU_FLAG_NONE },
    { "Sobre",   STATE_ABOUT,             ICON_ABOUT,     C_LIGHT_GREY, MENU_FLAG_NONE },
};

const uint8_t MAIN_MENU_COUNT = sizeof(MAIN_MENU_CARDS) / sizeof(MenuCard);

const MenuCard MEASURE_MENU_CARDS[] = {
    { "Resistor",  STATE_MEASURE_RESISTOR,   ICON_RESISTOR,      C_ORANGE,   MENU_FLAG_NONE },
    { "Capacitor",STATE_MEASURE_CAPACITOR,   ICON_CAPACITOR,     C_BLUE,    MENU_FLAG_NONE },
    { "Diodo",   STATE_MEASURE_DIODE,      ICON_DIODE,        C_RED,    MENU_FLAG_NONE },
    { "LED",     STATE_MEASURE_GENERIC,     ICON_LED,          C_GREEN,  MENU_FLAG_NONE },
    { "Transist", STATE_MEASURE_TRANSISTOR,  ICON_TRANSISTOR_NPN,C_PURPLE, MENU_FLAG_NONE },
    { "Indutor",  STATE_MEASURE_INDUCTOR,    ICON_INDUCTOR,       C_YELLOW,  MENU_FLAG_NONE },
};
const uint8_t MEASURE_MENU_COUNT = sizeof(MEASURE_MENU_CARDS) / sizeof(MenuCard);

const MenuCard SETTINGS_MENU_CARDS[] = {
    { "Brilho",    100, ICON_SETTINGS,   C_YELLOW,  MENU_FLAG_NONE },
    { "Som",       101, ICON_VOLTAGE, C_GREEN,  MENU_FLAG_NONE },
    { "Auto-OFF",  102, ICON_HISTORY,  C_CYAN,   MENU_FLAG_NONE },
    { "Temperat",  103, ICON_TEMP,    C_ORANGE,  MENU_FLAG_NONE },
    { "Calibrar",  104, ICON_SETTINGS, C_PURPLE, MENU_FLAG_NONE },
    { "Reset",    105, ICON_WARNING, C_RED,    MENU_FLAG_NONE },
};
const uint8_t SETTINGS_MENU_COUNT = sizeof(SETTINGS_MENU_CARDS) / sizeof(MenuCard);

// ============================================================================
// MENU STATE
// ============================================================================

static int8_t currentCardIndex = 0;
static int8_t selectedMenu = 0;
static bool menuNeedsRedraw = true;
static uint8_t currentFrame = 0;

// Grid configuration
static const uint8_t GRID_COLS = 3;
static const uint8_t GRID_ROWS = 4;
static const uint8_t CARDS_PER_PAGE = GRID_COLS * GRID_ROWS;

// ============================================================================
// INITIALIZATION
// ============================================================================

void menu_init() {
    currentCardIndex = 0;
    selectedMenu = 0;
    menuNeedsRedraw = true;
    DBG("[MENU] Initialized");
}

// ============================================================================
// MAIN MENU DRAW
// ============================================================================

void menu_draw() {
    clear_screen();
    
    draw_status_bar("Sondvolt", "v3.1");
    
    int8_t page = currentCardIndex / CARDS_PER_PAGE;
    int8_t startIdx = page * CARDS_PER_PAGE;
    int8_t cardsOnPage = min(CARDS_PER_PAGE, (int)MAIN_MENU_COUNT - startIdx);
    
    // Grid layout
    int16_t cardW = (SCREEN_W - 40) / GRID_COLS;
    int16_t cardH = (SCREEN_H - STATUS_BAR_H - FOOTER_H - 20) / GRID_ROWS;
    int16_t marginX = 10;
    int16_t marginY = STATUS_BAR_H + 10;
    int16_t gapX = 8;
    int16_t gapY = 8;
    
    for (int8_t i = 0; i < cardsOnPage; i++) {
        int8_t idx = startIdx + i;
        int8_t col = i % GRID_COLS;
        int8_t row = i / GRID_COLS;
        
        int16_t x = marginX + col * (cardW + gapX);
        int16_t y = marginY + row * (cardH + gapY);
        
        bool selected = (idx == currentCardIndex);
        draw_menu_card(x, y, cardW, cardH, &MAIN_MENU_CARDS[idx], selected);
    }
    
    // Page dots
    int8_t totalPages = (MAIN_MENU_COUNT + CARDS_PER_PAGE - 1) / CARDS_PER_PAGE;
    if (totalPages > 1) {
        draw_page_dots(page, totalPages);
    }
    
    draw_footer();
    menuNeedsRedraw = false;
}

// ============================================================================
// MENU CARD DRAW
// ============================================================================

void draw_menu_card(int16_t x, int16_t y, int16_t w, int16_t h,
               const MenuCard* card, bool selected) {
    uint16_t bg = selected ? color_darker(card->color, 2) : C_DARK_GREY;
    uint16_t border = selected ? C_WHITE : color_darker(card->color, 1);
    
    tft.fillRoundRect(x, y, w, h, 6, bg);
    tft.drawRoundRect(x, y, w, h, 6, border);
    
    if (selected) {
        tft.drawRoundRect(x + 2, y + 2, w - 4, h - 4, 4, card->color);
    }
    
    // Icon
    int16_t iconX = x + w / 2 - 16;
    int16_t iconY = y + h / 3;
    draw_component_icon(card->iconType, iconX, iconY, card->color);
    
    // Label
    tft.setTextColor(selected ? C_WHITE : C_LIGHT_GREY);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);
    tft.setCursor(x + w / 2, y + h - 12);
    tft.print(card->label);
    tft.setTextDatum(TL_DATUM);
    
    // Warning badge
    if (card->flags & MENU_FLAG_WARNING) {
        tft.fillCircle(x + w - 10, y + 10, 6, C_YELLOW);
        tft.setTextColor(C_BLACK);
        tft.setTextSize(1);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(x + w - 10, y + 10);
        tft.print("!");
        tft.setTextDatum(TL_DATUM);
    }
}

// ============================================================================
// PAGE DOTS
// ============================================================================

void draw_page_dots(int8_t page, int8_t total) {
    int8_t dotSize = 4;
    int8_t gap = 6;
    int16_t totalWidth = total * dotSize + (total - 1) * gap;
    int16_t startX = (SCREEN_W - totalWidth) / 2;
    int16_t y = SCREEN_H - FOOTER_H - 8;
    
    for (int8_t i = 0; i < total; i++) {
        int16_t x = startX + i * (dotSize + gap);
        uint16_t c = (i == page) ? C_GREEN : C_DARK_GREY;
        tft.fillCircle(x, y, dotSize / 2, c);
    }
}

// ============================================================================
// MENU NAVIGATION
// ============================================================================

void menu_navigate(int8_t direction) {
    int8_t newIndex = currentCardIndex + direction;
    
    if (newIndex < 0) {
        newIndex = MAIN_MENU_COUNT - 1;
    } else if (newIndex >= (int8_t)MAIN_MENU_COUNT) {
        newIndex = 0;
    }
    
    currentCardIndex = newIndex;
    menuNeedsRedraw = true;
    
    // Visual feedback
    led_flash_green(50);
}

void menu_select() {
    // Verificação de segurança antes do multímetro
    if (MAIN_MENU_CARDS[currentCardIndex].targetState == STATE_MULTIMETER) {
        if (!safety_check_before_multimeter()) {
            buzzer_error();
            menuNeedsRedraw = true;
            return;
        }
        
        // Verificação adicional para modo AC
        if (multimeter_get_mode() == MMODE_AC_VOLTAGE) {
            if (!safety_confirm_electrical_measurement()) {
                // Usuário não confirmou, retorna ao menu
                buzzer_error();
                menuNeedsRedraw = true;
                return;
            }
        }
    }
    
    previousAppState = currentAppState;
    currentAppState = MAIN_MENU_CARDS[currentCardIndex].targetState;
    
    led_flash_green(100);
    transition_slide_left();
}

void menu_back() {
    currentAppState = STATE_MENU;
    transition_slide_right();
    menu_refresh();
}

void menu_refresh() {
    menuNeedsRedraw = true;
}

void menu_updateSelection(int8_t index) {
    if (index != currentCardIndex) {
        currentCardIndex = index;
        menuNeedsRedraw = true;
    }
}

// ============================================================================
// MENU HANDLE
// ============================================================================

void menu_handle() {
    if (menuNeedsRedraw) {
        menu_draw();
    }
    
    // Button navigation
    if (btn_just_pressed(BTN_UP)) {
        menu_navigate(-GRID_COLS);
    } else if (btn_just_pressed(BTN_DOWN)) {
        menu_navigate(GRID_COLS);
    } else if (btn_just_pressed(BTN_LEFT)) {
        menu_navigate(-1);
    } else if (btn_just_pressed(BTN_RIGHT)) {
        menu_navigate(1);
    } else if (btn_just_pressed(BTN_CENTER) || btn_just_pressed(BTN_OK)) {
        menu_select();
    } else if (btn_just_pressed(BTN_BACK)) {
        menu_back();
    }
    
    // Touch handling
    int8_t touchedCard = menu_process_touch();
    if (touchedCard >= 0) {
        menu_updateSelection(touchedCard);
        delay(100);
        menu_select();
    }
}

// ============================================================================
// TOUCH PROCESSING
// ============================================================================

int menu_process_touch() {
    // Placeholder - touchscreen requires proper initialization
    // If touchscreen is available, add proper includes and setup
    return -1;
}

// ============================================================================
// MEASURE MENU
// ============================================================================

void measure_menu_draw() {
    clear_screen();
    draw_status_bar("Selecione o Tipo", "");
    
    int16_t cardW = (SCREEN_W - 40) / 3;
    int16_t cardH = 60;
    int16_t marginX = 10;
    int16_t marginY = STATUS_BAR_H + 10;
    
    for (int8_t i = 0; i < MEASURE_MENU_COUNT; i++) {
        int8_t col = i % 3;
        int8_t row = i / 3;
        int16_t x = marginX + col * (cardW + 4);
        int16_t y = marginY + row * (cardH + 8);
        
        bool selected = (i == currentCardIndex);
        draw_menu_card(x, y, cardW, cardH, &MEASURE_MENU_CARDS[i], selected);
    }
    
    draw_footer();
}

void measure_menu_handle() {
    if (btn_just_pressed(BTN_BACK)) {
        menu_back();
    }
}

// ============================================================================
// SETTINGS MENU
// ============================================================================

void settings_draw() {
    clear_screen();
    draw_status_bar("Configuracoes", "");
    
    int16_t cardW = (SCREEN_W - 40) / 2;
    int16_t cardH = 50;
    int16_t marginX = 10;
    int16_t marginY = STATUS_BAR_H + 10;
    
    static bool toggleStates[6] = {true, true, false, true, false, false};
    
    for (int8_t i = 0; i < SETTINGS_MENU_COUNT; i++) {
        int8_t col = i % 2;
        int8_t row = i / 2;
        int16_t x = marginX + col * (cardW + 8);
        int16_t y = marginY + row * (cardH + 8);
        
        uint16_t bg = toggleStates[i] ? C_GREEN : C_DARK_GREY;
        
        tft.fillRoundRect(x, y, cardW, cardH, 4, bg);
        tft.drawRoundRect(x, y, cardW, cardH, 4, C_WHITE);
        
        draw_component_icon(SETTINGS_MENU_CARDS[i].iconType, x + 4, y + 8, C_WHITE);
        
        tft.setTextColor(C_WHITE);
        tft.setTextSize(1);
        tft.setCursor(x + 30, y + 15);
        tft.print(SETTINGS_MENU_CARDS[i].label);
    }
    
    draw_footer();
}

void settings_handle() {
    if (btn_just_pressed(BTN_BACK)) {
        menu_back();
    }
}

// ============================================================================
// ABOUT SCREEN
// ============================================================================

void about_draw() {
    clear_screen();
    draw_status_bar("Sobre", "");
    
    // Icon
    draw_component_icon(ICON_MULTIMETER, SCREEN_W/2 - 20, 50, C_GREEN);
    
    // Title
    tft.setTextColor(C_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setCursor(SCREEN_W/2, 100);
    tft.print("SONDVOLT");
    tft.setTextSize(1);
    tft.setTextColor(C_GREEN);
    tft.setCursor(SCREEN_W/2, 120);
    tft.print("v3.1");
    tft.setTextColor(C_LIGHT_GREY);
    tft.setCursor(SCREEN_W/2, 140);
    tft.print("ESP32-2432S028R");
    tft.setCursor(SCREEN_W/2, 160);
    tft.print("Build: " __DATE__);
    tft.setTextDatum(TL_DATUM);
    
    draw_footer();
}

void about_handle() {
    if (btn_just_pressed(BTN_BACK) || btn_just_pressed(BTN_OK)) {
        menu_back();
    }
}

// ============================================================================
// HISTORY SCREEN
// ============================================================================

void history_draw() {
    clear_screen();
    draw_status_bar("Historico", "");
    
    struct { const char* name; float value; const char* unit; } items[] = {
        {"Resistor 1k", 0.98, "kOhm"},
        {"Cap 100uF", 94.5, "uF"},
        {"LED Red", 1.82, "V"},
        {"1N4007", 0.62, "V"}
    };
    
    for (int8_t i = 0; i < 4; i++) {
        int16_t y = STATUS_BAR_H + 15 + i * 45;
        
        tft.setTextColor(C_WHITE);
        tft.setTextSize(1);
        tft.setCursor(10, y);
        tft.print(items[i].name);
        
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f %s", items[i].value, items[i].unit);
        tft.setTextColor(C_GREEN);
        tft.setTextSize(2);
        tft.setTextDatum(TR_DATUM);
        tft.setCursor(SCREEN_W - 10, y + 15);
        tft.print(buf);
        tft.setTextDatum(TL_DATUM);
        
        tft.drawLine(10, y + 30, SCREEN_W - 10, y + 30, C_DARK_GREY);
    }
    
    draw_footer();
}

void history_handle() {
    if (btn_just_pressed(BTN_BACK)) {
        menu_back();
    }
}

// ============================================================================
// TRANSITIONS
// ============================================================================

void transition_fade_in() {
    for (int alpha = 0; alpha <= 255; alpha += 25) {
        tft.fillScreen(rgb565(alpha * 0.05f, alpha * 0.06f, alpha * 0.08f));
    }
}

void transition_fade_out() {
    for (int alpha = 255; alpha >= 0; alpha -= 25) {
        tft.fillScreen(rgb565(alpha * 0.05f, alpha * 0.06f, alpha * 0.08f));
    }
}

void transition_slide_left() {
    menuNeedsRedraw = true;
}

void transition_slide_right() {
    menuNeedsRedraw = true;
}

// ============================================================================
// STATS, CALIBRATION, SCANNER PLACEHOLDERS
// ============================================================================

void stats_draw() { draw_about_screen(); }
void stats_handle() { menu_back(); }

void calibration_draw() { draw_about_screen(); }
void calibration_handle() { menu_back(); }

void scanner_draw() { draw_about_screen(); }
void scanner_handle() { menu_back(); }

void draw_about_screen() { about_draw(); }

// ============================================================================
// MULTIMETER SCREEN - modo Multímetro AC/DC integrado
// ============================================================================

void multimeter_menu_draw() {
    clear_screen();
    draw_status_bar("Multimetro AC/DC", "");
    
    // Desenha modo atual
    const char* modeLabel;
    uint16_t modeColor;
    
    switch (multimeter_get_mode()) {
        case MMODE_DC_VOLTAGE:
            modeLabel = "VDC";
            modeColor = C_CYAN;
            break;
        case MMODE_AC_VOLTAGE:
            modeLabel = "VAC";
            modeColor = C_YELLOW;
            break;
        case MMODE_DC_CURRENT:
            modeLabel = "A";
            modeColor = C_GREEN;
            break;
        case MMODE_RESISTANCE:
            modeLabel = "Ohm";
            modeColor = C_ORANGE;
            break;
        case MMODE_CONTINUITY:
            modeLabel = "CURTO";
            modeColor = C_RED;
            break;
        case MMODE_POWER:
            modeLabel = "W";
            modeColor = C_PURPLE;
            break;
        default:
            modeLabel = "---";
            modeColor = C_GREY;
    }
    
    // Botões de modo (grid 2x3)
    int16_t btnW = 70;
    int16_t btnH = 35;
    int16_t marginX = 15;
    int16_t startY = STATUS_BAR_H + 15;
    
    const char* modes[] = {"VDC", "VAC", "A", "Ohm", "CURTO", "W"};
    uint16_t modeColors[] = {C_CYAN, C_YELLOW, C_GREEN, C_ORANGE, C_RED, C_PURPLE};
    
    for (int i = 0; i < 6; i++) {
        int col = i % 3;
        int row = i / 3;
        int16_t x = marginX + col * (btnW + 5);
        int16_t y = startY + row * (btnH + 5);
        
        uint16_t bg = (multimeter_get_mode() == i) ? modeColors[i] : C_DARK_GREY;
        uint16_t border = color_darker(modeColors[i], 1);
        
        tft.fillRoundRect(x, y, btnW, btnH, 4, bg);
        tft.drawRoundRect(x, y, btnW, btnH, 4, border);
        
        tft.setTextColor(C_WHITE);
        tft.setTextSize(1);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(x + btnW/2, y + btnH/2);
        tft.print(modes[i]);
    }
    
    // Leitura atual (parte inferior)
    MultimeterReading reading = multimeter_get_last_reading();
    
    char valueStr[32];
    if (reading.valid) {
        multimeter_format_value(reading.value, valueStr, sizeof(valueStr));
        
        uint16_t valueColor;
        if (reading.state == MSTATE_SHORT || reading.state == MSTATE_HIGH_VOLTAGE) {
            valueColor = C_ORANGE;
        } else if (reading.state == MSTATE_ERROR) {
            valueColor = C_RED;
        } else {
            valueColor = C_GREEN;
        }
        
        tft.setTextColor(valueColor);
        tft.setTextSize(4);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(SCREEN_W/2, 170);
        tft.print(valueStr);
        
        tft.setTextSize(2);
        tft.setCursor(SCREEN_W/2 + strlen(valueStr)*22 + 10, 170);
        tft.print(reading.unit);
        
        // Indicador de range
        tft.setTextSize(1);
        tft.setTextColor(C_GREY);
        tft.setTextDatum(TC_DATUM);
        const char* rangeStr = "AUTO";
        if (multimeter_get_range() == RANGE_LOW) rangeStr = "LOW";
        else if (multimeter_get_range() == RANGE_MED) rangeStr = "MED";
        else if (multimeter_get_range() == RANGE_HIGH) rangeStr = "HIGH";
        tft.setCursor(SCREEN_W - 40, 195);
        tft.print(rangeStr);
        
    } else {
        tft.setTextColor(C_GREY);
        tft.setTextSize(2);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(SCREEN_W/2, 170);
        tft.print("--.-");
    }
    
    tft.setTextDatum(TL_DATUM);
    
    // Alerta de segurança (simplificado)
    if (reading.state == MSTATE_SHORT) {
        tft.fillRect(50, 130, 220, 25, C_ORANGE);
        tft.setTextColor(C_BLACK);
        tft.setTextSize(1);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(160 - 60, 145);
        tft.print(F("SHORT CIRCUIT!"));
    } else if (reading.state == MSTATE_HIGH_VOLTAGE) {
        tft.fillRect(50, 130, 220, 25, C_RED);
        tft.setTextColor(C_WHITE);
        tft.setTextSize(1);
        tft.setTextDatum(TC_DATUM);
        tft.setCursor(160 - 70, 145);
        tft.print(F("DANGER! HIGH VOLTAGE!"));
    }
    
    // LED indicacao
    if (reading.state == MSTATE_SHORT || reading.state == MSTATE_HIGH_VOLTAGE) {
        set_red_led(true);
    } else if (reading.valid) {
        set_green_led(true);
    } else {
        set_green_led(false);
        set_red_led(false);
    }
    
    tft.setTextDatum(TL_DATUM);
    
    draw_footer();
}

void multimeter_menu_handle() {
    // Atualiza sistema de segurança
    safety_update();
    
    // Botões para trocar modo
    if (btn_just_pressed(BTN_UP)) {
        int newMode = (int)multimeter_get_mode() - 3;
        if (newMode < 0) newMode += 6;
        multimeter_set_mode((MultimeterMode)newMode);
    } else if (btn_just_pressed(BTN_DOWN)) {
        int newMode = ((int)multimeter_get_mode() + 3) % 6;
        multimeter_set_mode((MultimeterMode)newMode);
    } else if (btn_just_pressed(BTN_LEFT)) {
        int newMode = (int)multimeter_get_mode() - 1;
        if (newMode < 0) newMode = 5;
        multimeter_set_mode((MultimeterMode)newMode);
    } else if (btn_just_pressed(BTN_RIGHT)) {
        int newMode = ((int)multimeter_get_mode() + 1) % 6;
        multimeter_set_mode((MultimeterMode)newMode);
    }
    
    // Botão OK - inicia medição
    if (btn_just_pressed(BTN_OK)) {
        multimeter_read();
        buzzer_click();
    }
    
    // Botão BACK - sai do multímetro
    if (btn_just_pressed(BTN_BACK)) {
        multimeter_shutdown();
        menu_back();
    }
    
    // Atualiza leitura
    multimeter_handle();
}

// ============================================================================
// TOUCH HELPERS
// ============================================================================

int measure_process_touch() {
    //Placeholder - use touchscreen if available
    return -1;
}

int settings_process_touch() {
    //Placeholder - use touchscreen if available
    return -1;
}