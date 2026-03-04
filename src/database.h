#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>

// Estrutura para um componente compatível
typedef struct {
  const char *name;
  const char *type;
  float min_value;
  float max_value;
  const char *unit;
} ComponentEntry;

// Banco de dados de componentes compatíveis (PROGMEM)
const ComponentEntry componentDatabase[] PROGMEM = {
    // Resistores comuns
    {"Resistor 1R", "Resistor", 0.95, 1.05, "Ohm"},
    {"Resistor 10R", "Resistor", 9.5, 10.5, "Ohm"},
    {"Resistor 22R", "Resistor", 20.9, 23.1, "Ohm"},
    {"Resistor 33R", "Resistor", 31.35, 34.65, "Ohm"},
    {"Resistor 47R", "Resistor", 44.65, 49.35, "Ohm"},
    {"Resistor 68R", "Resistor", 64.6, 71.4, "Ohm"},
    {"Resistor 100R", "Resistor", 95.0, 105.0, "Ohm"},
    {"Resistor 150R", "Resistor", 142.5, 157.5, "Ohm"},
    {"Resistor 220R", "Resistor", 209.0, 231.0, "Ohm"},
    {"Resistor 330R", "Resistor", 313.5, 346.5, "Ohm"},
    {"Resistor 470R", "Resistor", 446.5, 493.5, "Ohm"},
    {"Resistor 680R", "Resistor", 646.0, 714.0, "Ohm"},
    {"Resistor 1K", "Resistor", 950.0, 1050.0, "Ohm"},
    {"Resistor 2K2", "Resistor", 2090.0, 2310.0, "Ohm"},
    {"Resistor 3K3", "Resistor", 3135.0, 3465.0, "Ohm"},
    {"Resistor 4K7", "Resistor", 4465.0, 4935.0, "Ohm"},
    {"Resistor 6K8", "Resistor", 6460.0, 7140.0, "Ohm"},
    {"Resistor 10K", "Resistor", 9500.0, 10500.0, "Ohm"},
    {"Resistor 22K", "Resistor", 20900.0, 23100.0, "Ohm"},
    {"Resistor 33K", "Resistor", 31350.0, 34650.0, "Ohm"},
    {"Resistor 47K", "Resistor", 44650.0, 49350.0, "Ohm"},
    {"Resistor 68K", "Resistor", 64600.0, 71400.0, "Ohm"},
    {"Resistor 100K", "Resistor", 95000.0, 105000.0, "Ohm"},
    {"Resistor 1M", "Resistor", 950000.0, 1050000.0, "Ohm"},

    // Capacitores cerâmicos
    {"Capacitor 10pF", "Capacitor", 9.5, 10.5, "pF"},
    {"Capacitor 22pF", "Capacitor", 20.9, 23.1, "pF"},
    {"Capacitor 47pF", "Capacitor", 44.65, 49.35, "pF"},
    {"Capacitor 100pF", "Capacitor", 95.0, 105.0, "pF"},
    {"Capacitor 1nF", "Capacitor", 0.95, 1.05, "nF"},
    {"Capacitor 10nF", "Capacitor", 9.5, 10.5, "nF"},
    {"Capacitor 100nF", "Capacitor", 0.095, 0.105, "uF"},
    {"Capacitor 1uF", "Capacitor", 0.95, 1.05, "uF"},

    // Capacitores eletrolíticos
    {"Capacitor Elético 10uF", "Capacitor Eletrolítico", 9.0, 11.0, "uF"},
    {"Capacitor Elético 47uF", "Capacitor Eletrolítico", 42.3, 51.7, "uF"},
    {"Capacitor Elético 100uF", "Capacitor Eletrolítico", 90.0, 110.0, "uF"},
    {"Capacitor Elético 220uF", "Capacitor Eletrolítico", 198.0, 242.0, "uF"},
    {"Capacitor Elético 470uF", "Capacitor Eletrolítico", 423.0, 517.0, "uF"},

    // Diodos
    {"Diodo 1N4148", "Diodo", 0.6, 0.7, "Vf"},
    {"Diodo 1N4007", "Diodo", 0.8, 1.1, "Vf"},
    {"Diodo Zener 5V1", "Diodo Zener", 4.8, 5.4, "Vf"},
    {"Diodo Zener 12V", "Diodo Zener", 11.4, 12.6, "Vf"},

    // LEDs
    {"LED Vermelho", "LED", 1.8, 2.2, "Vf"},
    {"LED Verde", "LED", 2.0, 2.5, "Vf"},
    {"LED Azul", "LED", 2.8, 3.2, "Vf"},
    {"LED Amarelo", "LED", 2.0, 2.4, "Vf"},

    // Transistores
    {"Transistor BC547", "Transistor NPN", 0.6, 0.7, "Vbe"},
    {"Transistor BC557", "Transistor PNP", 0.6, 0.7, "Vbe"},
    {"Transistor 2N2222", "Transistor NPN", 0.6, 0.8, "Vbe"},
    {"Transistor 2N3904", "Transistor NPN", 0.6, 0.7, "Vbe"},
    {"Transistor 2N3906", "Transistor PNP", 0.6, 0.7, "Vbe"},

    // Indutores
    {"Indutor 10uH", "Indutor", 9.5, 10.5, "uH"},
    {"Indutor 100uH", "Indutor", 90.0, 110.0, "uH"},
    {"Indutor 1mH", "Indutor", 0.9, 1.1, "mH"},

    // Fotorresistores
    {"LDR 5mm", "LDR", 5.0, 50.0, "kOhm (luz)"} // Valor varia com a luz
};

const int NUM_DATABASE_ENTRIES =
    sizeof(componentDatabase) / sizeof(componentDatabase[0]);

#endif // DATABASE_H
