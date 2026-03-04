#include "buzzer.h"
#include "globals.h"

// Variáveis para controle do buzzer
unsigned long buzzerStartTime = 0;
int buzzerDuration = 0;
bool buzzerActive = false;

// Toca um beep por uma duração específica
void play_beep(int duration_ms) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerStartTime = currentMillis;
    buzzerDuration = duration_ms;
    buzzerActive = true;
}

// Toca um beep longo (ex: para alertas de perigo)
void play_long_beep() {
    play_beep(2000); // Beep de 2 segundos
}

// Para o beep, se estiver ativo
void stop_beep() {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
}

// Função para ser chamada no loop para gerenciar o buzzer (não bloqueante)
void buzzer_update() {
    if (buzzerActive && (currentMillis - buzzerStartTime >= buzzerDuration)) {
        stop_beep();
    }
}
