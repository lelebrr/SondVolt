// ============================================================================
// Sondvolt v3.0 — Banco de Dados de Componentes
// Descrição: Estrutura do banco de dados na RAM + SD + busca
// ============================================================================
#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include "types.h"

// ============================================================================
// ITEM DO BANCO DE DADOS
// ============================================================================
struct ComponentInfo {
    ComponentType type;
    char name[20];
    char description[40];
    char unit[8];
    float minGood;      // Mínimo para status BOM
    float maxGood;     // Máximo para status BOM
    float minSuspect;   // Margem inferior para SUSPEITO
    float maxSuspect;   // Margem superior para SUSPEITO
    ComponentStatus defaultStatus;
};

struct ComponentDatabase {
    ComponentInfo items[20];
    uint8_t count;
    bool loaded;
};

// ============================================================================
// PROTÓTIPOS
// ============================================================================

// Inicialização do SD
bool db_init_sd();

// Carregar índice do banco de dados
bool db_load_index();

// Adicionar item ao banco de dados na RAM
int db_add(const ComponentInfo* info);

// Buscar por tipo
const ComponentInfo* db_find_by_type(ComponentType type);

// Buscar por nome (parcial)
int db_find_by_name(const char* name);

// Obter resultado de julgamento
ComponentStatus db_judge(ComponentType type, float value);

// Obter string do status
const char* db_status_string(ComponentStatus status);

// Obter cor do status
uint16_t db_status_color(ComponentStatus status);

// Obter cor do componente
uint16_t db_component_color(ComponentType type);

// Contagem do banco
int db_count();

// Verificar se há banco carregado
bool db_is_loaded();

// Carregar banco padrão na RAM (fallback quando SD não está presente)
void db_load_default();

// ============================================================================
// BANCO PADRÃO (hardcoded para quando SD não está disponível)
// ============================================================================
extern const ComponentInfo DB_DEFAULT[];
extern const uint8_t DB_DEFAULT_COUNT;

#endif // DATABASE_H