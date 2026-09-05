// ============================================================================
// Sondvolt v4.0 - Registro de Medicoes no Cartao SD
// ============================================================================
// Arquivo : logger.cpp
//
// Antes desta revisao, logger_write() so era chamado por task_manager.cpp,
// que nunca era criado. Ou seja: NADA era gravado no cartao, apesar de o
// manual prometer historico automatico. Agora a gravacao acontece na propria
// conclusao de cada medicao.
//
// Outras correcoes:
//   - Rotacao de arquivo: sem isso o CSV crescia sem limite ate encher o
//     cartao e a gravacao passava a falhar em silencio.
//   - Cabecalho de coluna, para o arquivo abrir direto em qualquer planilha.
//   - Data e hora relativas ao boot em formato legivel, no lugar de um
//     millis() cru que nao dizia nada ao usuario.
//   - Leitura do historico sem alocar objetos String por linha, que
//     fragmentavam o heap ate travar o aparelho depois de algumas centenas
//     de registros.
// ============================================================================

#include "logger.h"
#include "config.h"
#include "globals.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "diagnostics.h"
#include <SdFat.h>

extern SdFat sd;

static bool     gReady      = false;
static uint32_t gWriteCount = 0;

static const char* kCsvHeader =
    "tempo_ms;hh:mm:ss;componente;valor;unidade;status";

// ----------------------------------------------------------------------------
// Auxiliares
// ----------------------------------------------------------------------------

// Converte millis() em hh:mm:ss desde o boot.
static void uptime_string(uint32_t ms, char* buf, size_t len) {
    uint32_t s = ms / 1000UL;
    snprintf(buf, len, "%02lu:%02lu:%02lu",
             (unsigned long)(s / 3600UL),
             (unsigned long)((s % 3600UL) / 60UL),
             (unsigned long)(s % 60UL));
}

// Cria o arquivo com cabecalho se ele ainda nao existir.
static void ensure_header() {
    if (sd.exists(LOG_FILE_PATH)) return;

    FsFile f = sd.open(LOG_FILE_PATH, FILE_WRITE);
    if (!f) return;
    f.println(kCsvHeader);
    f.close();
}

// Renomeia o arquivo atual para .OLD quando ele passa do tamanho limite.
static void rotate_if_needed() {
    FsFile f = sd.open(LOG_FILE_PATH, FILE_READ);
    if (!f) return;

    uint32_t size = f.size();
    f.close();

    if (size < LOG_MAX_BYTES) return;

    sd.remove(LOG_FILE_BACKUP);
    if (sd.rename(LOG_FILE_PATH, LOG_FILE_BACKUP)) {
        LOG_SERIAL_FMT("[LOG] Arquivo rotacionado com %lu bytes\n",
                       (unsigned long)size);
        ensure_header();
    }
}

// ============================================================================
// CICLO DE VIDA
// ============================================================================

bool logger_init() {
    LOCK_TFT();

    // O cartao tem barramento SPI proprio na CYD, mas o mutex evita que a
    // tarefa de interface desenhe no meio de uma transacao lenta do cartao.
    SdSpiConfig fast(PIN_SD_CS, SHARED_SPI,
                     SD_SCK_MHZ(SD_SPI_SPEED_MHZ), &spiTFT_SD);

    bool ok = sd.begin(fast);

    if (!ok) {
        // Cartoes antigos ou cabos longos nem sempre aguentam 10 MHz.
        // Em vez de desistir, tentamos uma velocidade conservadora.
        LOG_SERIAL_F("[LOG] Falha em 10 MHz, tentando 4 MHz...");
        SdSpiConfig safeCfg(PIN_SD_CS, SHARED_SPI,
                            SD_SCK_MHZ(SD_SPI_SPEED_SAFE_MHZ), &spiTFT_SD);
        ok = sd.begin(safeCfg);
    }

    UNLOCK_TFT();

    if (!ok) {
        gReady        = false;
        sdCardPresent = false;
        sdCardError   = true;
        LOG_SERIAL_F("[LOG] Cartao SD nao detectado");
        return false;
    }

    gReady        = true;
    sdCardPresent = true;
    sdCardError   = false;

    LOCK_TFT();
    rotate_if_needed();
    ensure_header();
    UNLOCK_TFT();

    LOG_SERIAL_F("[LOG] Cartao SD pronto");
    return true;
}

