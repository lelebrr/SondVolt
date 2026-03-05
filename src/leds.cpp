#include "leds.h"
#include "globals.h"

bool flashingBothLeds = false;
bool flashingGreenLedSlow = false;
bool flashingRedLedFast = false;
unsigned long ledFlashTimer = 0;
unsigned long ledFlashInterval = 0;

// Atualiza o estado dos LEDs com base nas variáveis globais

void update_leds() {
  if (flashingBothLeds || flashingGreenLedSlow || flashingRedLedFast) {
    if (currentMillis - ledFlashTimer >= ledFlashInterval) {
      ledFlashTimer = currentMillis;
      if (flashingBothLeds) {
        greenLedState = !greenLedState;
        redLedState = !redLedState;
      } else if (flashingGreenLedSlow) {
        greenLedState = !greenLedState;
      } else if (flashingRedLedFast) {
        redLedState = !redLedState;
      }
    }
  }
  digitalWrite(LED_GREEN_PIN, greenLedState);
  digitalWrite(LED_RED_PIN, redLedState);
}

// Define o estado do LED verde (ligado/desligado)
void set_green_led(bool state) {
  greenLedState = state;
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
}

// Define o estado do LED vermelho (ligado/desligado)
void set_red_led(bool state) {
  redLedState = state;
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
}

// Faz ambos os LEDs piscarem rapidamente
void flash_both_leds(int delay_ms) {
  flashingBothLeds = true;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
  ledFlashInterval = delay_ms;
  ledFlashTimer = currentMillis;
}

// Faz o LED verde piscar lentamente
void flash_green_led_slow() {
  flashingBothLeds = false;
  flashingGreenLedSlow = true;
  flashingRedLedFast = false;
  ledFlashInterval = 500; // 500ms para piscar lento
  ledFlashTimer = currentMillis;
}

// Faz o LED vermelho piscar rapidamente
void flash_red_led_fast() {
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = true;
  ledFlashInterval = 100; // 100ms para piscar rápido
  ledFlashTimer = currentMillis;
}