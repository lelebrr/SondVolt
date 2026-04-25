// ============================================================================
// Sondvolt v3.0 — Logger (Sistema de Logging)
// Descrição: Logging de medições no MicroSD (arquivo CSV)
// ============================================================================
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "types.h" // Para HistoryItem

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

bool logger_init();
bool logger_write(const char* component, float value, const char* unit, const char* status);
void logger_log(LogLevel level, const char* message);
void logger_close();
void logger_clear();

// Função para recuperar histórico para a UI
uint8_t logger_get_recent(HistoryItem* buffer, uint8_t maxEntries);

#endif // LOGGER_H