void logger_close() {
    gReady = false;
}

bool logger_is_ready() { return gReady; }

// ============================================================================
// GRAVACAO
// ============================================================================

bool logger_write(const char* component, float value, const char* unit,
                  const char* status) {
    if (!gReady) return false;
    if (!deviceSettings.autoSaveHistory) return false;

    LOCK_TFT();

    // Verifica a rotacao a cada 50 gravacoes: consultar o tamanho do arquivo
    // toda vez seria caro demais.
    if ((gWriteCount % 50) == 0) rotate_if_needed();

    FsFile f = sd.open(LOG_FILE_PATH, FILE_WRITE | O_APPEND);
    if (!f) {
        UNLOCK_TFT();
        sdCardError = true;
        return false;
    }

    char clock[12];
    uptime_string(millis(), clock, sizeof(clock));

    char line[160];
    snprintf(line, sizeof(line), "%lu;%s;%s;%.6g;%s;%s",
             (unsigned long)millis(), clock,
             component ? component : "?",
             value,
             unit ? unit : "",
             status ? status : "");

    f.println(line);
    f.close();
    UNLOCK_TFT();

    gWriteCount++;
    diag_count_sd_write();
    return true;
}

bool logger_write_result(const char* component, float value, const char* unit,
                         ComponentStatus status) {
    extern const char* db_status_string(ComponentStatus);
    return logger_write(component, value, unit, db_status_string(status));
}

// ============================================================================
// LEITURA DO HISTORICO
// ============================================================================

