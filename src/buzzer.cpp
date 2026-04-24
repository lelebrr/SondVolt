// ============================================================================
// Sondvolt v3.0 — Buzzer (TFT_eSPI LEDC)
// ============================================================================

#include "buzzer.h"
#include "globals.h"
#include "config.h"

static bool buzzerActive = false;
static unsigned long buzzerStartTime = 0;
static unsigned long buzzerDuration = 0;
static unsigned int currentFreq = 0;

void buzzer_init() {
    ledcSetup(LEDC_CH_BUZZER, LEDC_FREQ_BUZZER, LEDC_TIMER_BIT);
    ledcAttachPin(PIN_BUZZER, LEDC_CH_BUZZER);
    ledcWrite(LEDC_CH_BUZZER, 0);
    LOG_SERIAL_F("Buzzer OK");
}

void buzzer_beep(unsigned int freq) {
    buzzer_beep_duration(freq, 100);
}

void buzzer_beep_duration(unsigned int freq, unsigned long durationMs) {
    if (deviceSettings.silentMode) return;
    buzzerStartTime = millis();
    buzzerDuration = durationMs;
    buzzerActive = true;
    currentFreq = freq;

    ledcSetup(LEDC_CH_BUZZER, freq, LEDC_TIMER_BIT);
    ledcAttachPin(PIN_BUZZER, LEDC_CH_BUZZER);
    ledcWrite(LEDC_CH_BUZZER, 128);
}

void buzzer_tone(unsigned int freq) {
    if (deviceSettings.silentMode) return;
    ledcSetup(LEDC_CH_BUZZER, freq, LEDC_TIMER_BIT);
    ledcWrite(LEDC_CH_BUZZER, 128);
    buzzerActive = true;
    currentFreq = freq;
}

void buzzer_no_tone() {
    ledcWrite(LEDC_CH_BUZZER, 0);
    buzzerActive = false;
}

void buzzer_update() {
    if (buzzerActive) {
        unsigned long now = millis();
        if (now - buzzerStartTime >= buzzerDuration) {
            buzzer_no_tone();
            buzzerActive = false;
        }
    }
}

void buzzer_ok() {
    if (deviceSettings.silentMode) return;
    buzzer_beep_duration(1000, 80);
    delay(100);
    buzzer_beep_duration(1500, 120);
}

void buzzer_error() {
    if (deviceSettings.silentMode) return;
    buzzer_beep_duration(300, 300);
}

void buzzer_alert() {
    if (deviceSettings.silentMode) return;
    buzzer_beep_duration(800, 150);
    delay(200);
    buzzer_beep_duration(600, 150);
    delay(200);
    buzzer_beep_duration(800, 150);
}

void buzzer_click() {
    if (deviceSettings.silentMode) return;
    buzzer_beep_duration(2000, 30);
}