// ============================================================================
// Component Tester PRO v3.0 — Logger (CYD Edition)
// ============================================================================
// Registra medições no arquivo LOG.TXT do SD Card.
// O SD Card é inicializado no barramento HSPI pelo database.cpp.
// ============================================================================

#include "logger.h"
#include "config.h"
#include "globals.h"
#include "utils.h"
#include <Arduino.h>
#include <SD.h>

// ============================================================================
// INICIALIZAÇÃO DO LOG
// ============================================================================
void log_init() {
  // O SD.begin() já é chamado por db_init_sd() no main.cpp
  // Aqui apenas verificamos/criamos o arquivo de log

  File logFile = SD.open(LOG_FILE_PATH, FILE_WRITE);
  if (logFile) {
    if (logFile.size() == 0) {
      // Escreve cabeçalho se arquivo estiver vazio
      logFile.println(F("CT PRO v3.0 CYD Edition — Log de Medicoes"));
      logFile.println(F("ms,Tipo,Valor,Temperatura,Julgamento"));
    }
    logFile.close();
    LOG_SERIAL_F("Logger: OK");
  } else {
    LOG_SERIAL_F("Logger: ERRO ao criar LOG.TXT");
  }
}

// ============================================================================
// REGISTRAR MEDIÇÃO
// ============================================================================
void log_measurement(const char *type, float value, float temp,
                     const char *judgment) {
  File logFile = SD.open(LOG_FILE_PATH, FILE_APPEND); // FILE_APPEND no ESP32
  if (logFile) {
    logFile.print(millis());
    logFile.print(',');
    logFile.print(type);
    logFile.print(',');
    fprint(logFile, value, 2);
    logFile.print(',');
    fprint(logFile, temp, 1);
    logFile.print(',');
    logFile.println(judgment);
    logFile.close();
  } else {
    LOG_SERIAL_F("Logger: ERRO ao escrever no SD");
  }
}
