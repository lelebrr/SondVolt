#include "thermal.h"
#include "buzzer.h"
#include "globals.h"
#include "leds.h"
#include "utils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <OneWire.h>


extern Adafruit_ILI9341 tft;

// Objeto OneWire
OneWire oneWireBus(ONEWIRE_BUS_PIN);

// Variáveis para controle da leitura da temperatura
unsigned long lastTempReadMillis = 0;
const long tempReadInterval = 500; // Leitura a cada 500ms

// Inicializa a sonda térmica
void thermal_init() {
  pinMode(ONEWIRE_BUS_PIN, INPUT_PULLUP);
  lastTempReadMillis = currentMillis;
  currentTemperature = 25.0;
}

// Manipula o estado da sonda térmica
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

    // Exibe a temperatura na tela com estilo moderno
    tft.fillRect(20, 80, 200, 100, UI_COLOR_BG);
    tft.setCursor(30, 110);
    tft.setTextSize(4);
    
    if (currentTemperature < TEMP_NORMAL_THRESHOLD) {
      tft.setTextColor(UI_COLOR_GREEN);
    } else if (currentTemperature < TEMP_HOT_THRESHOLD) {
      tft.setTextColor(0xFD20); // Orange
    } else {
      tft.setTextColor(UI_COLOR_RED);
    }
    
    fprint(tft, currentTemperature, 1);
    tft.setTextSize(2);
    tft.println('C');

    tft.setTextSize(1);
    tft.setCursor(30, 140);
    if (currentTemperature < TEMP_NORMAL_THRESHOLD) {
      tft.println(F("Status: OPERACAO NORMAL"));
    } else if (currentTemperature < TEMP_HOT_THRESHOLD) {
      tft.println(F("Status: AQUECIMENTO"));
    } else {
      tft.println(F("Status: PERIGO TERMICO!"));
    }
  }

  buttons_update();
  if (isBackPressed()) {
    firstRun = true;
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// Lê a temperatura do sensor DS18B20 (Simplified)
float read_temperature() {
  static float lastTemp = 25.0;
  byte data[2];
  
  if (!oneWireBus.reset()) return lastTemp;
  oneWireBus.write(0xCC); // Skip ROM
  oneWireBus.write(0x44); // Convert
  
  // Note: For true non-blocking, we should wait 750ms without delay()
  // But since we are in a dedicated state, we can use a small delay or a better state machine.
  // For now, let's keep it simple but improve it later if needed.
  delay(100); // Reduced delay for faster UI, might lose precision on high resolutions
  
  oneWireBus.reset();
  oneWireBus.write(0xCC);
  oneWireBus.write(0xBE); // Read
  data[0] = oneWireBus.read();
  data[1] = oneWireBus.read();
  
  float result = (float)((data[1] << 8) | data[0]) / 16.0;
  if (result > -50 && result < 150) {
    lastTemp = result;
    return result;
  }
  return lastTemp;
}

// Verifica alertas de temperatura e aciona LEDs/buzzer
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