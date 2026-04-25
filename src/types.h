// ============================================================================
// Sondvolt v3.0 — Tipos Globais
// ============================================================================
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

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
