#include "safety.h"
#include "config.h"
#include "pins.h"
#include "graphics.h"
#include "buzzer.h"
#include "display_globals.h"
#include "display_mutex.h"
#include <XPT2046_Touchscreen.h>


// ============================================================================
// VARIAVEIS GLOBAIS
// ============================================================================

SafetyStatus safetyStatus;
bool safetyCheckEnabled = false;
bool safetySoundEnabled = true;
bool safetyLedEnabled = true;
bool safetyAutoLockoutEnabled = true;

static TaskHandle_t safetyTaskHandle = nullptr;
static bool alertActive = false;

static bool map_touch_to_screen_safe(const TS_Point& raw, uint16_t& outX, uint16_t& outY) {
    if (raw.z < TOUCH_MIN_PRESSURE || raw.z > TOUCH_MAX_PRESSURE) {
        return false;
    }

    if (raw.x < TOUCH_MIN_X || raw.x > TOUCH_MAX_X || raw.y < TOUCH_MIN_Y || raw.y > TOUCH_MAX_Y) {
        return false;
    }

    int32_t mx = map(raw.x, TOUCH_MIN_X, TOUCH_MAX_X, SCREEN_WIDTH, 0);
    int32_t my = map(raw.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_HEIGHT);

    outX = (uint16_t)constrain(mx, 0, SCREEN_WIDTH - 1);
    outY = (uint16_t)constrain(my, 0, SCREEN_HEIGHT - 1);
    return true;
}

// ============================================================================
// INICIALIZACAO
// ============================================================================

void safety_init() {
    memset(&safetyStatus, 0, sizeof(SafetyStatus));

    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.hasFuseInstalled = false;
    safetyStatus.hasVaristorInstalled = false;
    safetyStatus.safetyAcknowledged = false; // Exigir primeiro aceite
    safetyCheckEnabled = true;

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);

    LOG_SERIAL_F("Sistema de seguranca inicializado");
}

void safety_reset() {
    memset(&safetyStatus, 0, sizeof(SafetyStatus));

    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.safetyAcknowledged = true;

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);

    safety_alert_stop();
}

// ============================================================================
// VERIFICACAO
// ============================================================================

SafetyCheckResult safety_check_voltage(float voltage) {
    SafetyCheckResult result;
    memset(&result, 0, sizeof(SafetyCheckResult));

    result.detectedVoltage = voltage;

    if(voltage >= SAFETY_VOLTAGE_DANGER_AC) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_CRITICAL;
        result.message = SAFETY_MSG_DANGER_220V;
    } else if(voltage >= SAFETY_VOLTAGE_220V_MIN && voltage <= SAFETY_VOLTAGE_220V_MAX) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_HIGH;
        result.message = SAFETY_MSG_DANGER_220V;
    } else if(voltage >= SAFETY_VOLTAGE_110V_MIN && voltage <= SAFETY_VOLTAGE_110V_MAX) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_MEDIUM;
        result.message = SAFETY_MSG_DANGER_HIGH;
    } else if(voltage >= 50.0f) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_LOW;
        result.message = SAFETY_MSG_DANGER_HIGH;
    } else {
        result.isSafe = true;
        result.alertLevel = SAFETY_ALERT_NONE;
        result.message = " ";
    }

    if(voltage >= SAFETY_VOLTAGE_DC_DANGER) {
        result.isDcDanger = true;
    }

    return result;
}

bool safety_is_ac_danger(float voltage) {
    return (voltage >= SAFETY_VOLTAGE_220V_MIN);
}

bool safety_is_dc_danger(float voltage) {
    return (voltage >= SAFETY_VOLTAGE_DC_DANGER);
}

bool safety_is_short_circuit(float resistance) {
    return (resistance < SAFETY_SHORT_CIRCUIT);
}

// ============================================================================
// ALERTAS
// ============================================================================

