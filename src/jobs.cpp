// ============================================================================
// Sondvolt v5.0 - Sistema de Trabalhos (implementacao)
// ============================================================================

#include "jobs.h"
#include "config.h"
#include "globals.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "database.h"
#include "diagnostics.h"
#include "netsvc.h"
#include <SdFat.h>
#include <Preferences.h>

extern SdFat sd;

static JobInfo    gActive;
static JobInfo    gList[JOB_MAX_LISTED];
static uint8_t    gListCount = 0;
static bool       gReady     = false;
static char       gReportPath[64] = { 0 };
static Preferences gPrefs;

// ============================================================================
// AUXILIARES
// ============================================================================

// Converte um nome livre em um nome de pasta valido no FAT 8.3:
// maiusculas, sem acento, sem espaco, no maximo 8 caracteres.
static void sanitize_dirname(const char* input, char* out, size_t outLen) {
    size_t j = 0;
    for (size_t i = 0; input[i] && j < outLen - 1 && j < JOB_MAX_DIR; i++) {
        char c = input[i];

        // Normaliza os acentos mais comuns do portugues em vez de descarta-los,
        // para "Joao" nao virar "JO".
        switch ((unsigned char)c) {
            case 0xC3: continue;                    // primeiro byte do UTF-8
            case 0xA1: case 0xA0: case 0xA3: case 0xA2: c = 'A'; break;
            case 0xA9: case 0xAA:                       c = 'E'; break;
            case 0xAD:                                  c = 'I'; break;
            case 0xB3: case 0xB5: case 0xB4:            c = 'O'; break;
            case 0xBA:                                  c = 'U'; break;
            case 0xA7:                                  c = 'C'; break;
            default: break;
        }

        if (c >= 'a' && c <= 'z')                   c = (char)(c - 32);
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            out[j++] = c;
        } else if ((c == ' ' || c == '-' || c == '_') && j > 0) {
            out[j++] = '_';
        }
    }
    if (j == 0) { out[j++] = 'J'; out[j++] = 'O'; out[j++] = 'B'; }
    out[j] = '\0';
}

static void job_path(const char* dirName, const char* file,
                     char* out, size_t outLen) {
    snprintf(out, outLen, "%s/%s/%s", JOBS_ROOT_DIR, dirName, file);
}

// Le uma linha sem alocar String (mesma razao do logger: nao fragmentar heap).
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

// Le "chave=valor" e devolve o valor se a chave bater.
static bool parse_kv(const char* line, const char* key,
                     char* out, size_t outLen) {
    size_t klen = strlen(key);
    if (strncmp(line, key, klen) != 0 || line[klen] != '=') return false;
    strncpy(out, line + klen + 1, outLen - 1);
    out[outLen - 1] = '\0';
    return true;
}

// ============================================================================
// GRAVACAO E LEITURA DO CABECALHO
// ============================================================================

static bool write_job_info(const JobInfo& job) {
    char path[64];
    job_path(job.dirName, "JOB.INF", path, sizeof(path));

    LOCK_TFT();
    FsFile f = sd.open(path, FILE_WRITE);
    if (!f) { UNLOCK_TFT(); return false; }

    char line[96];
    snprintf(line, sizeof(line), "nome=%s",     job.name);      f.println(line);
    snprintf(line, sizeof(line), "cliente=%s",  job.customer);  f.println(line);
    snprintf(line, sizeof(line), "aparelho=%s", job.device);    f.println(line);
    snprintf(line, sizeof(line), "criado=%lu",  (unsigned long)job.createdEpoch);
    f.println(line);
    snprintf(line, sizeof(line), "medicoes=%lu",
             (unsigned long)job.measurementCount);              f.println(line);
    snprintf(line, sizeof(line), "boas=%lu",  (unsigned long)job.goodCount);
    f.println(line);
    snprintf(line, sizeof(line), "ruins=%lu", (unsigned long)job.badCount);
    f.println(line);
    snprintf(line, sizeof(line), "situacao=%d", (int)job.status);
    f.println(line);

    f.close();
    UNLOCK_TFT();
    return true;
}

