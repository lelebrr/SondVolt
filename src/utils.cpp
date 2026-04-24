// ============================================================================
// Component Tester PRO v3.0 — Utilitários (CYD Edition)
// ============================================================================
// Funções auxiliares: impressão formatada, calibração, timeout, contadores.
// Migrado para TFT_eSPI e ADC 12-bit/3.3V do ESP32.
// ============================================================================

#include "utils.h"
#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "globals.h"
#include "leds.h"
#include "drawings.h"
#include "menu.h"
#include <TFT_eSPI.h>
#include <Print.h>

extern TFT_eSPI tft;

// ============================================================================
// IMPRESSÃO DE FLOAT COM PRECISÃO
// ============================================================================
void fprint(Print &p, float val, int prec) {
  if (val < 0) {
    p.print('-');
    val = -val;
  }
  p.print((long)val);
  if (prec > 0) {
    p.print('.');
    float frac = val - (long)val;
    while (prec--) {
      frac *= 10;
      p.print((int)frac);
      frac -= (int)frac;
    }
  }
}

// ============================================================================
// VARIÁVEIS DE ESTADO
// ============================================================================
unsigned long timeoutStartMillis = 0;
unsigned long timeoutDuration = 30000;
bool timeoutActive = false;
bool holdActive = false;
bool darkMode = false;
bool silentMode = false;
unsigned long measurementCount = 0;
unsigned long faultyCount = 0;

// ============================================================================
// MENSAGEM DE RETORNO
// ============================================================================
void showBackMsg() {
  tft.setTextSize(1);
  tft.setCursor(10, 80);
  tft.println(F("BACK/Touch volta"));
}

// ============================================================================
// CALIBRAÇÃO DAS SONDAS
// ============================================================================
// Adaptado para ADC 12-bit/3.3V do ESP32
void calibrate_probes() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Calibragem", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setCursor(25, 70);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Toque OK com probes em curto"));
  tft.setCursor(25, 90);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("Isso zerara os offsets."));

  draw_footer_modern();

  bool calibrating = true;
  while (calibrating) {
    buttons_update();

    if (isOkPressed()) {
      // Ler offset no ADC 12-bit (3.3V referência)
      float offset1 = analogRead(PIN_PROBE_MAIN) * ADC_VOLTS_PER_BIT;

      tft.fillRect(20, 65, 280, 100, UI_COLOR_BG);
      tft.setCursor(25, 70);
      tft.setTextColor(UI_COLOR_GREEN);
      tft.setTextSize(2);
      tft.println(F("Calibracao OK!"));

      tft.setTextSize(1);
      tft.setCursor(25, 100);
      tft.setTextColor(UI_COLOR_TEXT);
      tft.print(F("Offset Probe: "));
      fprint(tft, offset1, 4);
      tft.println(F(" V"));

      // Salvar offset
      deviceSettings.offset1 = offset1;
      saveSettings();

      tft.setCursor(25, 130);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.println(F("Valores salvos no NVS!"));

      delay(2000);
      calibrating = false;
    }

    if (isBackPressed()) {
      calibrating = false;
    }
  }
}

// ============================================================================
// EXIBIR OFFSETS
// ============================================================================
void set_offsets() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Offsets Atuais", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setCursor(25, 70);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.print(F("Offset 1: "));
  fprint(tft, deviceSettings.offset1, 4);
  tft.println(F(" V"));

  tft.setCursor(25, 90);
  tft.print(F("Offset 2: "));
  fprint(tft, deviceSettings.offset2, 4);
  tft.println(F(" V"));

  tft.setCursor(25, 120);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("Toque OK para recalibrar"));

  draw_footer_modern();
  delay(2000);
}

// ============================================================================
// TIMEOUT DE INATIVIDADE
// ============================================================================
void handle_timeout() {
  if (!timeoutActive) return;

  if (currentMillis - timeoutStartMillis >= timeoutDuration) {
    timeoutActive = false;
    currentAppState = STATE_MENU;
    tft.fillScreen(UI_COLOR_BG);
    flash_both_leds(500);
  }
}

unsigned long safe_millis() { return currentMillis; }

// ============================================================================
// TOGGLES
// ============================================================================
void toggle_dark_mode() {
  darkMode = !darkMode;
  if (darkMode) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  }
}

void toggle_silent_mode() {
  silentMode = !silentMode;
  if (silentMode) {
    stop_beep();
  }
}

// ============================================================================
// CONTADORES DE MEDIÇÃO
// ============================================================================
void increment_measurement_count() { measurementCount++; }
void increment_faulty_count() { faultyCount++; }

void get_measurement_stats(unsigned long *total, unsigned long *faulty) {
  *total = measurementCount;
  *faulty = faultyCount;
}

void reset_counters() {
  measurementCount = 0;
  faultyCount = 0;
}

// ============================================================================
// TIMEOUT
// ============================================================================
void start_timeout(unsigned long duration) {
  timeoutStartMillis = currentMillis;
  timeoutDuration = duration;
  timeoutActive = true;
}

bool is_timeout_active() { return timeoutActive; }

// ============================================================================
// HOLD
// ============================================================================
void toggle_hold() {
  holdActive = !holdActive;
  if (holdActive) {
    flash_green_led_slow();
  } else {
    set_green_led(false);
  }
}

bool is_hold_active() { return holdActive; }
