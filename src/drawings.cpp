// ============================================================================
// Sondvolt v3.1 — Component Drawings Implementation
// All icons drawn with vector primitives (no bitmaps)
// ============================================================================

#include "drawings.h"
#include "graphics.h"

// ============================================================================
// MAIN COMPONENT DRAWING FUNCTION
// ============================================================================

void draw_component_by_type(uint8_t type, int16_t x, int16_t y, uint16_t color) {
    switch (type) {
        case COMP_RESISTOR:        draw_resistor(x, y, color); break;
        case COMP_CAPACITOR:     draw_capacitor(x, y, color); break;
        case COMP_CAP_ELECTRO:   draw_capacitor_electrolytic(x, y, color); break;
        case COMP_CAP_CERAMIC:   draw_capacitor_ceramic(x, y, color); break;
        case COMP_DIODE:         draw_diode(x, y, color); break;
        case COMP_LED:         draw_led(x, y, color, false); break;
        case COMP_LED_RGB:     draw_led(x, y, color, true); break;
        case COMP_ZENER:       draw_zener(x, y, color); break;
        case COMP_SCHOTTKY:     draw_schottky(x, y, color); break;
        case COMP_TRANSISTOR_NPN: draw_transistor_npn(x, y, color); break;
        case COMP_TRANSISTOR_PNP: draw_transistor_pnp(x, y, color); break;
        case COMP_MOSFET_N:    draw_mosfet_n(x, y, color); break;
        case COMP_MOSFET_P:   draw_mosfet_p(x, y, color); break;
        case COMP_INDUCTOR:     draw_inductor(x, y, color); break;
        case COMP_CRYSTAL:      draw_crystal(x, y, color); break;
        case COMP_FUSE:       draw_fuse(x, y, color); break;
        case COMP_RELAY:       draw_relay(x, y, color); break;
        case COMP_IC:         draw_ic(x, y, color); break;
        case COMP_OPTO:      draw_optoisolator(x, y, color); break;
        case COMP_TRIAC:    draw_triac(x, y, color); break;
        case COMP_SCR:       draw_scr(x, y, color); break;
        default:            draw_resistor(x, y, color); break;
    }
}

// ============================================================================
// RESISTOR - IEC Zigzag Symbol
// ============================================================================

void draw_resistor(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Lead wires
    tft.drawLine(x, cy, x + 4, color);
    tft.drawLine(x + 36, cy, x + 40, color);
    
    // Zigzag - 6 peaks
    tft.drawLine(x + 4, cy, x + 6, cy, color);
    tft.drawLine(x + 6, cy, x + 8, cy - 6, color);
    tft.drawLine(x + 8, cy - 6, x + 11, cy + 6, color);
    tft.drawLine(x + 11, cy + 6, x + 14, cy - 6, color);
    tft.drawLine(x + 14, cy - 6, x + 17, cy + 6, color);
    tft.drawLine(x + 17, cy + 6, x + 20, cy - 6, color);
    tft.drawLine(x + 20, cy - 6, x + 23, cy + 6, color);
    tft.drawLine(x + 23, cy + 6, x + 26, cy - 6, color);
    tft.drawLine(x + 26, cy - 6, x + 29, cy + 6, color);
    tft.drawLine(x + 29, cy + 6, x + 32, cy, color);
    tft.drawLine(x + 32, cy, x + 36, cy, color);
    
    // Color bands (decorative)
    tft.fillRect(x + 8, cy - 8, 3, 16, color_darker(color, 1));
    tft.fillRect(x + 14, cy - 8, 3, 16, C_RED);
    tft.fillRect(x + 20, cy - 8, 3, 16, C_GREEN);
}

// ============================================================================
// CAPACITOR - Parallel Plates
// ============================================================================

void draw_capacitor(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Leads
    tft.drawLine(x, cy, x + 6, color);
    tft.drawLine(x + 34, cy, x + 40, color);
    
    // Left plate
    tft.drawLine(cx - 10, cy - 12, cx - 10, cy + 12, color);
    tft.drawLine(cx - 6, cy - 12, cx - 6, cy + 12, color);
    tft.fillRect(cx - 10, cy - 12, 4, 24, color);
    
    // Right plate
    tft.drawLine(cx + 2, cy - 12, cx + 2, cy + 12, color);
    tft.drawLine(cx + 6, cy - 12, cx + 6, cy + 12, color);
    tft.fillRect(cx + 2, cy - 12, 4, 24, color);
}

