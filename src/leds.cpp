// ============================================================================
// Sondvolt v3.0 — LEDs (simples, GPIO)
// ============================================================================

#include "leds.h"
#include "globals.h"
#include "config.h"

static unsigned long rgbFlashOnMs = 0;
static unsigned long rgbFlashOffMs = 0;
static unsigned long rgbFlashStart = 0;
static bool rgbFlashing = false;
static bool rgbLastState = false;
static uint8_t rgbTargetR = 0, rgbTargetG = 0, rgbTargetB = 0;

void leds_init() {
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    
    // Teste inicial (Branco)
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
    delay(200);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    
    LOG_SERIAL_F("LEDs RGB OK");
}

void set_green_led(bool on) {
    digitalWrite(PIN_LED_GREEN, on ? HIGH : LOW);
}

void set_red_led(bool on) {
    digitalWrite(PIN_LED_RED, on ? HIGH : LOW);
}

void set_blue_led(bool on) {
    digitalWrite(PIN_LED_BLUE, on ? HIGH : LOW);
}

void set_both_leds(bool on) {
    set_green_led(on);
    set_red_led(on);
}

void update_leds() {
}

void flash_ok() {
    static unsigned long lastToggle = 0;
    static bool state = false;
    unsigned long now = millis();
    if (now - lastToggle > 200) {
        set_green_led(state);
        state = !state;
        lastToggle = now;
    }
}

void flash_error() {
    static unsigned long lastToggle = 0;
    static bool state = false;
    unsigned long now = millis();
    if (now - lastToggle > 100) {
        set_red_led(state);
        state = !state;
        lastToggle = now;
    }
}

void flash_testing() {
    static unsigned long lastToggle = 0;
    static bool state = false;
    unsigned long now = millis();
    if (now - lastToggle > 500) {
        set_green_led(state);
        state = !state;
        lastToggle = now;
    }
}

void flash_alert() {
    static unsigned long lastToggle = 0;
    static bool state = false;
    unsigned long now = millis();
    if (now - lastToggle > 150) {
        set_red_led(state);
        state = !state;
        lastToggle = now;
    }
}

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    rgbTargetR = r;
    rgbTargetG = g;
    rgbTargetB = b;
    rgbFlashing = false;
    
    // PWM seria melhor, mas GPIO simples por enquanto para manter compatibilidade
    digitalWrite(PIN_LED_RED,   r > 128 ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, g > 128 ? HIGH : LOW);
    digitalWrite(PIN_LED_BLUE,  b > 128 ? HIGH : LOW);
}

void led_set_rgb_hex(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    led_set_rgb(r, g, b);
}

void led_flash_rgb(uint8_t r, uint8_t g, uint8_t b,
                  unsigned long onMs, unsigned long offMs) {
    rgbTargetR = r;
    rgbTargetG = g;
    rgbTargetB = b;
    rgbFlashOnMs = onMs;
    rgbFlashOffMs = offMs;
    rgbFlashStart = millis();
    rgbFlashing = true;
    rgbLastState = true;
    led_set_rgb(r, g, b);
}

void led_off() {
    rgbFlashing = false;
    set_both_leds(false);
}

void led_set_color(uint8_t colorIndex) {
    switch (colorIndex) {
        case LEDC_RED:     led_set_rgb(255, 0, 0); break;
        case LEDC_GREEN:   led_set_rgb(0, 255, 0); break;
        case LEDC_BLUE:    led_set_rgb(0, 0, 255); break;
        case LEDC_YELLOW:  led_set_rgb(255, 200, 0); break;
        case LEDC_CYAN:    led_set_rgb(0, 255, 255); break;
        case LEDC_MAGENTA: led_set_rgb(255, 0, 255); break;
        case LEDC_WHITE:   led_set_rgb(255, 255, 255); break;
        case 10:           led_set_rgb(128, 0, 128); break; // Purple for Thermal
    }
}

// Funções Profissionais (Requisito 4)
void led_status_good()    { led_set_rgb(0, 255, 0); }
void led_status_bad()     { led_set_rgb(255, 0, 0); }
void led_status_working() { led_set_rgb(0, 0, 255); }
void led_status_thermal() { led_set_rgb(128, 0, 128); }
void led_status_danger()  { led_set_rgb(255, 0, 0); }