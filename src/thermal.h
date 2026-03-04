#ifndef THERMAL_H
#define THERMAL_H

#include "config.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>


// Declaração do objeto OneWire e DallasTemperature
extern OneWire oneWireBus;
extern DallasTemperature sensors;

// Funções para manipulação da sonda térmica
void thermal_init();
void thermal_handle();
float read_temperature();
void check_temperature_alerts(float temp);

#endif // THERMAL_H