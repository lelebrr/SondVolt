// ============================================================================
// Sondvolt v3.x — Sistema de Seguranca Eletrica
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: safety.cpp
// Descricao: Implementacao do sistema de seguranca contra tensoes perigosas
// ============================================================================

#include "safety.h"
#include "config.h"
#include "pins.h"
#include "graphics.h"
#include "buzzer.h"
#include "display_globals.h"
#include "display_mutex.h"


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

// ============================================================================
// INICIALIZACAO
// ============================================================================

void safety_init() {
    memset(&safetyStatus, 0, sizeof(SafetyStatus));

    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.hasFuseInstalled = false;
    safetyStatus.hasVaristorInstalled = false;
    safetyStatus.safetyAcknowledged = true;

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);

    DBG("Sistema de seguranca inicializado");
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
    tft.fillScreen(SAFETY_COLOR_ALERT_BG);

    tft.setTextColor(SAFETY_COLOR_DANGER);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FMB);
    tft.drawString("ALERTA DE SEGURANCA", SCREEN_W/2, 40);

    char voltStr[32];
    snprintf(voltStr, sizeof(voltStr), "%.1f V", voltage);

    tft.setTextColor(C_WHITE);
    tft.setFreeFont(FMB);
    tft.drawString(voltStr, SCREEN_W/2, 80);

    tft.setTextColor(C_YELLOW);
    tft.setFreeFont(FMB);
    tft.drawString(message, SCREEN_W/2, 120);

    tft.fillRoundRect(20, 160, SCREEN_W - 40, 30, 4, C_WHITE);
    tft.setTextColor(C_BLACK);
    tft.drawString("CONTINUAR IGUALVEL", SCREEN_W/2, 175);

    tft.fillRoundRect(20, 200, SCREEN_W - 40, 30, 4, C_SURFACE);
    tft.setTextColor(C_TEXT);
    tft.drawString("CANCELAR", SCREEN_W/2, 215);
    UNLOCK_TFT();
}

void safety_draw_lockout_screen(unsigned long remainingMs) {
    LOCK_TFT();
    tft.fillScreen(C_BLACK);

    tft.setTextColor(SAFETY_COLOR_WARNING);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FMB);
    tft.drawString("BLOQUEADO", SCREEN_W/2, 60);

    tft.setTextColor(C_TEXT);
    tft.setFreeFont(FMB);
    tft.drawString("Equipamento bloqueado por", SCREEN_W/2, 90);
    tft.drawString("seguranca", SCREEN_W/2, 110);

    char timeStr[32];
    uint16_t seconds = remainingMs / 1000;
    snprintf(timeStr, sizeof(timeStr), "%d seg", seconds);

    tft.setTextColor(SAFETY_COLOR_DANGER);
    tft.setFreeFont(FMB);
    tft.drawString(timeStr, SCREEN_W/2, 150);

    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setFreeFont(FMBO);
    tft.drawString("Aguarde...", SCREEN_W/2, 190);
    UNLOCK_TFT();
}

void safety_draw_confirm_screen() {
    LOCK_TFT();
    tft.fillScreen(C_BACKGROUND);

    tft.setTextColor(C_WARNING);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FMB);
    tft.drawString("AVISO IMPORTANTE", SCREEN_W/2, 40);

    tft.setTextColor(C_TEXT);
    tft.setFreeFont(FMB);
    tft.drawString("Para medir 220V AC, voce deve ter:", SCREEN_W/2, 80);

    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setFreeFont(FMB);
    tft.drawString("* Fusivel de protecao (1A)", SCREEN_W/2, 110);
    tft.drawString("* Varistor (275V)", SCREEN_W/2, 130);
    tft.drawString("* Circuitos de protecao", SCREEN_W/2, 150);

    tft.fillRoundRect(10, 180, SCREEN_W/2 - 15, 30, 4, C_WHITE);
    tft.setTextColor(C_BLACK);
    tft.drawString("TENHO PROTECAO", SCREEN_W/2 - 40, 195);

    tft.fillRoundRect(SCREEN_W/2 + 5, 180, SCREEN_W/2 - 15, 30, 4, C_SURFACE);
    tft.setTextColor(C_TEXT);
    tft.drawString("CANCELAR", SCREEN_W/2 + 40, 195);
    UNLOCK_TFT();
}

void safety_draw_check_screen() {
    LOCK_TFT();
    tft.fillScreen(C_BLACK);

    tft.setTextColor(SAFETY_COLOR_SAFE);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FMB);
    tft.drawString("VERIFICANDO", SCREEN_W/2, 60);

    tft.setTextColor(C_TEXT);
    tft.setFreeFont(FMB);
    tft.drawString("Verificando tensoes...", SCREEN_W/2, 100);

    tft.drawString("Nao conecte componentes", SCREEN_W/2, 130);
    tft.drawString("energizados!", SCREEN_W/2, 150);
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

        DBG("Bloqueio de seguranca ativado");
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

    DBG("Bloqueio de seguranca desativado");
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
    tft.fillScreen(C_BLACK);

    tft.setTextColor(SAFETY_COLOR_WARNING);
    tft.setTextDatum(MC_DATUM);
    tft.setFreeFont(FMB);
    tft.drawString("AVISO DE SEGURANCA", SCREEN_W/2, 50);

    tft.setTextColor(C_TEXT);
    tft.setFreeFont(FMB);
    tft.drawString("Este equipamento pode", SCREEN_W/2, 90);
    tft.drawString("medir tensoes ate 250V AC", SCREEN_W/2, 115);

    tft.setTextColor(C_ERROR);
    tft.setFreeFont(FMB);
    tft.drawString("CUIDADO: 220V E PERIGOSO!", SCREEN_W/2, 150);

    tft.setTextColor(C_TEXT_SECONDARY);
    tft.setFreeFont(FMBO);
    tft.drawString("Use sempre protecao adequada", SCREEN_W/2, 185);
    tft.drawString("e Circuitos de protecao", SCREEN_W/2, 205);
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
        uint16_t x, y;
        if(tft.getTouch(&x, &y)) {
            if(y > 180 && y < 210) {
                if(x > 10 && x < SCREEN_W/2 - 15) {
                    safetyStatus.hasFuseInstalled = true;
                    safetyStatus.hasVaristorInstalled = true;
                    safetyStatus.safetyAcknowledged = true;

                    if(safetySoundEnabled) {
                        safety_alert_sound_confirm();
                    }

                    return true;
                } else if(x > SCREEN_W/2 + 5) {
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
    voltage = abs(voltage);
    
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
