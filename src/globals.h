// ============================================================================
// Sondvolt v3.0 - Variaveis Globais
// Descricao: Variaveis globais compartilhadas entre modulos
// Versao: CYD Edition para ESP32-2432S028R
// ============================================================================
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "config.h"
#include "database.h"

// ============================================================================
// ESTADO DA APLICACAO (maquina de estados)
// ============================================================================
enum AppState {
    STATE_SPLASH           = 0,
    STATE_MENU              = 1,

    // Modo Component Tester
    STATE_MEASURE_RESISTOR  = 10,
    STATE_MEASURE_CAPACITOR = 11,
    STATE_MEASURE_DIODE     = 12,
    STATE_MEASURE_TRANSISTOR = 13,
    STATE_MEASURE_INDUCTOR   = 14,
    STATE_MEASURE_IC         = 15,
    STATE_MEASURE_GENERIC   = 19,

    // Modo Multimestro
    STATE_MULTIMETER        = 20,

    // Ferramentas
    STATE_THERMAL_PROBE     = 30,
    STATE_SCANNER           = 31,
    STATE_CALIBRATION       = 32,

    // Configuracoes e Info
    STATE_SETTINGS         = 40,
    STATE_ABOUT             = 41,
    STATE_HISTORY            = 42,
    STATE_STATS              = 43
};

// Estado atual e anterior
extern volatile AppState currentAppState;
extern volatile AppState previousAppState;

// ============================================================================
// CONTROLE DE TEMPO
// ============================================================================
extern unsigned long lastActivityMs;       // Ultima interacao do usuario
extern unsigned long backlightOffTimeMs;   // Quando desligar o backlight
extern bool backlightOn;

// ============================================================================
// MEDICOES ATUAIS
// ============================================================================
extern float lastResistance;
extern float lastCapacitance;
extern float lastInductance;
extern float lastVoltage;
extern float lastCurrent;
extern float lastTemperature;

// ============================================================================
// ESTATISTICAS
// ============================================================================
extern uint32_t totalMeasurements;
extern uint32_t goodMeasurements;
extern uint32_t badMeasurements;

// ============================================================================
// CALIBRACAO DOS PROBES
// ============================================================================
extern float probeOffsetResistance;
extern float probeOffsetCapacitance;

// ============================================================================
// BANCO DE DADOS NA RAM
// ============================================================================
extern ComponentDatabase componentDB;

// ============================================================================
// SD CARD
// ============================================================================
extern bool sdCardPresent;
extern bool sdCardError;

// ============================================================================
// DISPLAY TFT
// ============================================================================
extern bool tftInitialized;

// ============================================================================
// BOTOES
// ============================================================================
enum Button {
    BTN_NONE     = 0,
    BTN_UP       = 1,
    BTN_DOWN     = 2,
    BTN_LEFT     = 3,
    BTN_RIGHT    = 4,
    BTN_OK       = 5,
    BTN_BACK     = 6
};

// ============================================================================
// MENSAGENS DE DISPLAY (FreeRTOS)
// ============================================================================
enum DisplayMsgType {
    MSG_NONE = 0,
    MSG_TEMPERATURE = 1,
    MSG_MEASUREMENT = 2,
    MSG_ERROR = 3
};

typedef struct {
    DisplayMsgType type;
    float value;
    char text[32];
} DisplayMessage;

// ============================================================================
// RESULTADO DE MEDICAO
// ============================================================================
typedef struct {
    float resistance;
    float capacitance;
    float inductance;
    float voltage;
    float current;
    ComponentType componentType;
    ComponentStatus status;
    unsigned long timestamp;
} MeasurementResult;

// ============================================================================
// ENTRADA DE LOG
// ============================================================================
typedef struct {
    unsigned long timestamp;
    char componentName[20];
    float value;
    char status[10];
} LogEntry;

// ============================================================================
// CONFIGURACOES DO DISPOSITIVO
// ============================================================================
typedef struct {
    uint8_t backlight;           // 0-255
    bool darkMode;
    bool silentMode;
    bool autoSleep;
    unsigned long autoSleepMs;
    bool soundEnabled;
    bool calibrated;
    float zmptScaleFactor;
} DeviceSettings;

extern DeviceSettings deviceSettings;

#endif // GLOBALS_H