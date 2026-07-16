// ============================================================================
// Sondvolt v3.2 — Sonda Térmica (DS18B20)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: thermal.cpp
// Descrição: Implementação da leitura de temperatura OneWire
// ============================================================================

#include "thermal.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

static OneWire oneWire(PIN_ONEWIRE);
static DallasTemperature sensors(&oneWire);

static float lastTemp = 0;

void thermal_init() {
    sensors.begin();
    sensors.setResolution(12);
}

float thermal_read() {
    sensors.requestTemperatures();
    lastTemp = sensors.getTempCByIndex(0);
    return lastTemp;
}

float thermal_get_last() {
    return lastTemp;
}

bool thermal_is_warning() {
    return (lastTemp > THERMAL_WARNING_TEMP);
}

bool thermal_is_critical() {
    return (lastTemp > THERMAL_CRITICAL_TEMP);
}