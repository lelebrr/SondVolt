// ============================================================================
// Sondvolt v3.1 — Component Drawings (Dedicated File)
// All component-specific vector icons
// For ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================================
// COMPONENT TYPE DEFINITIONS
// ============================================================================

#define COMP_RESISTOR         0
#define COMP_CAPACITOR      1
#define COMP_CAP_ELECTRO   2
#define COMP_CAP_CERAMIC   3
#define COMP_DIODE        4
#define COMP_LED          5
#define COMP_LED_RGB      6
#define COMP_ZENER        7
#define COMP_SCHOTTKY    8
#define COMP_TRANSISTOR_NPN  9
#define COMP_TRANSISTOR_PNP 10
#define COMP_MOSFET_N     11
#define COMP_MOSFET_P     12
#define COMP_INDUCTOR     13
#define COMP_CRYSTAL     14
#define COMP_FUSE       15
#define COMP_RELAY      16
#define COMP_IC         17
#define COMP_OPTO       18
#define COMP_TRIAC     19
#define COMP_SCR       20

// ============================================================================
// MAIN COMPONENT DRAWING FUNCTION
// ============================================================================

void draw_component_by_type(uint8_t type, int16_t x, int16_t y, uint16_t color);

void draw_resistor(int16_t x, int16_t y, uint16_t color);
void draw_capacitor(int16_t x, int16_t y, uint16_t color);
void draw_capacitor_electrolytic(int16_t x, int16_t y, uint16_t color);
void draw_capacitor_ceramic(int16_t x, int16_t y, uint16_t color);
void draw_diode(int16_t x, int16_t y, uint16_t color);
void draw_led(int16_t x, int16_t y, uint16_t color, bool rgb = false);
void draw_zener(int16_t x, int16_t y, uint16_t color);
void draw_schottky(int16_t x, int16_t y, uint16_t color);
void draw_transistor_npn(int16_t x, int16_t y, uint16_t color);
void draw_transistor_pnp(int16_t x, int16_t y, uint16_t color);
void draw_mosfet_n(int16_t x, int16_t y, uint16_t color);
void draw_mosfet_p(int16_t x, int16_t y, uint16_t color);
void draw_inductor(int16_t x, int16_t y, uint16_t color);
void draw_crystal(int16_t x, int16_t y, uint16_t color);
void draw_fuse(int16_t x, int16_t y, uint16_t color);
void draw_relay(int16_t x, int16_t y, uint16_t color);
void draw_ic(int16_t x, int16_t y, uint16_t color);
void draw_optoisolator(int16_t x, int16_t y, uint16_t color);
void draw_triac(int16_t x, int16_t y, uint16_t color);
void draw_scr(int16_t x, int16_t y, uint16_t color);

// ============================================================================
// COMPONENT VALUE LABELS
// ============================================================================

void draw_component_label(int16_t x, int16_t y, const char* text, uint16_t color, uint8_t size);

// ============================================================================
// RESULT INDICATORS
// ============================================================================

void draw_pass_indicator(int16_t x, int16_t y);
void draw_fail_indicator(int16_t x, int16_t y);
void draw_warning_indicator_small(int16_t x, int16_t y);

#endif