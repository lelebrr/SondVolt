// ============================================================================
// Component Tester PRO v3.0 — LEDs Indicadores (CYD Edition)
// ============================================================================
// A CYD tem um LED RGB integrado. Usamos:
//   - GPIO 16 → Canal Verde
//   - GPIO 17 → Canal Vermelho
// Nota: O LED RGB da CYD é ativo LOW em algumas versões.
// Ajuste a lógica se necessário (HIGH=ON vs LOW=ON).
// ============================================================================

#include "leds.h"
#include "globals.h"

// Variáveis de controle
bool flashingBothLeds = false;
bool flashingGreenLedSlow = false;
bool flashingRedLedFast = false;
unsigned long ledFlashTimer = 0;
unsigned long ledFlashInterval = 0;

// ============================================================================
// ATUALIZAÇÃO DOS LEDS (não-bloqueante)
// ============================================================================
void update_leds() {
  // Gerenciar animações de flash
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

  // Aplicar estado aos pinos
  // Nota: Alguns modelos da CYD têm LED ativo LOW (invertido).
  // Se os LEDs funcionam ao contrário, troque HIGH/LOW abaixo.
  digitalWrite(PIN_LED_GREEN, greenLedState ? HIGH : LOW);
  digitalWrite(PIN_LED_RED, redLedState ? HIGH : LOW);
}

// ============================================================================
// CONTROLE DIRETO DOS LEDS
// ============================================================================
void set_green_led(bool state) {
  greenLedState = state;
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
}

void set_red_led(bool state) {
  redLedState = state;
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
}

// ============================================================================
// ANIMAÇÕES DE FLASH
// ============================================================================
void flash_both_leds(int delay_ms) {
  flashingBothLeds = true;
  flashingGreenLedSlow = false;
  flashingRedLedFast = false;
  ledFlashInterval = delay_ms;
  ledFlashTimer = currentMillis;
}

void flash_green_led_slow() {
  flashingBothLeds = false;
  flashingGreenLedSlow = true;
  flashingRedLedFast = false;
  ledFlashInterval = 500;
  ledFlashTimer = currentMillis;
}

void flash_red_led_fast() {
  flashingBothLeds = false;
  flashingGreenLedSlow = false;
  flashingRedLedFast = true;
  ledFlashInterval = 100;
  ledFlashTimer = currentMillis;
}