void safety_trigger_alert(SafetyAlertLevel level) {
    safetyStatus.alertLevel = level;
    alertActive = true;

    if(level >= SAFETY_ALERT_HIGH) {
        safetyStatus.state = SAFETY_STATE_DANGER;

        if(safetySoundEnabled) {
            safety_alert_sound_danger();
        }

        if(safetyLedEnabled) {
            safety_alert_led_flash(true);
        }

        safety_draw_danger_screen(SAFETY_MSG_DANGER_220V, safetyStatus.lastDetectedVoltage);

    } else if(level >= SAFETY_ALERT_MEDIUM) {
        safetyStatus.state = SAFETY_STATE_WARNING;

        if(safetySoundEnabled) {
            buzzer_beep(BUZZER_FREQ_WARNING, BUZZER_DURATION_WARNING);
        }

        if(safetyLedEnabled) {
            digitalWrite(PIN_LED_RED, HIGH);
        }

    } else {
        safetyStatus.state = SAFETY_STATE_SAFE;
        safety_alert_stop();
    }
}

void safety_alert_led_flash(bool enable) {
    if(enable) {
        digitalWrite(PIN_LED_RED, HIGH);
        vTaskDelay(pdMS_TO_TICKS(SAFETY_LED_FLASH_FAST));
        digitalWrite(PIN_LED_RED, LOW);
        vTaskDelay(pdMS_TO_TICKS(SAFETY_LED_FLASH_FAST));
    } else {
        digitalWrite(PIN_LED_RED, LOW);
    }
}

void safety_alert_sound_danger() {
    for(uint8_t i = 0; i < 3; i++) {
        buzzer_beep(SAFETY_BEEP_DANGER, SAFETY_BEEP_DURATION_FAST);
        vTaskDelay(pdMS_TO_TICKS(SAFETY_BEEP_DURATION_FAST));
    }
}

void safety_alert_sound_confirm() {
    buzzer_beep(SAFETY_BEEP_CONFIRM, 200);
    vTaskDelay(pdMS_TO_TICKS(100));
    buzzer_beep(SAFETY_BEEP_OK, 200);
}

void safety_alert_stop() {
    alertActive = false;
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
}

// ============================================================================
// TELAS DE PERIGO
// ============================================================================

void safety_draw_danger_screen(const char* message, float voltage) {
    LOCK_TFT();
    tft.fillScreen(COLOR_BAD);

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);
    tft.setCursor(20, 40);
    tft.print("ALERTA DE SEGURANCA");

    char voltStr[32];
    snprintf(voltStr, sizeof(voltStr), "%.1f V", voltage);

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(3);
    tft.setCursor(SCREEN_WIDTH/2 - 40, 80);
    tft.print(voltStr);

    tft.setTextColor(COLOR_WARNING);
    tft.setTextSize(2);
    tft.setCursor(20, 120);
    tft.print(message);

    tft.fillRoundRect(20, 160, SCREEN_WIDTH - 40, 30, 4, COLOR_TEXT);
    tft.setTextColor(COLOR_BACKGROUND);
    tft.setCursor(40, 168);
    tft.print("CONTINUAR IGUALVEL");

    tft.fillRoundRect(20, 200, SCREEN_WIDTH - 40, 30, 4, COLOR_SURFACE);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(SCREEN_WIDTH/2 - 40, 208);
    tft.print("CANCELAR");
    UNLOCK_TFT();
}

void safety_draw_lockout_screen(unsigned long remainingMs) {
    LOCK_TFT();
    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextColor(COLOR_WARNING);
    tft.setTextSize(3);
    tft.setCursor(SCREEN_WIDTH/2 - 60, 60);
    tft.print("BLOQUEADO");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);
    tft.setCursor(20, 100);
    tft.print("Equipamento bloqueado");

    char timeStr[32];
    uint16_t seconds = remainingMs / 1000;
    snprintf(timeStr, sizeof(timeStr), "%d seg", seconds);

    tft.setTextColor(COLOR_BAD);
    tft.setTextSize(3);
    tft.setCursor(SCREEN_WIDTH/2 - 40, 150);
    tft.print(timeStr);

    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setTextSize(1);
    tft.setCursor(SCREEN_WIDTH/2 - 30, 190);
    tft.print("Aguarde...");
    UNLOCK_TFT();
}

