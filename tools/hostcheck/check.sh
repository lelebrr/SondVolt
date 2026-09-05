#!/usr/bin/env bash
# ============================================================================
# Sondvolt - Verificacao de compilacao sem hardware
# ============================================================================
# Compila E LINKA todo o firmware num PC Linux/macOS usando stubs das
# bibliotecas do Arduino. Nao produz um binario utilizavel: serve para pegar
# erros de sintaxe, de tipo e simbolos faltando ANTES de gravar na placa.
#
# Pega o que o PlatformIO pegaria, exceto o que depende do compilador xtensa
# de verdade (tamanho de binario, uso de flash, comportamento em runtime).
#
# Uso:
#   ./check.sh          verifica as duas revisoes de hardware
#   ./check.sh 0        verifica so a Rev A
#   ./check.sh 1        verifica so a Rev B
# ============================================================================

set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
SRC="$ROOT/src"
STUBS="$HERE/stubs"
OBJ="$HERE/.obj"

FLAGS=(
  -std=gnu++17
  -I "$STUBS"
  -I "$SRC"
  -I "$SRC/hal"
  -I "$SRC/domain"
  -I "$SRC/services"
  -I "$SRC/ui"
  -I "$SRC/assets"
  -Wall -Wextra
  -Wno-unused-parameter
  -Wno-missing-field-initializers
  -DUSER_SETUP_LOADED=1
  -DILI9341_2_DRIVER=1
)

REVS=("${1:-0}" "${2:-}")
if [ $# -eq 0 ]; then REVS=(0 1 2); fi

OVERALL=0

for REV in "${REVS[@]}"; do
  [ -z "$REV" ] && continue

  echo "============================================================"
  case "$REV" in 0) NAME="Rev A (fiacao original)";; 1) NAME="Rev B (sem pinos compartilhados)";; 2) NAME="Rev C (placa Bancada)";; *) NAME="?";; esac
  echo " Verificando SONDVOLT_HW_REV=$REV - $NAME"
  echo "============================================================"

  rm -rf "$OBJ"; mkdir -p "$OBJ"
  FAILED=0
  WARNED=0

  for f in $(find "$SRC" -name "*.cpp" | sort); do
    name="$(basename "$f" .cpp)"
    out="$(g++ "${FLAGS[@]}" -DSONDVOLT_HW_REV="$REV" -c "$f" -o "$OBJ/$name.o" 2>&1)"
    rc=$?

    if [ $rc -ne 0 ]; then
      printf '  [ERRO]   %s\n' "$name"
      echo "$out" | head -20 | sed 's/^/           /'
      FAILED=1
    elif [ -n "$out" ]; then
      printf '  [AVISO]  %s\n' "$name"
      echo "$out" | head -10 | sed 's/^/           /'
      WARNED=1
    else
      printf '  [ok]     %s\n' "$name"
    fi
  done

  if [ $FAILED -ne 0 ]; then
    echo
    echo "  >> Compilacao FALHOU na Rev $REV"
    OVERALL=1
    continue
  fi

  # Link: pega funcao declarada e nunca definida, que o -fsyntax-only deixa passar.
  g++ "${FLAGS[@]}" -c "$STUBS/stub_impl.cpp" -o "$OBJ/zz_stub.o" 2>/dev/null

  if link_err="$(g++ -std=gnu++17 "$OBJ"/*.o -o "$OBJ/sondvolt_host" -lm 2>&1)"; then
    echo
    if [ $WARNED -eq 0 ]; then
      echo "  >> Rev $REV: compila e linka, 0 erros e 0 avisos"
    else
      echo "  >> Rev $REV: compila e linka, mas com avisos (veja acima)"
      OVERALL=1
    fi
  else
    echo
    echo "  >> Simbolos nao resolvidos na Rev $REV:"
    echo "$link_err" | grep -oP "undefined reference to \`\K[^']+" \
                     | sort -u | sed 's/^/     /'
    OVERALL=1
  fi
  echo
done

rm -rf "$OBJ"

if [ $OVERALL -eq 0 ]; then
  echo "RESULTADO: tudo limpo."
else
  echo "RESULTADO: ha problemas a resolver."
fi
exit $OVERALL