// ============================================================================
// ELECTROLYTIC CAPACITOR - Cylinder with + Terminal
// ============================================================================

void draw_capacitor_electrolytic(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Can body
    tft.fillRoundRect(x + 8, cy - 14, 24, 28, 4, color);
    
    // Top mark
    tft.fillRect(x + 10, cy - 14, 20, 4, color_darker(color, 1));
    
    // Polarity symbol (-)
    tft.fillRect(x + 18, cy - 2, 4, 4, C_WHITE);
    
    // + terminal lead
    tft.drawLine(x + 2, cy - 14, x + 8, cy - 14, color);
    tft.drawLine(x + 32, cy - 14, x + 38, cy - 14, color);
    
    // - terminal lead
    tft.drawLine(x + 2, cy + 14, x + 8, cy + 14, color);
    tft.drawLine(x + 32, cy + 14, x + 38, cy + 14, color);
    
    // Plus sign
    tft.setTextColor(C_WHITE);
    tft.setTextSize(2);
    tft.setCursor(x + 6, y + 8);
    tft.print("+");
}

// ============================================================================
// CERAMIC CAPACITOR - Disc Type
// ============================================================================

void draw_capacitor_ceramic(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Leads
    tft.drawLine(x, cy, x + 6, color);
    tft.drawLine(x + 34, cy, x + 40, color);
    
    // Disc body
    tft.fillCircle(cx, cy, 10, color);
    tft.fillCircle(cx, cy, 6, C_WHITE);
    tft.drawCircle(cx, cy, 4, color);
}

// ============================================================================
// DIODE - Triangle + Bar
// ============================================================================

void draw_diode(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Leads
    tft.drawLine(x, cy, x + 4, color);
    tft.drawLine(x + 36, cy, x + 40, color);
    
    // Triangle (anode)
    tft.fillTriangle(cx - 12, cy - 8, cx - 12, cy + 8, cx + 2, cy, color);
    
    // Bar (cathode)
    tft.fillRect(cx + 2, cy - 10, 4, 20, color);
}

// ============================================================================
// LED - With Light Rays
// ============================================================================

void draw_led(int16_t x, int16_t y, uint16_t color, bool rgb) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Leads
    tft.drawLine(x + 2, cy, x + 8, color);
    tft.drawLine(x + 32, cy, x + 38, color);
    
    // LED dome
    tft.fillCircle(cx, cy, 8, color);
    tft.fillCircle(cx - 2, cy - 2, 4, color_brighter(color, 2));
    
    // Light rays
    tft.drawLine(cx, cy - 10, cx, cy - 16, color);
    tft.drawLine(cx - 7, cy - 7, cx - 11, cy - 12, color);
    tft.drawLine(cx + 7, cy - 7, cx + 11, cy - 12, color);
    tft.drawLine(cx - 10, cy, cx - 16, cy, color);
    tft.drawLine(cx + 10, cy, cx + 16, cy, color);
    tft.drawLine(cx - 7, cy + 7, cx - 11, cy + 12, color);
    tft.drawLine(cx + 7, cy + 7, cx + 11, cy + 12, color);
    
    // RGB dots
    if (rgb) {
        tft.fillCircle(cx - 4, cy + 3, 2, C_RED);
        tft.fillCircle(cx, cy + 3, 2, C_GREEN);
        tft.fillCircle(cx + 4, cy + 3, 2, C_BLUE);
    }
}

// ============================================================================
// ZENER DIODE - With Z Mark
// ============================================================================

void draw_zener(int16_t x, int16_t y, uint16_t color) {
    draw_diode(x, y, color);
    
    // Z marker
    tft.setTextColor(C_WHITE);
    tft.setTextSize(1);
    tft.setCursor(x + 14, y + 16);
    tft.print("Z");
}

