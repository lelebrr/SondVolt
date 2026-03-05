#!/usr/bin/env python3
# Script para contar componentes no arquivo

def count_components():
    # Ler o arquivo de dados
    with open('all_components_data.txt', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # Filtrar linhas que começam com // (comentários)
    data_lines = [line.strip() for line in lines if not line.strip().startswith('//')]
    
    print(f"Total de componentes: {len(data_lines)}")
    
    # Contar por categoria
    categories = {}
    for line in data_lines:
        if line:
            parts = line.split(',')
            if len(parts) >= 2:
                category = parts[1]
                categories[category] = categories.get(category, 0) + 1
    
    print("Por categoria:")
    for cat, count in sorted(categories.items()):
        print(f"  Categoria {cat}: {count} componentes")

if __name__ == "__main__":
    count_components()