// ============================================================================
// Sondvolt v5.0 - Pareamento e Classificacao de Componentes
// ============================================================================
// Arquivo : sorting.h
//
// Para que serve
// --------------
// Em amplificador de audio, fonte simetrica e ponte de medicao, o que
// importa nao e o valor absoluto do componente e sim quanto dois deles se
// parecem. Um par de transistores com hFE de 180 e 182 funciona melhor que
// um par de 200 e 260, mesmo o segundo estando "mais perto do nominal".
//
// Este modulo mede um lote, guarda os resultados e mostra quais pecas casam
// entre si dentro da tolerancia pedida.
//
// Custo de memoria: 40 pecas x 20 bytes = 800 bytes. Cabe folgado.
// ============================================================================

#ifndef SORTING_H
#define SORTING_H

#include <Arduino.h>
#include "types.h"
#include "analysis.h"

#define SORT_MAX_ITEMS   40
#define SORT_MAX_PAIRS   20

// ----------------------------------------------------------------------------
// Uma peca medida do lote
// ----------------------------------------------------------------------------
struct SortItem {
    uint8_t       id;          // numero sequencial mostrado ao usuario
    float         value;       // grandeza principal (hFE, ohms, farads...)
    ComponentType type;
    ComponentStatus status;
    bool          used;        // ja faz parte de um par formado
    bool          valid;
};

// ----------------------------------------------------------------------------
// Um par casado
// ----------------------------------------------------------------------------
struct SortPair {
    uint8_t idA;
    uint8_t idB;
    float   valueA;
    float   valueB;
    float   deviationPct;      // diferenca entre as duas, em porcentagem
    bool    valid;
};

// ----------------------------------------------------------------------------
// Resumo estatistico do lote
// ----------------------------------------------------------------------------
struct SortStats {
    uint8_t count;
    float   minValue;
    float   maxValue;
    float   average;
    float   spreadPct;         // dispersao do lote
    float   median;
    uint8_t pairsFound;
    ComponentType dominantType;
};

// ============================================================================
// SESSAO DE MEDICAO
// ============================================================================

// Limpa o lote e comeca uma sessao nova.
void sorting_begin(float tolerancePercent);

// Mede a peca conectada e a adiciona ao lote.
// Devolve o indice atribuido, ou 0xFF se o lote estiver cheio ou a leitura
// nao for utilizavel.
uint8_t sorting_add_current();

// Adiciona um valor ja medido (para quem quer alimentar de outra tela).
uint8_t sorting_add_value(float value, ComponentType type,
                          ComponentStatus status);

// Remove a ultima peca adicionada (o usuario errou o contato).
bool sorting_remove_last();

// Esvazia o lote.
void sorting_clear();

// Quantas pecas ja foram medidas.
uint8_t sorting_count();

// Peca por indice.
const SortItem* sorting_at(uint8_t index);

// ============================================================================
// PAREAMENTO
// ============================================================================

// Forma os pares dentro da tolerancia atual. Usa o algoritmo guloso sobre a
// lista ordenada: emparelha sempre os dois vizinhos mais proximos ainda
// livres, o que da o melhor conjunto de pares na pratica e e barato.
// Devolve quantos pares formou.
uint8_t sorting_compute_pairs();

uint8_t sorting_pair_count();
const SortPair* sorting_pair_at(uint8_t index);

// Tolerancia usada no pareamento.
void  sorting_set_tolerance(float percent);
float sorting_get_tolerance();

// Pecas que sobraram sem par.
uint8_t sorting_unpaired_count();

// ============================================================================
// ESTATISTICAS
// ============================================================================

SortStats sorting_stats();

// Texto curto avaliando a qualidade do lote, para a tela.
const char* sorting_quality_text();

// ============================================================================
// EXPORTACAO
// ============================================================================

// Grava o lote e os pares num CSV no cartao.
bool sorting_export(const char* name);

#endif // SORTING_H
