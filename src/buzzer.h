// ============================================================================
// Sondvolt v3.0 — Buzzer / Speaker PAM8002
// ============================================================================
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stddef.h>

void buzzer_init();
void buzzer_update();
void buzzer_beep(unsigned int freq, unsigned long durationMs = 100);
void buzzer_beep_duration(unsigned int freq, unsigned long durationMs);
void buzzer_tone(unsigned int freq);
void buzzer_no_tone();
void buzzer_play_pattern(const uint16_t* pattern, size_t count);
void buzzer_ok();
void buzzer_error();
void buzzer_alert();
void buzzer_click();

bool buzzer_enabled();

#endif // BUZZER_H