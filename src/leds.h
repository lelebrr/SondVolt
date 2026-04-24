// ============================================================================
// Sondvolt v3.0 — LEDs e RGB LED
// Descrição: Controle dos LEDs indicadores e RGB WS2812B integrado
// ============================================================================
#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

// ============================================================================
// PROTÓTIPOS
// ============================================================================

// LEDs indicadores (verde/vermelho)
void leds_init();
void update_leds();
void set_green_led(bool on);
void set_red_led(bool on);
void set_both_leds(bool on);

// Padrões de flashing
void flash_ok();         // Verde piscando (sucesso)
void flash_error();      // Vermelho piscando (erro)
void flash_testing();    // Verde lento (testando)
void flash_alert();     // Vermelho rápido (alerta)

// RGB LED WS2812B (integrado na CYD)
void rgb_init();
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);
void led_set_rgb_hex(uint32_t color);
void led_flash_rgb(uint8_t r, uint8_t g, uint8_t b,
               unsigned long onMs, unsigned long offMs);
void led_off();

// Cores predefinidas
void led_set_color(uint8_t colorIndex);
#define LEDC_RED     0
#define LEDC_GREEN   1
#define LEDC_BLUE    2
#define LEDC_YELLOW  3
#define LEDC_CYAN    4
#define LEDC_MAGENTA 5
#define LEDC_WHITE   6

#endif // LEDS_H