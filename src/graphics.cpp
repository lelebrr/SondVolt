// ============================================================================
// Sondvolt v3.2 — Gráficos e Interface (Desenho Manual 5x7)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================

#include "graphics.h"
#include "display_globals.h"
#include "fonts.h" 
#include "icons_bitmap.h"
#include "logo_bitmap.h"
#include "display_mutex.h"

void graphics_draw_header(const char* title) {
    LOCK_TFT();
    // Cabeçalho com degradê escuro
    tft.fillRect(0, 0, 320, 32, 0x1082); // Cinza escuro profundo
    tft.fillRect(0, 32, 320, 3, COLOR_PRIMARY); // Barra de destaque colorida
    
    // Texto alinhado totalmente à direita
    int16_t textWidth = strlen(title) * 6 * 2;
    draw_text_5x7(tft, 320 - textWidth - 15, 10, title, TFT_WHITE, 2);
    
    // Se não for o menu principal, desenha o botão voltar integrado
    if (strcmp(title, "MENU PRINCIPAL") != 0) {
        tft.fillRoundRect(5, 5, 25, 22, 4, COLOR_BAD);
        draw_text_5x7(tft, 12, 8, "<", TFT_WHITE, 1);
    }
    
    UNLOCK_TFT();
}

void graphics_draw_button(int16_t x, int16_t y, int16_t w, int16_t h, const char* label, uint16_t color) {
    tft.fillRoundRect(x, y, w, h, 8, COLOR_SURFACE);
    tft.drawRoundRect(x, y, w, h, 8, color);
    
    // Desenho Manual 5x7
    int16_t textLen = strlen(label) * 6 * 2;
    draw_text_5x7(tft, x + (w - textLen)/2, y + (h - 14)/2, label, TFT_WHITE, 2);
}

void graphics_draw_splash() {
    tft.fillScreen(TFT_BLACK);
    
    // Desenho Manual 5x7
    draw_text_5x7(tft, 320/2 - (8*6*4)/2, 100, "SONDVOLT", COLOR_PRIMARY, 4);
    draw_text_5x7(tft, 320/2 - (16*6*2)/2, 150, "Component Tester", TFT_WHITE, 2);
}

void graphics_draw_back_button() {
    // Botão Voltar menor e mais discreto
    tft.fillRoundRect(5, 5, 25, 22, 4, COLOR_BAD);
    draw_text_5x7(tft, 12, 8, "<", TFT_WHITE, 1);
}

void draw_component_icon(IconType type, int16_t x, int16_t y, uint16_t color) {
    uint16_t size = 32; 
    switch (type) {
        case ICON_RESISTOR:  draw_icon_resistor(x, y, size, color); break;
        case ICON_CAPACITOR: draw_icon_capacitor(x, y, size, color); break;
        case ICON_DIODE:     draw_icon_diode(x, y, size, color); break;
        case ICON_LED:       draw_icon_led(x, y, size, color); break;
        case ICON_TRANSISTOR_NPN: 
        case ICON_TRANSISTOR_PNP: draw_icon_transistor(x, y, size, color); break;
        case ICON_INDUCTOR:  draw_icon_inductor(x, y, size, color); break;
        case ICON_MULTIMETER: draw_icon_multimeter(x, y, size, color); break;
        case ICON_TEMP:       draw_icon_temp(x, y, size, color); break;
        case ICON_HISTORY:    draw_icon_history(x, y, size, color); break;
        case ICON_SETTINGS:   draw_icon_settings(x, y, size, color); break;
        case ICON_ABOUT:      draw_icon_about(x, y, size, color); break;
        default:
            tft.drawRect(x, y, size, size, color);
            tft.drawLine(x, y, x+size, y+size, color);
            break;
    }
}

void draw_icon_resistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t h = size / 3;
    tft.drawRect(x, y + h, size, h, color);
}

void draw_icon_capacitor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    tft.drawFastVLine(cx - 2, y, size, color);
    tft.drawFastVLine(cx + 2, y, size, color);
}

