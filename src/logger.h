#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include "config.h"

// Funções para logging no SD Card
void log_init();
void log_measurement(const char* type, float value, float temp, const char* judgment);

#endif // LOGGER_H
