#include "safety.h"
#include "config.h"
#include "hal.h"
#include "leds.h"
#include "multimeter.h"
#include "diagnostics.h"
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

static bool alertActive = false;

// A conversao de coordenadas do toque agora e unica no projeto (hal.cpp).
// Esta funcao existia com constantes e orientacao DIFERENTES das usadas na
// interface, entao os botoes das telas de seguranca ficavam espelhados.
static bool map_touch_to_screen_safe(uint16_t& outX, uint16_t& outY) {
    return hal_touch_read(&outX, &outY);
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

    // Os LEDs da CYD sao de anodo comum; a HAL cuida da polaridade e do
    // compartilhamento do GPIO4 com o barramento OneWire.
    hal_led_write(PIN_LED_RED, false);
    hal_led_write(PIN_LED_GREEN, true);

    LOG_SERIAL_F("[SEG] Sistema de seguranca inicializado");
}

void safety_reset() {
    memset(&safetyStatus, 0, sizeof(SafetyStatus));

    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.safetyAcknowledged = true;

    hal_led_write(PIN_LED_RED, false);
    hal_led_write(PIN_LED_GREEN, true);

    safety_alert_stop();
}

// ============================================================================
// VERIFICACAO
// ============================================================================

SafetyCheckResult safety_check_voltage(float voltage) {
    SafetyCheckResult result;
    memset(&result, 0, sizeof(SafetyCheckResult));

    result.detectedVoltage = voltage;

    // ATENCAO A ORDEM. Na versao anterior o primeiro ramo capturava tudo
    // acima de 50 V como CRITICO, deixando os ramos de 220 V e 127 V
    // inalcancaveis: o aparelho nunca distinguia uma rede normal de um surto.
    // Agora os limiares sao testados do maior para o menor.
    if(voltage > SAFETY_VOLTAGE_220V_MAX) {
        // Acima da tensao maxima de rede: surto ou ponta na fase errada.
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_CRITICAL;
        result.message = SAFETY_MSG_DANGER_HIGH;
    } else if(voltage >= SAFETY_VOLTAGE_220V_MIN) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_HIGH;
        result.message = SAFETY_MSG_DANGER_220V;
    } else if(voltage >= SAFETY_VOLTAGE_110V_MIN) {
        result.isAcDanger = true;
        result.alertLevel = SAFETY_ALERT_MEDIUM;
        result.message = SAFETY_MSG_DANGER_HIGH;
    } else if(voltage >= SAFETY_VOLTAGE_DANGER_AC) {
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
    // O limiar aqui era 180 V, enquanto safety_check_voltage() ja considera
    // perigoso a partir de 50 V. Quem chamasse esta funcao para decidir se
    // podia prosseguir tratava 127 V da rede como seguro. Um unico limiar
    // agora, e ele e o mesmo que o resto do modulo usa.
    return (voltage >= SAFETY_VOLTAGE_DANGER_AC);
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
            hal_led_write(PIN_LED_RED, true);
        }

    } else {
        safetyStatus.state = SAFETY_STATE_SAFE;
        safety_alert_stop();
    }
}

void safety_alert_led_flash(bool enable) {
    // Antes esta funcao bloqueava a tarefa por 200 ms a cada chamada. Agora
    // apenas configura o padrao; quem anima e leds_update().
    if(enable) led_status_danger();
    else       led_off();
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
    led_off();
    hal_led_write(PIN_LED_GREEN, true);
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
    tft.print("CONTINUAR MESMO ASSIM");

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

        hal_led_write(PIN_LED_GREEN, false);
        led_status_danger();
        diag_count_lockout();

        if(safetySoundEnabled) {
            safety_alert_sound_danger();
        }

        LOG_SERIAL_F("[SEG] Bloqueio de seguranca ativado");
    }
}

