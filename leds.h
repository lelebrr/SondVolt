#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include "config.h"

// Funções para controle dos LEDs
void update_leds();
void set_green_led(bool state);
void set_red_led(bool state);
void flash_both_leds(int delay_ms);
void flash_green_led_slow();
void flash_red_led_fast();

#endif // LEDS_H
