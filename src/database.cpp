// ============================================================================
// Sondvolt v4.0 - Banco de Dados de Componentes (implementacao)
// ============================================================================

#include "database.h"
#include "config.h"
#include "globals.h"
#include "display_globals.h"
#include <SdFat.h>
#include <string.h>
#include <math.h>

extern SdFat sd;

// ============================================================================
// CATALOGO INTERNO
// ============================================================================
// Componentes que realmente aparecem em bancada de conserto, com os
// parametros tipicos das folhas de dados dos fabricantes. Fica em flash e
// esta sempre disponivel, mesmo sem cartao SD.
//
// Campos: peca, tipo, nominal, minBom, maxBom, param2, unidade, pinagem, texto
// ============================================================================

static const ComponentInfo kCatalog[] = {
    // --- Transistores bipolares NPN de pequeno sinal -------------------------
    { "BC547",   COMP_TRANSISTOR_NPN,  300,  110,  800, 0.70f, "hFE", "E-B-C", "NPN 45V 100mA uso geral" },
    { "BC548",   COMP_TRANSISTOR_NPN,  300,  110,  800, 0.70f, "hFE", "E-B-C", "NPN 30V 100mA uso geral" },
    { "BC549",   COMP_TRANSISTOR_NPN,  500,  200, 1000, 0.70f, "hFE", "E-B-C", "NPN baixo ruido audio" },
    { "2N2222",  COMP_TRANSISTOR_NPN,  200,  100,  300, 0.70f, "hFE", "E-B-C", "NPN 40V 800mA chaveamento" },
    { "2N3904",  COMP_TRANSISTOR_NPN,  200,  100,  400, 0.70f, "hFE", "E-B-C", "NPN 40V 200mA uso geral" },
    { "BC337",   COMP_TRANSISTOR_NPN,  250,  100,  630, 0.70f, "hFE", "E-B-C", "NPN 45V 800mA driver" },
    { "BD139",   COMP_TRANSISTOR_NPN,   60,   25,  250, 0.70f, "hFE", "E-C-B", "NPN 80V 1.5A media potencia" },
    { "TIP31C",  COMP_TRANSISTOR_NPN,   50,   10,  100, 0.90f, "hFE", "B-C-E", "NPN 100V 3A potencia" },
    { "TIP41C",  COMP_TRANSISTOR_NPN,   75,   15,  150, 0.90f, "hFE", "B-C-E", "NPN 100V 6A potencia" },
    { "TIP120",  COMP_TRANSISTOR_NPN, 2500, 1000, 5000, 1.40f, "hFE", "B-C-E", "Darlington NPN 60V 5A" },
    { "S8050",   COMP_TRANSISTOR_NPN,  200,  100,  400, 0.70f, "hFE", "E-B-C", "NPN 25V 700mA SMD/TO92" },
    { "2N3055",  COMP_TRANSISTOR_NPN,   50,   20,   70, 1.10f, "hFE", "B-C-E", "NPN 60V 15A potencia TO-3" },

    // --- Transistores bipolares PNP -----------------------------------------
    { "BC557",   COMP_TRANSISTOR_PNP,  300,  110,  800, 0.70f, "hFE", "E-B-C", "PNP 45V 100mA uso geral" },
    { "BC558",   COMP_TRANSISTOR_PNP,  300,  110,  800, 0.70f, "hFE", "E-B-C", "PNP 30V 100mA uso geral" },
    { "2N3906",  COMP_TRANSISTOR_PNP,  200,  100,  300, 0.70f, "hFE", "E-B-C", "PNP 40V 200mA uso geral" },
    { "BC327",   COMP_TRANSISTOR_PNP,  250,  100,  630, 0.70f, "hFE", "E-B-C", "PNP 45V 800mA driver" },
    { "BD140",   COMP_TRANSISTOR_PNP,   60,   25,  250, 0.70f, "hFE", "E-C-B", "PNP 80V 1.5A media potencia" },
    { "TIP32C",  COMP_TRANSISTOR_PNP,   50,   10,  100, 0.90f, "hFE", "B-C-E", "PNP 100V 3A potencia" },
    { "TIP42C",  COMP_TRANSISTOR_PNP,   75,   15,  150, 0.90f, "hFE", "B-C-E", "PNP 100V 6A potencia" },
    { "TIP125",  COMP_TRANSISTOR_PNP, 2500, 1000, 5000, 1.40f, "hFE", "B-C-E", "Darlington PNP 60V 5A" },

    // --- MOSFETs -------------------------------------------------------------
    { "IRFZ44N", COMP_MOSFET_N,      0.017f, 0.010f, 0.030f, 4.0f, "Ohm", "G-D-S", "N-MOS 55V 49A Rds 17mOhm" },
    { "IRF540N", COMP_MOSFET_N,      0.044f, 0.030f, 0.070f, 4.0f, "Ohm", "G-D-S", "N-MOS 100V 33A" },
    { "IRF9540", COMP_MOSFET_P,      0.117f, 0.080f, 0.200f, 4.0f, "Ohm", "G-D-S", "P-MOS 100V 23A" },
    { "2N7000",  COMP_MOSFET_N,      5.0f,   3.0f,   8.0f,   2.1f, "Ohm", "S-G-D", "N-MOS 60V 200mA sinal" },
    { "AO3400",  COMP_MOSFET_N,      0.035f, 0.020f, 0.060f, 1.4f, "Ohm", "G-S-D", "N-MOS 30V 5.7A SOT-23" },

    // --- Diodos --------------------------------------------------------------
    { "1N4148",  COMP_DIODE,  0.72f, 0.60f, 0.85f, 100.0f, "V", "A-K", "Sinal rapido 100V 200mA" },
    { "1N4001",  COMP_DIODE,  0.75f, 0.60f, 0.95f,  50.0f, "V", "A-K", "Retificador 50V 1A" },
    { "1N4007",  COMP_DIODE,  0.75f, 0.60f, 0.95f, 1000.0f,"V", "A-K", "Retificador 1000V 1A" },
    { "1N5819",  COMP_DIODE,  0.32f, 0.20f, 0.45f,  40.0f, "V", "A-K", "Schottky 40V 1A" },
    { "1N5408",  COMP_DIODE,  0.80f, 0.65f, 1.00f, 1000.0f,"V", "A-K", "Retificador 1000V 3A" },
    { "SS34",    COMP_DIODE,  0.35f, 0.22f, 0.50f,  40.0f, "V", "A-K", "Schottky SMD 40V 3A" },
    { "BAT54",   COMP_DIODE,  0.28f, 0.18f, 0.40f,  30.0f, "V", "A-K", "Schottky sinal SOT-23" },

    // --- Zeners --------------------------------------------------------------
    { "1N4733",  COMP_ZENER,  5.10f, 4.80f, 5.40f, 1.0f, "V", "A-K", "Zener 5.1V 1W" },
    { "1N4742",  COMP_ZENER, 12.00f,11.40f,12.60f, 1.0f, "V", "A-K", "Zener 12V 1W" },
    { "1N4744",  COMP_ZENER, 15.00f,14.25f,15.75f, 1.0f, "V", "A-K", "Zener 15V 1W" },
    { "BZX55C3V3",COMP_ZENER, 3.30f, 3.10f, 3.50f, 0.5f, "V", "A-K", "Zener 3.3V 500mW" },

    // --- LEDs ----------------------------------------------------------------
    { "LED-VERM", COMP_LED, 1.90f, 1.60f, 2.10f, 20.0f, "V", "A-K", "LED vermelho 5mm" },
    { "LED-AMAR", COMP_LED, 2.05f, 1.90f, 2.30f, 20.0f, "V", "A-K", "LED amarelo 5mm" },
    { "LED-VERD", COMP_LED, 2.20f, 1.90f, 2.60f, 20.0f, "V", "A-K", "LED verde 5mm" },
    { "LED-AZUL", COMP_LED, 3.10f, 2.70f, 3.50f, 20.0f, "V", "A-K", "LED azul 5mm" },
    { "LED-IR",   COMP_LED, 1.30f, 1.10f, 1.60f, 50.0f, "V", "A-K", "LED infravermelho 940nm" },

    // --- Reguladores e integrados comuns -------------------------------------
    { "LM7805",  COMP_IC,  5.00f, 4.80f, 5.20f, 1.5f, "V", "I-G-O", "Regulador linear +5V 1A" },
    { "LM7812",  COMP_IC, 12.00f,11.50f,12.50f, 1.5f, "V", "I-G-O", "Regulador linear +12V 1A" },
    { "AMS1117", COMP_IC,  3.30f, 3.20f, 3.40f, 1.0f, "V", "G-O-I", "LDO 3.3V 1A SOT-223" },
    { "NE555",   COMP_IC,  0.00f, 0.00f, 0.00f, 16.0f,"V", "DIP-8", "Temporizador 4.5 a 16V" },
    { "LM358",   COMP_IC,  0.00f, 0.00f, 0.00f, 32.0f,"V", "DIP-8", "Amp. operacional duplo" },
    { "LM393",   COMP_IC,  0.00f, 0.00f, 0.00f, 36.0f,"V", "DIP-8", "Comparador duplo" },

    // --- Passivos de referencia ----------------------------------------------
    { "R-10K",   COMP_RESISTOR,  10000.0f,  9500.0f, 10500.0f, 0.25f, "Ohm", "1-2", "Resistor 10k 5% 1/4W" },
    { "R-1K",    COMP_RESISTOR,   1000.0f,   950.0f,  1050.0f, 0.25f, "Ohm", "1-2", "Resistor 1k 5% 1/4W" },
    { "R-220R",  COMP_RESISTOR,    220.0f,   209.0f,   231.0f, 0.25f, "Ohm", "1-2", "Resistor 220R 5% 1/4W" },
    { "C-100N",  COMP_CAPACITOR_CERAMIC, 1e-7f, 8e-8f, 1.2e-7f, 50.0f, "F", "1-2", "Ceramico 100nF 50V" },
    { "C-10U",   COMP_CAPACITOR_ELECTRO, 1e-5f, 8e-6f, 1.2e-5f, 50.0f, "F", "+/-", "Eletrolitico 10uF 50V" },
    { "C-1000U", COMP_CAPACITOR_ELECTRO, 1e-3f, 8e-4f, 1.2e-3f, 25.0f, "F", "+/-", "Eletrolitico 1000uF 25V" },
};

