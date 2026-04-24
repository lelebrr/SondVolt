// ============================================================================
// Sondvolt v3.x — Graficos e Icones (Implementacao)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: graphics.cpp
// Descricao: Implementacao de funcoes de desenho e icones vetoriais
// ============================================================================

#include "graphics.h"
#include "config.h"

// Variavel global do display externa
#include "display_globals.h"
#include "display_mutex.h"

SemaphoreHandle_t g_tft_mutex = NULL;


// ============================================================================
// UTILITARIOS DE COR
// ============================================================================
uint16_t color_darker(uint16_t color, uint8_t shift) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    r = (r > shift) ? r - shift : 0;
    g = (g > shift*2) ? g - shift*2 : 0; // G tem 6 bits
    b = (b > shift) ? b - shift : 0;
    return (r << 11) | (g << 5) | b;
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// ============================================================================
// INICIALIZACAO
// ============================================================================
void graphics_init() {
    if (g_tft_mutex == NULL) {
        g_tft_mutex = xSemaphoreCreateRecursiveMutex();
    }

    LOCK_TFT();
    tft.init();
    tft.setRotation(1); // Landscape (Horizontal) - 320x240
    tft.fillScreen(C_BACKGROUND);
    UNLOCK_TFT();
    
    // Configura PWM do Backlight
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
    
    tftInitialized = true;
}

bool graphics_init_safe() {
    if (g_tft_mutex == NULL) {
        g_tft_mutex = xSemaphoreCreateRecursiveMutex();
    }

    #ifdef __EXCEPTIONS
    try {
    #endif
        LOCK_TFT();
        graphics_init(); // Verifica se a inicialização foi bem sucedida
        bool initOk = tftInitialized;
        if (initOk) {
            tft.setRotation(1); // Landscape (Horizontal) - 320x240
            tft.fillScreen(C_BACKGROUND);
            UNLOCK_TFT();
            
            // Configura PWM do Backlight
            pinMode(PIN_TFT_BL, OUTPUT);
            digitalWrite(PIN_TFT_BL, HIGH);
            
            tftInitialized = true;
            return true;
        }
        UNLOCK_TFT();
        return false;
    #ifdef __EXCEPTIONS
    } catch (...) {
        // Em caso de exceção, libera o mutex
        if (g_tft_mutex) {
            xSemaphoreGiveRecursive(g_tft_mutex);
        }
    }
    #endif
    
    // Fallback: inicialização mínima sem display
    tftInitialized = false;
    return false;
}

// ============================================================================
// ICONE: RESISTOR
// ============================================================================
// Desenha um resistor com corpo e faixas coloridas
void draw_icon_resistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t w = size;
    uint16_t h = size / 3;
    uint16_t leadLen = size / 6;

    tft.fillRect(x, y + h/2 - 2, leadLen, 4, color);
    tft.fillRect(x + w - leadLen, y + h/2 - 2, leadLen, 4, color);

    uint16_t bodyX = x + leadLen;
    uint16_t bodyW = w - leadLen * 2;

    tft.fillRoundRect(bodyX, y, bodyW, h, 4, color);

    uint16_t bandColors[] = {C_RESISTOR, C_RED, C_GREEN};
    uint8_t numBands = 3;
    uint16_t bandW = bodyW / (numBands + 1);

    for(uint8_t i = 0; i < numBands; i++) {
        int16_t bx = bodyX + bandW * (i + 1) - bandW/2;
        tft.fillRect(bx, y + 1, bandW - 1, h - 2, bandColors[i]);
    }
}

// ============================================================================
// ICONE: CAPACITOR CERAMICO
// ============================================================================
// Desenha um capacitor ceramico (disco/lente)
void draw_icon_capacitor_ceramic(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t leadLen = size / 5;
    uint16_t discR = size / 3;

    tft.fillRect(x + size/2 - leadLen/2, y, leadLen, size/4, color);
    tft.fillRect(x + size/2 - leadLen/2, y + size - size/4, leadLen, size/4, color);

    tft.fillCircle(x + size/2, y + size/2, discR, color);
}

