#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Ferramenta de Geração de Database Interna para Component Tester PRO v3.0 (ESP32 CYD)
Gera um arquivo C++ com um array estático de componentes como fallback.
"""

import sys

def generate_db():
    # Caminho do arquivo de dados (CSV)
    data_file = 'sd_files/COMPBD.CSV'
    output_file = 'src/database_internal.cpp'
    
    try:
        with open(data_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Erro: Arquivo {data_file} não encontrado.")
        return

    cpp_header = """// ============================================================================
// Component Tester PRO v3.0 — Database Interna de Fallback (CYD Edition)
// ============================================================================
// Gerado automaticamente via tools/generate_db.py
// ============================================================================
#include "database.h"
#include <Arduino.h>

// No ESP32, não precisamos usar PROGMEM obrigatoriamente, 
// pois a Flash é mapeada diretamente no espaço de endereçamento.
const ComponentDB internal_db[] = {
"""

    cpp_footer = """};

const uint16_t internal_db_count = sizeof(internal_db) / sizeof(internal_db[0]);
"""

    components = []
    for line in lines:
        line = line.strip()
        if not line or line.startswith('//'):
            continue
            
        parts = line.split(',')
        if len(parts) >= 12:
            name = parts[0]
            cat = parts[1]
            v1 = parts[2]
            m1 = parts[3]
            ma1 = parts[4]
            v2 = parts[5]
            p1 = parts[6]
            p2 = parts[7]
            p3 = parts[8]
            desc = parts[9]
            use = parts[10]
            esr = parts[11]
            
            # Formatar para C++
            # struct ComponentDB { char name[20]; uint8_t cat; uint16_t v1, min1, max1, v2; uint8_t pin[3]; char desc[50]; char use[50]; uint8_t esr; }
            comp_line = f'    {{"{name[:19]}", {cat}, {v1}, {m1}, {ma1}, {v2}, {{{p1}, {p2}, {p3}}}, "{desc[:49]}", "{use[:49]}", {esr}}},'
            components.append(comp_line)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(cpp_header)
        f.write("\n".join(components))
        f.write("\n" + cpp_footer)
    
    print(f"Sucesso! Geradas {len(components)} entradas em {output_file}")

if __name__ == "__main__":
    generate_db()