static bool read_job_info(const char* dirName, JobInfo* out) {
    memset(out, 0, sizeof(JobInfo));
    strncpy(out->dirName, dirName, JOB_MAX_DIR);

    char path[64];
    job_path(dirName, "JOB.INF", path, sizeof(path));

    LOCK_TFT();
    FsFile f = sd.open(path, FILE_READ);
    if (!f) { UNLOCK_TFT(); return false; }

    char line[96], value[64];
    while (read_line(f, line, sizeof(line))) {
        if (parse_kv(line, "nome", value, sizeof(value)))
            strncpy(out->name, value, JOB_MAX_NAME);
        else if (parse_kv(line, "cliente", value, sizeof(value)))
            strncpy(out->customer, value, JOB_MAX_NAME);
        else if (parse_kv(line, "aparelho", value, sizeof(value)))
            strncpy(out->device, value, JOB_MAX_NAME);
        else if (parse_kv(line, "criado", value, sizeof(value)))
            out->createdEpoch = strtoul(value, nullptr, 10);
        else if (parse_kv(line, "medicoes", value, sizeof(value)))
            out->measurementCount = strtoul(value, nullptr, 10);
        else if (parse_kv(line, "boas", value, sizeof(value)))
            out->goodCount = strtoul(value, nullptr, 10);
        else if (parse_kv(line, "ruins", value, sizeof(value)))
            out->badCount = strtoul(value, nullptr, 10);
        else if (parse_kv(line, "situacao", value, sizeof(value)))
            out->status = (JobStatus)atoi(value);
    }
    f.close();
    UNLOCK_TFT();

    // Um trabalho sem nome no arquivo ainda e utilizavel: usa a pasta.
    if (out->name[0] == '\0') strncpy(out->name, dirName, JOB_MAX_NAME);
    out->valid = true;
    return true;
}

// ============================================================================
// CICLO DE VIDA
// ============================================================================

void jobs_init() {
    memset(&gActive, 0, sizeof(gActive));
    gListCount = 0;
    gReady     = false;

    if (!sdCardPresent) {
        LOG_SERIAL_F("[JOB] Sem cartao: sistema de trabalhos desativado");
        return;
    }

    LOCK_TFT();
    sd.mkdir(JOBS_ROOT_DIR);
    UNLOCK_TFT();
    gReady = true;

    // Reabre o trabalho que estava ativo antes do desligamento.
    if (gPrefs.begin("jobs", true)) {
        char last[JOB_MAX_DIR + 1] = { 0 };
        size_t n = gPrefs.getBytes("active", last, sizeof(last) - 1);
        gPrefs.end();

        if (n > 0 && last[0] != '\0') {
            if (jobs_open(last)) {
                LOG_SERIAL_FMT("[JOB] Trabalho reaberto: %s\n", gActive.name);
            }
        }
    }
}

bool jobs_available() { return gReady && sdCardPresent; }

// ============================================================================
// TRABALHO ATIVO
// ============================================================================

static void remember_active(const char* dirName) {
    if (!gPrefs.begin("jobs", false)) return;
    if (dirName && dirName[0]) gPrefs.putBytes("active", dirName, strlen(dirName) + 1);
    else                       gPrefs.remove("active");
    gPrefs.end();
}