// ============================================================================
// ICONE: CAPACITOR ELETROLITICO
// ============================================================================
// Desenha um capacitor eletrolitico (cilindro)
void draw_icon_capacitor_electro(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t w = size * 2 / 3;
    uint16_t h = size * 3 / 4;
    uint16_t leadLen = size / 4;

    tft.fillRect(x + w/4, y, leadLen, size/6, color);
    tft.fillRect(x + w/4 + w/2, y, leadLen, size/6, color);

    tft.fillRoundRect(x, y + size/6, w, h - size/6, 4, color);

    for(uint8_t i = 0; i < 3; i++) {
        tft.drawCircle(x + w/2, y + size/2 + i*4, w/4 - i*2, C_WHITE);
    }
}

// ============================================================================
// ICONE: DIODO
// ============================================================================
// Desenha um diodo (seta + barra)
void draw_icon_diode(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;
    uint16_t triSize = size / 2;

    tft.fillTriangle(cx, cy - triSize/2, cx, cy + triSize/2, cx + triSize/2, cy, color);
    tft.fillRect(cx - triSize/2, cy - triSize/4, triSize/4, triSize/2, color);
}

// ============================================================================
// ICONE: LED
// ============================================================================
// Desenha um LED com simbolo
void draw_icon_led(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;

    tft.fillCircle(cx, cy, size/3, color);

    int16_t triSize = size / 3;
    tft.fillTriangle(cx, cy - triSize, cx, cy + triSize/2, cx + triSize, cy, C_WHITE);
}

// ============================================================================
void draw_icon_transistor_npn(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;
    uint16_t len = size / 2;

    tft.drawLine(cx, cy - len, cx - len/2, cy, color);
    tft.drawLine(cx, cy + len, cx - len/2, cy, color);
    tft.drawLine(cx - len/2, cy, cx + len/2, cy, color);
}

void draw_icon_transistor_pnp(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    draw_icon_transistor_npn(x, y, size, color);
}

// ============================================================================
// ============================================================================
// ICONE: MOSFET
// ============================================================================

void draw_icon_mosfet_n(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;
    uint16_t len = size / 2;

    tft.drawLine(cx, cy - len, cx - len/2, cy, color);
    tft.drawLine(cx - len/2, cy, cx + len/2, cy, color);
    tft.fillRect(cx, cy - len/3 - 2, 4, 4, color);
}

void draw_icon_mosfet_p(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    draw_icon_mosfet_n(x, y, size, color);
}

// ============================================================================
// ICONE: INDUTOR
// ============================================================================
// Desenha um indutor (espirais)
void draw_icon_inductor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;
    uint16_t len = size * 2 / 3;

    tft.drawLine(x, cy, cx - len/2, cy, color);
    for(uint8_t i = 0; i < 3; i++) {
        tft.drawCircle(cx - len/2 + 5 + i*8, cy, 4, color);
    }
    tft.drawLine(cx + len/2, cy, x + size, cy, color);
}

// ============================================================================
// ICONE: CRISTAL
// ============================================================================
// Desenha um cristal (XTAL)
void draw_icon_crystal(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t w = size * 3 / 4;
    uint16_t h = size / 2;
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;

    tft.drawRect(x + (size-w)/2, y + (size-h)/2, w, h, color);
    tft.drawLine(cx - w/4, cy - h/4, cx + w/4, cy + h/4, color);
}

// ============================================================================
// ICONE: FUSIVEL
// ============================================================================
// Desenha um fusivel (tubo com fio)
void draw_icon_fuse(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t w = size * 2 / 3;
    uint16_t h = size / 3;
    uint16_t leadLen = size / 4;

    tft.fillRect(x, y + size/2 - 1, leadLen, 2, color);
    tft.fillRect(x + w - leadLen, y + size/2 - 1, leadLen, 2, color);
    tft.drawRoundRect(x + leadLen, y + (size-h)/2, w - leadLen*2, h, 2, color);
}

