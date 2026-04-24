// ============================================================================
// Sondvolt v3.0 — Logger (Sistema de Logging)
// Descrição: Logging de medições no MicroSD (arquivo CSV)
// ============================================================================
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

// ============================================================================
// NÍVEIS DE LOG
// ============================================================================
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
};

// ============================================================================
// PROTÓTIPOS
// ============================================================================

// Inicialização (abre/cria arquivo de log)
bool logger_init();

// Escreve entrada de log
bool logger_write(const char* component, float value,
               const char* unit, const char* status);

// Escreve timestamp + dados
bool logger_write_ex(unsigned long timestamp, const char* component,
                   float value, const char* unit, uint8_t status);

// Log com nível
void logger_log(LogLevel level, const char* message);

// Rotina de log (chamada periodicamente no loop)
void logger_update();

// Fecha arquivo
void logger_close();

// ============================================================================
// FORMATO DO CSV
// ============================================================================
// Timestamp;Component;Value;Unit;Status;Temperature;TotalMeasurements
// 1745000000;Resistor;1000.5;Ω;BOM;25.0;15

#endif // LOGGER_H