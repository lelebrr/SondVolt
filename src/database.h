// ============================================================================
// Component Tester PRO v3.0 — Database de Componentes (CYD Edition)
// ============================================================================
// Base de dados de componentes carregada do MicroSD Card.
// Otimizado para ESP32: buffer maior, índice em RAM por categoria.
// O SD Card da CYD usa barramento HSPI separado do TFT (VSPI).
// ============================================================================
#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <stdint.h>

// ============================================================================
// ESTRUTURA DE UM COMPONENTE NO BANCO DE DADOS
// ============================================================================
struct ComponentDB {
  char name[20];          // Nome/part number (ex: "BC547", "1N4148")
  uint8_t category;       // Categoria (CAT_BJT_NPN, CAT_DIODE, etc.)
  uint16_t value1;        // Valor primário (hFE, capacitância, resistência, Vf)
  uint16_t min1;          // Valor mínimo aceito para match
  uint16_t max1;          // Valor máximo aceito para match
  uint16_t value2;        // Valor secundário (Vbe, Vz, etc.)
  uint8_t pinout[3];      // Pinagem (BCE, ACK, etc.)
  char description[50];   // Descrição breve
  char common_use[50];    // Uso comum
  uint8_t typical_esr;    // ESR típico (capacitores)
};

// ============================================================================
// CATEGORIAS DE COMPONENTES
// ============================================================================
#define CAT_BJT_NPN       1
#define CAT_BJT_PNP       2
#define CAT_MOSFET_N      3
#define CAT_MOSFET_P      4
#define CAT_DIODE         5
#define CAT_ZENER         6
#define CAT_SCHOTTKY      7
#define CAT_LED           8
#define CAT_CAPACITOR     9
#define CAT_RESISTOR      10
#define CAT_INDUCTOR      11
#define CAT_OPTOCOUPLER   12
#define CAT_CRYSTAL       13
#define CAT_POTENTIOMETER 14
#define CAT_FUSE          15
#define CAT_VARISTOR      16
#define CAT_NTC           17
#define CAT_TRIAC         18
#define CAT_SCR           19
#define CAT_REGULATOR     20
#define CAT_RELAY         21
#define CAT_SENSOR        22
#define CAT_OTHER         23
#define CAT_COUNT         24    // Total de categorias

// ============================================================================
// ÍNDICE EM RAM (otimização para ESP32)
// ============================================================================
// Armazena o offset no arquivo e o número de entradas por categoria.
// Permite seek direto para a seção relevante do CSV.
struct CategoryIndex {
  uint32_t fileOffset;    // Posição no arquivo onde a categoria começa
  uint16_t count;         // Número de entradas dessa categoria
};

extern CategoryIndex categoryIndex[CAT_COUNT];
extern bool dbIndexLoaded;    // Flag: índice já foi construído

// ============================================================================
// PROTÓTIPOS
// ============================================================================
// Inicializa o SD Card no barramento HSPI e constrói o índice em RAM
bool db_init_sd();

// Constrói o índice de categorias a partir do CSV (chamado uma vez no boot)
bool db_load_index();

// Busca o melhor match para os valores medidos
ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1,
                          uint16_t measured_value2, uint16_t measured_esr);

// Imprime informações de um componente na Serial (debug)
void printComponentInfo(const ComponentDB &comp, uint16_t measured, uint16_t esr);

// Retorna o nome legível de uma categoria
const char* getCategoryName(uint8_t category);

#endif // DATABASE_H