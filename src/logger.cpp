// ============================================================================
// Sondvolt v3.0 — Logger (Implementação)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================

#include "logger.h"
#include "config.h"
#include "display_globals.h"
#include <SdFat.h>
#include "globals.h"

extern SdFat sd;
static FsFile logFile;
static bool logInitialized = false;

bool logger_init() {
    // Desativa CS do TFT antes de acessar o SD (mesmo barramento SPI)
    digitalWrite(PIN_TFT_CS, HIGH);
    
    // Usa SdSpiConfig para especificar o barramento SPI compartilhado
    SdSpiConfig spiConfig(PIN_SD_CS, SHARED_SPI, SD_SCK_MHZ(20), &spiTFT_SD);
    if (!sd.begin(spiConfig)) {
        LOG_SERIAL_F("[LOG] SD nao disponivel");
        sdCardPresent = false;
        return false;
    }
    logInitialized = true;
    sdCardPresent = true;
    LOG_SERIAL_F("[LOG] SD inicializado com sucesso");
    return true;
}

bool logger_write(const char* component, float value, const char* unit, const char* status) {
    if (!logInitialized) return false;

    logFile = sd.open(LOG_FILE_PATH, FILE_WRITE | O_APPEND);
    if (!logFile) return false;

    char buf[128];
    snprintf(buf, sizeof(buf), "%lu;%s;%.4f;%s;%s", millis(), component, value, unit, status);
    logFile.println(buf);
    logFile.close();
    return true;
}

uint8_t logger_get_recent(HistoryItem* buffer, uint8_t maxEntries) {
    if (!logInitialized) return 0;

    logFile = sd.open(LOG_FILE_PATH, FILE_READ);
    if (!logFile) return 0;

    uint8_t count = 0;
    String line;
    while (logFile.available()) {
        line = logFile.readStringUntil('\n');
        if (line.length() < 10) continue;

        int first = line.indexOf(';');
        int second = line.indexOf(';', first + 1);
        int third = line.indexOf(';', second + 1);
        int fourth = line.indexOf(';', third + 1);

        if (fourth == -1) continue;

        uint8_t targetIdx = (count < maxEntries) ? count : (maxEntries - 1);
        if(count >= maxEntries) {
            for(int i=0; i<maxEntries-1; i++) buffer[i] = buffer[i+1];
        }

        strncpy(buffer[targetIdx].componentName, line.substring(first + 1, second).c_str(), 31);
        buffer[targetIdx].value = line.substring(second + 1, third).toFloat();
        strncpy(buffer[targetIdx].unit, line.substring(third + 1, fourth).c_str(), 9);
        buffer[targetIdx].status = 0; 

        count++;
    }
    logFile.close();
    return (count > maxEntries) ? maxEntries : count;
}

void logger_clear() {
    if (!logInitialized) return;
    sd.remove(LOG_FILE_PATH);
}

void logger_log(LogLevel level, const char* message) {
    Serial.printf("[%d] %s\n", level, message);
}

void logger_update() {}
void logger_close() {}