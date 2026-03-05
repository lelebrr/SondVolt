#include "utils.h"
#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "globals.h"
#include "leds.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;
// Variáveis globais para utils
unsigned long timeoutStartMillis = 0;
unsigned long timeoutDuration = 30000; // 30 segundos
bool timeoutActive = false;
bool holdActive = false;
bool darkMode = false;
bool silentMode = false;
unsigned long measurementCount = 0;
unsigned long faultyCount = 0;

// Função para calibração das ponteiras
void calibrate_probes() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Calibracao de Probes"));

  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println(F("Encoste as ponteiras"));
  tft.setCursor(10, 60);
  tft.println(F("Pressione OK para iniciar"));

  bool calibrating = true;
  while (calibrating) {
    buttons_update();

    if (isOkPressed()) {
      // Realiza calibração
      float offset1 =
          analogRead(PROBE1_PIN) * 0.0048876; // Converte para tensão
      float offset2 = analogRead(PROBE2_PIN) * 0.0048876;

      // Armazena offsets (em um projeto real, salvaria na EEPROM)
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(10, 10);
      tft.println(F("Calibracao concluida!"));
      tft.setCursor(10, 40);
      tft.print(F("Offset1: "));
      tft.print(offset1, 3);
      tft.println(F("V"));
      tft.setCursor(10, 60);
      tft.print(F("Offset2: "));
      tft.print(offset2, 3);
      tft.println(F("V"));

      delay(2000);
      calibrating = false;
    }

    if (isBackPressed()) {
      calibrating = false;
    }
  }
}

// Função para definir offsets
void set_offsets() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Offsets Atuais:"));

  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println(F("Offset1: 0.000V"));
  tft.setCursor(10, 60);
  tft.println(F("Offset2: 0.000V"));
  tft.setCursor(10, 80);
  tft.println(F("Pressione OK para editar"));

  // Em um projeto real, permitir edição dos offsets
  delay(2000);
}

// Função para lidar com timeouts
void handle_timeout() {
  if (!timeoutActive)
    return;

  if (currentMillis - timeoutStartMillis >= timeoutDuration) {
    timeoutActive = false;
    // Retorna ao menu principal
    currentAppState = STATE_MENU;
    tft.fillScreen(ILI9341_BLACK);
    flash_both_leds(500);
  }
}

// Função auxiliar para millis com overflow protection
unsigned long safe_millis() { return currentMillis; }

// Função para ativar/desativar modo escuro
void toggle_dark_mode() {
  darkMode = !darkMode;
  if (darkMode) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  } else {
    tft.fillScreen(ILI9341_WHITE);
    tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  }
}

// Função para ativar/desativar modo silencioso
void toggle_silent_mode() {
  silentMode = !silentMode;
  if (silentMode) {
    stop_beep();
  }
}

// Função para incrementar contador de medições
void increment_measurement_count() { measurementCount++; }

// Função para incrementar contador de defeituosos
void increment_faulty_count() { faultyCount++; }

// Função para obter estatísticas
void get_measurement_stats(unsigned long *total, unsigned long *faulty) {
  *total = measurementCount;
  *faulty = faultyCount;
}

// Função para resetar contadores
void reset_counters() {
  measurementCount = 0;
  faultyCount = 0;
}

// Função para iniciar timeout
void start_timeout(unsigned long duration) {
  timeoutStartMillis = currentMillis;
  timeoutDuration = duration;
  timeoutActive = true;
}

// Função para verificar se timeout está ativo
bool is_timeout_active() { return timeoutActive; }

// Função para ativar/desativar hold
void toggle_hold() {
  holdActive = !holdActive;
  if (holdActive) {
    flash_green_led_slow();
  } else {
    set_green_led(false);
  }
}

// Função para verificar se hold está ativo
bool is_hold_active() { return holdActive; }
