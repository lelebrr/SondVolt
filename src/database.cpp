#include "database.h"
#include "globals.h"
#include <SD.h>
#include <stdlib.h>
#include <string.h>

// Arrays removidos. Agora os dados vêm do SD Card.

bool parseCSVLine(const char *line, ComponentDB &comp) {
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

ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1,
                          uint16_t measured_value2, uint16_t measured_esr) {
  ComponentDB bestMatch = {"", 0, 0, 0, 0, 0, {0xFF, 0xFF, 0xFF}, "", "", 0};
  uint16_t smallest_diff = 0xFFFF;

  File dbFile = SD.open("/COMPBD.CSV", FILE_READ);
  if (!dbFile) {
    LOG_SERIAL("DB Err");
    strncpy(bestMatch.name, "SD ERROR", sizeof(bestMatch.name) - 1);
    return bestMatch;
  }

  char line_buffer[150];
  size_t line_idx = 0;

  while (dbFile.available()) {
    char c = dbFile.read();
    if (c == '\n' || c == '\r') {
      if (line_idx > 0) {
        line_buffer[line_idx] = '\0';
        ComponentDB currentComp;
        if (parseCSVLine(line_buffer, currentComp)) {
          if (currentComp.category == category) {
            bool val1_match = (measured_value1 >= currentComp.min1 && measured_value1 <= currentComp.max1);
            
            // Se tivermos um valor secundário (ex: Vf) no DB, ele deve bater também
            bool val2_match = true;
            if (currentComp.value2 > 0 && measured_value2 > 0) {
                // Margem de 20% para o valor secundário
                uint16_t margin = currentComp.value2 / 5;
                if (measured_value2 < (currentComp.value2 - margin) || 
                    measured_value2 > (currentComp.value2 + margin)) {
                    val2_match = false;
                }
            }

            if (val1_match && val2_match) {
              uint16_t diff = abs((int)measured_value1 - (int)currentComp.value1);
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
  Serial.print(F("Name: "));
  Serial.println(comp.name);
  Serial.print(F("Category: "));
  Serial.println(getCategoryName(comp.category));
  Serial.print(F("Value: "));
  Serial.println(measured);
  Serial.print(F("Expected: "));
  Serial.println(comp.value1);
  Serial.print(F("Range: "));
  Serial.print(comp.min1);
  Serial.print(F("-"));
  Serial.println(comp.max1);
  Serial.print(F("ESR: "));
  Serial.println(esr);
  Serial.print(F("Typical ESR: "));
  Serial.println(comp.typical_esr);
  Serial.print(F("Description: "));
  Serial.println(comp.description);
  Serial.print(F("Use: "));
  Serial.println(comp.common_use);
}

const char *getCategoryName(uint8_t category) {
  switch (category) {
    case CAT_BJT_NPN: return "BJT NPN";
    case CAT_BJT_PNP: return "BJT PNP";
    case CAT_MOSFET_N: return "MOSFET N";
    case CAT_MOSFET_P: return "MOSFET P";
    case CAT_DIODE: return "Diode";
    case CAT_ZENER: return "Zener";
    case CAT_SCHOTTKY: return "Schottky";
    case CAT_LED: return "LED";
    case CAT_CAPACITOR: return "Capacitor";
    case CAT_RESISTOR: return "Resistor";
    case CAT_INDUCTOR: return "Inductor";
    case CAT_OPTOCOUPLER: return "Optocoupler";
    case CAT_CRYSTAL: return "Crystal";
    case CAT_REGULATOR: return "Regulator";
    case CAT_RELAY: return "Relay";
    case CAT_SENSOR: return "Sensor";
    default: return "Unknown";
  }
}
