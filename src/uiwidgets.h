// ============================================================================
// Sondvolt v4.0 - Componentes Visuais Reutilizaveis
// ============================================================================
// Arquivo : uiwidgets.h
// Objetivo: Pecas de interface que varias telas usam, desenhadas com um
//           visual consistente: notificacoes, medidores, graficos, teclado,
//           dialogos e o desenho do codigo de cores de resistor.
//
// Todas as funcoes de desenho assumem que o chamador NAO segura o mutex do
// display; elas mesmas fazem LOCK_TFT/UNLOCK_TFT quando precisam.
// ============================================================================

#ifndef UIWIDGETS_H
#define UIWIDGETS_H

#include <Arduino.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// Severidade de uma notificacao
// ----------------------------------------------------------------------------
enum ToastLevel {
    TOAST_INFO = 0,
    TOAST_SUCCESS,
    TOAST_WARNING,
    TOAST_ERROR
};

// ============================================================================
// 1. NOTIFICACOES (toast)
// ============================================================================
// Mensagem curta que aparece no rodape e some sozinha. Nao bloqueia nada:
// quem desenha e o widget_toast_render() chamado pelo laco da interface.

void widget_toast(const char* message, ToastLevel level = TOAST_INFO,
                  uint16_t durationMs = TIME_TOAST_DEFAULT);
void widget_toast_render();     // chamar a cada ciclo da UI
bool widget_toast_active();
void widget_toast_clear();

// ============================================================================
// 2. BARRA DE PROGRESSO
// ============================================================================
void widget_progress_bar(int16_t x, int16_t y, int16_t w, int16_t h,
                         uint8_t percent, uint16_t color);

// Barra com rotulo e porcentagem escrita ao lado.
void widget_progress_labeled(int16_t x, int16_t y, int16_t w,
                             const char* label, uint8_t percent,
                             uint16_t color);

// ============================================================================
// 3. GRAFICO EM TEMPO REAL
// ============================================================================
// Buffer circular de amostras desenhado como linha. Util para acompanhar
// tensao, temperatura ou corrente ao longo do tempo.

#define GRAPH_MAX_POINTS 120

struct GraphBuffer {
    float    points[GRAPH_MAX_POINTS];
    uint8_t  count;
    uint8_t  head;
    float    minValue;
    float    maxValue;
    bool     autoScale;
};

void  widget_graph_init(GraphBuffer* g, bool autoScale = true);
void  widget_graph_push(GraphBuffer* g, float value);
void  widget_graph_set_range(GraphBuffer* g, float minV, float maxV);
void  widget_graph_draw(GraphBuffer* g, int16_t x, int16_t y,
                        int16_t w, int16_t h, uint16_t color,
                        const char* unit = nullptr);
float widget_graph_average(const GraphBuffer* g);
float widget_graph_min(const GraphBuffer* g);
float widget_graph_max(const GraphBuffer* g);

// ============================================================================
// 4. HOLD / MIN / MAX
// ============================================================================
// Acumulador estatistico usado pelo multimetro. Separado do grafico porque
// precisa sobreviver a troca de tela.

struct MeasureTracker {
    float   current;
    float   minimum;
    float   maximum;
    float   average;
    double  sum;
    uint32_t samples;
    bool    hold;          // congela a leitura na tela
    float   heldValue;
};

void  tracker_reset(MeasureTracker* t);
void  tracker_push(MeasureTracker* t, float value);
void  tracker_toggle_hold(MeasureTracker* t);
float tracker_display_value(const MeasureTracker* t);
void  tracker_draw_stats(const MeasureTracker* t, int16_t x, int16_t y,
                         const char* unit);

// ============================================================================
// 5. MEDIDOR ANALOGICO (bargraph)
// ============================================================================
// Barra horizontal com zonas verde/amarela/vermelha, no estilo dos
// multimetros de bancada.

void widget_bargraph(int16_t x, int16_t y, int16_t w, int16_t h,
                     float value, float minV, float maxV,
                     float warnAt, float dangerAt);

// Arco de medidor no estilo velocimetro.
void widget_gauge(int16_t cx, int16_t cy, int16_t radius,
                  float value, float minV, float maxV, uint16_t color);

