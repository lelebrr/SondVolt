// ============================================================================
// Component Tester PRO v3.0 — LEDs Indicadores (CYD Edition)
// ============================================================================
#ifndef LEDS_H
#define LEDS_H

#include "config.h"
#include <Arduino.h>

// Variáveis globais para controle dos LEDs
extern bool flashingBothLeds;
extern bool flashingGreenLedSlow;
extern bool flashingRedLedFast;
extern unsigned long ledFlashTimer;
extern unsigned long ledFlashInterval;

extern bool greenLedState;
extern bool redLedState;

// Funções para controle dos LEDs (RGB integrado da CYD)
void update_leds();
void set_green_led(bool state);
void set_red_led(bool state);
void flash_both_leds(int delay_ms);
void flash_green_led_slow();
void flash_red_led_fast();

#endif // LEDS_H