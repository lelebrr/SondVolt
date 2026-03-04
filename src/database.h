#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>

// Estrutura para um componente compatível
typedef struct {
    const char* name;
    const char* type;
    float min_value;
    float max_value;
    const char* unit;
} ComponentEntry;

// Exemplo de banco de dados de componentes compatíveis (PROGMEM)
// Este é um placeholder. O conteúdo real deve ser preenchido com 40+ itens.
const ComponentEntry componentDatabase[] PROGMEM = {
    {"Resistor 100R", "Resistor", 99.0, 101.0, "Ohm"},
    {"Capacitor 100nF", "Capacitor", 0.095, 0.105, "uF"},
    {"Diodo 1N4148", "Diodo", 0.6, 0.7, "Vf"},
    {"Transistor BC547", "Transistor NPN", 0.6, 0.7, "Vbe"},
    {"Indutor 100uH", "Indutor", 90.0, 110.0, "uH"},
    // Adicionar mais 35+ itens aqui
};

const int NUM_DATABASE_ENTRIES = sizeof(componentDatabase) / sizeof(componentDatabase[0]);

#endif // DATABASE_H
