// ============================================================================
// Sondvolt v5.0 - Expansor de Linhas de Controle (PCF8574)
// ============================================================================
// Arquivo : expander.h
//
// Por que este modulo existe
// --------------------------
// A CYD nao tem NENHUM GPIO livre. Somados display, touch, cartao, entradas
// analogicas, LEDs, buzzer e I2C, os 24 pinos utilizaveis do ESP32-WROOM
// estao todos ocupados. Todo recurso novo da Rev C - fonte de 12 V para
// Zener, acoplamento AC do medidor de ripple, atenuador do osciloscopio,
// shunt do tracador de curva - precisa de uma linha de controle, e nao ha
// pino de onde tirar.
//
// O PCF8574 resolve isso com 8 linhas a custo de zero GPIO: ele mora no
// barramento I2C que ja existe.
//
// Cuidado eletrico importante
// ---------------------------
// As saidas do PCF8574 sao DRENO ABERTO com um pull-up interno fraco
// (~100 uA). Elas puxam bem para baixo, mas sao fracas para cima. Portanto:
//   - Acionar transistor/MOSFET: OK
//   - Alimentar rele ou LED direto: NAO, use um transistor
//   - Entrada logica de outro CI: OK, com pull-up externo de 10k
//
// Se o expansor nao estiver instalado, todas as funcoes devolvem false e os
// recursos que dependem dele ficam desativados na interface - nunca
// simulados.
// ============================================================================

#ifndef EXPANDER_H
#define EXPANDER_H

#include <Arduino.h>
#include "pins.h"

// ----------------------------------------------------------------------------
// Linhas nomeadas (os numeros vem de pins.h)
// ----------------------------------------------------------------------------
enum ExpanderLine {
    EXP_LINE_BOOST      = EXP_BOOST_ENABLE,
    EXP_LINE_RIPPLE     = EXP_RIPPLE_COUPLE,
    EXP_LINE_SIGGEN     = EXP_SIGGEN_OUTPUT,
    EXP_LINE_ATTEN      = EXP_SCOPE_ATTEN,
    EXP_LINE_CURVE      = EXP_CURVE_SENSE,
    EXP_LINE_ISOLATE    = EXP_PROBE_ISOLATE,
    EXP_LINE_AUX1       = EXP_AUX_1,
    EXP_LINE_AUX2       = EXP_AUX_2
};

// ============================================================================
// CICLO DE VIDA
// ============================================================================

// Procura o PCF8574 no barramento e zera todas as linhas.
// Devolve false se o chip nao responder - o firmware segue funcionando, com
// os recursos da placa de expansao desativados.
bool expander_init();

// Verdadeiro se o expansor respondeu no boot.
bool expander_present();

// Refaz a deteccao (util depois de plugar a placa a quente).
bool expander_redetect();

// ============================================================================
// CONTROLE
// ============================================================================

// Liga ou desliga uma linha. Devolve false se o expansor estiver ausente ou
// se a transacao I2C falhar.
bool expander_write(ExpanderLine line, bool level);

// Estado atual de uma linha, lido do cache local (nao gera trafego I2C).
bool expander_read(ExpanderLine line);

// Escreve as 8 linhas de uma vez. Mais eficiente quando varias mudam juntas.
bool expander_write_all(uint8_t mask);

// Byte de estado atual.
uint8_t expander_state();

// Desliga tudo. Chamado ao sair de qualquer modo que use a placa, para nao
// deixar o boost de 12 V ligado por engano.
void expander_all_off();

// ============================================================================
// ATALHOS DE ALTO NIVEL
// ============================================================================
// Cada um destes encapsula a sequencia correta - inclusive os tempos de
// acomodacao, que sao faceis de esquecer e dificeis de depurar.

// Liga a fonte de 12 V e espera estabilizar. Devolve false se nao houver
// expansor (sem ele nao ha como ligar o boost).
bool expander_boost_enable(bool on);

// Insere ou remove o capacitor de acoplamento da entrada de ripple.
bool expander_ripple_coupling(bool on);

// Conecta a saida do gerador de sinal na ponta 1.
bool expander_siggen_output(bool on);

// Seleciona o atenuador do osciloscopio: false = 1x, true = 10x.
bool expander_scope_attenuator(bool tenX);

// Insere o resistor shunt do tracador de curva.
bool expander_curve_sense(bool on);

// Isola as pontas de prova do circuito de medicao (protecao ao medir rede).
bool expander_probe_isolate(bool isolated);

// Nome legivel de uma linha, para a tela de diagnostico.
const char* expander_line_name(ExpanderLine line);

#endif // EXPANDER_H
