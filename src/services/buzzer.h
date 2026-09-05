// ============================================================================
// Sondvolt v3.0 — Buzzer / Audio (PAM8002)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <Arduino.h>

// Frequências Comuns
#define BUZZ_FREQ_LOW     440
#define BUZZ_FREQ_MID     880
#define BUZZ_FREQ_HIGH    1760
#define BUZZ_FREQ_CLICK   2000

// Aliases para Multímetro e UI
#define BUZZER_FREQ_OK          BUZZ_FREQ_MID
#define BUZZER_FREQ_BTN         BUZZ_FREQ_CLICK
#define BUZZER_FREQ_WARNING     BUZZ_FREQ_HIGH
#define BUZZER_FREQ_ERROR       BUZZ_FREQ_LOW
#define BUZZER_FREQ_DEFAULT     BUZZ_FREQ_MID

#define BUZZER_DURATION_OK      100
#define BUZZER_DURATION_WARNING 300
#define BUZZER_DURATION_ERROR   500

// Protótipos
void buzzer_init();
void buzzer_update();
void buzzer_beep(unsigned int freq, unsigned long durationMs = 100);
void buzzer_tone(unsigned int freq);
void buzzer_no_tone();

// Funções de Feedback Profissional
void buzzer_ok();
void buzzer_error();
void buzzer_alert();
void buzzer_click();
void buzzer_measure_start();
void buzzer_measure_end();
void buzzer_success();
void buzzer_discharge();
void buzzer_completion();

#endif // BUZZER_H