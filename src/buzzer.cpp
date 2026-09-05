// ============================================================================
// Sondvolt v4.0 - Buzzer / Audio
// ============================================================================
// Arquivo : buzzer.cpp
//
// Duas correcoes importantes em relacao a versao anterior:
//   1. buzzer_init() nunca era chamado, entao o canal LEDC nunca era
//      configurado e nenhum som saia da placa.
//   2. buzzer_update() tambem nunca era chamado, entao o tom iniciado por
//      buzzer_beep() ficava tocando para sempre. Agora o desligamento e
//      garantido pela propria tarefa de interface.
//
// As sequencias de varios tons usam vTaskDelay em vez de delay() para nao
// travar a tarefa que as chamou.
// ============================================================================

#include "buzzer.h"
#include "hal.h"
#include "config.h"
#include "globals.h"

static bool     gInitialized  = false;
static bool     gToneActive   = false;
static uint32_t gToneStartMs  = 0;
static uint32_t gToneDuration = 0;

// Volume implementado como duty cycle: 50 % e o maximo de energia num
// buzzer piezo; abaixo disso o som fica mais suave.
static uint32_t duty_for_volume() {
    const uint32_t full = (1UL << BUZZER_PWM_BITS) / 2;   // 50 %
    return deviceSettings.strongBeep ? full : (full / 3);
}

void buzzer_init() {
    if (gInitialized) return;
    hal_pwm_attach(PIN_BUZZER, LEDC_CH_BUZZER, 1000, BUZZER_PWM_BITS);
    hal_pwm_stop(PIN_BUZZER, LEDC_CH_BUZZER);
    gInitialized = true;
    LOG_SERIAL_F("[BUZ] Buzzer pronto");
}

static bool sound_allowed() {
    return gInitialized && deviceSettings.soundEnabled && !deviceSettings.silentMode;
}

void buzzer_tone(unsigned int freq) {
    if (!sound_allowed()) return;
    hal_pwm_tone(PIN_BUZZER, LEDC_CH_BUZZER, freq);
    hal_pwm_write(PIN_BUZZER, LEDC_CH_BUZZER, duty_for_volume());
}

void buzzer_no_tone() {
    if (!gInitialized) return;
    hal_pwm_stop(PIN_BUZZER, LEDC_CH_BUZZER);
    gToneActive = false;
}

void buzzer_beep(unsigned int freq, unsigned long durationMs) {
    if (!sound_allowed()) return;
    buzzer_tone(freq);
    gToneStartMs  = millis();
    gToneDuration = durationMs;
    gToneActive   = true;
}

// Chamada a cada ciclo da interface: e o que garante que o tom pare.
void buzzer_update() {
    if (!gToneActive) return;
    if ((millis() - gToneStartMs) >= gToneDuration) {
        buzzer_no_tone();
    }
}

// Toca um tom de forma sincrona, cedendo o processador enquanto espera.
static void blocking_tone(unsigned int freq, uint32_t ms) {
    if (!sound_allowed()) return;
    buzzer_tone(freq);
    vTaskDelay(pdMS_TO_TICKS(ms));
    buzzer_no_tone();
}

// ----------------------------------------------------------------------------
// Vocabulario sonoro do aparelho
// ----------------------------------------------------------------------------
// Cada evento tem uma assinatura distinta para o tecnico reconhecer o
// resultado sem tirar os olhos da placa.

void buzzer_click() {
    buzzer_beep(BUZZ_FREQ_CLICK, 25);
}

void buzzer_ok() {
    // Duas notas subindo: deu certo.
    blocking_tone(1200, 45);
    blocking_tone(1800, 70);
}

void buzzer_error() {
    // Nota grave e longa: deu errado.
    blocking_tone(380, 350);
}

void buzzer_alert() {
    // Tres bipes agudos: exige atencao imediata.
    for (uint8_t i = 0; i < 3; i++) {
        blocking_tone(2200, 60);
        vTaskDelay(pdMS_TO_TICKS(70));
    }
}

void buzzer_measure_start() { buzzer_beep(600, 40); }
void buzzer_measure_end()   { buzzer_beep(1200, 60); }

void buzzer_success() {
    blocking_tone(1500, 50);
    blocking_tone(2000, 90);
}

void buzzer_discharge() {
    // Varredura descendente enquanto o capacitor perde carga.
    if (!sound_allowed()) return;
    for (int f = 2000; f > 400; f -= 160) {
        buzzer_tone(f);
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    buzzer_no_tone();
}

void buzzer_completion() {
    blocking_tone(880, 90);
    blocking_tone(1320, 130);
}
