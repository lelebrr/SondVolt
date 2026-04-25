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
float lastTemperature = 0.0f;
bool isDischarging = false;
float dischargeProgress = 0.0f;

ComparatorRef referenceComp = {0.0f, COMP_UNKNOWN, "", false};

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
// BANCO DE DADOS NA RAM
// ============================================================================
ComponentDatabase componentDB;

// ============================================================================
// DISPLAY TFT
// ============================================================================
bool tftInitialized = false;

// ============================================================================
// CONFIGURACOES DO DISPOSITIVO
// ============================================================================
DeviceSettings deviceSettings = {
    .backlight = 200,
    .darkMode = true,
    .silentMode = false,
    .autoSleep = true,
    .autoSleepMs = TIME_BACKLIGHT_OFF,
    .soundEnabled = true,
    .calibrated = false,
    .zmptScaleFactor = ZMPT_CALIBRATION,
    .themeColor = COLOR_PRIMARY,
    .unitsMetric = true,
    .themeIdx = 0,
    .showGrid = true,
    .animations = true,
    .autoSaveHistory = true,
    .confirmActions = true,
    .expertMode = false,
    .languageIdx = 0,
    .strongBeep = false
};

LogEntry recentTests[6] = {0};
void update_recent_tests(const char* name, float value, const char* status) {
    for (int i = 5; i > 0; i--) recentTests[i] = recentTests[i-1];
    recentTests[0].timestamp = millis();
    strncpy(recentTests[0].componentName, name, 19);
    recentTests[0].value = value;
    strncpy(recentTests[0].status, status, 9);
}

// ============================================================================
// SISTEMA DE CORES DINAMICAS
// ============================================================================
uint16_t clr_back    = 0x0863;
uint16_t clr_surf    = 0x10C4;
uint16_t clr_text    = 0xFFFF;
uint16_t clr_dim     = 0xAD55;
uint16_t clr_primary = 0x07FF;

void colors_update() {
    if (deviceSettings.darkMode) {
        // MODO NOTURNO (Dark)
        clr_back    = 0x0863;
        clr_surf    = 0x10C4;
        clr_text    = 0xFFFF;
        clr_dim     = 0xAD55;
    } else {
        // MODO DIA (Light)
        clr_back    = 0xFFFF; 
        clr_surf    = 0xE73C; 
        clr_text    = 0x0863; 
        clr_dim     = 0x4208; 
    }
    clr_primary = deviceSettings.themeColor;
}