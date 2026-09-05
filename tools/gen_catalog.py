#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Gerador do catálogo interno de componentes do Sondvolt.

POR QUE ESTE ARQUIVO SUBSTITUI generate_db.py
---------------------------------------------
O gerador anterior tinha quatro defeitos que o tornavam inútil:

1. Emitia `const ComponentDB internal_db[]`. Esse tipo NÃO EXISTE em
   database.h, que declara `ComponentInfo`. O arquivo gerado nunca compilou.
2. Não escapava aspas nem barras invertidas. Uma descrição com `"` gerava
   C++ inválido — e, num gerador que lê arquivo externo, isso é injeção de
   código, não só um bug de sintaxe.
3. Usava `line.split(',')`, que quebra em qualquer campo CSV com vírgula
   dentro de aspas.
4. Em FileNotFoundError fazia `return` e o processo saía com código 0,
   então um build automatizado achava que tinha dado certo.

Uso:
    python3 tools/gen_catalog.py [entrada.csv] [saida.h]

Sem argumentos, lê `sd_files/sdcard/COMPBD.CSV` e escreve
`src/assets/catalog_generated.h`.

QUANDO USAR
-----------
O catálogo que o firmware realmente usa é o `kCatalog` escrito à mão em
`src/domain/database.cpp`: 53 componentes com parâmetros conferidos em folha
de dados. Ele é curado de propósito.

O `COMPBD.CSV` que acompanha o projeto é majoritariamente SINTÉTICO — códigos
como 2N2000, 2N2001, 2N2002 gerados em massa, que não correspondem a peça
nenhuma. Gerar um catálogo em flash a partir dele só gastaria espaço com
ruído, e por isso a saída deste script NÃO entra no build por padrão.

