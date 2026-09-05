// ============================================================================
// Sondvolt v5.0 - Pareamento de Componentes (implementacao)
// ============================================================================

#include "sorting.h"
#include "measurements.h"
#include "database.h"
#include "config.h"
#include "globals.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "netsvc.h"
#include <SdFat.h>
#include <math.h>

extern SdFat sd;

static SortItem gItems[SORT_MAX_ITEMS];
static SortPair gPairs[SORT_MAX_PAIRS];
static uint8_t  gCount     = 0;
static uint8_t  gPairCount = 0;
static float    gTolerance = 5.0f;

// ============================================================================
// SESSAO
// ============================================================================

void sorting_begin(float tolerancePercent) {
    sorting_clear();
    sorting_set_tolerance(tolerancePercent);
}

void sorting_clear() {
    memset(gItems, 0, sizeof(gItems));
    memset(gPairs, 0, sizeof(gPairs));
    gCount = gPairCount = 0;
}

void sorting_set_tolerance(float percent) {
    if (percent < 0.1f)  percent = 0.1f;
    if (percent > 50.0f) percent = 50.0f;
    gTolerance = percent;
}

float   sorting_get_tolerance() { return gTolerance; }
uint8_t sorting_count()         { return gCount; }

const SortItem* sorting_at(uint8_t index) {
    return (index < gCount) ? &gItems[index] : nullptr;
}

uint8_t sorting_add_value(float value, ComponentType type,
                          ComponentStatus status) {
    if (gCount >= SORT_MAX_ITEMS) return 0xFF;

    // Valor nao utilizavel: recusar e melhor que poluir o lote e estragar a
    // estatistica de todas as outras pecas.
    if (value <= 0.0f || isnan(value) || isinf(value)) return 0xFF;

    SortItem& item = gItems[gCount];
    item.id     = (uint8_t)(gCount + 1);
    item.value  = value;
    item.type   = type;
    item.status = status;
    item.used   = false;
    item.valid  = true;

    gCount++;
    gPairCount = 0;   // os pares antigos deixaram de valer
    return (uint8_t)(gCount - 1);
}

uint8_t sorting_add_current() {
    AnalysisResult r = measurements_run_full_test();
    if (!r.valid) return 0xFF;

    // Escolhe a grandeza que faz sentido comparar para cada tipo.
    float value = r.resistance;
    switch (r.type) {
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO: value = r.capacitance;    break;
        case COMP_INDUCTOR:          value = r.inductance;     break;
        case COMP_DIODE:
        case COMP_LED:
        case COMP_ZENER:             value = r.forwardVoltage; break;
        case COMP_TRANSISTOR_NPN:
        case COMP_TRANSISTOR_PNP:    value = r.gain;           break;
        default: break;
    }
    return sorting_add_value(value, r.type, r.status);
}

bool sorting_remove_last() {
    if (gCount == 0) return false;
    gCount--;
    memset(&gItems[gCount], 0, sizeof(SortItem));
    gPairCount = 0;
    return true;
}

// ============================================================================
// PAREAMENTO
// ============================================================================

// Indices ordenados por valor crescente. Ordenacao por insercao: com no
// maximo 40 itens, e mais rapida na pratica que qualquer algoritmo
// sofisticado, e ocupa menos codigo.
static void sorted_indices(uint8_t* order, uint8_t n) {
    for (uint8_t i = 0; i < n; i++) order[i] = i;

    for (uint8_t i = 1; i < n; i++) {
        uint8_t key = order[i];
        int8_t  j   = (int8_t)i - 1;
        while (j >= 0 && gItems[order[j]].value > gItems[key].value) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }
}

uint8_t sorting_compute_pairs() {
    gPairCount = 0;
    memset(gPairs, 0, sizeof(gPairs));
    for (uint8_t i = 0; i < gCount; i++) gItems[i].used = false;

    if (gCount < 2) return 0;

    uint8_t order[SORT_MAX_ITEMS];
    sorted_indices(order, gCount);

    // Percorre a lista ordenada emparelhando vizinhos. Como a lista esta
    // ordenada, o vizinho imediato e sempre o candidato mais proximo - nao
    // adianta procurar mais longe.
    for (uint8_t i = 0; i + 1 < gCount && gPairCount < SORT_MAX_PAIRS; i++) {
        uint8_t a = order[i];
        uint8_t b = order[i + 1];

        if (gItems[a].used || gItems[b].used) continue;

        float va = gItems[a].value;
        float vb = gItems[b].value;
        if (va <= 0.0f) continue;

        // Desvio em relacao a media do par: mais justo que usar uma das duas
        // como referencia arbitraria.
        float mean = (va + vb) / 2.0f;
        float dev  = (mean > 0.0f) ? (fabsf(vb - va) / mean) * 100.0f : 999.0f;

        if (dev <= gTolerance) {
            SortPair& p = gPairs[gPairCount];
            p.idA          = gItems[a].id;
            p.idB          = gItems[b].id;
            p.valueA       = va;
            p.valueB       = vb;
            p.deviationPct = dev;
            p.valid        = true;

            gItems[a].used = true;
            gItems[b].used = true;
            gPairCount++;
            i++;   // pula o proximo, ja consumido
        }
    }
    return gPairCount;
}

