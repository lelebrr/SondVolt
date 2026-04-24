// ============================================================================
// Sondvolt v3.0 — Sistema de Menu e Navegação
// Descrição: Menu principal com grid cards e navegação fluida
// ============================================================================
#ifndef MENU_H
#define MENU_H

#include "config.h"
#include <Adafruit_GFX.h>

// ============================================================================
// ESTADO DA APLICAÇÃO (máquina de estados)
// ============================================================================
enum AppState {
    STATE_SPLASH           = 0,
    STATE_MENU              = 1,

    // Modo Component Tester
    STATE_MEASURE_RESISTOR  = 10,
    STATE_MEASURE_CAPACITOR = 11,
    STATE_MEASURE_DIODE     = 12,
    STATE_MEASURE_TRANSISTOR = 13,
    STATE_MEASURE_INDUCTOR   = 14,
    STATE_MEASURE_IC         = 15,
    STATE_MEASURE_GENERIC   = 19,

    // Modo Multímetro
    STATE_MULTIMETER        = 20,

    // Ferramentas
    STATE_THERMAL_PROBE     = 30,
    STATE_SCANNER           = 31,
    STATE_CALIBRATION       = 32,

    // Configurações e Info
    STATE_SETTINGS         = 40,
    STATE_ABOUT             = 41,
    STATE_HISTORY            = 42,
    STATE_STATS              = 43
};

// ============================================================================
// ESTRUTURAS DE DADOS DO MENU
// ============================================================================
// Item do grid de menu principal
typedef struct {
    const char* label;          // Texto do card
    AppState targetState;      // Estado ao selecionar
    uint8_t iconType;         // Tipo de ícone (usado em graphics)
    uint16_t color;           // Cor do card
    uint8_t flags;            // Flags extras
} MenuCard;

#define MENU_FLAG_NONE       0x00
#define MENU_FLAG_WARNING    0x01  // Mostra badge de alerta
#define MENU_FLAG_BADGE     0x02  // Mostra badge numérico

// Item de navegação (botões / retorno)
typedef struct {
    const char* label;
    uint8_t iconId;
} NavButton;

// ============================================================================
// MENU PRINCIPAL — CONFIGURAÇÃO DOS CARDS
// ============================================================================
extern const MenuCard MAIN_MENU_CARDS[];
extern const uint8_t MAIN_MENU_COUNT;

// ============================================================================
// MENU DE MEDIÇÃO — CONFIGURAÇÃO
// ============================================================================
extern const MenuCard MEASURE_MENU_CARDS[];
extern const uint8_t MEASURE_MENU_COUNT;

// ============================================================================
// MENU DE CONFIGURAÇÕES — ITENS
// ============================================================================
extern const MenuCard SETTINGS_MENU_CARDS[];
extern const uint8_t SETTINGS_MENU_COUNT;

// ============================================================================
// PROTÓTIPOS — MENU PRINCIPAL
// ============================================================================

// Inicialização do sistema de menu
void menu_init();

// Loop principal do menu (chamado no loop)
void menu_handle();

// Desenhar o menu principal
void menu_draw();

// Atualizar highlight do card selecionado
void menu_updateSelection(int index);

// Navegação entre cards
void menu_navigate(int direction);

// Selecionar card atual
void menu_select();

// Retornar ao menu anterior
void menu_back();

// Forçar redesenho do menu
void menu_refresh();

// ============================================================================
// PROTÓTIPOS — SUB-MENUS
// ============================================================================

// Desenhar menu de medição
void measure_menu_draw();
void measure_menu_handle();

// Desenhar menu de configuração
void settings_draw();
void settings_handle();

// Desenhar tela sobre
void about_draw();
void about_handle();

// Desenhar histórico
void history_draw();
void history_handle();

// Desenhar estatísticas
void stats_draw();
void stats_handle();

// Desenhar scanner
void scanner_draw();
void scanner_handle();

// Calibração
void calibration_draw();
void calibration_handle();

// ============================================================================
// PROTÓTIPOS — COMPONENTES DA UI
// ============================================================================

// Desenha a barra de status superior (hora, temp, SD, etc.)
void draw_status_bar();

// Desenha o rodapé com botões de navegação
void draw_footer();

// Desenha card do menu (com borda, ícone, label e highlight)
void draw_menu_card(int x, int y, int w, int h,
                   const MenuCard* card, bool selected);

// Desenha card de resultado de medição
void draw_result_card(const char* component, const char* value,
                     const char* unit, const char* status, uint16_t statusColor);

// Desenha modal de confirmação
bool draw_confirm_modal(const char* title, const char* message);

// Desenha seletor de valor (slider)
int draw_slider_modal(const char* title, int min, int max, int value);

// ============================================================================
// ANIMAÇÕES E TRANSIÇÕES
// ============================================================================

// Transição de fade-out para próxima tela
void transition_out();

// Transição de fade-in vinda da tela anterior
void transition_in();

// Transição slide para a esquerda
void transition_slide_left();

// Transição slide para a direita
void transition_slide_right();

// ============================================================================
// TOUCH HANDLING NO MENU
// ============================================================================

// Processa toque e retorna índice do card selecionado (-1 = nenhum)
int menu_process_touch();

// Processa toque no menu de medição
int measure_process_touch();

// Processa toque no menu de configuração
int settings_process_touch();

#endif // MENU_H