Use esta ferramenta quando você tiver um CSV próprio, com peças de verdade,
e quiser que elas fiquem disponíveis sem cartão SD. Depois de gerar, inclua
`catalog_generated.h` em `database.cpp` e concatene com `kCatalog`.
"""

from __future__ import annotations

import csv
import sys
from pathlib import Path

# ----------------------------------------------------------------------------
# Mapeamento dos códigos de tipo do CSV para o enum ComponentType
# ----------------------------------------------------------------------------
CSV_TYPE_TO_ENUM = {
    1:  "COMP_TRANSISTOR_NPN",
    2:  "COMP_TRANSISTOR_PNP",
    3:  "COMP_MOSFET_N",
    4:  "COMP_MOSFET_P",
    5:  "COMP_DIODE",
    6:  "COMP_ZENER",
    7:  "COMP_LED",
    8:  "COMP_INDUCTOR",
    9:  "COMP_CAPACITOR",
    10: "COMP_RESISTOR",
    11: "COMP_IC",
}

# Unidade principal de cada tipo, para a interface saber o que escrever.
TYPE_UNIT = {
    "COMP_TRANSISTOR_NPN": "hFE",
    "COMP_TRANSISTOR_PNP": "hFE",
    "COMP_MOSFET_N":       "Ohm",
    "COMP_MOSFET_P":       "Ohm",
    "COMP_DIODE":          "V",
    "COMP_ZENER":          "V",
    "COMP_LED":            "V",
    "COMP_INDUCTOR":       "H",
    "COMP_CAPACITOR":      "F",
    "COMP_RESISTOR":       "Ohm",
    "COMP_IC":             "V",
}

# Limite do catálogo em flash. Acima disso o custo deixa de compensar: o
# arquivo COMPBD.CSV no cartão já cobre os 5.726 registros por varredura.
MAX_ENTRIES = 120


def escape_c_string(text: str, max_len: int) -> str:
    """Torna um texto seguro para virar literal C.

    Escapa barra invertida e aspas, remove caracteres de controle, e corta no
    comprimento máximo. Sem isto, um campo com aspas geraria código inválido —
    ou pior, código válido que o autor do CSV escolheu.
    """
    if text is None:
        return ""

    cleaned = []
    for ch in str(text)[:max_len]:
        if ch in ('"', "\\"):
            cleaned.append("\\" + ch)
        elif ch == "\n" or ch == "\r" or ch == "\t":
            cleaned.append(" ")
        elif ord(ch) < 32 or ord(ch) > 126:
            # O firmware desenha com uma fonte ASCII de 5x7: acento não
            # renderiza. Melhor traduzir do que mostrar caractere quebrado.
            cleaned.append(TRANSLITERATE.get(ch, "?"))
        else:
            cleaned.append(ch)
    return "".join(cleaned)


TRANSLITERATE = {
    "á": "a", "à": "a", "ã": "a", "â": "a", "ä": "a",
    "é": "e", "ê": "e", "è": "e",
    "í": "i", "î": "i",
    "ó": "o", "õ": "o", "ô": "o", "ö": "o",
    "ú": "u", "û": "u", "ü": "u",
    "ç": "c", "ñ": "n",
    "Á": "A", "À": "A", "Ã": "A", "Â": "A",
    "É": "E", "Ê": "E",
    "Í": "I", "Ó": "O", "Õ": "O", "Ô": "O",
    "Ú": "U", "Ç": "C",
    "Ω": "Ohm", "µ": "u", "°": "",
}


def c_float(value: float) -> str:
    """Formata um float como literal C valido.

    "%.6g" de 300.0 devolve "300", e "300f" nao compila: o sufixo f exige
    ponto decimal. Esta funcao garante que ele sempre exista.
    """
    text = f"{value:.6g}"
    if "e" in text or "E" in text:
        mantissa, _, expoente = text.partition("e")
        if "." not in mantissa:
            mantissa += ".0"
        return f"{mantissa}e{expoente}f"
    if "." not in text:
        text += ".0"
    return text + "f"


# Pinagem tipica por familia. O COMPBD.CSV nao traz esse campo - a versao
# anterior gravava ali a CATEGORIA por engano, e a tela mostrava "General"
# onde deveria aparecer "E-B-C".
PINOUT_BY_TYPE = {
    "COMP_TRANSISTOR_NPN": "E-B-C",
    "COMP_TRANSISTOR_PNP": "E-B-C",
    "COMP_MOSFET_N":       "G-D-S",
    "COMP_MOSFET_P":       "G-D-S",
    "COMP_DIODE":          "A-K",
    "COMP_ZENER":          "A-K",
    "COMP_LED":            "A-K",
    "COMP_INDUCTOR":       "1-2",
    "COMP_CAPACITOR":      "1-2",
    "COMP_RESISTOR":       "1-2",
    "COMP_IC":             "DIP",
}

# Prefixos de peca que existem de verdade no comercio. O CSV do cartao tem
# milhares de codigos sinteticos gerados em massa (2N1000, 2N1001, ...) que
# nao correspondem a componente nenhum; gravar isso em flash so gasta espaco.
KNOWN_PREFIXES = (
    "BC", "BD", "BF", "TIP", "2N2", "2N3", "2N4", "2N5", "2N7",
    "1N4", "1N5", "IRF", "IRL", "AO", "SS", "MPS", "S80", "BS",
    "LM", "NE", "TL", "UA", "CD", "SN", "74", "AMS", "MC", "BAT",
    "BZX", "1N7", "MJ", "STP", "FQP", "IRFZ",
)


def is_known_part(name: str) -> bool:
    upper = name.upper()
    return any(upper.startswith(p) for p in KNOWN_PREFIXES)


def parse_float(value: str, default: float = 0.0) -> float:
    try:
        return float(str(value).strip())
    except (TypeError, ValueError):
        return default


def parse_int(value: str, default: int = 0) -> int:
    try:
        return int(float(str(value).strip()))
    except (TypeError, ValueError):
        return default


def load_rows(path: Path) -> list[dict]:
    """Lê o CSV com o módulo `csv`, que respeita aspas e vírgulas internas."""
    rows: list[dict] = []
    seen_names: set[str] = set()

    with path.open("r", encoding="utf-8", errors="replace", newline="") as fh:
        reader = csv.reader(fh)
        for line_no, parts in enumerate(reader, start=1):
            if not parts or not parts[0].strip():
                continue
            if parts[0].lstrip().startswith("#"):
                continue
            if len(parts) < 6:
                print(f"  aviso: linha {line_no} com {len(parts)} campos, ignorada",
                      file=sys.stderr)
                continue

            name = parts[0].strip()
            if not name or name in seen_names:
                continue          # duplicata no CSV: fica com a primeira
            seen_names.add(name)

            csv_type = parse_int(parts[1])
            enum_type = CSV_TYPE_TO_ENUM.get(csv_type)
            if enum_type is None:
                continue          # tipo desconhecido: não inventa

            nominal = parse_float(parts[2])
            minimum = parse_float(parts[3], nominal * 0.8)
            maximum = parse_float(parts[4], nominal * 1.2)
            param2  = parse_float(parts[5]) if len(parts) > 5 else 0.0

            # O CSV guarda Vbe em milivolts; o firmware trabalha em volts.
            if enum_type in ("COMP_TRANSISTOR_NPN", "COMP_TRANSISTOR_PNP"):
                param2 = param2 / 1000.0

            description = parts[9] if len(parts) > 9 else ""

            rows.append({
                "name":    escape_c_string(name, 19),
                "type":    enum_type,
                "nominal": nominal,
                "min":     minimum,
                "max":     maximum,
                "param2":  param2,
                "unit":    TYPE_UNIT.get(enum_type, ""),
                "pinout":  PINOUT_BY_TYPE.get(enum_type, ""),
                "desc":    escape_c_string(description, 39),
            })

    return rows


def emit_header(rows: list[dict], out_path: Path, source: Path) -> None:
    lines = [
        "// " + "=" * 74,
        "// Sondvolt - Catalogo de componentes (GERADO AUTOMATICAMENTE)",
        "// " + "=" * 74,
        "//",
        "// NAO EDITE ESTE ARQUIVO A MAO.",
        "// Ele e produzido por tools/gen_catalog.py a partir de:",
        f"//     {source.as_posix()}",
        "//",
        "// Para regerar:",
        "//     python3 tools/gen_catalog.py",
        "//",
        f"// Entradas: {len(rows)}",
        "// " + "=" * 74,
        "",
        "#ifndef CATALOG_GENERATED_H",
        "#define CATALOG_GENERATED_H",
        "",
        '#include "database.h"',
        "",
        "// O tipo e ComponentInfo, o mesmo declarado em database.h. O gerador",
        "// anterior emitia um tipo chamado ComponentDB que nunca existiu, e por",
        "// isso o arquivo gerado nao compilava.",
        "static const ComponentInfo kCatalogGenerated[] = {",
    ]

    for r in rows:
        lines.append(
            '    {{ "{name}", {type}, {nominal}, {min}, {max}, {param2}, '
            '"{unit}", "{pinout}", "{desc}" }},'.format(
                name=r["name"], type=r["type"],
                nominal=c_float(r["nominal"]),
                min=c_float(r["min"]),
                max=c_float(r["max"]),
                param2=c_float(r["param2"]),
                unit=r["unit"], pinout=r["pinout"], desc=r["desc"])
        )

    lines += [
        "};",
        "",
        "static const uint16_t kCatalogGeneratedCount =",
        "    sizeof(kCatalogGenerated) / sizeof(kCatalogGenerated[0]);",
        "",
        "#endif // CATALOG_GENERATED_H",
        "",
    ]

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    root = Path(__file__).resolve().parent.parent

    src = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "sd_files/sdcard/COMPBD.CSV"
    dst = Path(sys.argv[2]) if len(sys.argv) > 2 else root / "src/assets/catalog_generated.h"

    if not src.is_file():
        # Sair com código de erro: um `return` silencioso fazia o build
        # automatizado achar que tinha dado certo.
        print(f"erro: arquivo de entrada nao encontrado: {src}", file=sys.stderr)
        return 1

    rows = load_rows(src)
    if not rows:
        print(f"erro: nenhuma linha valida em {src}", file=sys.stderr)
        return 1

    # Prioriza peças que existem no comércio. O CSV do cartão tem milhares de
    # códigos sintéticos (2N1000, 2N1001, ...) gerados em massa; gravá-los em
    # flash gasta espaço sem ajudar ninguém — eles continuam consultáveis por
    # varredura no cartão.
    known = [r for r in rows if is_known_part(r["name"])]
    other = [r for r in rows if not is_known_part(r["name"])]

    if known:
        print(f"  {len(known)} peças de código conhecido, "
              f"{len(other)} sintéticas descartadas do catálogo em flash")
        rows = sorted(known, key=lambda r: r["name"])
    else:
        rows = sorted(rows, key=lambda r: r["name"])

    if len(rows) > MAX_ENTRIES:
        print(f"  {len(rows)} entradas encontradas, mantendo as {MAX_ENTRIES} "
              f"mais relevantes (o resto continua no cartao SD)")
        rows = rows[:MAX_ENTRIES]

    emit_header(rows, dst, src.relative_to(root) if src.is_relative_to(root) else src)

    print(f"gerado {dst.relative_to(root)} com {len(rows)} componentes")
    return 0


if __name__ == "__main__":
    sys.exit(main())