void draw_icon_diode(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawTriangle(x, y, x, y + size, x + size, y + size/2, color);
    tft.drawFastVLine(x + size, y, size, color);
}

void draw_icon_led(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    draw_icon_diode(x, y, size, color);
    tft.drawLine(x + size/2, y - 5, x + size, y - 10, color);
}

void draw_icon_transistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawFastVLine(x + 5, y, size, color);
    tft.drawLine(x + 5, y + size/2, x + size, y, color);
    tft.drawLine(x + 5, y + size/2, x + size, y + size, color);
}

void draw_icon_inductor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    for(int i=0; i<3; i++) {
        tft.drawCircle(x + 8 + i*8, y + size/2, 5, color);
    }
}
void draw_icon_multimeter(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawRect(x + 5, y, size - 10, size, color);
    tft.fillCircle(x + size/2, y + size/2, 5, color);
    tft.drawLine(x + size/2, y + size/2, x + size/2 + 4, y + size/2 - 4, color);
}

void draw_icon_temp(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size - 8, 6, color);
    tft.fillRect(x + size/2 - 2, y + 2, 4, size - 10, color);
}

void draw_icon_history(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, size/2 - 2, color);
    tft.drawLine(x + size/2, y + size/2, x + size/2, y + 8, color);
    tft.drawLine(x + size/2, y + size/2, x + size/2 + 6, y + size/2, color);
}

void draw_icon_settings(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, 6, color);
    for(int i=0; i<8; i++) {
        float angle = i * 45 * 0.0174532925f;
        int16_t x1 = x + size/2 + cos(angle) * 7;
        int16_t y1 = y + size/2 + sin(angle) * 7;
        int16_t x2 = x + size/2 + cos(angle) * 12;
        int16_t y2 = y + size/2 + sin(angle) * 12;
        tft.drawLine(x1, y1, x2, y2, color);
    }
}

void draw_icon_about(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawCircle(x + size/2, y + size/2, size/2 - 2, color);
    tft.fillRect(x + size/2 - 1, y + size/2 - 2, 2, 8, color);
    tft.fillCircle(x + size/2, y + size/2 - 6, 2, color);
}
void draw_bitmap_icon(IconType type, int16_t x, int16_t y) {
    const uint16_t* bitmap = nullptr;
    
    switch (type) {
        case ICON_RESISTOR:  bitmap = icon_resistor_32x32; break;
        case ICON_CAPACITOR: bitmap = icon_capacitor_32x32; break;
        case ICON_DIODE:     bitmap = icon_diode_32x32; break;
        case ICON_LED:       bitmap = icon_led_32x32; break;
        case ICON_TRANSISTOR_NPN:
        case ICON_TRANSISTOR_PNP: bitmap = icon_transistor_32x32; break;
        case ICON_INDUCTOR:  bitmap = icon_inductor_32x32; break;
        case ICON_MULTIMETER: bitmap = icon_multimeter_32x32; break;
        case ICON_TEMP:       bitmap = icon_temp_32x32; break;
        case ICON_HISTORY:    bitmap = icon_history_32x32; break;
        case ICON_SETTINGS:   bitmap = icon_settings_32x32; break;
        case ICON_ABOUT:      bitmap = icon_about_32x32; break;
        case ICON_AUTO:       bitmap = icon_auto_32x32; break;
        default: break;
    }

    if (bitmap) {
        LOCK_TFT();
        tft.pushImage(x, y, 32, 32, bitmap);
        UNLOCK_TFT();
    }
}

void draw_logo_full() {
    LOCK_TFT();
    tft.fillScreen(TFT_BLACK);
    int16_t y_centered = (240 - logo_full_height) / 2;
    tft.pushImage(0, y_centered, logo_full_width, logo_full_height, logo_full_bitmap);
    UNLOCK_TFT();
}

void draw_logo_small(int16_t x, int16_t y) {
    LOCK_TFT();
    tft.pushImage(x, y, logo_small_width, logo_small_height, logo_small_bitmap);
    UNLOCK_TFT();
}