// ============================================================================
// ICONE: VARISTOR
// ============================================================================
// Desenha um varistor (simbolo semicondutor)
void draw_icon_varistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t w = size * 2 / 3;
    uint16_t h = size / 2;

    tft.drawRect(x, y + size/2 - h/2, w, h, color);
    tft.drawLine(x, y + size/2 - h/2, x + w, y + size/2 + h/2, color);
}

// ============================================================================
// ICONE: POTENCIOMETRO
// ============================================================================
// Desenha um potenciometro
void draw_icon_potentiometer(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t r = size / 3;
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;

    tft.fillCircle(cx, cy, r, color);
    tft.drawLine(cx, cy, cx, y, C_WHITE);
}

// ============================================================================
// ICONE: OPTOACOPLADOR
// ============================================================================
// Desenha um optoacoplador (LED + transistor)
void draw_icon_optocoupler(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawRect(x, y, size, size, color);
    tft.fillCircle(x + size/4, y + size/2, 4, color);
}

// ============================================================================
// ICONE: COMPONENTE DESCONHECIDO
// ============================================================================
// Desenha um ponto de interrogacao
void draw_icon_unknown(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    uint16_t cy = y + size/2;
    uint16_t r = size/3;

    tft.fillCircle(cx, cy, r, color);
    tft.setTextColor(C_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("?", cx, cy);
}

// ============================================================================


// Icones do Menu
void draw_icon_multimeter(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawRect(x + 4, y + 2, size - 8, size - 4, color);
    tft.drawRect(x + 8, y + 6, size - 16, 10, color); // Screen
    tft.fillCircle(x + size/2, y + size - 10, 6, color); // Dial
}

void draw_icon_temp(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.fillRoundRect(x + size/2 - 2, y + 2, 4, size - 8, 2, color);
    tft.fillCircle(x + size/2, y + size - 6, 6, color);
}

void draw_icon_history(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, size/2 - 2, color);
    tft.drawLine(x + size/2, y + size/2, x + size/2, y + size/4, color);
    tft.drawLine(x + size/2, y + size/2, x + size/2 + size/4, y + size/2, color);
}

void draw_icon_settings(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, size/4, color);
    for(int i=0; i<8; i++) {
        float a = i * 45 * PI / 180;
        tft.drawLine(x + size/2 + cos(a)*size/4, y + size/2 + sin(a)*size/4,
                    x + size/2 + cos(a)*size/2, y + size/2 + sin(a)*size/2, color);
    }
}

void draw_icon_about(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, size/2 - 2, color);
    tft.fillCircle(x + size/2, y + size/4 + 2, 2, color);
    tft.fillRect(x + size/2 - 1, y + size/2 - 2, 2, size/3, color);
}

void draw_icon_warning(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawTriangle(x + size/2, y + 2, x + 2, y + size - 2, x + size - 2, y + size - 2, color);
    tft.fillRect(x + size/2 - 1, y + size/3, 2, size/3, color);
    tft.fillCircle(x + size/2, y + size - 6, 2, color);
}

void draw_icon_voltage(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawLine(x + size/2, y + 2, x + size/4, y + size/2, color);
    tft.drawLine(x + size/4, y + size/2, x + size/2 + 2, y + size/2 - 2, color);
    tft.drawLine(x + size/2 + 2, y + size/2 - 2, x + size/2 - 2, y + size - 2, color);
}

void draw_component_icon(IconType icon, int16_t x, int16_t y, uint16_t color) {
    uint16_t size = 32; // Padrao
    switch(icon) {
        case ICON_RESISTOR: draw_icon_resistor(x, y, size, color); break;
        case ICON_CAPACITOR: draw_icon_capacitor_ceramic(x, y, size, color); break;
        case ICON_DIODE: draw_icon_diode(x, y, size, color); break;
        case ICON_LED: draw_icon_led(x, y, size, color); break;
        case ICON_TRANSISTOR_NPN: draw_icon_transistor_npn(x, y, size, color); break;
        case ICON_TRANSISTOR_PNP: draw_icon_transistor_pnp(x, y, size, color); break;
        case ICON_INDUCTOR: draw_icon_inductor(x, y, size, color); break;
        case ICON_MULTIMETER: draw_icon_multimeter(x, y, size, color); break;
        case ICON_TEMP: draw_icon_temp(x, y, size, color); break;
        case ICON_HISTORY: draw_icon_history(x, y, size, color); break;
        case ICON_SETTINGS: draw_icon_settings(x, y, size, color); break;
        case ICON_ABOUT: draw_icon_about(x, y, size, color); break;
        case ICON_WARNING: draw_icon_warning(x, y, size, color); break;
        case ICON_VOLTAGE: draw_icon_voltage(x, y, size, color); break;
        default: draw_icon_unknown(x, y, size, color); break;
    }
}

