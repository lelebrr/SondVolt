// ============================================================================
// Sondvolt v3.0 - Variaveis Globais
// Descricao: Variaveis globais compartilhadas entre modulos
// Versao: CYD Edition para ESP32-2432S028R
// ============================================================================
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "config.h"

#include "types.h"
#include "database.h"

// Estado atual e anterior
extern volatile AppState currentAppState;
extern volatile AppState previousAppState;


// ============================================================================
// TIPOS DE ICONES — Definidos em types.h
// ============================================================================

// ============================================================================
// TIPOS DE COMPONENTES — Definidos em types.h
// ============================================================================

// ============================================================================
// STATUS DE MEDICAO / COMPONENTE — Definidos em types.h
// ============================================================================

typedef ComponentStatus MeasurementStatus;

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
extern bool isDischarging;
extern float dischargeProgress;            // 0.0 a 1.0

// ============================================================================
// MODO COMPARADOR
// ============================================================================
typedef struct {
    float value;
    ComponentType type;
    char name[16];
    bool isSet;
} ComparatorRef;

extern ComparatorRef referenceComp;

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
// BOTOES — Definidos em buttons.h
// ============================================================================

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
    uint16_t themeColor;         // Cor primária do sistema
    bool unitsMetric;            // true = Metrico, false = Imperial
    uint8_t themeIdx;            // Indice do tema selecionado
    bool showGrid;
    bool animations;
    bool autoSaveHistory;
    bool confirmActions;
    bool expertMode;
    uint8_t languageIdx;         // 0=PT, 1=EN, 2=ES
    bool strongBeep;
} DeviceSettings;

extern DeviceSettings deviceSettings;

// ============================================================================
// COMPONENTES RECENTES
// ============================================================================
extern LogEntry recentTests[6];
void update_recent_tests(const char* name, float value, const char* status);

// ============================================================================
// SISTEMA DE CORES DINAMICAS
// ============================================================================
extern uint16_t clr_back;
extern uint16_t clr_surf;
extern uint16_t clr_text;
extern uint16_t clr_dim;
extern uint16_t clr_primary;

void colors_update();



#endif // GLOBALS_H