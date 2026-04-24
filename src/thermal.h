// ============================================================================
// Component Tester PRO v3.0 — Sonda Térmica DS18B20 (CYD Edition)
// ============================================================================
#ifndef THERMAL_H
#define THERMAL_H

#include "config.h"
#include <Arduino.h>
#include <OneWire.h>

// Objeto OneWire (GPIO 4 na CYD)
extern OneWire oneWireBus;

// Funções para manipulação da sonda térmica
void thermal_init();
void thermal_handle();
float read_temperature();
void check_temperature_alerts(float temp);

#endif // THERMAL_H