// ============================================================================
// Component Tester PRO v3.0 — Variáveis Globais e Estados (CYD Edition)
// ============================================================================
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "config.h"

// ============================================================================
// ESTADOS DA APLICAÇÃO
// ============================================================================
extern enum AppState {
  STATE_SPLASH,           // Tela de splash (boot)
  STATE_MENU,             // Menu principal (grid 2x3)
  STATE_MEASURING,        // Sub-menu de medições
  STATE_THERMAL_PROBE,    // Sonda térmica DS18B20
  STATE_SETTINGS,         // Menu de configurações
  STATE_ABOUT,            // Tela "Sobre"
  STATE_HISTORY,          // Histórico de medições
  STATE_SCANNER,          // Auto-detect de componentes
  STATE_STATS,            // Estatísticas de uso
  STATE_MULTIMETER,       // Multímetro AC/DC (NOVO)
  STATE_TOUCH_CALIBRATE   // Calibração do Touchscreen (NOVO)
} currentAppState;

// ============================================================================
// TIMERS GLOBAIS
// ============================================================================
extern unsigned long previousMillis;
extern unsigned long currentMillis;

// ============================================================================
// CONTROLE DE LEDS
// ============================================================================
extern bool flashingBothLeds;
extern bool flashingGreenLedSlow;
extern bool flashingRedLedFast;
extern unsigned long ledFlashTimer;
extern unsigned long ledFlashInterval;

extern bool greenLedState;
extern bool redLedState;

// ============================================================================
// CONTROLE DO BUZZER
// ============================================================================
extern unsigned long buzzerStartTime;
extern unsigned long buzzerDuration;
extern bool buzzerActive;

// ============================================================================
// SENSOR TÉRMICO
// ============================================================================
extern float currentTemperature;

// ============================================================================
// NAVEGAÇÃO
// ============================================================================
extern int currentMenuItem;
extern int totalMenuItems;

// ============================================================================
// HISTÓRICO DE MEDIÇÕES (expandido para ESP32 com mais RAM)
// ============================================================================
struct MeasurementHistory {
  char name[MAX_MEASUREMENT_NAME];  // Nome da medição (ex: "Cap", "Res", "VacRMS")
  float value;                       // Valor medido
  float temp;                        // Temperatura no momento da medição
  bool isGood;                       // Julgamento: Bom (true) / Ruim (false)
};

extern MeasurementHistory measurementHistory[HISTORY_SIZE];
extern int historyIndex;
extern int historyCount;

// ============================================================================
// CONFIGURAÇÕES DO DISPOSITIVO (persistidas via NVS/Preferences)
// ============================================================================
struct Settings {
  float offset1;                  // Offset de calibração do probe 1
  float offset2;                  // Offset de calibração do probe 2
  bool darkMode;                  // Modo escuro (toggle)
  bool silentMode;                // Modo silencioso (sem buzzer)
  unsigned long timeoutDuration;  // Timeout de inatividade (ms)
  unsigned long totalMeasurements; // Contador total de medições
  unsigned long faultyMeasurements; // Contador de medições com falha
  uint8_t backlight;              // Nível de backlight (0-255)
  float zmptScaleFactor;          // Fator de calibração do ZMPT101B
  uint16_t touchCalibration[5];   // Dados de calibração do touchscreen
};
extern Settings deviceSettings;

// ============================================================================
// FUNÇÕES GLOBAIS
// ============================================================================
void loadSettings();    // Carrega settings do NVS (Preferences)
void saveSettings();    // Salva settings no NVS (Preferences)
void addToHistory(const char* name, float value, float temp, bool isGood);

#endif // GLOBALS_H