// ============================================================================
// Sondvolt v4.0 - LEDs indicadores
// ============================================================================
// Arquivo : leds.cpp
//
// Correcoes em relacao a versao anterior:
//   1. leds_init() nunca era chamado; os pinos nunca viravam saida.
//   2. O LED RGB da CYD e de ANODO COMUM: nivel baixo acende. O codigo
//      antigo escrevia HIGH para acender, ou seja, os LEDs ficavam invertidos.
//   3. Os padroes de pisca-pisca dependiam de ser chamados continuamente,
//      mas ninguem os chamava. Agora existe leds_update(), acionado pela
//      tarefa de interface, e os padroes viram estados.
//   4. O LED vermelho divide o GPIO4 com o OneWire e o azul divide o GPIO17
//      com a descarga; a HAL cuida da arbitragem.
// ============================================================================

#include "leds.h"
#include "hal.h"
#include "globals.h"
#include "config.h"

// ----------------------------------------------------------------------------
// Padroes de sinalizacao
// ----------------------------------------------------------------------------
enum LedPattern {
    LED_PATTERN_OFF = 0,
    LED_PATTERN_SOLID,
    LED_PATTERN_BLINK_SLOW,
    LED_PATTERN_BLINK_FAST,
    LED_PATTERN_PULSE
};

struct LedChannel {
    uint8_t    pin;
    LedPattern pattern;
    uint16_t   periodMs;
    bool       phase;
    uint32_t   lastToggle;
};

static LedChannel gCh[3] = {
    { PIN_LED_RED,   LED_PATTERN_OFF, 0, false, 0 },
    { PIN_LED_GREEN, LED_PATTERN_OFF, 0, false, 0 },
    { PIN_LED_BLUE,  LED_PATTERN_OFF, 0, false, 0 }
};

static bool gInitialized = false;

enum { CH_RED = 0, CH_GREEN = 1, CH_BLUE = 2 };

static void channel_set(uint8_t index, LedPattern pattern, uint16_t periodMs) {
    if (index > 2) return;
    gCh[index].pattern    = pattern;
    gCh[index].periodMs   = periodMs;
    gCh[index].lastToggle = millis();
    gCh[index].phase      = (pattern == LED_PATTERN_SOLID);
    hal_led_write(gCh[index].pin, gCh[index].phase);
}

void leds_init() {
    if (gInitialized) return;
    // Os pinos ja foram configurados por hal_init(); aqui so garantimos o
    // estado apagado e um teste visual rapido.
    for (uint8_t i = 0; i < 3; i++) {
        gCh[i].pattern = LED_PATTERN_OFF;
        hal_led_write(gCh[i].pin, false);
    }

    // Sequencia curta de teste: vermelho, verde, azul.
    for (uint8_t i = 0; i < 3; i++) {
        hal_led_write(gCh[i].pin, true);
        vTaskDelay(pdMS_TO_TICKS(70));
        hal_led_write(gCh[i].pin, false);
    }

    gInitialized = true;
    LOG_SERIAL_F("[LED] LEDs indicadores prontos");
}

