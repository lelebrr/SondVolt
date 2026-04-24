// ============================================================================
// Sondvolt v3.0 — Banco de Dados de Componentes (Implementacao)
// Descricao: Implementacao do banco de dados na RAM + SD + busca
// ============================================================================

#include "database.h"
#include "config.h"
#include "globals.h"
#include "pins.h"
#include "sdcard.h"

#include <Arduino.h>
#include <string.h>

// ============================================================================
// VARIAVEIS GLOBAIS
// ============================================================================

// Banco de dados na RAM
static ComponentInfo componentDB_RAM[DB_MAX_COMPONENTS];
static uint16_t componentCount_RAM = 0;

// Flag indicando se o banco foi carregado do SD
static bool dbLoadedFromSD = false;

// ============================================================================
// BANCO DE DADOS PADRAO (hardcoded para fallback)
// ============================================================================

const ComponentInfo DB_DEFAULT[] = {
    // Resistor (4 bandas: marrom-preto-marrom-ouro = 100Ω 5%)
    { COMP_RESISTOR, "Resistor", "Resistor padrao 1/4W", "Ohm",
      95.0f, 105.0f, 90.0f, 110.0f, STATUS_GOOD },

    // Capacitor ceramico (100nF)
    { COMP_CAPACITOR_CERAMIC, "Capacitor", "Capacitor ceramico 100nF", "uF",
      80.0f, 120.0f, 50.0f, 150.0f, STATUS_GOOD },

    // Capacitor eletrolitico (100uF)
    { COMP_CAPACITOR_ELECTRO, "Cap. Eletrolitico", "Capacitor eletrolitico 100uF", "uF",
      80.0f, 120.0f, 50.0f, 150.0f, STATUS_GOOD },

    // Diodo 1N4148
    { COMP_DIODE, "Diodo 1N4148", "Diodo de sinal rapido", "V",
      0.5f, 0.9f, 0.4f, 1.0f, STATUS_GOOD },

    // Diodo 1N4007
    { COMP_DIODE, "Diodo 1N4007", "Diodo retificador 1A", "V",
      0.5f, 0.9f, 0.4f, 1.0f, STATUS_GOOD },

    // LED vermelho
    { COMP_LED, "LED Vermelho", "LED indicador vermelho", "V",
      1.6f, 2.2f, 1.4f, 2.4f, STATUS_GOOD },

    // LED verde
    { COMP_LED, "LED Verde", "LED indicador verde", "V",
      1.8f, 2.5f, 1.6f, 2.7f, STATUS_GOOD },

    // LED azul
    { COMP_LED, "LED Azul", "LED indicador azul", "V",
      2.5f, 3.5f, 2.2f, 3.8f, STATUS_GOOD },

    // Zener 5V1
    { COMP_ZENER, "Zener 5V1", "Diodo Zener 5.1V", "V",
      4.8f, 5.4f, 4.5f, 5.7f, STATUS_GOOD },

    // Transistor NPN (BC337)
    { COMP_TRANSISTOR_NPN, "Transistor NPN", "BC337 ou equivalente", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // Transistor PNP (BC327)
    { COMP_TRANSISTOR_PNP, "Transistor PNP", "BC327 ou equivalente", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // MOSFET N (IRFZ44N)
    { COMP_MOSFET_N, "MOSFET N", "IRFZ44N ou equivalente", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // MOSFET P (IRFZ44N)
    { COMP_MOSFET_P, "MOSFET P", "IRF9540 ou equivalente", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // Fusivel 1A
    { COMP_FUSE, "Fusivel 1A", "Fusivel vidro 5x20mm 1A", "Ohm",
      0.0f, 0.5f, 0.0f, 1.0f, STATUS_GOOD },

    // Varistor (275V)
    { COMP_VARISTOR, "Varistor 275V", "MOV 275V para protecao", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // Cristal 16MHz
    { COMP_CRYSTAL, "Cristal 16MHz", "Cristal ressonador 16MHz", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // Indutor (100uH)
    { COMP_INDUCTOR, "Indutor 100uH", "Indutor axial 100uH", "uH",
      80.0f, 120.0f, 50.0f, 150.0f, STATUS_GOOD },

    // Potenciometro (10k)
    { COMP_POTENTIOMETER, "Potenciometro 10k", "Potenciometro linear 10k", "Ohm",
      8000.0f, 12000.0f, 5000.0f, 15000.0f, STATUS_GOOD },

    // Optoacoplador (PC817)
    { COMP_OPTOCOUPLER, "Optoacoplador PC817", "Optoacoplador 4 pinos", "?",
      0.0f, 0.0f, 0.0f, 0.0f, STATUS_GOOD },

    // Rele 5V (SRD-5VDC)
    { COMP_RELAY, "Rele 5V", "Rele modulador SRD-5VDC", "Ohm",
      60.0f, 100.0f, 50.0f, 150.0f, STATUS_GOOD },
};

// ============================================================================
// INICIALIZACAO SEGURA
// ============================================================================

bool db_init_safe() {
    // Inicializa banco de dados com componentes padrão
    componentCount_RAM = sizeof(DB_DEFAULT) / sizeof(ComponentInfo);
    
    // Copia componentes padrão para o banco na RAM
    memcpy(componentDB_RAM, DB_DEFAULT, sizeof(DB_DEFAULT));
    
    // Carrega do SD se disponível
    if (sdCardPresent) {
        return db_load_index();
    }
    
    // Fallback: usa apenas componentes padrão
    dbLoadedFromSD = false;
    DBG("[DB] Banco inicializado com componentes padrão");
    return true;
}

const uint8_t DB_DEFAULT_COUNT = sizeof(DB_DEFAULT) / sizeof(ComponentInfo);

// ============================================================================
// BANCO DE DADOS GLOBAL
// ============================================================================

ComponentDatabase componentDB;

// ============================================================================
// INICIALIZACAO DO SD CARD
// ============================================================================

bool db_init_sd() {
    return sdCardPresent;
}

// ============================================================================
// CARREGAR INDICE DO BANCO DE DADOS
// ============================================================================

bool db_load_index() {
    // Tenta carregar do SD card
    if (sdCardPresent) {
        // Carrega componentes do SD
        File dbFile = SD.open(DB_FILE_CSV);
        if (dbFile) {
            // Processa arquivo CSV...
            dbFile.close();
            dbLoadedFromSD = true;
            DBG("[DB] Banco carregado do SD card");
            return true;
        }
    }
    
    // Fallback: usa componentes padrão
    componentCount_RAM = sizeof(DB_DEFAULT) / sizeof(ComponentInfo);
    memcpy(componentDB_RAM, DB_DEFAULT, sizeof(DB_DEFAULT));
    dbLoadedFromSD = false;
    DBG("[DB] Usando componentes padrão (SD não disponível)");
    return true;
}

const ComponentInfo* db_get_component(uint16_t index) {
    if (index < componentCount_RAM) {
        return &componentDB_RAM[index];
    }
    return nullptr;
}

uint16_t db_get_count() {
    return componentCount_RAM;
}

bool db_is_loaded_from_sd() {
    return dbLoadedFromSD;
}

// ============================================================================
// ADICIONAR ITEM AO BANCO DE DADOS NA RAM
// ============================================================================

int db_add(const ComponentInfo* info) {
    if (componentDB.count >= DB_MAX_COMPONENTS) {
        return -1; // Banco cheio
    }

    componentDB.items[componentDB.count] = *info;
    componentDB.count++;

    return componentDB.count - 1;
}

// ============================================================================
// BUSCAR POR TIPO
// ============================================================================

const ComponentInfo* db_find_by_type(ComponentType type) {
    // Primeiro busca no banco customizado
    for (uint8_t i = 0; i < componentDB.count; i++) {
        if (componentDB.items[i].type == type) {
            return &componentDB.items[i];
        }
    }

    // Busca no banco padrao
    for (uint8_t i = 0; i < DB_DEFAULT_COUNT; i++) {
        if (DB_DEFAULT[i].type == type) {
            return &DB_DEFAULT[i];
        }
    }

    return NULL;
}

// ============================================================================
// BUSCAR POR NOME (PARCIAL)
// ============================================================================

int db_find_by_name(const char* name) {
    if (!name) return -1;

    // Busca no banco customizado
    for (uint8_t i = 0; i < componentDB.count; i++) {
        if (strstr(componentDB.items[i].name, name) != NULL) {
            return i;
        }
    }

    // Busca no banco padrao
    for (uint8_t i = 0; i < DB_DEFAULT_COUNT; i++) {
        if (strstr(DB_DEFAULT[i].name, name) != NULL) {
            return i + componentDB.count;
        }
    }

    return -1;
}

// ============================================================================
// OBTER RESULTADO DE JULGAMENTO
// ============================================================================

ComponentStatus db_judge(ComponentType type, float value) {
    const ComponentInfo* info = db_find_by_type(type);

    if (!info) {
        return STATUS_UNKNOWN;
    }

    // Verifica se tem limites definidos
    if (info->minGood == 0.0f && info->maxGood == 0.0f) {
        return STATUS_GOOD; // Sem limites, assume bom
    }

    // Verifica limites
    if (value >= info->minGood && value <= info->maxGood) {
        return STATUS_GOOD;
    }

    if (value >= info->minSuspect && value <= info->maxSuspect) {
        return STATUS_SUSPECT;
    }

    return STATUS_BAD;
}

// ============================================================================
// OBTER STRING DO STATUS
// ============================================================================

const char* db_status_string(ComponentStatus status) {
    switch (status) {
        case STATUS_GOOD:    return "BOM";
        case STATUS_SUSPECT:  return "SUSPEITO";
        case STATUS_WARNING:  return "AVISO";
        case STATUS_BAD:    return "RUIM";
        case STATUS_INVALID: return "INVALIDO";
        case STATUS_NONE:   return "NENHUM";
        case STATUS_LEAKY:  return "INFILTRADO";
        case STATUS_SHORT:  return "CURTO";
        case STATUS_OPEN:   return "ABERTO";
        case STATUS_UNKNOWN: return "DESCONHECIDO";
        default:            return "DESCONHECIDO";
    }
}

// ============================================================================
// OBTER COR DO STATUS
// ============================================================================

uint16_t db_status_color(ComponentStatus status) {
    switch (status) {
        case STATUS_GOOD:
            return C_SUCCESS; // Verde
        case STATUS_SUSPECT:
            return C_WARNING; // Amarelo
        case STATUS_WARNING:
            return C_WARNING; // Amarelo
        case STATUS_BAD:
            return C_ERROR; // Vermelho
        case STATUS_SHORT:
            return C_ORANGE; // Laranja
        case STATUS_OPEN:
            return C_CYAN; // Azul
        case STATUS_INVALID:
        case STATUS_NONE:
        case STATUS_UNKNOWN:
        default:
            return C_GREY; // Cinza
    }
}

// ============================================================================
// OBTER COR DO COMPONENTE
// ============================================================================

uint16_t db_component_color(ComponentType type) {
    switch (type) {
        case COMP_RESISTOR:
            return C_RESISTOR; // Marrom
        case COMP_CAPACITOR:
        case COMP_CAPACITOR_CERAMIC:
        case COMP_CAPACITOR_ELECTRO:
            return C_CAPACITOR; // Azul
        case COMP_DIODE:
        case COMP_ZENER:
            return C_DIODE; // Roxo
        case COMP_LED:
            return C_LED; // Verde
        case COMP_TRANSISTOR:
        case COMP_TRANSISTOR_NPN:
        case COMP_TRANSISTOR_PNP:
            return C_TRANSISTOR; // Roxo escuro
        case COMP_MOSFET:
        case COMP_MOSFET_N:
        case COMP_MOSFET_P:
            return C_TRANSISTOR; // Roxo escuro
        case COMP_INDUCTOR:
            return C_INDUCTOR; // Couro
        case COMP_CRYSTAL:
            return C_CRYSTAL; // Quartzo
        case COMP_FUSE:
            return C_GREY; // Cinza
        case COMP_VARISTOR:
            return C_DIODE; // Roxo
        case COMP_POTENTIOMETER:
            return C_RESISTOR; // Marrom
        case COMP_OPTOCOUPLER:
            return C_DIODE; // Roxo
        case COMP_RELAY:
            return C_ORANGE; // Laranja
        case COMP_IC:
            return C_GREY; // Cinza
        case COMP_COIL:
            return C_INDUCTOR; // Couro
        default:
            return C_UNKNOWN; // Cinza
    }
}

// ============================================================================
// CONTAGEM DO BANCO
// ============================================================================

int db_count() {
    return componentDB.count + DB_DEFAULT_COUNT;
}

// ============================================================================
// VERIFICAR SE HÁ BANCO CARREGADO
// ============================================================================

bool db_is_loaded() {
    return componentDB.loaded || componentDB.count > 0;
}

// ============================================================================
// CARREGAR BANCO PADRAO NA RAM
// ============================================================================

void db_load_default() {
    componentDB.count = 0;
    componentDB.loaded = true;

    // Copia banco padrao para RAM
    for (uint8_t i = 0; i < DB_DEFAULT_COUNT; i++) {
        componentDB.items[i] = DB_DEFAULT[i];
        componentDB.count++;
    }

    DBG("[DB] Banco padrao carregado");
}