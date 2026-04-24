// ============================================================================
// Sondvolt v3.0 - Variaveis Globais (Implementacao)
// Descricao: Variaveis globais compartilhadas entre modulos
// Versao: CYD Edition para ESP32-2432S028R
// ============================================================================

#include "globals.h"
#include "config.h"
#include "database.h"

// ============================================================================
// ESTADO DA APLICACAO
// ============================================================================
volatile AppState currentAppState = STATE_SPLASH;
volatile AppState previousAppState = STATE_SPLASH;

// ============================================================================
// CONTROLE DE TEMPO
// ============================================================================
unsigned long lastActivityMs = 0;
unsigned long backlightOffTimeMs = 0;
bool backlightOn = true;

// ============================================================================
// MEDICOES ATUAIS
// ============================================================================
float lastResistance = 0.0f;
float lastCapacitance = 0.0f;
float lastInductance = 0.0f;
float lastVoltage = 0.0f;
float lastCurrent = 0.0f;
float lastTemperature = 25.0f;

// ============================================================================
// ESTATISTICAS
// ============================================================================
uint32_t totalMeasurements = 0;
uint32_t goodMeasurements = 0;
uint32_t badMeasurements = 0;

// ============================================================================
// CALIBRACAO DOS PROBES
// ============================================================================
float probeOffsetResistance = 0.0f;
float probeOffsetCapacitance = 0.0f;

// Variaveis de calibração para measurements.cpp
float probeOffsetR = 0.0f;
float probeOffsetC = 0.0f;

// ============================================================================
// SD CARD
// ============================================================================
bool sdCardPresent = false;
bool sdCardError = false;

// ============================================================================
// DISPLAY TFT
// ============================================================================
bool tftInitialized = false;

// ============================================================================
// CONFIGURACOES DO DISPOSITIVO
// ============================================================================
DeviceSettings deviceSettings = {
    .backlight = 200,
    .darkMode = false,
    .silentMode = false,
    .autoSleep = true,
    .autoSleepMs = TIME_BACKLIGHT_OFF,
    .soundEnabled = true,
    .calibrated = false,
    .zmptScaleFactor = ZMPT_CALIBRATION
};