// ============================================================================
// SCHOTTKY DIODE - With S Mark
// ============================================================================

void draw_schottky(int16_t x, int16_t y, uint16_t color) {
    draw_diode(x, y, color);
    
    // S marker
    tft.setTextColor(C_WHITE);
    tft.setTextSize(1);
    tft.setCursor(x + 14, y + 16);
    tft.print("S");
}

// ============================================================================
// TRANSISTOR NPN - Standard Symbol
// ============================================================================

void draw_transistor_npn(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Emitter with arrow OUT
    tft.drawLine(x, cy, x + 14, color);
    tft.drawLine(x + 10, cy - 4, x + 14, cy, color);
    tft.drawLine(x + 10, cy + 4, x + 14, cy, color);
    
    // Collector
    tft.drawLine(x + 26, cy, x + 40, color);
    
    // Base lead
    tft.drawLine(cx, cy - 12, cx, cy + 12, color);
    
    // Base connections
    tft.drawLine(cx - 6, cy - 6, cx, cy, color);
    tft.drawLine(cx - 6, cy + 6, cx, cy, color);
    
    // Enclosure circle
    tft.drawCircle(cx, cy, 16, color);
}

// ============================================================================
// TRANSISTOR PNP - Arrow Inverted
// ============================================================================

void draw_transistor_pnp(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Emitter with arrow IN
    tft.drawLine(x, cy, x + 14, color);
    tft.drawLine(x + 14, cy - 4, x + 10, cy, color);
    tft.drawLine(x + 14, cy + 4, x + 10, cy, color);
    
    // Collector
    tft.drawLine(x + 26, cy, x + 40, color);
    
    // Base lead
    tft.drawLine(cx, cy - 12, cx, cy + 12, color);
    
    // Base connections
    tft.drawLine(cx - 6, cy - 6, cx, cy, color);
    tft.drawLine(cx - 6, cy + 6, cx, cy, color);
    
    // Enclosure circle
    tft.drawCircle(cx, cy, 16, color);
}

// ============================================================================
// MOSFET N-CHANNEL - Three Lines
// ============================================================================

void draw_mosfet_n(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Source
    tft.drawLine(x + 2, cy - 4, x + 14, cy - 4, color);
    tft.drawLine(x + 2, cy + 4, x + 14, cy + 4, color);
    tft.drawLine(x + 2, cy - 4, x + 2, cy + 4, color);
    
    // Drain
    tft.drawLine(x + 26, cy - 4, x + 38, cy - 4, color);
    tft.drawLine(x + 26, cy + 4, x + 38, cy + 4, color);
    tft.drawLine(x + 38, cy - 4, x + 38, cy + 4, color);
    
    // Channel lines
    tft.drawLine(x + 16, cy - 4, x + 24, cy - 4, color);
    tft.drawLine(x + 16, cy, x + 24, cy, color);
    tft.drawLine(x + 16, cy + 4, x + 24, cy + 4, color);
    
    // Gate
    tft.drawLine(x + 16, cy - 12, x + 24, cy - 12, color);
    tft.drawLine(x + 16, cy + 12, x + 24, cy + 12, color);
    tft.drawLine(x + 16, cy - 12, x + 16, cy + 12, color);
    
    // Body connection
    tft.drawLine(x + 10, cy + 4, x + 10, cy + 12, color);
    tft.drawLine(x + 2, cy + 12, x + 18, cy + 12, color);
}

// ============================================================================
// MOSFET P-CHANNEL
// ============================================================================

void draw_mosfet_p(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Source
    tft.drawLine(x + 2, cy - 4, x + 14, cy - 4, color);
    tft.drawLine(x + 2, cy + 4, x + 14, cy + 4, color);
    tft.drawLine(x + 2, cy - 4, x + 2, cy + 4, color);
    
    // Drain
    tft.drawLine(x + 26, cy - 4, x + 38, cy - 4, color);
    tft.drawLine(x + 26, cy + 4, x + 38, cy + 4, color);
    tft.drawLine(x + 38, cy - 4, x + 38, cy + 4, color);
    
    // Channel with circles
    tft.drawLine(x + 16, cy - 6, x + 24, cy - 6, color);
    tft.drawLine(x + 16, cy, x + 24, cy, color);
    tft.drawLine(x + 16, cy + 6, x + 24, cy + 6, color);
    
    // Gate (inverted)
    tft.drawLine(x + 16, cy - 12, x + 24, cy - 12, color);
    tft.drawLine(x + 16, cy + 12, x + 24, cy + 12, color);
    tft.drawLine(x + 24, cy - 12, x + 24, cy + 12, color);
    
    // Body
    tft.drawLine(x + 10, cy - 4, x + 10, cy - 12, color);
    tft.drawLine(x + 2, cy - 12, x + 18, cy - 12, color);
}

