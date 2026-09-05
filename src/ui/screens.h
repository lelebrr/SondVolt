// ============================================================================
// Sondvolt v5.0 - Telas dos Instrumentos de Bancada
// ============================================================================
// Arquivo : screens.h
// Objetivo: Manter as telas novas fora de ui.cpp, que ja passava de 1500
//           linhas. Cada funcao aqui desenha uma tela inteira e trata os
//           toques dela.
//
// Contrato de todas as telas
// --------------------------
//   screen_X_enter()  - chamada uma vez ao entrar; aloca o que for preciso
//   screen_X_draw()   - redesenha; chamada periodicamente pela UI
//   screen_X_touch()  - trata um toque; devolve true se consumiu o evento
//   screen_X_exit()   - chamada ao sair; OBRIGATORIA para liberar hardware
//
// A funcao de saida nao e opcional: o osciloscopio monopoliza o ADC e o
// gerador de sinal segura o pino de excitacao. Esquecer de sair deixa o
// resto do aparelho sem conseguir medir nada.
// ============================================================================

#ifndef SCREENS_H
#define SCREENS_H

#include <Arduino.h>
#include "globals.h"

// ============================================================================
// OSCILOSCOPIO
// ============================================================================
void screen_scope_enter();
void screen_scope_draw();
bool screen_scope_touch(uint16_t x, uint16_t y);
void screen_scope_exit();

// ============================================================================
// TRACADOR DE CURVA I-V
// ============================================================================
void screen_curve_enter();
void screen_curve_draw();
bool screen_curve_touch(uint16_t x, uint16_t y);
void screen_curve_exit();

// ============================================================================
// MEDIDOR DE RIPPLE
// ============================================================================
void screen_ripple_enter();
void screen_ripple_draw();
bool screen_ripple_touch(uint16_t x, uint16_t y);
void screen_ripple_exit();

// ============================================================================
// GERADOR DE SINAL
// ============================================================================
void screen_siggen_enter();
void screen_siggen_draw();
bool screen_siggen_touch(uint16_t x, uint16_t y);
void screen_siggen_exit();

// ============================================================================
// TESTE DE ZENER
// ============================================================================
void screen_zener_enter();
void screen_zener_draw();
bool screen_zener_touch(uint16_t x, uint16_t y);
void screen_zener_exit();

// ============================================================================
// TRABALHOS
// ============================================================================
void screen_jobs_enter();
void screen_jobs_draw();
bool screen_jobs_touch(uint16_t x, uint16_t y);
void screen_jobs_exit();

// ============================================================================
// PAREAMENTO
// ============================================================================
void screen_sorting_enter();
void screen_sorting_draw();
bool screen_sorting_touch(uint16_t x, uint16_t y);
void screen_sorting_exit();

// ============================================================================
// REDE
// ============================================================================
void screen_network_enter();
void screen_network_draw();
bool screen_network_touch(uint16_t x, uint16_t y);
void screen_network_exit();

// ============================================================================
// CAMERA TERMICA
// ============================================================================
void screen_thermalcam_enter();
void screen_thermalcam_draw();
bool screen_thermalcam_touch(uint16_t x, uint16_t y);
void screen_thermalcam_exit();

// ============================================================================
// DESPACHO
// ============================================================================
// Chamadas por ui.cpp. Devolvem false se o estado nao for de uma tela deste
// modulo, deixando a UI seguir com o tratamento antigo.

bool screens_handles(AppState state);
void screens_enter(AppState state);
void screens_draw(AppState state);
bool screens_touch(AppState state, uint16_t x, uint16_t y);
void screens_exit(AppState state);

#endif // SCREENS_H
