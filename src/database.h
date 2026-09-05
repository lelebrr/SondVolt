// ============================================================================
// Sondvolt v4.0 - Banco de Dados de Componentes
// ============================================================================
// Arquivo : database.h
// Objetivo: Catalogo de componentes reais em flash + consulta ao arquivo
//           COMPBD.CSV do cartao SD.
//
// Estrategia de memoria
// ---------------------
// O CSV do cartao tem milhares de linhas. Carrega-lo inteiro na RAM
// consumiria dezenas de KB sem beneficio, entao as buscas no SD sao feitas
// por varredura sob demanda (streaming). O que fica permanentemente em
// memoria e apenas o catalogo curado abaixo, com os componentes que
// realmente aparecem em bancada.
//
// Formato de cada linha do COMPBD.CSV
//   nome, tipo, nominal, minimo, maximo, parametro2, p1, p2, p3,
//   descricao, categoria, flag
// ============================================================================

#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include "types.h"

// ----------------------------------------------------------------------------
// Codigos de tipo usados no arquivo CSV (nao confundir com ComponentType)
// ----------------------------------------------------------------------------
enum DbCsvType {
    DBCSV_NPN       = 1,
    DBCSV_PNP       = 2,
    DBCSV_MOSFET_N  = 3,
    DBCSV_MOSFET_P  = 4,
    DBCSV_DIODE     = 5,
    DBCSV_ZENER     = 6,
    DBCSV_LED       = 7,
    DBCSV_INDUCTOR  = 8,
    DBCSV_CAPACITOR = 9,
    DBCSV_RESISTOR  = 10,
    DBCSV_IC        = 11
};

// ----------------------------------------------------------------------------
// Uma entrada do catalogo
// ----------------------------------------------------------------------------
struct ComponentInfo {
    const char*   partNumber;    // "BC547"
    ComponentType type;
    float         nominal;       // hFE, ohms, farads, volts...
    float         minGood;       // limite inferior de "bom"
    float         maxGood;       // limite superior de "bom"
    float         param2;        // Vf em volts, Vce max, tensao do Zener...
    const char*   unit;          // "hFE", "Ohm", "F", "V"
    const char*   pinout;        // "E-B-C", "A-K", ...
    const char*   description;   // texto curto para a tela
};

// ----------------------------------------------------------------------------
// Resultado de uma busca no cartao SD
// ----------------------------------------------------------------------------
struct DbRecord {
    char  name[24];
    char  description[40];
    char  category[20];
    uint8_t csvType;
    float nominal;
    float minVal;
    float maxVal;
    float param2;
    bool  found;
};

// ----------------------------------------------------------------------------
// Estatisticas do banco (exibidas na tela de diagnostico)
// ----------------------------------------------------------------------------
struct DbStats {
    uint16_t catalogCount;    // entradas do catalogo interno
    uint32_t sdRecordCount;   // linhas validas contadas no CSV do cartao
    bool     sdAvailable;
    uint32_t lastScanMs;
};

// Estrutura mantida por compatibilidade com o codigo antigo.
struct ComponentDatabase {
    uint16_t count;
    bool     loaded;
};

// ============================================================================
// CICLO DE VIDA
// ============================================================================

// Prepara o catalogo interno e, se houver cartao, indexa o CSV.
void db_init();

// Reindexa o CSV do cartao (apos trocar o cartao, por exemplo).
void db_reload();

// Estatisticas atuais.
DbStats db_get_stats();

// Quantidade total de componentes conhecidos (catalogo + cartao).
uint16_t get_db_count();

// ============================================================================
// CATALOGO INTERNO (sempre disponivel, nao depende do cartao)
// ============================================================================

// Numero de entradas do catalogo em flash.
uint16_t db_catalog_size();

// Entrada por indice, ou nullptr se o indice for invalido.
const ComponentInfo* db_catalog_at(uint16_t index);

// Busca exata por codigo de peca, ignorando maiusculas/minusculas.
const ComponentInfo* db_catalog_find(const char* partNumber);

// Primeira entrada do tipo pedido a partir de startIndex. Serve para navegar
// listas filtradas na interface. Escreve o indice encontrado em outIndex.
const ComponentInfo* db_catalog_next_of_type(ComponentType type,
                                             uint16_t startIndex,
                                             uint16_t* outIndex);

// Quantas entradas existem de um determinado tipo.
uint16_t db_catalog_count_type(ComponentType type);

// Componente do catalogo cujo valor nominal mais se aproxima do medido.
const ComponentInfo* db_catalog_closest(ComponentType type, float value);

// ============================================================================
// CONSULTA AO CARTAO SD (varredura sob demanda)
// ============================================================================

// Procura um codigo de peca no COMPBD.CSV. Devolve found = false se o cartao
// nao estiver montado ou a peca nao existir.
DbRecord db_sd_find(const char* partNumber);

// Procura ate maxResults pecas do tipo indicado cujo valor nominal caia
// dentro de +-tolerancePct do valor medido. Devolve quantas foram achadas.
uint8_t db_sd_find_by_value(uint8_t csvType, float value, float tolerancePct,
                            DbRecord* results, uint8_t maxResults);

// Sugere substitutos: mesma familia e parametros compativeis.
uint8_t db_sd_find_equivalents(const char* partNumber,
                               DbRecord* results, uint8_t maxResults);

// ============================================================================
// JULGAMENTO E APRESENTACAO
// ============================================================================

// Classifica um valor medido como bom, suspeito ou ruim.
ComponentStatus db_judge(ComponentType type, float value);

// Versao que tambem considera um codigo de peca conhecido.
ComponentStatus db_judge_part(const char* partNumber, float value);

// Texto curto do status ("BOM", "SUSPEITO", "RUIM").
const char* db_status_string(ComponentStatus status);

// Cor RGB565 associada ao status.
uint16_t db_status_color(ComponentStatus status);

// Converte entre a codificacao do CSV e o enum interno.
ComponentType db_csv_type_to_component(uint8_t csvType);
uint8_t       db_component_to_csv_type(ComponentType type);

#endif // DATABASE_H
