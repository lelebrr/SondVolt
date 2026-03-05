#include "database.h"
#include <SD.h>
#include <stdlib.h>
#include <string.h>

// Arrays removidos. Agora os dados vêm do SD Card.

// Função para processar uma linha do CSV e preencher o ComponentDB
bool parseCSVLine(const char *line, ComponentDB &comp) {
  // Exemplo de linha: BC547,1,110,100,120,300,1,2,3,BC547 - Transistor NPN
  // geral,Amplificador chaveamento,0
  char buffer[150];
  strncpy(buffer, line, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, ",");
  if (!token)
    return false;
  strncpy(comp.name, token, sizeof(comp.name) - 1);
  comp.name[sizeof(comp.name) - 1] = '\0';

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.category = atoi(token);

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.value1 = atoi(token);

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.min1 = atoi(token);

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.max1 = atoi(token);

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.value2 = atoi(token);

  for (int i = 0; i < 3; i++) {
    token = strtok(NULL, ",");
    if (!token)
      return false;
    comp.pinout[i] = atoi(token);
  }

  token = strtok(NULL, ",");
  if (!token)
    return false;
  strncpy(comp.description, token, sizeof(comp.description) - 1);
  comp.description[sizeof(comp.description) - 1] = '\0';

  token = strtok(NULL, ",");
  if (!token)
    return false;
  strncpy(comp.common_use, token, sizeof(comp.common_use) - 1);
  comp.common_use[sizeof(comp.common_use) - 1] = '\0';

  token = strtok(NULL, ",");
  if (!token)
    return false;
  comp.typical_esr = atoi(token);

  return true;
}

// Funções de exemplo (implementação completa necessária)
ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1,
                          uint16_t measured_value2, uint16_t measured_esr) {
  ComponentDB bestMatch = {"", 0, 0, 0, 0, 0, {0xFF, 0xFF, 0xFF}, "", "", 0};
  uint16_t smallest_diff = 0xFFFF;

  File dbFile = SD.open("/COMPBD.CSV", FILE_READ);
  if (!dbFile) {
    Serial.println(F("Failed to open COMPBD.CSV"));
    strncpy(bestMatch.name, "SD ERROR", sizeof(bestMatch.name) - 1);
    return bestMatch;
  }

  char line_buffer[150];
  size_t line_idx = 0;

  // Lendo linha por linha
  while (dbFile.available()) {
    char c = dbFile.read();
    if (c == '\n' || c == '\r') {
      if (line_idx > 0) {
        line_buffer[line_idx] = '\0';
        ComponentDB currentComp;
        if (parseCSVLine(line_buffer, currentComp)) {
          // Verifica se a categoria bate (ou se a busca é genelica) e checa se
          // valor bate na tolerância range
          if (currentComp.category == category) {
            if (measured_value1 >= currentComp.min1 &&
                measured_value1 <= currentComp.max1) {
              uint16_t diff =
                  abs((int)measured_value1 - (int)currentComp.value1);
              if (diff < smallest_diff) {
                smallest_diff = diff;
                bestMatch = currentComp;
              }
            }
          }
        }
        line_idx = 0;
      }
    } else {
      if (line_idx < sizeof(line_buffer) - 1) {
        line_buffer[line_idx++] = c;
      }
    }
  }
  dbFile.close();

  return bestMatch;
}

void printComponentInfo(const ComponentDB &comp, uint16_t measured,
                        uint16_t esr) {
  // Implementação necessária
}

const char *getCategoryName(uint8_t category) {
  // Implementação necessária
  return "";
}
