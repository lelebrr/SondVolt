#ifndef BUZZER_H
#define BUZZER_H

#include "config.h"
#include <Arduino.h>

// Funções para controle do buzzer
void play_beep(int duration_ms);
void play_long_beep();
void stop_beep();
void buzzer_update();

#endif // BUZZER_H