void safety_draw_confirm_screen() {
    LOCK_TFT();
    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextColor(COLOR_WARNING);
    tft.setTextSize(2);
    tft.setCursor(20, 40);
    tft.print("AVISO IMPORTANTE");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(1);
    tft.setCursor(10, 80);
    tft.print("Para medir 220V AC com seguranca:");

    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setCursor(10, 110);
    tft.print("* Fusivel Rapido 5A Instalado");
    tft.setCursor(10, 130);
    tft.print("* Varistor 14D431 + TVS P6KE400A");
    tft.setCursor(10, 150);
    tft.print("* Filtro RC 100nF/10uF");

    tft.fillRoundRect(10, 180, SCREEN_WIDTH/2 - 15, 30, 4, COLOR_TEXT);
    tft.setTextColor(COLOR_BACKGROUND);
    tft.setCursor(15, 188);
    tft.print("TENHO PROTECAO");

    tft.fillRoundRect(SCREEN_WIDTH/2 + 5, 180, SCREEN_WIDTH/2 - 15, 30, 4, COLOR_SURFACE);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(SCREEN_WIDTH/2 + 15, 188);
    tft.print("CANCELAR");
    UNLOCK_TFT();
}

void safety_draw_check_screen() {
    LOCK_TFT();
    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextColor(COLOR_GOOD);
    tft.setTextSize(2);
    tft.setCursor(SCREEN_WIDTH/2 - 60, 60);
    tft.print("VERIFICANDO");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(1);
    tft.setCursor(SCREEN_WIDTH/2 - 60, 100);
    tft.print("Verificando tensoes...");

    tft.setCursor(20, 130);
    tft.print("Nao conecte componentes");
    tft.setCursor(20, 150);
    tft.print("energizados!");
    UNLOCK_TFT();
}

// ============================================================================
// BLOQUEIO
// ============================================================================

void safety_activate_lockout() {
    if(safetyAutoLockoutEnabled) {
        safetyStatus.state = SAFETY_STATE_LOCKOUT;
        safetyStatus.lockoutEndTime = millis() + SAFETY_LOCKOUT_MS;

        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, HIGH);

        if(safetySoundEnabled) {
            safety_alert_sound_danger();
        }

        LOG_SERIAL_F("Bloqueio de seguranca ativado");
    }
}

void safety_deactivate_lockout() {
    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.lockoutEndTime = 0;

    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, LOW);

    if(safetySoundEnabled) {
        buzzer_beep(SAFETY_BEEP_OK, 200);
    }

    LOG_SERIAL_F("Bloqueio de seguranca desativado");
}

bool safety_is_locked_out() {
    if(safetyStatus.state == SAFETY_STATE_LOCKOUT) {
        unsigned long now = millis();
        if(now >= safetyStatus.lockoutEndTime) {
            safety_deactivate_lockout();
            return false;
        }
        return true;
    }
    return false;
}

bool safety_can_proceed() {
    if(safety_is_locked_out()) {
        return false;
    }

    if(!safetyStatus.safetyAcknowledged) {
        return false;
    }

    if(safetyStatus.state >= SAFETY_STATE_DANGER) {
        return false;
    }

    return true;
}

void safety_acknowledge_warning() {
    safetyStatus.safetyAcknowledged = true;
    safetyStatus.state = SAFETY_STATE_SAFE;
}

// ============================================================================
// SPLASH DE SEGURANCA
// ============================================================================

void safety_draw_splash() {
    LOCK_TFT();
    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextColor(COLOR_WARNING);
    tft.setTextSize(2);
    tft.setCursor(20, 50);
    tft.print("AVISO DE SEGURANCA");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(1);
    tft.setCursor(10, 90);
    tft.print("Este equipamento pode");
    tft.setCursor(10, 115);
    tft.print("medir tensoes ate 250V AC");

    tft.setTextColor(COLOR_BAD);
    tft.setTextSize(2);
    tft.setCursor(10, 150);
    tft.print("CUIDADO: 220V E PERIGOSO!");

    tft.setTextColor(COLOR_TEXT_DIM);
    tft.setTextSize(1);
    tft.setCursor(10, 185);
    tft.print("Use sempre protecao adequada");
    tft.setCursor(10, 205);
    tft.print("e Circuitos de protecao");
    UNLOCK_TFT();
}

