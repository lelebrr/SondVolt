#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "database.h"
#include "drawings.h"
#include "globals.h"
#include "leds.h"
#include "logger.h"
#include "menu.h"
#include "thermal.h"
#include "utils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <SPI.h>

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

enum AppState currentAppState = STATE_SPLASH;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
bool greenLedState = false;
bool redLedState = false;
float currentTemperature = 0.0;
int currentMenuItem = 0;
int totalMenuItems = 0;

MeasurementHistory measurementHistory[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;

Settings deviceSettings = {0.0, 0.0, false, false, 30000, 0, 0};

#define TFT_ROTATION 1

#include "measurements.h"

void loadSettings() {
  if (EEPROM.read(0) == 0xAB) {
    EEPROM.get(1, deviceSettings.offset1);
    EEPROM.get(5, deviceSettings.offset2);
    deviceSettings.darkMode = EEPROM.read(9);
    deviceSettings.silentMode = EEPROM.read(10);
    unsigned long storedTimeout;
    EEPROM.get(11, storedTimeout);
    if (storedTimeout > 0) deviceSettings.timeoutDuration = storedTimeout;
    EEPROM.get(15, deviceSettings.totalMeasurements);
    EEPROM.get(19, deviceSettings.faultyMeasurements);
  } else {
    deviceSettings.offset1 = 0.0;
    deviceSettings.offset2 = 0.0;
    deviceSettings.darkMode = false;
    deviceSettings.silentMode = false;
    deviceSettings.timeoutDuration = 30000;
    deviceSettings.totalMeasurements = 0;
    deviceSettings.faultyMeasurements = 0;
  }
}

void saveSettings() {
  EEPROM.write(0, 0xAB);
  EEPROM.put(1, deviceSettings.offset1);
  EEPROM.put(5, deviceSettings.offset2);
  EEPROM.write(9, deviceSettings.darkMode ? 1 : 0);
  EEPROM.write(10, deviceSettings.silentMode ? 1 : 0);
  EEPROM.put(11, deviceSettings.timeoutDuration);
  EEPROM.put(15, deviceSettings.totalMeasurements);
  EEPROM.put(19, deviceSettings.faultyMeasurements);
}

void addToHistory(const char* name, float value, float temp, bool isGood) {
  strncpy(measurementHistory[historyIndex].name, name, MAX_MEASUREMENT_NAME - 1);
  measurementHistory[historyIndex].name[MAX_MEASUREMENT_NAME - 1] = '\0';
  measurementHistory[historyIndex].value = value;
  measurementHistory[historyIndex].temp = temp;
  measurementHistory[historyIndex].isGood = isGood;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  if (historyCount < HISTORY_SIZE) historyCount++;
  
  deviceSettings.totalMeasurements++;
  if (!isGood) deviceSettings.faultyMeasurements++;
  saveSettings();
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("CT PRO v2.1 Starting..."));

  tft.begin();
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(UI_COLOR_BG);

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  buttons_init();

  // Splash Screen v2.1 Animada
  tft.fillScreen(UI_COLOR_BG);
  
  // Desenha logo CT PRO com brilho
  for (int i = 0; i < 5; i++) {
    tft.drawRoundRect(80 - i, 60 - i, 160 + i*2, 60 + i*2, 10, 0x0005 + i*0x0100);
    delay(50);
  }
  
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setTextSize(4);
  tft.setCursor(95, 75);
  tft.print(F("CT PRO"));
  
  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(105, 115);
  tft.print(F("Elite Firmware v2.1"));
  
  // Barra de Progresso Animada
  int barW = 200;
  int barH = 10;
  int barX = (tft.width() - barW) / 2;
  int barY = 180;
  
  tft.drawRoundRect(barX - 2, barY - 2, barW + 4, barH + 4, 4, UI_COLOR_HILIGHT);
  
  for (int i = 0; i <= 100; i += 2) {
    int currentW = (barW * i) / 100;
    tft.fillRoundRect(barX, barY, currentW, barH, 2, UI_COLOR_ACCENT);
    
    tft.fillRect(barX + barW/2 - 20, barY + 15, 60, 10, UI_COLOR_BG);
    tft.setCursor(barX + barW/2 - 10, barY + 15);
    tft.setTextColor(UI_COLOR_TEXT);
    tft.print(i);
    tft.print(F("%"));
    
    if (i == 20) {
      if (!SD.begin(SD_CS_PIN)) Serial.println(F("SD Err!"));
    }
    if (i == 50) loadSettings();
    if (i == 80) thermal_init();
    
    delay(20);
  }
  
  play_beep(200);
  delay(500);
  
  currentAppState = STATE_MENU;
  menu_init();
}

void loop() {
  currentMillis = millis();
  buttons_update();
  buzzer_update();

  switch (currentAppState) {
    case STATE_SPLASH:
      break;
    case STATE_MENU:
      menu_handle();
      break;
    case STATE_MEASURING:
      measurements_handle();
      break;
    case STATE_THERMAL_PROBE:
      thermal_handle();
      break;
    case STATE_SETTINGS:
      handle_settings_menu();
      break;
    case STATE_ABOUT:
      // About handle is inside menu.cpp in this version
      break;
    case STATE_HISTORY:
      handle_history();
      break;
    case STATE_SCANNER:
      handle_scanner();
      break;
    case STATE_STATS:
      handle_stats();
      break;
  }

  update_leds();
}