void safety_deactivate_lockout() {
    safetyStatus.state = SAFETY_STATE_SAFE;
    safetyStatus.lockoutEndTime = 0;

    led_off();
    hal_led_write(PIN_LED_GREEN, true);

    if(safetySoundEnabled) {
        buzzer_beep(SAFETY_BEEP_OK, 200);
    }

    LOG_SERIAL_F("[SEG] Bloqueio de seguranca desativado");
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
        uint32_t now = millis();
        uint32_t remaining = (safetyStatus.lockoutEndTime > now)
                           ? (safetyStatus.lockoutEndTime - now) : 0;
        safety_draw_lockout_screen(remaining);
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
        {
            uint16_t tx = 0;
            uint16_t ty = 0;

            if (!map_touch_to_screen_safe(tx, ty)) {
                vTaskDelay(pdMS_TO_TICKS(30));
                continue;
            }

            // As areas de toque tem que casar com os retangulos realmente
            // desenhados por safety_draw_confirm_screen(). Antes o botao de
            // confirmar aceitava toque ate x=145 mas era desenhado ate 155, e
            // o de cancelar nao tinha limite superior nenhum - dava para
            // cancelar tocando fora da tela do dialogo.
            const int16_t kBtnY = 182, kBtnH = 30;
            const int16_t kOkX  = 10,  kOkW  = SCREEN_WIDTH / 2 - 20;
            const int16_t kNoX  = SCREEN_WIDTH / 2 + 10;
            const int16_t kNoW  = SCREEN_WIDTH / 2 - 20;

            if(ty >= kBtnY && ty <= kBtnY + kBtnH) {
                if(tx >= kOkX && tx <= kOkX + kOkW) {
                    safetyStatus.hasFuseInstalled = true;
                    safetyStatus.hasVaristorInstalled = true;
                    safetyStatus.safetyAcknowledged = true;

                    if(safetySoundEnabled) {
                        safety_alert_sound_confirm();
                    }

                    return true;
                } else if(tx >= kNoX && tx <= kNoX + kNoW) {
                    if(safetySoundEnabled) {
                        buzzer_beep(BUZZER_FREQ_ERROR, 100);
                    }

                    return false;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(30));
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

    // A tela de bloqueio e desenhada UMA vez, e o contador e atualizado a
    // cada segundo. A versao anterior redesenhava a tela inteira a cada
    // ciclo de 100 ms, a partir da tarefa de medicao, brigando com a tarefa
    // de interface pelo barramento SPI.
    static bool     lockoutScreenDrawn = false;
    static uint32_t lastCountdown = 0;

    if(safety_is_locked_out()) {
        uint32_t now = millis();
        // Subtracao entre unsigned da a volta: sem o teste, um lockout que ja
        // venceu mostraria 49 dias de contagem regressiva.
        uint32_t remaining = (safetyStatus.lockoutEndTime > now)
                           ? (safetyStatus.lockoutEndTime - now) : 0;

        if(!lockoutScreenDrawn || (now - lastCountdown) >= 1000) {
            safety_draw_lockout_screen(remaining);
            lockoutScreenDrawn = true;
            lastCountdown = now;
        }
    } else {
        lockoutScreenDrawn = false;
    }

    // O pisca-pisca do LED e responsabilidade de leds_update(); aqui apenas
    // garantimos que o padrao certo esteja armado.
    if(alertActive && safetyLedEnabled) {
        led_status_danger();
    }
}

// ============================================================================
// DETECAO AUTOMATICA
// ============================================================================

SafetyCheckResult safety_detect_danger() {
    SafetyCheckResult result;
    memset(&result, 0, sizeof(SafetyCheckResult));

    // ------------------------------------------------------------------
    // ESTE ERA O BUG MAIS PERIGOSO DO FIRMWARE
    // ------------------------------------------------------------------
    // A versao anterior calculava:
    //     voltage = (adc - 2048) * ZMPT_SCALE_FACTOR / 2048
    // com ZMPT_SCALE_FACTOR igual a 1.0, ou seja, um numero normalizado
    // entre 0 e 1. Esse valor era entao comparado com limiares de 50 V,
    // 180 V e 250 V. Como 1.0 nunca chega a 50, safety_check_voltage()
    // SEMPRE devolvia "seguro" e o bloqueio automatico jamais disparava.
    // O sistema de protecao existia no papel e nao fazia nada.
    //
    // Agora usamos o mesmo motor True RMS calibrado do multimetro, que
    // devolve volts de verdade.
    // ------------------------------------------------------------------
    float voltage = multimeter_read_ac_voltage_rms();

    safetyStatus.lastDetectedVoltage = voltage;
    safetyStatus.lastCheckTime = millis();

    result = safety_check_voltage(voltage);

    if(!result.isSafe) {
        safetyStatus.dangerCount++;

        // Tres leituras perigosas seguidas: nao e ruido, e a rede.
        if(safetyStatus.dangerCount >= 3) {
            safety_activate_lockout();
        }
    } else {
        safetyStatus.dangerCount = 0;
    }

    return result;
}

// Tensao perigosa medida na ultima verificacao, em volts.
float safety_get_last_voltage() {
    return safetyStatus.lastDetectedVoltage;
}

// Texto curto do estado atual, para a barra de status.
const char* safety_state_text() {
    switch(safetyStatus.state) {
        case SAFETY_STATE_SAFE:         return "SEGURO";
        case SAFETY_STATE_CHECKING:     return "VERIFICANDO";
        case SAFETY_STATE_DANGER:       return "PERIGO";
        case SAFETY_STATE_LOCKOUT:      return "BLOQUEADO";
        case SAFETY_STATE_CONFIRMATION: return "CONFIRMANDO";
        case SAFETY_STATE_WARNING:      return "ATENCAO";
        default:                        return "?";
    }
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



