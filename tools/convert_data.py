#!/usr/bin/env python3
# pyre-ignore-all-errors
# Script para converter dados de componentes para formato C++

def convert_to_cplusplus():
    # Ler o arquivo de dados
    with open('all_components_data.txt', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # Filtrar linhas que começam com // (comentários)
    data_lines = [line.strip() for line in lines if not line.strip().startswith('//')]
    
    # Gerar código C++
    cpp_parts = ["""#include "database.h"

// BJT NPN - 100 componentes com valores reais
const ComponentDB bjt_npn_db[] PROGMEM = {
"""]
    
    for i, line in enumerate(data_lines):
        if line:
            parts = line.split(',')
            if len(parts) >= 10:
                name = parts[0]
                category = parts[1]
                value1 = parts[2]
                min1 = parts[3]
                max1 = parts[4]
                value2 = parts[5]
                pin1 = parts[6]
                pin2 = parts[7]
                pin3 = parts[8]
                description = parts[9]
                common_use = parts[10] if len(parts) > 10 else ""
                esr = parts[11] if len(parts) > 11 else "0"
                
                cpp_parts.append(f'    {{{name}, {category}, {value1}, {min1}, {max1}, {value2}, {{{pin1}, {pin2}, {pin3}}}, "{description}", "{common_use}", {esr}}},\n')
    
    cpp_parts.append("};\n\n")
    cpp_parts.append("""const uint16_t bjt_npn_count = sizeof(bjt_npn_db) / sizeof(bjt_npn_db[0]);

ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1, uint16_t measured_value2, uint16_t measured_esr) {
    ComponentDB bestMatch = {"", 0, 0, 0, 0, 0, {0xFF, 0xFF, 0xFF}, "", "", 0};
    uint16_t smallest_diff = 0xFFFF;

    for (uint16_t i = 0; i < bjt_npn_count; i++) {
        ComponentDB current;
        memcpy_P(&current, &bjt_npn_db[i], sizeof(ComponentDB));
        
        if (current.category == category) {
            bool val1_match = (measured_value1 >= current.min1 && measured_value1 <= current.max1);
            bool val2_match = true;
            
            if (current.value2 > 0 && measured_value2 > 0) {
                uint16_t margin = current.value2 / 5;
                if (measured_value2 < (current.value2 - margin) || measured_value2 > (current.value2 + margin)) {
                    val2_match = false;
                }
            }

            if (val1_match && val2_match) {
                uint16_t diff = abs((int)measured_value1 - (int)current.value1);
                if (diff < smallest_diff) {
                    smallest_diff = diff;
                    bestMatch = current;
                }
            }
        }
    }
    return bestMatch;
}

void printComponentInfo(const ComponentDB &comp, uint16_t measured, uint16_t esr) {
    Serial.print(F("Nome: ")); Serial.println(comp.name);
    Serial.print(F("Desc: ")); Serial.println(comp.description);
    Serial.print(F("Uso: ")); Serial.println(comp.common_use);
}

const char* getCategoryName(uint8_t category) {
    switch (category) {
        case 1: return "BJT NPN"; case 2: return "BJT PNP";
        case 3: return "MOSFET N"; case 4: return "MOSFET P";
        case 5: return "Diodo"; case 6: return "Zener";
        case 7: return "Schottky"; case 8: return "LED";
        case 9: return "Capacitor"; case 10: return "Resistor";
        case 11: return "Indutor"; case 12: return "Opto";
        default: return "Outro";
    }
}
""")
    
    cpp_code = "".join(cpp_parts)
    
    # Escrever o arquivo C++
    with open('database.cpp', 'w', encoding='utf-8') as f:
        f.write(cpp_code)
    
    print("Arquivo database.cpp gerado com sucesso!")

if __name__ == "__main__":
    convert_to_cplusplus()