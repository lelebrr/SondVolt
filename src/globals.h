#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

// Variáveis globais para estado da máquina
extern enum AppState { STATE_SPLASH, STATE_MENU, STATE_MEASURING, STATE_THERMAL_PROBE, STATE_SETTINGS, STATE_ABOUT } currentAppState;

// Variáveis globais para controle de tempo
extern unsigned long previousMillis;
extern unsigned long currentMillis;

// Variáveis globais para status dos LEDs
extern bool flashingBothLeds;
extern bool flashingGreenLedSlow;
extern bool flashingRedLedFast;
extern unsigned long ledFlashTimer;
extern int ledFlashInterval;

extern bool greenLedState;
extern bool redLedState;

// Variáveis globais para controle do buzzer
extern unsigned long buzzerStartTime;
extern int buzzerDuration;
extern bool buzzerActive;

// Variáveis globais para a sonda térmica
extern float currentTemperature;

// Variáveis globais para o menu
extern int currentMenuItem;
extern int totalMenuItems;

#endif // GLOBALS_H
