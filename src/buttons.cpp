// ============================================================================
// Sondvolt v3.0 — Botoes e Touchscreen (Implementacao)
// Descricao: Sistema de entrada unificado (touch XPT2046 + botoes fisicos)
// ============================================================================

#include "buttons.h"
#include "config.h"
#include "pins.h"
#include "display_globals.h"
#include "globals.h"
#include <SPI.h>

extern SPIClass touchSPI;


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

    LOG_SERIAL_F("[BTN] Inicializando sistema de botões e touch...");
    LOG_SERIAL_FMT("[BTN] Configuração touch - MinX:%d, MaxX:%d, MinY:%d, MaxY:%d\n",
                 TOUCH_MIN_X, TOUCH_MAX_X, TOUCH_MIN_Y, TOUCH_MAX_Y);

    // Tenta inicializar touch screen
    bool touchOk = false;
    #ifdef __EXCEPTIONS
    try {
    #endif
        // Verifica se o display está inicializado primeiro
        if (!tftInitialized) {
            LOG_SERIAL_F("[BTN] ERRO: Display não inicializado, aguardando...");
            return;
        }
        
        LOG_SERIAL_F("[BTN] Display inicializado, configurando touch controller...");
        
        // Inicializa o barramento SPI dedicado para o touch na CYD
        touchSPI.begin(PIN_HSPI_SCLK, PIN_HSPI_MISO, PIN_HSPI_MOSI, PIN_TOUCH_CS);
        
        // Inicializa o controlador XPT2046
        if (touch.begin(touchSPI)) {
            touch.setRotation(3); // Rotação 3 (paisagem invertido) para sincronizar com display
            touchOk = true;
            LOG_SERIAL_F("[BTN] Touch controller XPT2046 inicializado com sucesso");
        } else {
            LOG_SERIAL_F("[BTN] ERRO: Falha ao iniciar controlador XPT2046");
            touchOk = false;
        }
        
        // Teste rápido do touch
        if (touchOk && touch.touched()) {
            TS_Point p = touch.getPoint();
            LOG_SERIAL_FMT("[BTN] Touch testado com sucesso - X=%d, Y=%d, Z=%d\n", p.x, p.y, p.z);
        } else if (touchOk) {
            LOG_SERIAL_F("[BTN] Touch configurado (Aguardando toque)");
        }

    #ifdef __EXCEPTIONS
    } catch (...) {
        DBG("[BTN] Touch screen não disponível");
        LOG_SERIAL_F("[BTN] EXCEÇÃO: Falha ao configurar touch controller");
    }
    #endif

    if (!touchOk) {
        LOG_SERIAL_F("[BTN] Touch screen não inicializado, usando botoes físicos apenas");
        // Touch não disponível, sistema funciona com botões físicos apenas
    }

    gButtonsInitialized = true;
    LOG_SERIAL_FMT("[BTN] Sistema de botoes inicializado (touch: %s)\n", touchOk ? "OK" : "FALHOU");
}

// ============================================================================
// ATUALIZACAO (chamar no loop)
// ============================================================================

void buttons_update() {
    if (!gButtonsInitialized) {
        buttons_init();
    }

    unsigned long now = millis();

    // Limpa flags "just" do frame anterior
    for (int i = 0; i < BTN_TOTAL; i++) {
        gButtons[i].justPressed = false;
        gButtons[i].justReleased = false;
    }

    // Processa touch usando o controlador XPT2046
    if (touch.touched()) {
        TS_Point p = touch.getPoint();
        uint16_t tx = p.x;
        uint16_t ty = p.y;
        
        // Validação básica das coordenadas
        if (tx < TOUCH_MIN_X || tx > TOUCH_MAX_X || ty < TOUCH_MIN_Y || ty > TOUCH_MAX_Y) {
            // Coordenadas inválidas, ignora
            return;
        }
        
        // Log de touch detectado (a cada 500ms para evitar spam)
        static unsigned long lastTouchLog = 0;
        if (now - lastTouchLog > 500) {
            LOG_SERIAL_FMT("[BTN] Touch detectado - Raw: X=%d, Y=%d, Z=%d\n", tx, ty, p.z);
            
            // Mapeia e exibe coordenadas mapeadas
            int16_t mappedX = map(tx, TOUCH_MIN_X, TOUCH_MAX_X, SCREEN_WIDTH, 0);
            int16_t mappedY = map(ty, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_HEIGHT);
            LOG_SERIAL_FMT("[BTN] Mapeado: X=%d, Y=%d\n", mappedX, mappedY);
            
            lastTouchLog = now;
        }
        
        if (!gTouchPressed) {
            gTouchPressed = true;
            gTouchPressStart = now;
            gLastTouch.x = tx;
            gLastTouch.y = ty;
            gLastTouch.z = p.z;
            
            // Define justPressed APENAS no momento inicial do toque
            gButtons[BTN_TOUCH].justPressed = true;
        }
    } else {

        if (gTouchPressed) {
            gTouchPressed = false;
            gButtons[BTN_TOUCH].justReleased = true;
            LOG_SERIAL_F("[BTN] Touch liberado");
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
    if (touch.touched()) {
        TS_Point tp = touch.getPoint();
        
        // Validação básica das coordenadas
        if (tp.x >= TOUCH_MIN_X && tp.x <= TOUCH_MAX_X &&
            tp.y >= TOUCH_MIN_Y && tp.y <= TOUCH_MAX_Y) {
            
            // Mapeia coordenadas para a tela (ROTAÇÃO 3 - Paisagem invertido)
            p.x = map(tp.x, TOUCH_MIN_X, TOUCH_MAX_X, SCREEN_WIDTH, 0);
            p.y = map(tp.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_HEIGHT);
            p.z = tp.z;
            
            // Log para depuração (a cada 2s)
            static unsigned long lastDebugLog = 0;
            unsigned long now = millis();
            if (now - lastDebugLog > 2000) {
                LOG_SERIAL_FMT("[TOUCH] Coordenadas mapeadas: X=%d, Y=%d (Raw: %d, %d)\n",
                             p.x, p.y, tp.x, tp.y);
                lastDebugLog = now;
            }
        } else {
            // Coordenadas inválidas, retorna zeros
            p.x = 0;
            p.y = 0;
            p.z = 0;
        }
    } else {
        p.x = 0;
        p.y = 0;
        p.z = 0;
    }
    return p;
}

TouchPoint touch_get_raw_point() {
    TouchPoint p;
    if (touch.touched()) {
        TS_Point tp = touch.getPoint();
        p.x = tp.x;
        p.y = tp.y;
        p.z = tp.z;
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