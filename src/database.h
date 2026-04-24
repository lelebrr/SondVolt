// ============================================================================
// Sondvolt v3.0 — Banco de Dados de Componentes
// Descrição: Estrutura do banco de dados na RAM + SD + busca
// ============================================================================
#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>

// ============================================================================
// TIPOS DE COMPONENTES
// ============================================================================
enum ComponentType {
    COMP_RESISTOR    = 0,
    COMP_CAPACITOR   = 1,
    COMP_ELECTROLYTIC = 2,
    COMP_DIODE       = 3,
    COMP_LED         = 4,
    COMP_ZENER       = 5,
    COMP_TRANSISTOR  = 6,
    COMP_MOSFET      = 7,
    COMP_INDUCTOR   = 8,
    COMP_RELAY       = 9,
    COMP_IC         = 10,
    COMP_COIL       = 11,
    COMP_UNKNOWN     = 99
};

// ============================================================================
// JULGAMENTO DE COMPONENTE
// ============================================================================
enum ComponentStatus {
    STATUS_GOOD     = 0,   // Bom
    STATUS_SUSPECT = 1,   // Suspeito (marginal)
    STATUS_BAD     = 2,   // Ruim
    STATUS_NONE    = 3    // Sem identificação
};

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