void clear_screen() {
    LOCK_TFT();
    tft.fillScreen(C_BACKGROUND);
    UNLOCK_TFT();
}

void draw_status_bar(const char* title, const char* subtitle) {
    LOCK_TFT();
    tft.fillRect(0, 0, SCREEN_W, STATUS_BAR_H, C_SURFACE);
    tft.setTextColor(C_TEXT);
    tft.setFreeFont(FONT_NORMAL);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(title, 10, STATUS_BAR_H/2);
    
    if(subtitle) {
        tft.setTextDatum(MR_DATUM);
        tft.setTextColor(C_PRIMARY);
        tft.drawString(subtitle, SCREEN_W - 10, STATUS_BAR_H/2);
    }
    
    tft.drawLine(0, STATUS_BAR_H, SCREEN_W, STATUS_BAR_H, C_PRIMARY);
    UNLOCK_TFT();
}

void draw_footer(const char* left, const char* right) {
    LOCK_TFT();
    tft.fillRect(0, SCREEN_H - FOOTER_H, SCREEN_W, FOOTER_H, C_SURFACE);
    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setFreeFont(FONT_SMALL);
    
    if(left) {
        tft.setTextDatum(ML_DATUM);
        tft.drawString(left, 10, SCREEN_H - FOOTER_H/2);
    }
    
    if(right) {
        tft.setTextDatum(MR_DATUM);
        tft.drawString(right, SCREEN_W - 10, SCREEN_H - FOOTER_H/2);
    }
    UNLOCK_TFT();
}

void draw_progress_bar(int16_t x, int16_t y, uint16_t w, uint16_t h, float percent, uint16_t color) {
    LOCK_TFT();
    tft.drawRect(x, y, w, h, C_BORDER);
    int16_t fillW = (int16_t)((w - 2) * percent / 100.0f);
    if(fillW > 0) {
        tft.fillRect(x + 1, y + 1, fillW, h - 2, color);
    }
    UNLOCK_TFT();
}

void draw_status_indicator(MeasurementStatus status, int16_t x, int16_t y, uint16_t size) {
    uint16_t color = C_GREY;
    if(status == STATUS_GOOD) color = C_GREEN;
    else if(status == STATUS_WARNING || status == STATUS_SUSPECT) color = C_YELLOW;
    else if(status == STATUS_BAD) color = C_RED;
    else if(status == STATUS_SHORT) color = C_ORANGE;
    else if(status == STATUS_OPEN) color = C_BLUE;
    
    LOCK_TFT();
    tft.fillCircle(x, y, size/2, color);
    tft.drawCircle(x, y, size/2 + 2, C_WHITE);
    UNLOCK_TFT();
}

void draw_splash_screen() {
    LOCK_TFT();
    tft.fillScreen(C_BACKGROUND);
    tft.setFreeFont(FM12);
    tft.setTextColor(C_PRIMARY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("SONDVOLT", SCREEN_W/2, SCREEN_H/2 - 20);
    
    tft.setFreeFont(FM9);
    tft.setTextColor(C_TEXT);
    tft.drawString("Component Tester PRO", SCREEN_W/2, SCREEN_H/2 + 20);
    
    tft.setTextColor(C_TEXT_SECONDARY);
    tft.drawString("v3.0.0", SCREEN_W/2, SCREEN_H/2 + 50);
    UNLOCK_TFT();
}