bool jobs_create(const char* name, const char* customer, const char* device) {
    if (!jobs_available() || !name || !name[0]) return false;

    char dir[JOB_MAX_DIR + 1];
    sanitize_dirname(name, dir, sizeof(dir));

    // Se a pasta ja existir, acrescenta um digito ate achar uma livre.
    char path[64];
    snprintf(path, sizeof(path), "%s/%s", JOBS_ROOT_DIR, dir);

    LOCK_TFT();
    if (sd.exists(path)) {
        size_t base = strlen(dir);
        if (base > JOB_MAX_DIR - 1) base = JOB_MAX_DIR - 1;
        bool found = false;
        for (char suffix = '2'; suffix <= '9'; suffix++) {
            dir[base] = suffix;
            dir[base + 1] = '\0';
            snprintf(path, sizeof(path), "%s/%s", JOBS_ROOT_DIR, dir);
            if (!sd.exists(path)) { found = true; break; }
        }
        if (!found) { UNLOCK_TFT(); return false; }
    }
    bool made = sd.mkdir(path);
    UNLOCK_TFT();
    if (!made) return false;

    memset(&gActive, 0, sizeof(gActive));
    strncpy(gActive.dirName,  dir,                       JOB_MAX_DIR);
    strncpy(gActive.name,     name,                      JOB_MAX_NAME);
    strncpy(gActive.customer, customer ? customer : "",  JOB_MAX_NAME);
    strncpy(gActive.device,   device   ? device   : "",  JOB_MAX_NAME);
    gActive.createdEpoch  = net_epoch_now();   // 0 se nao houver relogio
    gActive.createdMillis = millis();
    gActive.status        = JOB_STATUS_OPEN;
    gActive.valid         = true;

    write_job_info(gActive);

    // Cria o CSV ja com cabecalho, para abrir direto em planilha.
    char csv[64];
    job_path(dir, "MEDICOES.CSV", csv, sizeof(csv));
    LOCK_TFT();
    FsFile f = sd.open(csv, FILE_WRITE);
    if (f) {
        f.println("quando;componente;valor;unidade;status;observacao");
        f.close();
    }
    UNLOCK_TFT();

    remember_active(dir);
    LOG_SERIAL_FMT("[JOB] Trabalho criado: %s (%s)\n", gActive.name, dir);
    return true;
}

bool jobs_open(const char* dirName) {
    if (!jobs_available() || !dirName || !dirName[0]) return false;
    if (!read_job_info(dirName, &gActive)) return false;
    remember_active(dirName);
    return true;
}

void jobs_close() {
    if (gActive.valid) write_job_info(gActive);
    memset(&gActive, 0, sizeof(gActive));
    remember_active(nullptr);
}

bool jobs_finish() {
    if (!gActive.valid) return false;
    gActive.status = JOB_STATUS_DONE;
    write_job_info(gActive);
    bool ok = jobs_generate_report(gActive.dirName);
    jobs_close();
    return ok;
}

const JobInfo* jobs_active()     { return gActive.valid ? &gActive : nullptr; }
bool           jobs_has_active() { return gActive.valid; }
const char*    jobs_active_name(){ return gActive.valid ? gActive.name : ""; }

// ============================================================================
// REGISTRO
// ============================================================================

// Carimbo de tempo: data e hora reais se houver relogio, senao o tempo
// desde o boot. Nunca inventa uma data.
static void timestamp_text(char* out, size_t outLen) {
    if (net_time_valid()) {
        net_format_datetime(out, outLen);
    } else {
        uint32_t s = millis() / 1000UL;
        snprintf(out, outLen, "+%02lu:%02lu:%02lu",
                 (unsigned long)(s / 3600UL),
                 (unsigned long)((s % 3600UL) / 60UL),
                 (unsigned long)(s % 60UL));
    }
}

static bool append_measurement(const char* label, float value,
                               const char* unit, ComponentStatus status,
                               const char* note) {
    if (!gActive.valid) return false;

    char path[64];
    job_path(gActive.dirName, "MEDICOES.CSV", path, sizeof(path));

    LOCK_TFT();
    FsFile f = sd.open(path, FILE_WRITE | O_APPEND);
    if (!f) { UNLOCK_TFT(); return false; }

    char when[24];
    timestamp_text(when, sizeof(when));

    char line[160];
    snprintf(line, sizeof(line), "%s;%s;%.6g;%s;%s;%s",
             when,
             label ? label : "?",
             value,
             unit ? unit : "",
             db_status_string(status),
             note ? note : "");
    f.println(line);
    f.close();
    UNLOCK_TFT();

    gActive.measurementCount++;
    if (status == STATUS_GOOD)                                gActive.goodCount++;
    else if (status == STATUS_BAD || status == STATUS_SHORT)  gActive.badCount++;

    // Regrava o cabecalho a cada 10 medicoes: se faltar energia, perde-se
    // pouco, e nao castiga o cartao a cada leitura.
    if ((gActive.measurementCount % 10) == 0) write_job_info(gActive);

    diag_count_sd_write();
    return true;
}

