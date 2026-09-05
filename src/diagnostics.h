// ============================================================================
// Sondvolt v4.0 - Diagnostico, Autoteste e Persistencia de Configuracoes
// ============================================================================
// Arquivo : diagnostics.h
// Objetivo: Saber, a qualquer momento, se o aparelho esta saudavel - e
//           conseguir provar isso ao usuario numa tela.
//
// Cobre tres assuntos que andam juntos:
//   1. Autoteste de hardware executado no boot
//   2. Monitoramento continuo de memoria, pilha e tempo de execucao
//   3. Persistencia das configuracoes e estatisticas na NVS
// ============================================================================

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// Subsistemas verificados pelo autoteste
// ----------------------------------------------------------------------------
enum SelfTestItem {
    TEST_DISPLAY = 0,
    TEST_TOUCH,
    TEST_SDCARD,
    TEST_PROBES,
    TEST_THERMAL,
    TEST_INA219,
    TEST_ZMPT,
    TEST_BUZZER,
    TEST_LEDS,
    TEST_PSRAM,
    TEST_COUNT
};

enum SelfTestResult {
    TEST_RESULT_PENDING = 0,
    TEST_RESULT_PASS,
    TEST_RESULT_WARN,      // presente mas fora do esperado
    TEST_RESULT_FAIL,      // ausente ou com defeito
    TEST_RESULT_SKIPPED    // opcional e nao instalado
};

struct SelfTestEntry {
    SelfTestItem   item;
    SelfTestResult result;
    char           detail[32];
};

// ----------------------------------------------------------------------------
// Instantaneo da saude do sistema
// ----------------------------------------------------------------------------
struct SystemHealth {
    uint32_t freeHeap;
    uint32_t minFreeHeap;
    uint32_t totalHeap;
    uint8_t  heapUsedPct;
    uint32_t uptimeMs;
    uint32_t uiStackFree;        // palavras livres na pilha da tarefa de UI
    uint32_t measureStackFree;
    uint8_t  cpuFreqMhz;
    float    chipTempC;
    uint32_t loopCount;
    bool     heapWarning;        // heap abaixo do limite seguro
    bool     stackWarning;       // alguma pilha perto do fim
};

// ============================================================================
// 1. AUTOTESTE
// ============================================================================

// Roda a bateria completa. Se progressCb nao for nulo, e chamado a cada item
// com o progresso de 0 a 100 e uma mensagem curta para a tela de boot.
void diag_run_selftest(void (*progressCb)(uint8_t pct, const char* msg) = nullptr);

// Resultado de um item especifico.
const SelfTestEntry* diag_selftest_get(SelfTestItem item);

// Quantos itens falharam de fato (nao conta os pulados).
uint8_t diag_selftest_failures();

// Verdadeiro se o aparelho pode operar com seguranca.
bool diag_selftest_passed();

// Nome legivel de um item e de um resultado.
const char* diag_item_name(SelfTestItem item);
const char* diag_result_name(SelfTestResult result);
uint16_t    diag_result_color(SelfTestResult result);

// ============================================================================
// 2. MONITORAMENTO CONTINUO
// ============================================================================

// Prepara os contadores. Chamar uma vez no setup().
void diag_init();

// Atualiza o instantaneo. Barato o suficiente para rodar a cada ciclo.
void diag_update();

// Ultimo instantaneo coletado.
SystemHealth diag_get_health();

// Registra as tarefas para que a pilha delas possa ser monitorada.
void diag_register_task_ui(TaskHandle_t handle);
void diag_register_task_measure(TaskHandle_t handle);

// Alimenta o watchdog da tarefa atual.
void diag_feed_watchdog();

// Habilita o watchdog de hardware com o tempo limite indicado.
bool diag_watchdog_enable(uint32_t timeoutSeconds = 15);

// Texto de uma linha resumindo a saude, para a barra de status.
const char* diag_health_summary();

// ============================================================================
// 3. ESTATISTICAS DE USO
// ============================================================================

struct UsageStats {
    uint32_t totalMeasurements;
    uint32_t goodMeasurements;
    uint32_t badMeasurements;
    uint32_t bootCount;
    uint32_t totalUptimeMinutes;   // acumulado entre todos os boots
    uint32_t sdWrites;
    uint32_t safetyLockouts;
};

// Contabiliza uma medicao concluida.
void diag_count_measurement(ComponentStatus status);

// Contabiliza uma gravacao no cartao e um bloqueio de seguranca.
void diag_count_sd_write();
void diag_count_lockout();

// Estatisticas acumuladas.
UsageStats diag_get_usage();

// Zera os contadores (pede confirmacao na interface antes de chamar).
void diag_reset_usage();

// ============================================================================
// 4. PERSISTENCIA NA NVS
// ============================================================================

// Carrega DeviceSettings e as estatisticas da flash. Se nao houver nada
// gravado, aplica os padroes de fabrica.
bool settings_load();

// Grava as configuracoes atuais. Retorna false se a NVS recusar a escrita.
bool settings_save();

// Grava apenas as estatisticas (mais frequente, menos desgaste da flash).
bool settings_save_usage();

// Restaura os padroes de fabrica e grava.
void settings_factory_reset();

// Marca as configuracoes como alteradas. A gravacao real e adiada alguns
// segundos para nao escrever na flash a cada toque na tela.
void settings_mark_dirty();

// Grava se houver alteracao pendente e o tempo de espera ja tiver passado.
// Chamar periodicamente a partir da tarefa de medicao.
void settings_flush_if_needed();

#endif // DIAGNOSTICS_H
