// ============================================================================
// Sondvolt v3.0 — Botoes e Touchscreen (Implementacao)
// Descricao: Sistema de entrada unificado (touch XPT2046 + botoes fisicos)
// ============================================================================

#include "buttons.h"
#include "config.h"
#include "pins.h"
#include "globals.h"
#include "display_globals.h"

#include <string.h>

// ============================================================================
// VARIAVEIS
// ============================================================================

// Estados dos botoes (edge detection)
static struct {
    bool pressed;
    bool justPressed;
    bool justReleased;
    bool longPressed;
    unsigned long lastPressMs;
    unsigned long pressDuration;
} gButtons[BTN_TOTAL];

// Touch
static TouchPoint gLastTouch;
static bool gTouchPressed = false;
static unsigned long gTouchPressStart = 0;

// Inicializado
static bool gButtonsInitialized = false;

// ============================================================================
// INICIALIZACAO
// ============================================================================

void buttons_init() {
    if (gButtonsInitialized) return;

    memset(gButtons, 0, sizeof(gButtons));
    gLastTouch.x = 0;
    gLastTouch.y = 0;
    gLastTouch.z = 0;

    // Tenta inicializar touch screen
    bool touchOk = false;
    #ifdef __EXCEPTIONS
    try {
    #endif
        // Inicializa o display primeiro
        if (tftInitialized) {
            uint16_t touchData[4] = { TOUCH_MIN_X, TOUCH_MAX_X, TOUCH_MIN_Y, TOUCH_MAX_Y };
            tft.setTouch(touchData);
            touchOk = true; // Assume sucesso se display estiver inicializado
        }
    #ifdef __EXCEPTIONS
    } catch (...) {
        DBG("[BTN] Touch screen não disponível");
    }
    #endif

    if (!touchOk) {
        LOG_SERIAL_F("[BTN] Touch screen não inicializado, usando botoes físicos apenas");
        // Touch não disponível, sistema funciona com botões físicos apenas
    }

    gButtonsInitialized = true;
    LOG_SERIAL_F("[BTN] Sistema de botoes inicializado (touch: OK)");
}

// ============================================================================
// ATUALIZACAO (chamar no loop)
// ============================================================================

void buttons_update() {
    if (!gButtonsInitialized) {
        buttons_init();
    }

    unsigned long now = millis();

    // Processa touch
    uint16_t tx, ty;
    if (tft.getTouch(&tx, &ty)) {
        if (!gTouchPressed) {
            gTouchPressed = true;
            gTouchPressStart = now;
            gLastTouch.x = tx;
            gLastTouch.y = ty;
            gLastTouch.z = 1;
        }
        gButtons[BTN_TOUCH].justPressed = true;
    } else {
        if (gTouchPressed) {
            gTouchPressed = false;
            gButtons[BTN_TOUCH].justReleased = true;
        }
    }

    // Atualiza botoes fisicos se definidos
#if HAS_PHYSICAL_BUTTONS
    update_physical_button(BTN_UP, PIN_BTN_UP);
    update_physical_button(BTN_DOWN, PIN_BTN_DOWN);
    update_physical_button(BTN_LEFT, PIN_BTN_LEFT);
    update_physical_button(BTN_RIGHT, PIN_BTN_RIGHT);
    update_physical_button(BTN_CENTER, PIN_BTN_OK);
    update_physical_button(BTN_BACK, PIN_BTN_BACK);
#endif

    // Limpa flags "just" apos processar
    for (int i = 0; i < BTN_TOTAL; i++) {
        gButtons[i].justPressed = false;
        gButtons[i].justReleased = false;
    }
}

// Atualiza botao fisico
#if HAS_PHYSICAL_BUTTONS
static void update_physical_button(int btnId, int pin) {
    if (btnId >= BTN_TOTAL) return;

    unsigned long now = millis();
    bool currentState = (digitalRead(pin) == LOW);

    if (currentState != gButtons[btnId].pressed) {
        if (currentState) {
            gButtons[btnId].pressed = true;
            gButtons[btnId].justPressed = true;
            gButtons[btnId].lastPressMs = now;
        } else {
            gButtons[btnId].pressed = false;
            gButtons[btnId].justReleased = true;
            gButtons[btnId].pressDuration = now - gButtons[btnId].lastPressMs;
        }
    }

    // Long press
    if (gButtons[btnId].pressed) {
        if ((now - gButtons[btnId].lastPressMs) >= LONG_PRESS_MS) {
            gButtons[btnId].longPressed = true;
        }
    }
}
#endif

// ============================================================================
// TOUCH
// ============================================================================

bool touch_is_pressed() {
    return gTouchPressed;
}

TouchPoint touch_get_point() {
    TouchPoint p;
    uint16_t tx, ty;

    if (tft.getTouch(&tx, &ty)) {
        // Mapeia coordenadas para a tela
        p.x = map(tx, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_W);
        p.y = map(ty, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_H);
        p.z = 1;
    } else {
        p.x = 0;
        p.y = 0;
        p.z = 0;
    }

    return p;
}

TouchPoint touch_get_raw_point() {
    TouchPoint p;
    uint16_t tx, ty;

    if (tft.getTouch(&tx, &ty)) {
        p.x = tx;
        p.y = ty;
        p.z = 1;
    } else {
        p.x = 0;
        p.y = 0;
        p.z = 0;
    }

    return p;
}

// ============================================================================
// QUERY DE NAVEGACAO
// ============================================================================

bool btn_just_pressed(int btnId) {
    if (btnId >= BTN_TOTAL) return false;
    return gButtons[btnId].justPressed;
}

bool btn_is_pressed(int btnId) {
    if (btnId >= BTN_TOTAL) return false;

    // Touch e considerado sempre pressionado enquanto toca
    if (btnId == BTN_TOUCH) {
        return gTouchPressed;
    }

    return gButtons[btnId].pressed;
}

bool btn_long_pressed(int btnId) {
    if (btnId >= BTN_TOTAL) return false;
    return gButtons[btnId].longPressed;
}

bool btn_long_pressed_any() {
    for (int i = 0; i < BTN_TOTAL; i++) {
        if (gButtons[i].longPressed) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// UTILITARIOS
// ============================================================================

bool buzzer_enabled() {
    return deviceSettings.soundEnabled;
}