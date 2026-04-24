// ============================================================================
// Sondvolt v3.0 — Sistema de SD Card
// Descricao: Implementacao do gerenciamento do cartao SD
// ============================================================================

#include "sdcard.h"
#include "config.h"
#include "globals.h"

#include <SPI.h>

// ============================================================================
// VARIAVEIS
// ============================================================================

static bool mounted = false;
static SDCardInfo cardInfo;
static SPIClass* sdSPI = nullptr;

// ============================================================================
// INICIALIZACAO
// ============================================================================

bool sd_init() {
    if (mounted) {
        return true;
    }

    // Inicializa barramento SPI para SD
    // Sem tratamento de exceções devido a restrições de compilação
    sdSPI = new SPIClass(HSPI);
    sdSPI->begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    
    if (sd_mount()) {
        return true;
    }

    // Fallback: SD card não disponível, mas sistema continua funcionando
    sdCardPresent = false;
    sdCardError = false; // Não é erro, apenas não está disponível
    mounted = false;
    
    DBG("[SD] SD card não disponível, usando modo offline");
    return false;
}

bool sd_mount() {
    if (mounted) {
        return true;
    }

    // Desabilita touch CS para evitar conflito
    pinMode(PIN_TOUCH_CS, OUTPUT);
    digitalWrite(PIN_TOUCH_CS, HIGH);

    // Inicializa cartao
    if (!SD.begin(PIN_SD_CS, *sdSPI, SD_FREQ_DEFAULT)) {
        DBG("[SD] Falha ao inicializar cartao");
        sdCardPresent = false;
        sdCardError = true;
        cardInfo.state = SD_STATE_NOT_MOUNTED;
        return false;
    }

    // Verifica tipo do cartao
    uint8_t cardType = SD.cardType();
    cardInfo.type = cardType;
    cardInfo.speed = SD_FREQ_DEFAULT / 1000000;

    if (cardType == CARD_NONE) {
        DBG("[SD] Cartao nao detectado");
        sdCardPresent = false;
        cardInfo.state = SD_STATE_NOT_MOUNTED;
        return false;
    }

    // Obtem informacoes do cartao
    cardInfo.totalBytes = SD.totalBytes();
    cardInfo.usedBytes = SD.usedBytes();
    cardInfo.writeEnabled = cardType != CARD_SDHC;
    cardInfo.state = SD_STATE_MOUNTED;

    mounted = true;
    sdCardPresent = true;
    sdCardError = false;

    DBG("[SD] Cartao mountado com sucesso");
    return true;
}

void sd_unmount() {
    if (mounted) {
        SD.end();
        mounted = false;
        sdCardPresent = false;
        cardInfo.state = SD_STATE_NOT_MOUNTED;
        DBG("[SD] Cartao desmontado");
    }
}

// ============================================================================
// INFORMACOES
// ============================================================================

SDCardInfo sd_get_info() {
    if (mounted) {
        cardInfo.totalBytes = SD.totalBytes();
        cardInfo.usedBytes = SD.usedBytes();
    }
    return cardInfo;
}

bool sd_is_ready() {
    return mounted && sdCardPresent;
}

uint32_t sd_free_space() {
    if (!mounted) return 0;
    return SD.totalBytes() - SD.usedBytes();
}

// ============================================================================
// OPERACOES DE ARQUIVO
// ============================================================================

File sd_open_file(const char* path, const char* mode) {
    if (!mounted) {
        return File();
    }
    return SD.open(path, FILE_WRITE);
}

bool sd_close_file(File* file) {
    if (file && file->available()) {
        file->close();
        return true;
    }
    return false;
}

bool sd_file_exists(const char* path) {
    if (!mounted) return false;
    return SD.exists(path);
}

bool sd_delete_file(const char* path) {
    if (!mounted) return false;
    return SD.remove(path);
}

bool sd_rename_file(const char* oldPath, const char* newPath) {
    if (!mounted) return false;
    return SD.rename(oldPath, newPath);
}

// ============================================================================
// ESCRITA
// ============================================================================

size_t sd_write_line(File* file, const char* line) {
    if (!file || !file->available()) return 0;
    return file->println(line);
}

size_t sd_write_csv(File* file, const char* data) {
    if (!file || !file->available()) return 0;
    return file->println(data);
}

// ============================================================================
// LISTAGEM
// ============================================================================

bool sd_list_dir(const char* path) {
    if (!mounted) return false;

    File root = SD.open(path);
    if (!root) return false;

    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;

        DBG(entry.name());
        entry.close();
    }

    root.close();
    return true;
}

// ============================================================================
// UTILITARIOS
// ============================================================================

const char* sd_get_error_string() {
    if (sdCardError) {
        return "Erro no cartao SD";
    }
    if (!sdCardPresent) {
        return "Cartao nao presente";
    }
    if (!mounted) {
        return "Cartao nao mountado";
    }
    return "OK";
}

void sd_print_info() {
    if (!mounted) {
        DBG("[SD] Cartao nao esta mountado");
        return;
    }

    DBG("[SD] Informacoes do Cartao:");
    DBG_VAL("Tipo", cardInfo.type);
    DBG_VAL("Total (bytes)", cardInfo.totalBytes);
    DBG_VAL("Usado (bytes)", cardInfo.usedBytes);
    DBG_VAL("Livre (bytes)", sd_free_space());
}