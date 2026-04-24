// ============================================================================
// Component Tester PRO v3.0 — Buzzer/Speaker (CYD Edition)
// ============================================================================
#ifndef BUZZER_H
#define BUZZER_H

#include "config.h"
#include <Arduino.h>

// Funções para controle do buzzer/speaker via LEDC PWM
void buzzer_init();           // Inicializa o canal LEDC para o buzzer
void play_beep(int duration_ms);  // Toca um beep com duração em ms
void play_long_beep();        // Toca um beep longo (2 segundos)
void play_tone(int freq, int duration_ms); // Toca um tom em frequência específica
void stop_beep();             // Para o beep imediatamente
void buzzer_update();         // Gerenciador não-bloqueante (chamar no loop)

#endif // BUZZER_H