// ============================================================================
// 6. DIALOGOS
// ============================================================================
// Estes bloqueiam ate o usuario responder ou o tempo esgotar. Devem ser
// chamados apenas a partir da tarefa de interface.

// Devolve true se o usuario confirmou.
bool widget_confirm(const char* title, const char* message,
                    const char* okLabel = "CONFIRMAR",
                    const char* cancelLabel = "CANCELAR",
                    uint32_t timeoutMs = TIME_CONFIRM_TIMEOUT);

// Tela de erro amigavel com codigo, causa provavel e o que fazer.
void widget_error_screen(const char* title, const char* cause,
                         const char* whatToDo, uint16_t errorCode = 0);

// Aviso modal simples com um botao de OK.
void widget_alert(const char* title, const char* message);

// ============================================================================
// 7. TECLADO NA TELA
// ============================================================================
// Entrada de texto para nomear trabalhos e buscar componentes.
// Devolve true se o usuario confirmou; escreve o resultado em out.

bool widget_keyboard(const char* title, char* out, size_t outLen,
                     bool numericOnly = false);

// ============================================================================
// 8. LISTAS COM ROLAGEM
// ============================================================================

struct ScrollList {
    uint16_t itemCount;
    uint16_t selected;
    uint16_t topIndex;
    uint8_t  visibleRows;
    int16_t  x, y, w, rowHeight;
};

void widget_list_init(ScrollList* list, int16_t x, int16_t y, int16_t w,
                      uint8_t visibleRows, int16_t rowHeight,
                      uint16_t itemCount);
void widget_list_draw(ScrollList* list,
                      const char* (*itemText)(uint16_t index),
                      uint16_t (*itemColor)(uint16_t index) = nullptr);
bool widget_list_handle_touch(ScrollList* list, int16_t tx, int16_t ty,
                              uint16_t* activatedIndex);
void widget_list_scroll(ScrollList* list, int8_t delta);

// ============================================================================
// 9. CODIGO DE CORES DE RESISTOR
// ============================================================================
// Desenha o corpo bege do resistor com as quatro faixas coloridas
// correspondentes ao valor medido.

void widget_resistor_bands(int16_t x, int16_t y, int16_t w, int16_t h,
                           float ohms);

// ============================================================================
// 10. RENDERIZACAO SEM FLICKER (sprite)
// ============================================================================
// O painel do valor principal e redesenhado duas vezes por segundo. Desenhar
// direto na tela significa apagar e repintar, e o olho enxerga isso como um
// piscar constante - cansativo em uso prolongado.
//
// A solucao e montar o quadro num sprite em RAM e envia-lo de uma vez. Um
// sprite de 225x105 pixels em 16 bits ocupa cerca de 47 KB, o que caberia,
// mas deixaria pouca folga com o WiFi ligado. Usamos apenas a faixa do
// numero: 225x48, ou 21 KB. O ganho visual e quase o mesmo pelo terco do
// custo de memoria.

// Reserva o sprite. Devolve false se nao houver RAM suficiente - nesse caso
// o desenho cai automaticamente para o modo direto, sem quebrar nada.
bool widget_sprite_begin();

// Libera a memoria do sprite.
void widget_sprite_end();

// Verdadeiro se o sprite esta disponivel.
bool widget_sprite_ready();

// Desenha o valor principal com fonte adaptativa, sem flicker.
void widget_value_sprite(int16_t x, int16_t y, int16_t w, int16_t h,
                         const char* value, const char* unit,
                         uint16_t color, uint16_t background);

// ============================================================================
// 11. CABECALHOS E CARTOES
// ============================================================================

// Cartao com titulo pequeno em cima e valor grande embaixo.
void widget_value_card(int16_t x, int16_t y, int16_t w, int16_t h,
                       const char* label, const char* value,
                       const char* unit, uint16_t accentColor);

// Faixa de status colorida com icone textual.
void widget_status_chip(int16_t x, int16_t y, const char* text,
                        uint16_t color);

// Linha divisoria com titulo de secao.
void widget_section_divider(int16_t x, int16_t y, int16_t w, const char* title);

// Indicador circular de atividade (spinner). step avanca a cada chamada.
void widget_spinner(int16_t cx, int16_t cy, int16_t radius, uint8_t step,
                    uint16_t color);

#endif // UIWIDGETS_H
