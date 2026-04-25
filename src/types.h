// ============================================================================
// Sondvolt v3.0 — Tipos Globais
// ============================================================================
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// ============================================================================
// ESTADOS DA APLICACAO
// ============================================================================
enum AppState {
    STATE_SPLASH           = 0,
    STATE_MENU              = 1,
    STATE_MEASURE_RESISTOR  = 10,
    STATE_MEASURE_CAPACITOR = 11,
    STATE_MEASURE_DIODE     = 12,
    STATE_MEASURE_TRANSISTOR = 13,
    STATE_MEASURE_INDUCTOR   = 14,
    STATE_MEASURE_IC         = 15,
    STATE_MEASURE_LED        = 16,
    STATE_MEASURE_GENERIC   = 19,
    STATE_MULTIMETER        = 20,
    STATE_THERMAL_PROBE     = 30,
    STATE_THERMAL_CAMERA    = 31,
    STATE_SCANNER           = 32,
    STATE_CALIBRATION       = 33,
    STATE_SUBMENU_TEMP      = 35,
    STATE_SUBMENU_MAIS      = 36,
    STATE_SETTINGS         = 40,
    STATE_ABOUT             = 41,
    STATE_HISTORY            = 42,
    STATE_STATS              = 43,
    STATE_COMPARATOR         = 45,
    STATE_HELP               = 50
};

// ============================================================================
// MODOS DO MULTIMETRO
// ============================================================================
enum MultimeterMode {
    MMODE_DC_VOLTAGE = 0,      // Tensão DC (0-26V)
    MMODE_AC_VOLTAGE = 1,      // Tensão AC (110V/220V)
    MMODE_DC_CURRENT = 2,      // Corrente DC (0-3.2A)
    MMODE_RESISTANCE = 3,      // Resistência (0-1MΩ)
    MMODE_CONTINUITY = 4,     // Continuidade/curto
    MMODE_POWER = 5           // Potência DC (calculada)
};

enum MeasurementRange {
    RANGE_AUTO = 0,
    RANGE_LOW = 1,
    RANGE_MED = 2,
    RANGE_HIGH = 3
};

enum MultimeterState {
    MSTATE_IDLE = 0,          // Em espera
    MSTATE_MEASURING = 1,     // Medindo
    MSTATE_ERROR = 2,         // Erro detectado
    MSTATE_OVERLOAD = 3,      // Sobrecarga
    MSTATE_SHORT = 4,         // Curto-circuito detectado
    MSTATE_HIGH_VOLTAGE = 5     // Tensão alta detectada
};

// ============================================================================
// TIPOS DE COMPONENTES
// ============================================================================
enum ComponentType {
    COMP_RESISTOR    = 0,
    COMP_CAPACITOR   = 1,
    COMP_CAPACITOR_CERAMIC = 2,
    COMP_CAPACITOR_ELECTRO = 3,
    COMP_DIODE       = 4,
    COMP_LED         = 5,
    COMP_ZENER       = 6,
    COMP_TRANSISTOR  = 7,
    COMP_TRANSISTOR_NPN = 8,
    COMP_TRANSISTOR_PNP = 9,
    COMP_MOSFET      = 10,
    COMP_MOSFET_N    = 11,
    COMP_MOSFET_P    = 12,
    COMP_INDUCTOR    = 13,
    COMP_CRYSTAL     = 14,
    COMP_FUSE       = 15,
    COMP_VARISTOR   = 16,
    COMP_POTENTIOMETER = 17,
    COMP_OPTOCOUPLER = 18,
    COMP_RELAY      = 19,
    COMP_IC         = 20,
    COMP_COIL       = 21,
    COMP_GENERIC    = 22,
    COMP_UNKNOWN    = 99,
    COMP_NONE       = 100
};

// ============================================================================
// STATUS DE MEDICAO
// ============================================================================
enum ComponentStatus {
    STATUS_GOOD     = 0,
    STATUS_SUSPECT  = 1,
    STATUS_WARNING  = 2,
    STATUS_BAD      = 3,
    STATUS_INVALID  = 4,
    STATUS_NONE     = 5,
    STATUS_LEAKY    = 6,
    STATUS_SHORT    = 7,
    STATUS_OPEN     = 8,
    STATUS_UNKNOWN = 9
};

// ============================================================================
// TIPOS DE ICONES
// ============================================================================
enum IconType {
    ICON_RESISTOR,
    ICON_CAPACITOR,
    ICON_DIODE,
    ICON_LED,
    ICON_TRANSISTOR_NPN,
    ICON_TRANSISTOR_PNP,
    ICON_MOSFET_N,
    ICON_MOSFET_P,
    ICON_INDUCTOR,
    ICON_CRYSTAL,
    ICON_FUSE,
    ICON_VARISTOR,
    ICON_POTENTIOMETER,
    ICON_OPTOCOUPLER,
    ICON_MULTIMETER,
    ICON_TEMP,
    ICON_HISTORY,
    ICON_SETTINGS,
    ICON_ABOUT,
    ICON_WARNING,
    ICON_VOLTAGE,
    ICON_CONTINUITY,
    ICON_AUTO,
    ICON_UNKNOWN
};

// ============================================================================
// ESTRUTURAS DE DADOS DE UI
// ============================================================================
struct HistoryItem {
    char componentName[32];
    float value;
    char unit[10];
    uint8_t status;
    unsigned long timestamp;
};

#endif
