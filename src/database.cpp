// ============================================================================
// Component Tester PRO v3.0 — Database de Componentes (CYD Edition)
// ============================================================================
// Migrado para ESP32 com as seguintes otimizações:
//   1. SD Card inicializado no barramento HSPI (separado do TFT no VSPI)
//   2. Índice de categorias em RAM para busca O(1) por categoria
//   3. Buffer de leitura otimizado de 512 bytes
//   4. Seek direto para a posição da categoria no arquivo CSV
// ============================================================================

#include "database.h"
#include "config.h"
#include "globals.h"
#include <SD.h>
#include <SPI.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================
CategoryIndex categoryIndex[CAT_COUNT];
bool dbIndexLoaded = false;

// Instância SPI para o SD Card (HSPI — separado do VSPI usado pelo TFT)
SPIClass sdSPI(HSPI);

// ============================================================================
// PARSER DE LINHA CSV
// ============================================================================
// Formato esperado: nome,categoria,value1,min1,max1,value2,pin1,pin2,pin3,desc,uso,esr
static bool parseCSVLine(const char *line, ComponentDB &comp) {
  // Buffer local para tokenização (strtok modifica a string)
  char buffer[256];
  strncpy(buffer, line, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  // 1. Nome do componente
  char *token = strtok(buffer, ",");
  if (!token) return false;
  strncpy(comp.name, token, sizeof(comp.name) - 1);
  comp.name[sizeof(comp.name) - 1] = '\0';

  // 2. Categoria
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.category = atoi(token);

  // 3. Valor primário
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.value1 = atoi(token);

  // 4. Valor mínimo
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.min1 = atoi(token);

  // 5. Valor máximo
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.max1 = atoi(token);

  // 6. Valor secundário
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.value2 = atoi(token);

  // 7-9. Pinagem (3 bytes)
  for (int i = 0; i < 3; i++) {
    token = strtok(NULL, ",");
    if (!token) return false;
    comp.pinout[i] = atoi(token);
  }

  // 10. Descrição
  token = strtok(NULL, ",");
  if (!token) return false;
  strncpy(comp.description, token, sizeof(comp.description) - 1);
  comp.description[sizeof(comp.description) - 1] = '\0';

  // 11. Uso comum
  token = strtok(NULL, ",");
  if (!token) return false;
  strncpy(comp.common_use, token, sizeof(comp.common_use) - 1);
  comp.common_use[sizeof(comp.common_use) - 1] = '\0';

  // 12. ESR típico
  token = strtok(NULL, ",");
  if (!token) return false;
  comp.typical_esr = atoi(token);

  return true;
}

// ============================================================================
// INICIALIZAÇÃO DO SD CARD (HSPI)
// ============================================================================
// A CYD usa pinos HSPI dedicados para o SD Card:
//   SCK=18, MOSI=23, MISO=19, CS=5
// Isso é separado do barramento VSPI usado pelo TFT_eSPI.
bool db_init_sd() {
  // Inicializa barramento HSPI para o SD Card
  sdSPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

  // Inicializa o cartão SD com frequência de 20MHz para performance
  if (!SD.begin(PIN_SD_CS, sdSPI, 20000000)) {
    LOG_SERIAL_F("ERRO: SD Card nao encontrado");
    return false;
  }

  // Verificar tipo de cartão
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    LOG_SERIAL_F("SD Card: Nenhum cartao detectado!");
    return false;
  }

  // Log de informações do cartão
  Serial.print(F("SD Card tipo: "));
  switch (cardType) {
    case CARD_MMC:  Serial.println(F("MMC")); break;
    case CARD_SD:   Serial.println(F("SDSC")); break;
    case CARD_SDHC: Serial.println(F("SDHC")); break;
    default:        Serial.println(F("Desconhecido")); break;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.print(F("SD Card tamanho: "));
  Serial.print((uint32_t)cardSize);
  Serial.println(F(" MB"));

  LOG_SERIAL_F("SD Card: OK");
  return true;
}

// ============================================================================
// CONSTRUÇÃO DO ÍNDICE EM RAM
// ============================================================================
// Percorre o CSV uma vez e registra o offset de início de cada categoria.
// Com 520KB de RAM, podemos armazenar este índice facilmente.
bool db_load_index() {
  // Resetar índice
  for (int i = 0; i < CAT_COUNT; i++) {
    categoryIndex[i].fileOffset = 0;
    categoryIndex[i].count = 0;
  }

  File dbFile = SD.open(DB_FILE_PATH, FILE_READ);
  if (!dbFile) {
    LOG_SERIAL_F("DB: Arquivo COMPBD.CSV nao encontrado!");
    dbIndexLoaded = false;
    return false;
  }

  char line_buffer[256];
  size_t line_idx = 0;
  uint32_t lineStart = 0;    // Posição do início da linha atual
  uint32_t currentPos = 0;   // Posição atual no arquivo
  int totalEntries = 0;

  while (dbFile.available()) {
    char c = dbFile.read();
    currentPos++;

    if (c == '\n' || c == '\r') {
      if (line_idx > 0) {
        line_buffer[line_idx] = '\0';

        // Extrair categoria rapidamente (segundo campo após a primeira vírgula)
        char *firstComma = strchr(line_buffer, ',');
        if (firstComma) {
          int cat = atoi(firstComma + 1);
          if (cat >= 0 && cat < CAT_COUNT) {
            // Se é a primeira entrada desta categoria, registrar offset
            if (categoryIndex[cat].count == 0) {
              categoryIndex[cat].fileOffset = lineStart;
            }
            categoryIndex[cat].count++;
            totalEntries++;
          }
        }

        line_idx = 0;
      }
      lineStart = currentPos; // Próxima linha começa aqui
    } else {
      if (line_idx < sizeof(line_buffer) - 1) {
        line_buffer[line_idx++] = c;
      }
    }
  }

  // Processar última linha (se não terminar com \n)
  if (line_idx > 0) {
    line_buffer[line_idx] = '\0';
    char *firstComma = strchr(line_buffer, ',');
    if (firstComma) {
      int cat = atoi(firstComma + 1);
      if (cat >= 0 && cat < CAT_COUNT) {
        if (categoryIndex[cat].count == 0) {
          categoryIndex[cat].fileOffset = lineStart;
        }
        categoryIndex[cat].count++;
        totalEntries++;
      }
    }
  }

  dbFile.close();

  // Log do resultado
  Serial.print(F("DB: Indice construido - "));
  Serial.print(totalEntries);
  Serial.println(F(" componentes"));

  for (int i = 0; i < CAT_COUNT; i++) {
    if (categoryIndex[i].count > 0) {
      Serial.print(F("  Cat ")); Serial.print(i);
      Serial.print(F(": ")); Serial.print(categoryIndex[i].count);
      Serial.print(F(" entradas @ offset ")); Serial.println(categoryIndex[i].fileOffset);
    }
  }

  dbIndexLoaded = true;
  return true;
}

// ============================================================================
// BUSCA DO MELHOR MATCH
// ============================================================================
// Otimizado para ESP32: usa o índice em RAM para pular direto para a
// categoria relevante. Ainda percorre todas as entradas da categoria,
// mas não precisa ler o arquivo inteiro.
ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1,
                          uint16_t measured_value2, uint16_t measured_esr) {
  ComponentDB bestMatch = {"", 0, 0, 0, 0, 0, {0xFF, 0xFF, 0xFF}, "", "", 0};
  uint16_t smallest_diff = 0xFFFF;

  File dbFile = SD.open(DB_FILE_PATH, FILE_READ);
  if (!dbFile) {
    LOG_SERIAL_F("DB Err: Nao foi possivel abrir COMPBD.CSV");
    strncpy(bestMatch.name, "SD ERROR", sizeof(bestMatch.name) - 1);
    return bestMatch;
  }

  // Se o índice está carregado, fazer seek para a posição da categoria
  if (dbIndexLoaded && category < CAT_COUNT && categoryIndex[category].count > 0) {
    dbFile.seek(categoryIndex[category].fileOffset);
    LOG_SERIAL("DB: Seek direto para categoria");
  }

  char line_buffer[256];
  size_t line_idx = 0;
  int matchesChecked = 0;
  int maxToCheck = dbIndexLoaded ? categoryIndex[category].count + 50 : 0xFFFF; // margem de segurança

  while (dbFile.available() && matchesChecked < maxToCheck) {
    char c = dbFile.read();

    if (c == '\n' || c == '\r') {
      if (line_idx > 0) {
        line_buffer[line_idx] = '\0';
        ComponentDB currentComp;

        if (parseCSVLine(line_buffer, currentComp)) {
          if (currentComp.category == category) {
            matchesChecked++;

            // Verificar se o valor medido está dentro da faixa
            bool val1_match = (measured_value1 >= currentComp.min1 &&
                               measured_value1 <= currentComp.max1);

            // Verificar valor secundário (margem de 20%)
            bool val2_match = true;
            if (currentComp.value2 > 0 && measured_value2 > 0) {
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
          } else if (dbIndexLoaded && currentComp.category != category) {
            // Se estamos usando o índice e já passamos da categoria, parar
            if (matchesChecked > 0) break;
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

// ============================================================================
// INFORMAÇÕES DE DEBUG
// ============================================================================
void printComponentInfo(const ComponentDB &comp, uint16_t measured, uint16_t esr) {
  Serial.print(F("Name: "));       Serial.println(comp.name);
  Serial.print(F("Category: "));   Serial.println(getCategoryName(comp.category));
  Serial.print(F("Value: "));      Serial.println(measured);
  Serial.print(F("Expected: "));   Serial.println(comp.value1);
  Serial.print(F("Range: "));      Serial.print(comp.min1);
  Serial.print(F("-"));            Serial.println(comp.max1);
  Serial.print(F("ESR: "));        Serial.println(esr);
  Serial.print(F("Typical ESR: ")); Serial.println(comp.typical_esr);
  Serial.print(F("Description: ")); Serial.println(comp.description);
  Serial.print(F("Use: "));        Serial.println(comp.common_use);
}

// ============================================================================
// NOME DAS CATEGORIAS
// ============================================================================
const char* getCategoryName(uint8_t category) {
  switch (category) {
    case CAT_BJT_NPN:       return "BJT NPN";
    case CAT_BJT_PNP:       return "BJT PNP";
    case CAT_MOSFET_N:      return "MOSFET N";
    case CAT_MOSFET_P:      return "MOSFET P";
    case CAT_DIODE:         return "Diode";
    case CAT_ZENER:         return "Zener";
    case CAT_SCHOTTKY:      return "Schottky";
    case CAT_LED:           return "LED";
    case CAT_CAPACITOR:     return "Capacitor";
    case CAT_RESISTOR:      return "Resistor";
    case CAT_INDUCTOR:      return "Inductor";
    case CAT_OPTOCOUPLER:   return "Optocoupler";
    case CAT_CRYSTAL:       return "Crystal";
    case CAT_POTENTIOMETER: return "Potentiometer";
    case CAT_FUSE:          return "Fuse";
    case CAT_VARISTOR:      return "Varistor";
    case CAT_NTC:           return "NTC";
    case CAT_TRIAC:         return "TRIAC";
    case CAT_SCR:           return "SCR";
    case CAT_REGULATOR:     return "Regulator";
    case CAT_RELAY:         return "Relay";
    case CAT_SENSOR:        return "Sensor";
    case CAT_OTHER:         return "Other";
    default:                return "Unknown";
  }
}
