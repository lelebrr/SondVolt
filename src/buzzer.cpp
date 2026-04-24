// ============================================================================
// Component Tester PRO v3.0 — Buzzer/Speaker (CYD Edition)
// ============================================================================
// O ESP32 não tem analogWrite()/tone() nativos.
// Usamos o periférico LEDC (LED Control) para gerar PWM no speaker.
// A CYD tem um conector de speaker no GPIO 26.
// ============================================================================

#include "buzzer.h"
#include "globals.h"
#include <Arduino.h>

// Variáveis para controle do buzzer (não-bloqueante)
unsigned long buzzerStartTime = 0;
unsigned long buzzerDuration = 0;
bool buzzerActive = false;

// ============================================================================
// INICIALIZAÇÃO DO BUZZER VIA LEDC
// ============================================================================
void buzzer_init() {
  // Configura o pino do buzzer com LEDC
  // ESP32 Arduino Core 3.x: ledcAttach(pin, freq, resolution)
  ledcAttach(PIN_BUZZER, LEDC_FREQ_BUZZER, LEDC_RESOLUTION);
  ledcWrite(PIN_BUZZER, 0); // Começa desligado
}

// ============================================================================
// TOCAR BEEP
// ============================================================================
void play_beep(int duration_ms) {
  if (deviceSettings.silentMode) return;

  // Gerar tom quadrado a 2kHz com duty cycle de 50%
  ledcWriteTone(PIN_BUZZER, LEDC_FREQ_BUZZER);
  buzzerStartTime = currentMillis;
  buzzerDuration = duration_ms;
  buzzerActive = true;
}

// ============================================================================
// TOCAR TOM EM FREQUÊNCIA ESPECÍFICA
// ============================================================================
void play_tone(int freq, int duration_ms) {
  if (deviceSettings.silentMode) return;

  ledcWriteTone(PIN_BUZZER, freq);
  buzzerStartTime = currentMillis;
  buzzerDuration = duration_ms;
  buzzerActive = true;
}

// ============================================================================
// BEEP LONGO (alerta)
// ============================================================================
void play_long_beep() {
  play_beep(2000);
}

// ============================================================================
// PARAR BEEP
// ============================================================================
void stop_beep() {
  ledcWriteTone(PIN_BUZZER, 0); // Frequência 0 = silêncio
  buzzerActive = false;
}

// ============================================================================
// UPDATE (não-bloqueante, chamar a cada loop)
// ============================================================================
void buzzer_update() {
  if (buzzerActive && (currentMillis - buzzerStartTime >= buzzerDuration)) {
    stop_beep();
  }
}
