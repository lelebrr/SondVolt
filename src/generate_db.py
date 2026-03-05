#!/usr/bin/env python3
# pyre-ignore-all-errors
# Script para gerar database.cpp a partir dos dados dos componentes

def convert_to_cplusplus():
    # Ler o arquivo de dados
    with open('all_components_data.txt', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # Filtrar linhas que começam com // (comentários)
    data_lines = [line.strip() for line in lines if not line.strip().startswith('//')]
    
    # Gerar código C++
    cpp_code = """#include "database.h"

// BJT NPN - 100 componentes com valores reais
const ComponentDB bjt_npn_db[] PROGMEM = {
"""
    
    import typing
    components: "typing.List[str]" = []
    for i, line in enumerate(data_lines):
        if line:
            parts = line.split(',')
            if len(parts) >= 10:
                name = parts[0]
                category = parts[1]
                value1 = int(float(parts[2]))
                min1 = int(float(parts[3]))
                max1 = int(float(parts[4]))
                value2 = int(float(parts[5]))
                pin1 = parts[6]
                pin2 = parts[7]
                pin3 = parts[8]
                description = parts[9]
                common_use = ",".join(parts[10:-1]).strip() if len(parts) > 10 else ""
                esr = parts[-1].strip() if len(parts) > 10 else "0"
                
                # Construir a linha do componente
                component_line = f'    {{"{name}", {category}, {value1}, {min1}, {max1}, {value2}, {{{pin1}, {pin2}, {pin3}}}, "{description}", "{common_use}", {esr}}},'
                components.append(component_line)
    
    # Adicionar todos os componentes
    cpp_code += "\n".join(components) + "\n};\n\n"
    
    cpp_code += """const uint16_t bjt_npn_count = 100;

// Funções de exemplo (implementação completa necessária)
ComponentDB findBestMatch(uint8_t category, uint16_t measured_value1, uint16_t measured_value2, uint16_t measured_esr) {
    ComponentDB empty = {"", 0, 0, 0, 0, 0, {0xFF, 0xFF, 0xFF}, "", "", 0};
    return empty;
}

void printComponentInfo(const ComponentDB &comp, uint16_t measured, uint16_t esr) {
    // Implementação necessária
}

const char* getCategoryName(uint8_t category) {
    // Implementação necessária
    return "";
}
"""
    
    # Escrever o arquivo C++
    with open('database.cpp', 'w', encoding='utf-8') as f:
        f.write(cpp_code)
    
    print("Arquivo database.cpp gerado com sucesso!")

if __name__ == "__main__":
    convert_to_cplusplus()