bool jobs_record(const AnalysisResult& result) {
    if (!gActive.valid) return false;

    float value = result.resistance;
    switch (result.type) {
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO: value = result.capacitance;    break;
        case COMP_INDUCTOR:          value = result.inductance;     break;
        case COMP_DIODE:
        case COMP_LED:
        case COMP_ZENER:             value = result.forwardVoltage; break;
        case COMP_TRANSISTOR_NPN:
        case COMP_TRANSISTOR_PNP:    value = result.gain;           break;
        default: break;
    }

    return append_measurement(result.label, value,
                              analysis_type_unit(result.type),
                              result.status, result.detail);
}

bool jobs_record_value(const char* label, float value, const char* unit,
                       ComponentStatus status) {
    return append_measurement(label, value, unit, status, "");
}

bool jobs_add_note(const char* text) {
    if (!gActive.valid || !text || !text[0]) return false;

    char path[64];
    job_path(gActive.dirName, "NOTAS.TXT", path, sizeof(path));

    LOCK_TFT();
    FsFile f = sd.open(path, FILE_WRITE | O_APPEND);
    if (!f) { UNLOCK_TFT(); return false; }

    char when[24];
    timestamp_text(when, sizeof(when));
    char line[160];
    snprintf(line, sizeof(line), "[%s] %s", when, text);
    f.println(line);
    f.close();
    UNLOCK_TFT();
    return true;
}

// ============================================================================
// LISTAGEM
// ============================================================================

uint8_t jobs_scan() {
    gListCount = 0;
    if (!jobs_available()) return 0;

    LOCK_TFT();
    FsFile root = sd.open(JOBS_ROOT_DIR, O_RDONLY);
    if (!root) { UNLOCK_TFT(); return 0; }

    FsFile entry;
    char name[16];
    while (gListCount < JOB_MAX_LISTED && entry.openNext(&root, O_RDONLY)) {
        if (entry.isDirectory()) {
            entry.getName(name, sizeof(name));
            if (name[0] != '\0' && name[0] != '.') {
                strncpy(gList[gListCount].dirName, name, JOB_MAX_DIR);
                gList[gListCount].dirName[JOB_MAX_DIR] = '\0';
                gList[gListCount].valid = true;
                gListCount++;
            }
        }
        entry.close();
    }
    root.close();
    UNLOCK_TFT();

    // Le o cabecalho de cada um para ter o nome completo na lista.
    for (uint8_t i = 0; i < gListCount; i++) {
        char dir[JOB_MAX_DIR + 1];
        strncpy(dir, gList[i].dirName, sizeof(dir));
        dir[JOB_MAX_DIR] = '\0';
        read_job_info(dir, &gList[i]);
    }
    return gListCount;
}

uint8_t jobs_count() { return gListCount; }

const JobInfo* jobs_at(uint8_t index) {
    return (index < gListCount) ? &gList[index] : nullptr;
}

const char* jobs_name_at(uint8_t index) {
    if (index >= gListCount) return "";
    return gList[index].name[0] ? gList[index].name : gList[index].dirName;
}

bool jobs_delete(const char* dirName) {
    if (!jobs_available() || !dirName || !dirName[0]) return false;

    // Se for o trabalho ativo, fecha antes de apagar.
    if (gActive.valid && strcmp(gActive.dirName, dirName) == 0) jobs_close();

    char path[64];
    LOCK_TFT();
    const char* files[] = { "JOB.INF", "MEDICOES.CSV", "NOTAS.TXT", "RELATOR.TXT" };
    for (uint8_t i = 0; i < 4; i++) {
        job_path(dirName, files[i], path, sizeof(path));
        sd.remove(path);
    }
    snprintf(path, sizeof(path), "%s/%s", JOBS_ROOT_DIR, dirName);
    bool ok = sd.remove(path);   // so remove pasta vazia, que e o caso agora
    UNLOCK_TFT();

    return ok;
}

