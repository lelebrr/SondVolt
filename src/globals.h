#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define SERIAL_DEBUG 0
#if SERIAL_DEBUG
#define LOG_SERIAL(x) Serial.println(F(x))
#define LOG_BEGIN(x) Serial.begin(x)
#else
#define LOG_SERIAL(x)
#define LOG_BEGIN(x)
#endif

#define HISTORY_SIZE 10
#define MAX_MEASUREMENT_NAME 8

extern enum AppState {
  STATE_SPLASH,
  STATE_MENU,
  STATE_MEASURING,
  STATE_THERMAL_PROBE,
  STATE_SETTINGS,
  STATE_ABOUT,
  STATE_HISTORY,
  STATE_SCANNER,
  STATE_STATS
} currentAppState;

extern unsigned long previousMillis;
extern unsigned long currentMillis;

extern bool flashingBothLeds;
extern bool flashingGreenLedSlow;
extern bool flashingRedLedFast;
extern unsigned long ledFlashTimer;
extern unsigned long ledFlashInterval;

extern bool greenLedState;
extern bool redLedState;

extern unsigned long buzzerStartTime;
extern unsigned long buzzerDuration;
extern bool buzzerActive;

extern float currentTemperature;

extern int currentMenuItem;
extern int totalMenuItems;

struct MeasurementHistory {
  char name[MAX_MEASUREMENT_NAME];
  float value;
  float temp;
  bool isGood;
};

extern MeasurementHistory measurementHistory[HISTORY_SIZE];
extern int historyIndex;
extern int historyCount;

struct Settings {
  float offset1;
  float offset2;
  bool darkMode;
  bool silentMode;
  unsigned long timeoutDuration;
  unsigned long totalMeasurements;
  unsigned long faultyMeasurements;
};
extern Settings deviceSettings;

void loadSettings();
void saveSettings();
void addToHistory(const char* name, float value, float temp, bool isGood);

#endif