// ============================================================================
// INDUCTOR - Coils
// ============================================================================

void draw_inductor(int16_t x, int16_t y, uint16_t color) {
    int16_t cy = y + 20;
    
    // Lead
    tft.drawLine(x, cy, x + 4, color);
    
    // Coils (semicircles)
    tft.drawArc(x + 6, cy, 4, 4, 180, 0, color);
    tft.drawArc(x + 14, cy, 4, 4, 180, 0, color);
    tft.drawArc(x + 22, cy, 4, 4, 180, 0, color);
    tft.drawArc(x + 30, cy, 4, 4, 180, 0, color);
    
    // Lead
    tft.drawLine(x + 36, cy, x + 40, color);
}

// ============================================================================
// CRYSTAL - Two Plates
// ============================================================================

void draw_crystal(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Leads
    tft.drawLine(x + 2, cy, x + 8, color);
    tft.drawLine(x + 32, cy, x + 38, color);
    
    // Body
    tft.fillRect(cx - 10, cy - 6, 20, 12, color);
    tft.fillRect(cx - 6, cy - 10, 12, 4, color);
    tft.fillRect(cx - 6, cy + 6, 12, 4, color);
    
    // Internal plates
    tft.fillRect(cx - 6, cy - 2, 4, 4, C_WHITE);
    tft.fillRect(cx + 2, cy - 2, 4, 4, C_WHITE);
}

// ============================================================================
// FUSE - Wire in Tube
// ============================================================================

void draw_fuse(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // End caps
    tft.fillRect(x + 2, cy - 6, 6, 12, color);
    tft.fillRect(x + 32, cy - 6, 6, 12, color);
    
    // Glass tube
    tft.fillRect(x + 8, cy - 4, 24, 8, color_lerp(color, C_WHITE, 0.3f));
    tft.drawRect(x + 8, cy - 4, 24, 8, color);
    
    // Element wire
    tft.drawLine(x + 12, cy, x + 28, cy, color);
}

// ============================================================================
// RELAY - Coil + Contacts
// ============================================================================

void draw_relay(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Coil
    tft.drawLine(x + 2, cy, x + 6, color);
    tft.drawArc(x + 8, cy, 3, 3, 180, 0, color);
    tft.drawArc(x + 13, cy, 3, 3, 180, 0, color);
    tft.drawArc(x + 18, cy, 3, 3, 180, 0, color);
    tft.drawLine(x + 21, cy, x + 25, color);
    
    // Contact 1 (NC)
    tft.drawLine(x + 28, cy - 10, x + 36, cy - 6, color);
    tft.drawLine(x + 28, cy - 10, x + 36, cy - 14, color);
    
    // Contact 2 (NO)
    tft.drawLine(x + 28, cy + 10, x + 36, cy + 6, color);
    tft.drawLine(x + 28, cy + 4, x + 34, cy + 10, color);
}

// ============================================================================
// IC - Chip with Pins
// ============================================================================

void draw_ic(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Body
    tft.fillRoundRect(x + 8, cy - 12, 24, 24, 3, color);
    
    // Notch
    tft.fillTriangle(cx, cy - 12, cx - 3, cy - 9, cx + 3, cy - 9, C_DARK);
    
    // Pins (4 on each side)
    for (int i = 0; i < 4; i++) {
        tft.drawLine(x + 4, cy - 8 + i * 5, x + 8, cy - 8 + i * 5, color);
        tft.drawLine(x + 32, cy - 8 + i * 5, x + 36, cy - 8 + i * 5, color);
    }
    
    // Dot
    tft.fillCircle(cx + 6, cy + 6, 2, C_WHITE);
}