// Le uma linha sem alocar String, evitando fragmentacao do heap.
static bool read_line(FsFile& f, char* buf, size_t bufLen) {
    if (bufLen == 0) return false;
    size_t i = 0;
    int c;
    bool got = false;

    while ((c = f.read()) >= 0) {
        got = true;
        if (c == '\n') break;
        if (c == '\r') continue;
        if (i < bufLen - 1) buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return got;
}

// Copia o campo `field` (separado por ponto e virgula) para dest.
static bool field_at(const char* line, uint8_t field, char* dest, size_t destLen) {
    const char* p = line;
    for (uint8_t i = 0; i < field; i++) {
        p = strchr(p, ';');
        if (!p) { dest[0] = '\0'; return false; }
        p++;
    }
    const char* end = strchr(p, ';');
    size_t len = end ? (size_t)(end - p) : strlen(p);
    if (len >= destLen) len = destLen - 1;
    memcpy(dest, p, len);
    dest[len] = '\0';
    return true;
}

uint8_t logger_get_recent(HistoryItem* buffer, uint8_t maxEntries) {
    if (!gReady || !buffer || maxEntries == 0) return 0;

    LOCK_TFT();
    FsFile f = sd.open(LOG_FILE_PATH, FILE_READ);
    if (!f) { UNLOCK_TFT(); return 0; }

    char line[160];
    char field[40];
    uint8_t count = 0;

    while (read_line(f, line, sizeof(line))) {
        if (line[0] == '\0') continue;
        if (strncmp(line, "tempo_ms", 8) == 0) continue;   // cabecalho
        if (strchr(line, ';') == nullptr)      continue;

        // Buffer circular: mantemos sempre as ULTIMAS maxEntries linhas.
        // A versao anterior deslocava o array inteiro a cada linha lida, o
        // que ficava lento quando o arquivo passava de algumas centenas de
        // registros.
        uint8_t slot = count % maxEntries;
        HistoryItem& item = buffer[slot];
        memset(&item, 0, sizeof(HistoryItem));

        if (field_at(line, 2, field, sizeof(field)))
            strncpy(item.componentName, field, sizeof(item.componentName) - 1);
        if (field_at(line, 3, field, sizeof(field)))
            item.value = atof(field);
        if (field_at(line, 4, field, sizeof(field)))
            strncpy(item.unit, field, sizeof(item.unit) - 1);
        if (field_at(line, 5, field, sizeof(field))) {
            if      (strncmp(field, "BOM", 3) == 0)      item.status = STATUS_GOOD;
            else if (strncmp(field, "SUSPEITO", 8) == 0) item.status = STATUS_SUSPECT;
            else if (strncmp(field, "RUIM", 4) == 0)     item.status = STATUS_BAD;
            else                                          item.status = STATUS_UNKNOWN;
        }
        if (field_at(line, 0, field, sizeof(field)))
            item.timestamp = strtoul(field, nullptr, 10);

        count++;
    }
    f.close();
    UNLOCK_TFT();

    if (count == 0) return 0;
    if (count <= maxEntries) return count;

    // O buffer circular deixou as linhas fora de ordem; realinha para que
    // buffer[0] seja a mais antiga das que sobraram.
    uint8_t start = count % maxEntries;
    if (start != 0) {
        HistoryItem temp[16];
        uint8_t n = (maxEntries < 16) ? maxEntries : 16;
        for (uint8_t i = 0; i < n; i++) temp[i] = buffer[(start + i) % n];
        for (uint8_t i = 0; i < n; i++) buffer[i] = temp[i];
    }
    return maxEntries;
}

uint32_t logger_count_entries() {
    if (!gReady) return 0;

    LOCK_TFT();
    FsFile f = sd.open(LOG_FILE_PATH, FILE_READ);
    if (!f) { UNLOCK_TFT(); return 0; }

    char line[160];
    uint32_t n = 0;
    while (read_line(f, line, sizeof(line))) {
        if (line[0] != '\0' && strncmp(line, "tempo_ms", 8) != 0) n++;
    }
    f.close();
    UNLOCK_TFT();
    return n;
}

void logger_clear() {
    if (!gReady) return;
    LOCK_TFT();
    sd.remove(LOG_FILE_PATH);
    ensure_header();
    UNLOCK_TFT();
    gWriteCount = 0;
    LOG_SERIAL_F("[LOG] Historico apagado");
}

// ============================================================================
// RELATORIO
// ============================================================================

// Exporta um resumo legivel para /RELATOR/<nome>.TXT, pronto para entregar
// ao cliente junto com o aparelho consertado.
bool logger_export_report(const char* jobName) {
    if (!gReady || !jobName || !jobName[0]) return false;

    LOCK_TFT();
    sd.mkdir(REPORT_DIR);

    char path[64];
    snprintf(path, sizeof(path), "%s/%.20s.TXT", REPORT_DIR, jobName);

    FsFile out = sd.open(path, FILE_WRITE);
    if (!out) { UNLOCK_TFT(); return false; }

    char clock[12];
    uptime_string(millis(), clock, sizeof(clock));

    out.println("========================================");
    out.println(" SONDVOLT - RELATORIO DE MEDICOES");
    out.println("========================================");
    out.print("Trabalho : "); out.println(jobName);
    out.print("Firmware : "); out.println(FW_NAME " v" FW_VERSION);
    out.print("Tempo    : "); out.println(clock);
    out.println("");
    out.println("COMPONENTE           VALOR        STATUS");
    out.println("----------------------------------------");

    FsFile in = sd.open(LOG_FILE_PATH, FILE_READ);
    uint32_t rows = 0;
    if (in) {
        char line[160], name[40], value[24], unit[16], status[16];
        while (read_line(in, line, sizeof(line))) {
            if (line[0] == '\0' || strncmp(line, "tempo_ms", 8) == 0) continue;
            field_at(line, 2, name,   sizeof(name));
            field_at(line, 3, value,  sizeof(value));
            field_at(line, 4, unit,   sizeof(unit));
            field_at(line, 5, status, sizeof(status));

            char row[96];
            snprintf(row, sizeof(row), "%-20.20s %8.8s %-4.4s %s",
                     name, value, unit, status);
            out.println(row);
            rows++;
        }
        in.close();
    }

    out.println("----------------------------------------");
    char footer[48];
    snprintf(footer, sizeof(footer), "Total de medicoes: %lu",
             (unsigned long)rows);
    out.println(footer);
    out.close();
    UNLOCK_TFT();

    LOG_SERIAL_FMT("[LOG] Relatorio gravado em %s\n", path);
    return true;
}

void logger_log(LogLevel level, const char* message) {
    static const char* kNames[] = { "DEBUG", "INFO", "AVISO", "ERRO", "CRITICO" };
    uint8_t idx = (level <= LOG_CRITICAL) ? (uint8_t)level : 1;
    LOG_SERIAL_FMT("[%s] %s\n", kNames[idx], message ? message : "");
}