uint8_t sorting_pair_count() { return gPairCount; }

const SortPair* sorting_pair_at(uint8_t index) {
    return (index < gPairCount) ? &gPairs[index] : nullptr;
}

uint8_t sorting_unpaired_count() {
    uint8_t n = 0;
    for (uint8_t i = 0; i < gCount; i++) if (!gItems[i].used) n++;
    return n;
}

// ============================================================================
// ESTATISTICAS
// ============================================================================

SortStats sorting_stats() {
    SortStats s;
    memset(&s, 0, sizeof(s));
    s.count = gCount;
    if (gCount == 0) return s;

    s.minValue = gItems[0].value;
    s.maxValue = gItems[0].value;
    double sum = 0.0;

    // Conta os tipos para descobrir qual predomina no lote.
    //
    // COMP_UNKNOWN vale 99 e COMP_NONE vale 100, bem acima do tamanho do
    // vetor. A versao anterior simplesmente os ignorava e devolvia o indice
    // 0 (COMP_RESISTOR) como dominante - ou seja, um lote inteiro de pecas
    // nao identificadas era reportado como "Resistor". Agora eles tem uma
    // posicao propria no fim do vetor.
    const uint8_t kTypeSlots = 24;
    const uint8_t kSlotOther = kTypeSlots;      // COMP_UNKNOWN / COMP_NONE
    uint8_t typeCount[kTypeSlots + 1];
    memset(typeCount, 0, sizeof(typeCount));

    for (uint8_t i = 0; i < gCount; i++) {
        float v = gItems[i].value;
        if (v < s.minValue) s.minValue = v;
        if (v > s.maxValue) s.maxValue = v;
        sum += v;

        uint8_t t = (uint8_t)gItems[i].type;
        typeCount[(t < kTypeSlots) ? t : kSlotOther]++;
    }

    s.average = (float)(sum / gCount);

    uint8_t best = 0;
    for (uint8_t t = 1; t <= kTypeSlots; t++) {
        if (typeCount[t] > typeCount[best]) best = t;
    }
    s.dominantType = (best == kSlotOther) ? COMP_UNKNOWN : (ComponentType)best;

    // Mediana sobre a lista ordenada.
    uint8_t order[SORT_MAX_ITEMS];
    sorted_indices(order, gCount);
    s.median = (gCount % 2 == 1)
             ? gItems[order[gCount / 2]].value
             : (gItems[order[gCount / 2 - 1]].value +
                gItems[order[gCount / 2]].value) / 2.0f;

    // Dispersao: quanto o lote inteiro varia em relacao a media.
    s.spreadPct = (s.average > 0.0f)
                ? ((s.maxValue - s.minValue) / s.average) * 100.0f : 0.0f;

    s.pairsFound = gPairCount;
    return s;
}

const char* sorting_quality_text() {
    if (gCount < 2) return "meça mais peças";

    SortStats s = sorting_stats();

    // Os limiares vem do uso pratico: abaixo de 5 % de dispersao o lote
    // inteiro serve para qualquer aplicacao casada.
    if (s.spreadPct < 5.0f)  return "lote muito uniforme";
    if (s.spreadPct < 15.0f) return "lote uniforme";
    if (s.spreadPct < 40.0f) return "dispersao moderada";
    return "lote muito disperso";
}

// ============================================================================
// EXPORTACAO
// ============================================================================

bool sorting_export(const char* name) {
    if (!sdCardPresent || gCount == 0) return false;

    char path[64];
    snprintf(path, sizeof(path), "/PAREADO/%.8s.CSV", name ? name : "LOTE");

    LOCK_TFT();
    sd.mkdir("/PAREADO");
    FsFile f = sd.open(path, FILE_WRITE);
    if (!f) { UNLOCK_TFT(); return false; }

    char when[24];
    net_format_datetime(when, sizeof(when));

    char line[128];
    snprintf(line, sizeof(line), "# Sondvolt - lote pareado em %s", when);
    f.println(line);
    snprintf(line, sizeof(line), "# tolerancia=%.1f%%", gTolerance);
    f.println(line);
    f.println("");

    f.println("peca;valor;tipo;status;pareada");
    for (uint8_t i = 0; i < gCount; i++) {
        snprintf(line, sizeof(line), "%u;%.6g;%s;%s;%s",
                 (unsigned)gItems[i].id, gItems[i].value,
                 analysis_type_name(gItems[i].type),
                 db_status_string(gItems[i].status),
                 gItems[i].used ? "sim" : "nao");
        f.println(line);
    }

    f.println("");
    f.println("par;peca_A;peca_B;valor_A;valor_B;desvio_pct");
    for (uint8_t i = 0; i < gPairCount; i++) {
        snprintf(line, sizeof(line), "%u;%u;%u;%.6g;%.6g;%.2f",
                 (unsigned)(i + 1), (unsigned)gPairs[i].idA,
                 (unsigned)gPairs[i].idB, gPairs[i].valueA,
                 gPairs[i].valueB, gPairs[i].deviationPct);
        f.println(line);
    }

    f.close();
    UNLOCK_TFT();

    LOG_SERIAL_FMT("[PAR] Lote exportado para %s\n", path);
    return true;
}