static const uint16_t kCatalogCount = sizeof(kCatalog) / sizeof(kCatalog[0]);

// ============================================================================
// ESTADO
// ============================================================================
static DbStats gStats = { 0, 0, false, 0 };

// ============================================================================
// AUXILIARES DE TEXTO
// ============================================================================

static bool str_iequals(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'a' && ca <= 'z') ca -= 32;
        if (cb >= 'a' && cb <= 'z') cb -= 32;
        if (ca != cb) return false;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

// Copia o campo de indice `field` de uma linha CSV para dest.
// Devolve false se o campo nao existir.
static bool csv_field(const char* line, uint8_t field, char* dest, size_t destLen) {
    if (!line || !dest || destLen == 0) return false;

    const char* p = line;
    for (uint8_t i = 0; i < field; i++) {
        p = strchr(p, ',');
        if (!p) { dest[0] = '\0'; return false; }
        p++;
    }
    const char* end = strchr(p, ',');
    size_t len = end ? (size_t)(end - p) : strlen(p);

    // Remove CR/LF e espacos no fim
    while (len > 0 && (p[len - 1] == '\r' || p[len - 1] == '\n' || p[len - 1] == ' ')) len--;
    if (len >= destLen) len = destLen - 1;

    memcpy(dest, p, len);
    dest[len] = '\0';
    return true;
}

