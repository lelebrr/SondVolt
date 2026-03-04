#include "logger.h"
#include "globals.h"

// Inicializa o logging (já feito no setup do main, mas a função pode ser usada
// para re-inicializar)
void log_init() {
  // O SD.begin(SD_CS_PIN) já é chamado no setup do main.cpp
  // Aqui poderíamos adicionar lógica para verificar a existência do arquivo
  // LOG.TXT ou criar cabeçalhos se necessário.

  // Verifica se o SD Card está disponível
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("Erro: SD Card não inicializado!"));
    return;
  }

  // Cria arquivo LOG.TXT se não existir
  File logFile = SD.open(F("LOG.TXT"), FILE_WRITE);
  if (logFile) {
    if (logFile.size() == 0) {
      // Escreve cabeçalho se arquivo estiver vazio
      logFile.println(F("=== Component Tester PRO v2.0 - Log de Medicoes ==="));
      logFile.println(F("Timestamp(ms),Tipo,Valor,Temp(C),Julgamento"));
      logFile.println(F("==============================================="));
    }
    logFile.close();
  } else {
    Serial.println(F("Erro: Não foi possível criar arquivo LOG.TXT"));
  }
}

// Registra uma medição no arquivo LOG.TXT
void log_measurement(const char *type, float value, float temp,
                     const char *judgment) {
  File logFile = SD.open(F("LOG.TXT"), FILE_WRITE);
  if (logFile) {
    logFile.print(F("Time: "));
    logFile.print(millis());
    logFile.print(F("ms, Type: "));
    logFile.print(type);
    logFile.print(F(", Value: "));
    logFile.print(value, 3);
    logFile.print(F(", Temp: "));
    logFile.print(temp, 1);
    logFile.print(F(", Judgment: "));
    logFile.println(judgment);
    logFile.close();
  } else {
    Serial.println(F("Erro ao abrir LOG.TXT para escrita!"));
  }
}