// ============================================================================
// RELATORIO
// ============================================================================

const char* jobs_report_path() { return gReportPath; }

bool jobs_generate_report(const char* dirName) {
    if (!jobs_available()) return false;

    JobInfo job;
    if (dirName == nullptr) {
        if (!gActive.valid) return false;
        job = gActive;
    } else {
        if (!read_job_info(dirName, &job)) return false;
    }

    job_path(job.dirName, "RELATOR.TXT", gReportPath, sizeof(gReportPath));

    LOCK_TFT();
    FsFile out = sd.open(gReportPath, FILE_WRITE);
    if (!out) { UNLOCK_TFT(); return false; }

    char when[24];
    timestamp_text(when, sizeof(when));

    out.println("========================================");
    out.println("  SONDVOLT - RELATORIO DE SERVICO");
    out.println("========================================");

    char line[128];
    snprintf(line, sizeof(line), "Trabalho : %s", job.name);       out.println(line);
    snprintf(line, sizeof(line), "Cliente  : %s", job.customer);   out.println(line);
    snprintf(line, sizeof(line), "Aparelho : %s", job.device);     out.println(line);
    snprintf(line, sizeof(line), "Emitido  : %s", when);           out.println(line);
    snprintf(line, sizeof(line), "Firmware : %s v%s", FW_NAME, FW_VERSION);
    out.println(line);
    out.println("");

    out.println("MEDICOES REALIZADAS");
    out.println("----------------------------------------");
    out.println("QUANDO     COMPONENTE        VALOR  STATUS");

    char path[64];
    job_path(job.dirName, "MEDICOES.CSV", path, sizeof(path));
    FsFile in = sd.open(path, FILE_READ);
    uint32_t rows = 0;

    if (in) {
        char raw[160];
        while (read_line(in, raw, sizeof(raw))) {
            if (raw[0] == '\0' || strncmp(raw, "quando", 6) == 0) continue;

            // Reaproveita o proprio CSV: substitui ';' por espacos alinhados.
            char cols[6][40];
            uint8_t col = 0, ci = 0;
            memset(cols, 0, sizeof(cols));
            for (size_t i = 0; raw[i] && col < 6; i++) {
                if (raw[i] == ';') { cols[col][ci] = '\0'; col++; ci = 0; }
                else if (ci < 39)  { cols[col][ci++] = raw[i]; }
            }
            if (col < 6) cols[col][ci] = '\0';

            snprintf(line, sizeof(line), "%-10.10s %-16.16s %8.8s %s",
                     cols[0], cols[1], cols[2], cols[4]);
            out.println(line);
            rows++;
        }
        in.close();
    }

    out.println("----------------------------------------");
    snprintf(line, sizeof(line), "Total de medicoes : %lu", (unsigned long)rows);
    out.println(line);
    snprintf(line, sizeof(line), "Aprovadas         : %lu",
             (unsigned long)job.goodCount);
    out.println(line);
    snprintf(line, sizeof(line), "Reprovadas        : %lu",
             (unsigned long)job.badCount);
    out.println(line);
    out.println("");

    // Anexa as notas do tecnico, se houver.
    job_path(job.dirName, "NOTAS.TXT", path, sizeof(path));
    FsFile notes = sd.open(path, FILE_READ);
    if (notes) {
        out.println("OBSERVACOES");
        out.println("----------------------------------------");
        char raw[160];
        while (read_line(notes, raw, sizeof(raw))) {
            if (raw[0]) out.println(raw);
        }
        notes.close();
        out.println("");
    }

    out.println("========================================");
    out.close();
    UNLOCK_TFT();

    LOG_SERIAL_FMT("[JOB] Relatorio gravado em %s\n", gReportPath);
    return true;
}

const char* jobs_status_text(JobStatus status) {
    switch (status) {
        case JOB_STATUS_OPEN:      return "EM ANDAMENTO";
        case JOB_STATUS_DONE:      return "FINALIZADO";
        case JOB_STATUS_CANCELLED: return "CANCELADO";
        default:                   return "?";
    }
}