static float csv_field_float(const char* line, uint8_t field) {
    char buf[24];
    if (!csv_field(line, field, buf, sizeof(buf))) return 0.0f;
    return atof(buf);
}

static uint8_t csv_field_uint(const char* line, uint8_t field) {
    char buf[12];
    if (!csv_field(line, field, buf, sizeof(buf))) return 0;
    return (uint8_t)atoi(buf);
}

// Preenche um DbRecord a partir de uma linha completa do CSV.
static void csv_to_record(const char* line, DbRecord* rec) {
    memset(rec, 0, sizeof(DbRecord));
    csv_field(line, 0, rec->name,        sizeof(rec->name));
    rec->csvType = csv_field_uint(line, 1);
    rec->nominal = csv_field_float(line, 2);
    rec->minVal  = csv_field_float(line, 3);
    rec->maxVal  = csv_field_float(line, 4);
    rec->param2  = csv_field_float(line, 5);
    csv_field(line, 9,  rec->description, sizeof(rec->description));
    csv_field(line, 10, rec->category,    sizeof(rec->category));
    rec->found = (rec->name[0] != '\0');
}

// ----------------------------------------------------------------------------
// Leitura de uma linha do cartao sem alocar String (evita fragmentar o heap)
// ----------------------------------------------------------------------------
static bool sd_read_line(FsFile& f, char* buf, size_t bufLen) {
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

// ============================================================================
// CICLO DE VIDA
// ============================================================================

void db_init() {
    gStats.catalogCount  = kCatalogCount;
    gStats.sdRecordCount = 0;
    gStats.sdAvailable   = false;
    gStats.lastScanMs    = millis();

    componentDB.count  = kCatalogCount;
    componentDB.loaded = true;   // o catalogo interno sempre carrega

    if (!sdCardPresent) {
        LOG_SERIAL_FMT("[DB] Catalogo interno: %u componentes (sem cartao SD)\n",
                       (unsigned)kCatalogCount);
        return;
    }

    FsFile file = sd.open(DB_FILE_CSV, O_RDONLY);
    if (!file) {
        LOG_SERIAL_FMT("[DB] %s nao encontrado. Usando apenas o catalogo interno.\n",
                       DB_FILE_CSV);
        return;
    }

    // Conta apenas linhas realmente validas (com nome e tipo).
    char line[160];
    uint32_t valid = 0;
    while (sd_read_line(file, line, sizeof(line))) {
        if (line[0] == '\0' || line[0] == '#') continue;
        if (strchr(line, ',') == nullptr)       continue;
        valid++;
    }
    file.close();

    gStats.sdRecordCount = valid;
    gStats.sdAvailable   = (valid > 0);
    gStats.lastScanMs    = millis();

    LOG_SERIAL_FMT("[DB] Catalogo interno: %u | Cartao SD: %lu registros\n",
                   (unsigned)kCatalogCount, (unsigned long)valid);
}

void db_reload() {
    LOG_SERIAL_F("[DB] Reindexando banco de dados...");
    db_init();
}

DbStats db_get_stats() { return gStats; }

uint16_t get_db_count() {
    uint32_t total = (uint32_t)kCatalogCount + gStats.sdRecordCount;
    return (total > 65535U) ? 65535U : (uint16_t)total;
}

// ============================================================================
// CATALOGO INTERNO
// ============================================================================

uint16_t db_catalog_size() { return kCatalogCount; }

const ComponentInfo* db_catalog_at(uint16_t index) {
    return (index < kCatalogCount) ? &kCatalog[index] : nullptr;
}

const ComponentInfo* db_catalog_find(const char* partNumber) {
    if (!partNumber || !partNumber[0]) return nullptr;
    for (uint16_t i = 0; i < kCatalogCount; i++) {
        if (str_iequals(kCatalog[i].partNumber, partNumber)) return &kCatalog[i];
    }
    return nullptr;
}

const ComponentInfo* db_catalog_next_of_type(ComponentType type,
                                             uint16_t startIndex,
                                             uint16_t* outIndex) {
    for (uint16_t i = startIndex; i < kCatalogCount; i++) {
        if (kCatalog[i].type == type) {
            if (outIndex) *outIndex = i;
            return &kCatalog[i];
        }
    }
    return nullptr;
}

uint16_t db_catalog_count_type(ComponentType type) {
    uint16_t n = 0;
    for (uint16_t i = 0; i < kCatalogCount; i++) {
        if (kCatalog[i].type == type) n++;
    }
    return n;
}

const ComponentInfo* db_catalog_closest(ComponentType type, float value) {
    const ComponentInfo* best = nullptr;
    float bestErr = 0.0f;

    for (uint16_t i = 0; i < kCatalogCount; i++) {
        if (kCatalog[i].type != type)   continue;
        if (kCatalog[i].nominal <= 0.0f) continue;

        // Erro relativo: compara ordens de grandeza de forma justa.
        float err = fabsf(value - kCatalog[i].nominal) / kCatalog[i].nominal;
        if (!best || err < bestErr) { best = &kCatalog[i]; bestErr = err; }
    }
    return best;
}

// ============================================================================
// CONSULTA AO CARTAO
// ============================================================================

DbRecord db_sd_find(const char* partNumber) {
    DbRecord rec;
    memset(&rec, 0, sizeof(rec));

    if (!sdCardPresent || !partNumber || !partNumber[0]) return rec;

    FsFile file = sd.open(DB_FILE_CSV, O_RDONLY);
    if (!file) return rec;

    char line[160];
    char name[24];
    while (sd_read_line(file, line, sizeof(line))) {
        if (!csv_field(line, 0, name, sizeof(name))) continue;
        if (str_iequals(name, partNumber)) {
            csv_to_record(line, &rec);
            break;
        }
    }
    file.close();
    return rec;
}

uint8_t db_sd_find_by_value(uint8_t csvType, float value, float tolerancePct,
                            DbRecord* results, uint8_t maxResults) {
    if (!sdCardPresent || !results || maxResults == 0) return 0;
    if (value <= 0.0f) return 0;

    FsFile file = sd.open(DB_FILE_CSV, O_RDONLY);
    if (!file) return 0;

    const float lo = value * (1.0f - tolerancePct / 100.0f);
    const float hi = value * (1.0f + tolerancePct / 100.0f);

    char line[160];
    uint8_t found = 0;

    while (found < maxResults && sd_read_line(file, line, sizeof(line))) {
        if (csv_field_uint(line, 1) != csvType) continue;
        float nominal = csv_field_float(line, 2);
        if (nominal < lo || nominal > hi)       continue;

        csv_to_record(line, &results[found]);
        if (results[found].found) found++;
    }
    file.close();
    return found;
}

uint8_t db_sd_find_equivalents(const char* partNumber,
                               DbRecord* results, uint8_t maxResults) {
    if (!results || maxResults == 0) return 0;

    // Primeiro descobre os parametros da peca de referencia.
    DbRecord ref = db_sd_find(partNumber);
    if (!ref.found) {
        // Cai para o catalogo interno se a peca nao estiver no cartao.
        const ComponentInfo* info = db_catalog_find(partNumber);
        if (!info) return 0;
        ref.csvType = db_component_to_csv_type(info->type);
        ref.nominal = info->nominal;
        ref.found   = true;
    }
    if (ref.nominal <= 0.0f) return 0;

    // Equivalente util na pratica: mesmo tipo e ganho/valor dentro de 30 %.
    uint8_t n = db_sd_find_by_value(ref.csvType, ref.nominal, 30.0f,
                                    results, maxResults);

    // Remove a propria peca da lista de substitutos.
    for (uint8_t i = 0; i < n; i++) {
        if (str_iequals(results[i].name, partNumber)) {
            for (uint8_t j = i; j + 1 < n; j++) results[j] = results[j + 1];
            n--;
            break;
        }
    }
    return n;
}

// ============================================================================
// JULGAMENTO
// ============================================================================

ComponentStatus db_judge(ComponentType type, float value) {
    if (value <= 0.0f) return STATUS_INVALID;

    const ComponentInfo* info = db_catalog_closest(type, value);
    if (!info) {
        // Sem referencia no catalogo nao ha como julgar honestamente.
        return STATUS_UNKNOWN;
    }
    if (value >= info->minGood && value <= info->maxGood) return STATUS_GOOD;

    // Ate 25 % fora da janela de "bom" ainda pode ser tolerancia de fabrica.
    if (value >= info->minGood * 0.75f && value <= info->maxGood * 1.25f)
        return STATUS_SUSPECT;

    return STATUS_BAD;
}

ComponentStatus db_judge_part(const char* partNumber, float value) {
    const ComponentInfo* info = db_catalog_find(partNumber);
    if (!info) {
        DbRecord rec = db_sd_find(partNumber);
        if (!rec.found) return STATUS_UNKNOWN;
        if (value >= rec.minVal && value <= rec.maxVal) return STATUS_GOOD;
        if (value >= rec.minVal * 0.75f && value <= rec.maxVal * 1.25f)
            return STATUS_SUSPECT;
        return STATUS_BAD;
    }
    if (value >= info->minGood && value <= info->maxGood) return STATUS_GOOD;
    if (value >= info->minGood * 0.75f && value <= info->maxGood * 1.25f)
        return STATUS_SUSPECT;
    return STATUS_BAD;
}

const char* db_status_string(ComponentStatus status) {
    switch (status) {
        case STATUS_GOOD:    return "BOM";
        case STATUS_SUSPECT: return "SUSPEITO";
        case STATUS_WARNING: return "ATENCAO";
        case STATUS_BAD:     return "RUIM";
        case STATUS_LEAKY:   return "COM FUGA";
        case STATUS_SHORT:   return "EM CURTO";
        case STATUS_OPEN:    return "ABERTO";
        case STATUS_INVALID: return "INVALIDO";
        case STATUS_NONE:    return "---";
        default:             return "?";
    }
}

uint16_t db_status_color(ComponentStatus status) {
    switch (status) {
        case STATUS_GOOD:    return C_SUCCESS;
        case STATUS_SUSPECT:
        case STATUS_WARNING:
        case STATUS_LEAKY:   return C_WARNING;
        case STATUS_BAD:
        case STATUS_SHORT:   return C_ERROR;
        case STATUS_OPEN:
        case STATUS_INVALID:
        case STATUS_NONE:
        default:             return C_TEXT_SECONDARY;
    }
}

// ============================================================================
// CONVERSAO DE TIPOS
// ============================================================================

ComponentType db_csv_type_to_component(uint8_t csvType) {
    switch (csvType) {
        case DBCSV_NPN:       return COMP_TRANSISTOR_NPN;
        case DBCSV_PNP:       return COMP_TRANSISTOR_PNP;
        case DBCSV_MOSFET_N:  return COMP_MOSFET_N;
        case DBCSV_MOSFET_P:  return COMP_MOSFET_P;
        case DBCSV_DIODE:     return COMP_DIODE;
        case DBCSV_ZENER:     return COMP_ZENER;
        case DBCSV_LED:       return COMP_LED;
        case DBCSV_INDUCTOR:  return COMP_INDUCTOR;
        case DBCSV_CAPACITOR: return COMP_CAPACITOR;
        case DBCSV_RESISTOR:  return COMP_RESISTOR;
        case DBCSV_IC:        return COMP_IC;
        default:              return COMP_UNKNOWN;
    }
}

uint8_t db_component_to_csv_type(ComponentType type) {
    switch (type) {
        case COMP_TRANSISTOR_NPN: return DBCSV_NPN;
        case COMP_TRANSISTOR_PNP: return DBCSV_PNP;
        case COMP_MOSFET_N:       return DBCSV_MOSFET_N;
        case COMP_MOSFET_P:       return DBCSV_MOSFET_P;
        case COMP_DIODE:          return DBCSV_DIODE;
        case COMP_ZENER:          return DBCSV_ZENER;
        case COMP_LED:            return DBCSV_LED;
        case COMP_INDUCTOR:       return DBCSV_INDUCTOR;
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO: return DBCSV_CAPACITOR;
        case COMP_RESISTOR:       return DBCSV_RESISTOR;
        case COMP_IC:             return DBCSV_IC;
        default:                  return 0;
    }
}