void safety_show_splash_animated() {
    safety_draw_splash();

    for(uint8_t i = 0; i < 3; i++) {
        buzzer_beep(SAFETY_BEEP_WARNING, 100);
        vTaskDelay(pdMS_TO_TICKS(150));
    }

    vTaskDelay(pdMS_TO_TICKS(3000));
}

// ============================================================================
// MENU
// ============================================================================

bool safety_check_before_multimeter() {
    if(!safetyCheckEnabled) {
        return true;
    }

    if(safety_is_locked_out()) {
        safety_draw_lockout_screen(safetyStatus.lockoutEndTime - millis());
        return false;
    }

    SafetyCheckResult check = safety_detect_danger();

    if(!check.isSafe) {
        safety_trigger_alert(check.alertLevel);

        if(safetyStatus.state == SAFETY_STATE_DANGER) {
            safety_activate_lockout();
        }

        return false;
    }

    return true;
}

bool safety_confirm_electrical_measurement() {
    safety_draw_confirm_screen();

    safetyStatus.state = SAFETY_STATE_CONFIRMATION;

    uint32_t startTime = millis();

    while(millis() - startTime < TIME_CONFIRM_TIMEOUT) {
        if(touch.touched()) {
            TS_Point raw = touch.getPoint();
            uint16_t tx = 0;
            uint16_t ty = 0;

            if (!map_touch_to_screen_safe(raw, tx, ty)) {
                delay(50);
                vTaskDelay(pdMS_TO_TICKS(1));
                continue;
            }

            // Mapeamento para botões já em coordenada de tela
            if(ty > 180 && ty < 210) {
                if(tx > 10 && tx < SCREEN_WIDTH/2 - 15) {
                    safetyStatus.hasFuseInstalled = true;
                    safetyStatus.hasVaristorInstalled = true;
                    safetyStatus.safetyAcknowledged = true;

                    if(safetySoundEnabled) {
                        safety_alert_sound_confirm();
                    }

                    return true;
                } else if(tx > SCREEN_WIDTH/2 + 5) {
                    if(safetySoundEnabled) {
                        buzzer_beep(BUZZER_FREQ_ERROR, 100);
                    }

                    return false;
                }
            }
        }

        delay(50);
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    return false;
}

// ============================================================================
// ATUALIZACAO
// ============================================================================

void safety_update() {
    if(!safetyCheckEnabled) {
        return;
    }

    if(safety_is_locked_out()) {
        safety_draw_lockout_screen(safetyStatus.lockoutEndTime - millis());
    }

    if(alertActive && safetyLedEnabled) {
        static unsigned long lastFlash = 0;
        if(millis() - lastFlash > SAFETY_LED_FLASH_FAST) {
            digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
            lastFlash = millis();
        }
    }
}

// ============================================================================
// DETECAO AUTOMATICA
// ============================================================================

SafetyCheckResult safety_detect_danger() {
    SafetyCheckResult result;
    memset(&result, 0, sizeof(SafetyCheckResult));

    uint16_t adcValue = analogRead(PIN_ADC_ZMPT);

    float voltage = (adcValue - ZMPT_ZERO_POINT) * ZMPT_SCALE_FACTOR / 2048.0f;
    voltage = fabsf(voltage);
    
    vTaskDelay(1); // Pequeno yield para o sistema

    safetyStatus.lastDetectedVoltage = voltage;
    safetyStatus.lastCheckTime = millis();

    result = safety_check_voltage(voltage);

    if(!result.isSafe) {
        safetyStatus.dangerCount++;

        if(safetyStatus.dangerCount >= 3) {
            safety_activate_lockout();
        }
    } else {
        safetyStatus.dangerCount = 0;
    }

    return result;
}

// ============================================================================
// CONFIGURACAO
// ============================================================================

void safety_set_check_enabled(bool enabled) {
    safetyCheckEnabled = enabled;
}

void safety_set_sound_enabled(bool enabled) {
    safetySoundEnabled = enabled;
}

void safety_set_led_enabled(bool enabled) {
    safetyLedEnabled = enabled;
}

void safety_set_lockout_enabled(bool enabled) {
    safetyAutoLockoutEnabled = enabled;
}