// Avanca os padroes. Chamar a cada ciclo da interface.
void leds_update() {
    const uint32_t now = millis();

    for (uint8_t i = 0; i < 3; i++) {
        LedChannel& c = gCh[i];

        // Nao mexe no LED enquanto o pino estiver emprestado a outro recurso.
        if (i == CH_RED  && hal_bus_is_busy(HAL_BUS_ONEWIRE))   continue;
        if (i == CH_BLUE && hal_bus_is_busy(HAL_BUS_DISCHARGE)) continue;

        switch (c.pattern) {
            case LED_PATTERN_OFF:
            case LED_PATTERN_SOLID:
                break;

            case LED_PATTERN_BLINK_SLOW:
            case LED_PATTERN_BLINK_FAST:
                if ((now - c.lastToggle) >= c.periodMs) {
                    c.phase = !c.phase;
                    hal_led_write(c.pin, c.phase);
                    c.lastToggle = now;
                }
                break;

            case LED_PATTERN_PULSE:
                // Pulso curto e espacado: presente, mas discreto.
                if (c.phase && (now - c.lastToggle) >= 60) {
                    c.phase = false;
                    hal_led_write(c.pin, false);
                    c.lastToggle = now;
                } else if (!c.phase && (now - c.lastToggle) >= c.periodMs) {
                    c.phase = true;
                    hal_led_write(c.pin, true);
                    c.lastToggle = now;
                }
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// Controle direto
// ----------------------------------------------------------------------------

void set_red_led(bool on) {
    channel_set(CH_RED, on ? LED_PATTERN_SOLID : LED_PATTERN_OFF, 0);
}

void set_green_led(bool on) {
    channel_set(CH_GREEN, on ? LED_PATTERN_SOLID : LED_PATTERN_OFF, 0);
}

void set_blue_led(bool on) {
    channel_set(CH_BLUE, on ? LED_PATTERN_SOLID : LED_PATTERN_OFF, 0);
}

void set_both_leds(bool on) {
    set_red_led(on);
    set_green_led(on);
}

void led_off() {
    for (uint8_t i = 0; i < 3; i++) channel_set(i, LED_PATTERN_OFF, 0);
}

// ----------------------------------------------------------------------------
// Padroes nomeados
// ----------------------------------------------------------------------------

void flash_ok() {
    channel_set(CH_RED,   LED_PATTERN_OFF, 0);
    channel_set(CH_GREEN, LED_PATTERN_BLINK_SLOW, 200);
}

void flash_error() {
    channel_set(CH_GREEN, LED_PATTERN_OFF, 0);
    channel_set(CH_RED,   LED_PATTERN_BLINK_FAST, 100);
}

void flash_testing() {
    channel_set(CH_GREEN, LED_PATTERN_PULSE, 700);
}

void flash_alert() {
    channel_set(CH_GREEN, LED_PATTERN_OFF, 0);
    channel_set(CH_RED,   LED_PATTERN_BLINK_FAST, 120);
}

// ----------------------------------------------------------------------------
// Cor composta
// ----------------------------------------------------------------------------

void led_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    // Sem PWM por canal, o LED RGB da CYD e tratado como tres LEDs on/off.
    // O limiar de 128 aproxima a cor pedida.
    set_red_led(r > 128);
    set_green_led(g > 128);
    set_blue_led(b > 128);
}

void led_set_rgb_hex(uint32_t color) {
    led_set_rgb((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void led_flash_rgb(uint8_t r, uint8_t g, uint8_t b,
                   unsigned long onMs, unsigned long offMs) {
    (void)offMs;
    uint16_t period = (uint16_t)((onMs > 0) ? onMs : 200);
    channel_set(CH_RED,   (r > 128) ? LED_PATTERN_BLINK_SLOW : LED_PATTERN_OFF, period);
    channel_set(CH_GREEN, (g > 128) ? LED_PATTERN_BLINK_SLOW : LED_PATTERN_OFF, period);
    channel_set(CH_BLUE,  (b > 128) ? LED_PATTERN_BLINK_SLOW : LED_PATTERN_OFF, period);
}

void led_set_color(uint8_t colorIndex) {
    switch (colorIndex) {
        case LEDC_RED:     led_set_rgb(255,   0,   0); break;
        case LEDC_GREEN:   led_set_rgb(  0, 255,   0); break;
        case LEDC_BLUE:    led_set_rgb(  0,   0, 255); break;
        case LEDC_YELLOW:  led_set_rgb(255, 255,   0); break;
        case LEDC_CYAN:    led_set_rgb(  0, 255, 255); break;
        case LEDC_MAGENTA: led_set_rgb(255,   0, 255); break;
        case LEDC_WHITE:   led_set_rgb(255, 255, 255); break;
        case LEDC_PURPLE:  led_set_rgb(255,   0, 255); break;
        default:           led_off(); break;
    }
}

// ----------------------------------------------------------------------------
// Status de alto nivel
// ----------------------------------------------------------------------------

void led_status_good()    { led_off(); set_green_led(true); }
void led_status_bad()     { led_off(); set_red_led(true); }
void led_status_working() { led_off(); channel_set(CH_BLUE, LED_PATTERN_PULSE, 600); }
void led_status_thermal() { led_off(); channel_set(CH_BLUE, LED_PATTERN_BLINK_SLOW, 350); }
void led_status_danger()  { led_off(); channel_set(CH_RED,  LED_PATTERN_BLINK_FAST, 90); }

// RGB WS2812B: a CYD nao traz esse LED de fabrica. Mantido como no-op para
// nao quebrar chamadas antigas.
void rgb_init() {}