// ============================================================================
// OPTOISOLATOR - LED + Transistor
// ============================================================================

void draw_optoisolator(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Housing (split)
    tft.drawLine(x + 8, y + 4, x + 8, y + 36, color);
    tft.drawLine(x + 32, y + 4, x + 32, y + 36, color);
    tft.drawLine(x + 8, y + 4, x + 32, y + 4, color);
    tft.drawLine(x + 8, y + 36, x + 32, y + 36, color);
    
    // LED side
    tft.fillTriangle(cx - 8, cy - 6, cx - 8, cy + 6, cx - 2, cy, color);
    tft.drawLine(x + 2, cy, x + 8, cy, color);
    tft.drawLine(x + 20, cy, x + 40, cy, color);
    
    // Transistor side (NPN)
    tft.drawLine(cx + 4, cy - 12, cx + 4, cy + 12, color);
    tft.drawLine(cx, cy, cx + 4, cy - 4, color);
    tft.drawLine(cx, cy, cx + 4, cy + 4, color);
}

// ============================================================================
// TRIAC - Bidirectional Thyristor
// ============================================================================

void draw_triac(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // MT1 to MT2 path
    tft.drawLine(x, cy + 6, x + 40, cy + 6, color);
    tft.drawLine(x, cy - 6, x + 40, cy - 6, color);
    
    // Gate
    tft.drawLine(x + 8, cy, x + 18, cy, color);
    tft.drawLine(x + 18, cy, x + 24, cy - 6, color);
    tft.drawLine(x + 18, cy, x + 24, cy + 6, color);
    
    // Arrows (bidirectional)
    tft.fillTriangle(x + 28, cy, x + 28, cy - 4, x + 32, cy, color);
    tft.fillTriangle(x + 28, cy, x + 28, cy + 4, x + 24, cy, color);
}

// ============================================================================
// SCR - Silicon Controlled Rectifier
// ============================================================================

void draw_scr(int16_t x, int16_t y, uint16_t color) {
    int16_t cx = x + 20;
    int16_t cy = y + 20;
    
    // Anode to Cathode
    tft.drawLine(x, cy, x + 40, cy, color);
    
    // Gate
    tft.drawLine(x + 8, cy + 10, x + 16, cy + 10, color);
    tft.drawLine(x + 16, cy + 10, x + 24, cy, color);
    
    // Arrow
    tft.fillTriangle(x + 26, cy, x + 26, cy - 4, x + 30, cy, color);
    
    // Enclosure
    tft.drawCircle(cx, cy, 16, color);
}

// ============================================================================
// COMPONENT LABEL
// ============================================================================

void draw_component_label(int16_t x, int16_t y, const char* text, uint16_t color, uint8_t size) {
    tft.setTextColor(color);
    tft.setTextSize(size);
    tft.setCursor(x, y);
    tft.print(text);
}

// ============================================================================
// RESULT INDICATORS
// ============================================================================

void draw_pass_indicator(int16_t x, int16_t y) {
    tft.fillCircle(x + 15, y + 15, 18, C_GREEN);
    tft.setTextColor(C_DARK);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setCursor(x + 15, y + 15);
    tft.print("OK");
    tft.setTextDatum(TL_DATUM);
}

void draw_fail_indicator(int16_t x, int16_t y) {
    tft.fillCircle(x + 15, y + 15, 18, C_RED);
    tft.setTextColor(C_WHITE);
    tft.setTextSize(3);
    tft.setTextDatum(TC_DATUM);
    tft.setCursor(x + 15, y + 15);
    tft.print("X");
    tft.setTextDatum(TL_DATUM);
}

void draw_warning_indicator_small(int16_t x, int16_t y) {
    tft.fillTriangle(x + 8, y, x, y + 16, x + 16, y + 16, C_YELLOW);
    tft.setTextColor(C_DARK);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);
    tft.setCursor(x + 8, y + 6);
    tft.print("!");
    tft.setTextDatum(TL_DATUM);
}