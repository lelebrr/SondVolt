// ============================================================================
// Component Tester PRO v3.0 — Sonda Térmica DS18B20 (CYD Edition)
// ============================================================================
// Sensor DS18B20 conectado ao GPIO 4 via protocolo OneWire.
// A lógica é idêntica ao Arduino Uno — o OneWire é totalmente compatível.
// ============================================================================

#include "thermal.h"
#include "buzzer.h"
#include "config.h"
#include "globals.h"
#include "leds.h"
#include "utils.h"
#include "drawings.h"
#include "buttons.h"
#include "menu.h"
#include <TFT_eSPI.h>
#include <OneWire.h>

extern TFT_eSPI tft;

// Objeto OneWire no pino da CYD
OneWire oneWireBus(PIN_ONEWIRE);

// Variáveis para controle da leitura
unsigned long lastTempReadMillis = 0;
const long tempReadInterval = 500; // Leitura a cada 500ms

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================
void thermal_init() {
  pinMode(PIN_ONEWIRE, INPUT_PULLUP);
  lastTempReadMillis = currentMillis;
  currentTemperature = 25.0f;
  LOG_SERIAL_F("Sonda termica: GPIO 4 (DS18B20)");
}

// ============================================================================
// HANDLER (chamado no loop quando STATE_THERMAL_PROBE)
// ============================================================================
void thermal_handle() {
  static bool firstRun = true;
  if (firstRun) {
    tft.fillScreen(UI_COLOR_BG);
    draw_status_bar();
    draw_modern_card("Sonda Termica", UI_COLOR_ACCENT);
    firstRun = false;
  }

  if (currentMillis - lastTempReadMillis >= tempReadInterval) {
    lastTempReadMillis = currentMillis;
    currentTemperature = read_temperature();
    check_temperature_alerts(currentTemperature);

    // Exibe a temperatura com estilo moderno
    tft.fillRect(20, 80, 260, 100, UI_COLOR_BG);
    tft.setCursor(30, 100);
    tft.setTextSize(4);

    // Cor baseada na faixa de temperatura
    if (currentTemperature < TEMP_NORMAL_THRESHOLD) {
      tft.setTextColor(UI_COLOR_GREEN);
    } else if (currentTemperature < TEMP_HOT_THRESHOLD) {
      tft.setTextColor(UI_COLOR_ORANGE);
    } else {
      tft.setTextColor(UI_COLOR_RED);
    }

    fprint(tft, currentTemperature, 1);
    tft.setTextSize(2);
    tft.print(F(" C"));

    // Status textual
    tft.setTextSize(1);
    tft.setCursor(30, 140);
    if (currentTemperature < TEMP_NORMAL_THRESHOLD) {
      tft.setTextColor(UI_COLOR_GREEN);
      tft.println(F("Status: OPERACAO NORMAL"));
    } else if (currentTemperature < TEMP_HOT_THRESHOLD) {
      tft.setTextColor(UI_COLOR_ORANGE);
      tft.println(F("Status: AQUECIMENTO"));
    } else {
      tft.setTextColor(UI_COLOR_RED);
      tft.println(F("Status: PERIGO TERMICO!"));
    }

    // Barra visual de temperatura (0-150°C)
    int barMaxW = 260;
    int barW = (int)(currentTemperature * barMaxW / 150.0f);
    if (barW > barMaxW) barW = barMaxW;
    if (barW < 0) barW = 0;

    tft.drawRect(28, 165, barMaxW + 4, 14, UI_COLOR_GREY);
    uint16_t barColor = (currentTemperature < TEMP_NORMAL_THRESHOLD) ? UI_COLOR_GREEN :
                         (currentTemperature < TEMP_HOT_THRESHOLD) ? UI_COLOR_ORANGE : UI_COLOR_RED;
    tft.fillRect(30, 167, barW, 10, barColor);

    // Labels da barra
    tft.setCursor(30, 182);
    tft.setTextColor(UI_COLOR_GREY);
    tft.setTextSize(1);
    tft.print(F("0"));
    tft.setCursor(135, 182);
    tft.print(F("70"));
    tft.setCursor(210, 182);
    tft.print(F("110"));
    tft.setCursor(270, 182);
    tft.print(F("150C"));

    // Sensor info
    tft.setCursor(30, 200);
    tft.setTextColor(UI_COLOR_GREY);
    tft.print(F("Sensor: DS18B20 | GPIO "));
    tft.print(PIN_ONEWIRE);
  }

  buttons_update();
  if (isBackPressed()) {
    firstRun = true;
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// ============================================================================
// LEITURA DO DS18B20 (protocolo OneWire direto)
// ============================================================================
float read_temperature() {
  static float lastTemp = 25.0f;
  byte data[2];

  if (!oneWireBus.reset()) return lastTemp;
  oneWireBus.write(0xCC); // Skip ROM (apenas 1 sensor)
  oneWireBus.write(0x44); // Iniciar conversão

  // Delay para conversão (750ms para 12-bit, mas usamos delay menor para UI fluida)
  delay(100);

  oneWireBus.reset();
  oneWireBus.write(0xCC);
  oneWireBus.write(0xBE); // Ler scratchpad
  data[0] = oneWireBus.read();
  data[1] = oneWireBus.read();

  float result = (float)((data[1] << 8) | data[0]) / 16.0f;

  // Validação de range (-50°C a +150°C)
  if (result > -50.0f && result < 150.0f) {
    lastTemp = result;
    return result;
  }
  return lastTemp;
}

// ============================================================================
// ALERTAS DE TEMPERATURA
// ============================================================================
void check_temperature_alerts(float temp) {
  static unsigned long lastAlertMillis = 0;
  static unsigned long beepInterval = 2000;

  if (temp >= TEMP_DANGER_THRESHOLD) {
    set_red_led(true);
    beepInterval = 200;
    if (currentMillis - lastAlertMillis >= beepInterval) {
      lastAlertMillis = currentMillis;
      play_beep(150);
    }
  } else if (temp >= TEMP_HOT_THRESHOLD) {
    flash_red_led_fast();
    beepInterval = 500;
    if (currentMillis - lastAlertMillis >= beepInterval) {
      lastAlertMillis = currentMillis;
      play_beep(100);
    }
  } else if (temp >= TEMP_NORMAL_THRESHOLD) {
    set_red_led(false);
    beepInterval = 1000;
    if (currentMillis - lastAlertMillis >= beepInterval) {
      lastAlertMillis = currentMillis;
      play_beep(50);
    }
  } else {
    set_red_led(false);
    stop_beep();
  }
}