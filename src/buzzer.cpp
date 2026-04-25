// ============================================================================
// Sondvolt v3.0 — Buzzer (Implementação)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================

#include "buzzer.h"
#include "config.h"
#include "globals.h"

static bool buzzerActive = false;
static unsigned long buzzerStartTime = 0;
static unsigned long buzzerDuration = 0;

void buzzer_init() {
    pinMode(PIN_BUZZER, OUTPUT);
    // Usando LEDC para controle de tom no ESP32
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(PIN_BUZZER, 1000, 10);
#else
    ledcSetup(0, 1000, 10);
    ledcAttachPin(PIN_BUZZER, 0);
#endif
    buzzer_no_tone();
}

void buzzer_beep(unsigned int freq, unsigned long durationMs) {
    if (!deviceSettings.soundEnabled) return;
    
    buzzer_tone(freq);
    buzzerStartTime = millis();
    buzzerDuration = durationMs;
    buzzerActive = true;
}

void buzzer_tone(unsigned int freq) {
    if (!deviceSettings.soundEnabled) return;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWriteTone(PIN_BUZZER, freq);
#else
    ledcWriteTone(0, freq);
#endif
}

void buzzer_no_tone() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PIN_BUZZER, 0);
#else
    ledcWrite(0, 0);
#endif
    buzzerActive = false;
}

void buzzer_update() {
    if (buzzerActive) {
        if (millis() - buzzerStartTime >= buzzerDuration) {
            buzzer_no_tone();
        }
    }
}

// --- Funções de Feedback ---

void buzzer_ok() {
    // Som agudo e curto (ascendente para sucesso)
    buzzer_beep(1200, 50);
    delay(60);
    buzzer_beep(1800, 80);
}

void buzzer_error() {
    // Som grave e longo (descendente para erro)
    buzzer_beep(400, 400);
}

void buzzer_alert() {
    // Sequência de 3 bips curtos
    for(int i=0; i<3; i++) {
        buzzer_beep(2000, 50);
        delay(100);
    }
}

void buzzer_click() {
    buzzer_beep(BUZZ_FREQ_CLICK, 30);
}

void buzzer_measure_start() {
    buzzer_beep(600, 50);
}

void buzzer_measure_end() {
    buzzer_beep(1200, 80);
}

void buzzer_success() {
    buzzer_beep(1500, 50);
    delay(50);
    buzzer_beep(2000, 100);
}

void buzzer_discharge() {
    // Som descendente "sci-fi" para descarga
    for(int f=2000; f>400; f-=100) {
        buzzer_tone(f);
        delay(30);
    }
    buzzer_no_tone();
}

void buzzer_completion() {
    buzzer_beep(880, 100);
    delay(50);
    buzzer_beep(1320, 150);
}