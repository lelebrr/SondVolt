// ============================================================================
// Sondvolt v3.2 — Gráficos e Interface (Desenho Manual 5x7)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================

#include "globals.h"
#include "graphics.h"
#include "display_globals.h"
#include "fonts.h" 
#include "icons_bitmap.h"
#include "logo_bitmap.h"
#include "display_mutex.h"
#include "visual.h"
#include "theme.h"


void graphics_draw_header(const char* title) {
    LOCK_TFT();
    // Cabeçalho Moderno com Gradiente e Glow Inferior
    for(int i=0; i<V_HEADER_H; i++) {
        uint16_t lineCol = color_mix(V_DEEP_BLUE, V_BG_DARK, (i * 255) / V_HEADER_H);
        tft.drawFastHLine(0, i, 320, lineCol);
    }
    // Linha Neon de separação com glow sutil
    tft.drawFastHLine(0, V_HEADER_H - 1, 320, V_CYAN_ELECTRIC);
    tft.drawFastHLine(0, V_HEADER_H - 2, 320, color_mix(V_CYAN_ELECTRIC, V_BG_DARK, 180));
    
    // Texto com sombra suave (evita efeito de "texto duplicado")
    uint8_t titleSize = 2;
    int16_t textWidth = strlen(title) * 6 * titleSize;
    if (textWidth > 255) {
        titleSize = 1;
        textWidth = strlen(title) * 6 * titleSize;
    }

    int16_t titleX = 320 - textWidth - 15;
    int16_t titleY = (titleSize == 2) ? 12 : 16;
    uint16_t shadowColor = color_mix(V_BG_DARK, V_PURE_WHITE, 235);

    draw_text_5x7(tft, titleX + 1, titleY + 1, title, shadowColor, titleSize); // Sombra curta/sutil
    draw_text_5x7(tft, titleX, titleY, title, V_PURE_WHITE, titleSize); // Principal
    
    // Botão Voltar Integrado (Se não for Home/Menu)
    if (strcmp(title, "MENU PRINCIPAL") != 0 && strcmp(title, "SONDVOLT HOME") != 0) {
        tft.fillRoundRect(8, 8, 38, 26, 6, V_ALERT);
        tft.drawRoundRect(8, 8, 38, 26, 6, V_PURE_WHITE);
        draw_text_5x7(tft, 19, 13, "<", V_PURE_WHITE, 2);
        
    }
    UNLOCK_TFT();
}

void graphics_draw_button(int16_t x, int16_t y, int16_t w, int16_t h, const char* label, uint16_t color) {
    // Sombra Neon (Glow suave na base)
    tft.fillRoundRect(x + 1, y + 1, w, h + 2, V_RADIUS_MD, color_mix(color, V_BG_DARK, 200));
    
    // Corpo do Botão com Borda Dupla
    tft.fillRoundRect(x, y, w, h, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(x, y, w, h, V_RADIUS_MD, color);
    tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, V_RADIUS_MD, color_mix(color, V_BG_DARK, 100));
    
    // Texto Centralizado
    int16_t textLen = strlen(label) * 6;
    draw_text_5x7(tft, x + (w - textLen)/2, y + (h - 7)/2, label, V_PURE_WHITE, 1);
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
void graphics_draw_help_button() {
    LOCK_TFT();
    // Botão "i" com brilho neon
    tft.fillCircle(295, 20, 14, V_BG_SURFACE); 
    tft.drawCircle(295, 20, 14, V_CYAN_ELECTRIC);
    // Efeito de brilho externo (simulado)
    tft.drawCircle(295, 20, 15, color_mix(V_CYAN_ELECTRIC, V_BG_DARK, 150));
    
    draw_text_5x7(tft, 292, 14, "i", V_CYAN_ELECTRIC, 2);
    UNLOCK_TFT();
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
        case ICON_CONTINUITY: draw_icon_continuity(x, y, size, color); break;
        default:
            tft.drawRect(x, y, size, size, color);
            tft.drawLine(x, y, x+size, y+size, color);
            break;
    }
}

void draw_icon_resistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t h = size / 3;
    tft.fillRect(x, y + h, size, h, color); // Preenchido para traço grosso
    tft.drawRect(x - 1, y + h - 1, size + 2, h + 2, V_PURE_WHITE); // Borda de destaque
}

void draw_icon_capacitor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    uint16_t cx = x + size/2;
    tft.fillRect(cx - 4, y, 3, size, color); // Placa 1 grossa
    tft.fillRect(cx + 1, y, 3, size, color); // Placa 2 grossa
}

void draw_icon_diode(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.fillTriangle(x, y, x, y + size, x + size - 4, y + size/2, color);
    tft.fillRect(x + size - 3, y, 4, size, color); // Barra grossa
}

void draw_icon_led(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    draw_icon_diode(x, y, size, color);
    // Setas de brilho mais grossas
    tft.drawLine(x + size/2, y - 4, x + size, y - 9, V_PURE_WHITE);
    tft.drawLine(x + size/2 + 2, y - 4, x + size + 2, y - 9, V_PURE_WHITE);
}

void draw_icon_transistor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.fillRect(x + 2, y, 4, size, color); // Base grossa
    tft.drawLine(x + 6, y + size/2, x + size, y, color);
    tft.drawLine(x + 7, y + size/2, x + size + 1, y, color); // Linha dupla
    tft.drawLine(x + 6, y + size/2, x + size, y + size, color);
    tft.drawLine(x + 7, y + size/2, x + size + 1, y + size, color); // Linha dupla
}

void draw_icon_inductor(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    for(int i=0; i<3; i++) {
        tft.drawCircle(x + 8 + i*8, y + size/2, 5, color);
        tft.drawCircle(x + 8 + i*8, y + size/2, 4, color); // Linha dupla interna
    }
}
void draw_icon_multimeter(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    tft.drawRoundRect(x + 3, y, size - 6, size, 4, color);
    tft.drawRoundRect(x + 4, y + 1, size - 8, size - 2, 3, color); // Borda dupla
    tft.fillCircle(x + size/2, y + size/2 + 4, 6, color);
    tft.drawLine(x + size/2, y + size/2 + 4, x + size/2 + 5, y + size/2 - 2, V_PURE_WHITE);
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

void draw_icon_continuity(int16_t x, int16_t y, uint16_t size, uint16_t color) {
    const int16_t yMid = y + size / 2;

    // Terminais
    tft.fillCircle(x + 3, yMid, 3, color);
    tft.fillCircle(x + size - 3, yMid, 3, color);

    // Linha da esquerda até a onda
    tft.drawLine(x + 6, yMid, x + 10, yMid, color);

    // Onda tipo "continuidade" (zig-zag suave)
    tft.drawLine(x + 10, yMid, x + 14, yMid - 5, color);
    tft.drawLine(x + 14, yMid - 5, x + 18, yMid + 5, color);
    tft.drawLine(x + 18, yMid + 5, x + 22, yMid - 5, color);
    tft.drawLine(x + 22, yMid - 5, x + 26, yMid + 5, color);

    // Linha da onda até o terminal direito
    tft.drawLine(x + 26, yMid, x + size - 6, yMid, color);

    // Indicador sonoro (beep)
    tft.drawCircle(x + size/2, yMid - 10, 3, color);
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
        case ICON_CONTINUITY:
            LOCK_TFT();
            draw_icon_continuity(x, y, 32, V_CYAN_ELECTRIC);
            UNLOCK_TFT();
            return;
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
