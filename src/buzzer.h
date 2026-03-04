#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "config.h"

// Funções para controle do buzzer
void play_beep(int duration_ms);
void play_long_beep();
void stop_beep();

#endif // BUZZER_H
