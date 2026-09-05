// ============================================================================
// Sondvolt v4.0 - Ponto de Entrada
// ============================================================================
// Arquivo : main.cpp
//
// O QUE ESTAVA FALTANDO AQUI
// --------------------------
// A versao anterior inicializava apenas cinco subsistemas. Nunca chamava:
//     buzzer_init()      -> o canal LEDC nunca era configurado, nenhum som
//                           saia da placa
//     buzzer_update()    -> um tom iniciado nunca era desligado
//     leds_init()        -> os pinos dos LEDs nunca viravam saida
//     leds_update()      -> os padroes de pisca-pisca nunca avancavam
//     db_init()          -> o banco de componentes nunca era carregado
//     calibration_init() -> os offsets gravados nunca eram lidos
//     settings_load()    -> as preferencias do usuario nunca eram restauradas
//     logger_write()     -> nenhuma medicao chegava ao cartao SD
//
// Ou seja: som, LEDs, banco de dados, calibracao, configuracoes e historico
// estavam todos escritos, todos corretos, e nenhum deles rodava.
//
// ORDEM DE INICIALIZACAO
// ----------------------
// A ordem abaixo nao e arbitraria. hal_init() precisa vir antes de qualquer
// coisa que toque em pinos; o display precisa existir antes da interface; o
// cartao precisa estar montado antes do banco de dados; e as tarefas so sao
// criadas quando todo o hardware ja esta em estado conhecido.
// ============================================================================

#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "hal.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "globals.h"

#include "ui.h"
#include "menu.h"
#include "buzzer.h"
#include "leds.h"
#include "logger.h"
#include "database.h"
#include "calibration.h"
#include "diagnostics.h"
#include "measurements.h"
#include "multimeter.h"
#include "thermal.h"
#include "safety.h"
#include "uiwidgets.h"
#include "expander.h"
#include "jobs.h"
#include "scope.h"
#include "thermalcam.h"
#include "sorting.h"
#include "netsvc.h"

// ============================================================================
// PARAMETROS DAS TAREFAS
// ============================================================================
namespace {

// A tarefa de interface roda a 50 Hz: rapido o bastante para o toque parecer
// instantaneo, devagar o bastante para nao monopolizar o barramento SPI.
constexpr TickType_t UI_TASK_PERIOD      = pdMS_TO_TICKS(20);
constexpr TickType_t MEASURE_TASK_PERIOD = pdMS_TO_TICKS(100);

// A pilha da interface e maior porque as telas usam buffers locais e
// chamadas recursivas de desenho.
constexpr uint32_t UI_TASK_STACK      = 6144;
constexpr uint32_t MEASURE_TASK_STACK = 4096;

constexpr UBaseType_t UI_TASK_PRIORITY      = 2;
constexpr UBaseType_t MEASURE_TASK_PRIORITY = 1;

// Fixar cada tarefa num nucleo evita que o escalonador as migre no meio de
// uma operacao sensivel a tempo. A interface fica no nucleo 1 (o mesmo do
// laco Arduino) e a medicao no nucleo 0, que e onde o stack de WiFi tambem
// roda - ambos sao trabalhos que toleram interrupcao, ao contrario do
// desenho da tela.
constexpr BaseType_t UI_TASK_CORE      = 1;
constexpr BaseType_t MEASURE_TASK_CORE = 0;

TaskHandle_t gTaskUI      = nullptr;
TaskHandle_t gTaskMeasure = nullptr;

} // namespace

// ============================================================================
// TAREFA DE INTERFACE
// ============================================================================
// Responsavel por tudo que o usuario ve e toca. Nunca faz medicao lenta.

void TaskUserInterface(void* pvParameters) {
    (void)pvParameters;

    for (;;) {
        uint16_t tx = 0, ty = 0;

        // A leitura do toque usa o barramento HSPI, separado do display, mas
        // o mutex e mantido durante o desenho para nao intercalar transacoes.
        if (hal_touch_read(&tx, &ty)) {
            ui_handle_touch(tx, ty);
        } else {
            ui_reset_touch_state();
        }

        LOCK_TFT();
        ui_update();
        widget_toast_render();
        UNLOCK_TFT();

        // Servicos leves que precisam de um pulso regular.
        buzzer_update();
        leds_update();

        vTaskDelay(UI_TASK_PERIOD);
    }
}

// ============================================================================
// TAREFA DE MEDICAO
// ============================================================================
// Faz o trabalho lento e potencialmente bloqueante, longe da interface.

void TaskMeasurement(void* pvParameters) {
    (void)pvParameters;

    uint32_t healthTick = 0;

    for (;;) {
        // --- Vigilancia eletrica, sempre ativa ------------------------------
        safety_update();

        // Fora do modo multimetro, tensao nas pontas e sempre acidente.
        if (currentAppState != STATE_MULTIMETER) {
            safety_detect_danger();
        }

        // --- Medicao conforme a tela ativa ----------------------------------
        switch (currentAppState) {
            case STATE_MEASURE_RESISTOR:
                lastResistance = measurements_get_raw_resistance();
                break;

            case STATE_MEASURE_CAPACITOR:
                lastCapacitance = measurements_get_raw_capacitance();
                break;

            case STATE_MEASURE_INDUCTOR:
                lastInductance = measurements_get_inductance();
                break;

            case STATE_MULTIMETER:
                multimeter_handle();
                break;

            case STATE_THERMAL_PROBE:
            case STATE_THERMAL_CAMERA:
                // Leitura assincrona: dispara a conversao e colhe depois,
                // sem travar a tarefa por 800 ms.
                if (thermal_update()) {
                    lastTemperature = thermal_get_last();
                }
                break;

            case STATE_SORTING:
                // O pareamento mede sob demanda, ao toque do usuario.
                break;

            case STATE_SCOPE:
            case STATE_CURVE_TRACER:
            case STATE_RIPPLE:
            case STATE_SIGGEN:
            case STATE_ZENER:
                // Estes instrumentos assumem o controle do ADC ou do
                // circuito de excitacao. Medir em paralelo corromperia a
                // captura, entao a tarefa de medicao fica de fora.
                break;

            case STATE_MEASURE_DIODE:
            case STATE_MEASURE_LED:
            case STATE_MEASURE_TRANSISTOR:
            case STATE_MEASURE_IC:
            case STATE_MEASURE_GENERIC:
            case STATE_SCANNER:
            case STATE_COMPARATOR:
                measurements_update();
                break;

            // Telas sem medicao: menus, ajustes, historico, ajuda.
            case STATE_SPLASH:
            case STATE_MENU:
            case STATE_SUBMENU_TEMP:
            case STATE_SUBMENU_MAIS:
            case STATE_SETTINGS:
            case STATE_ABOUT:
            case STATE_HISTORY:
            case STATE_STATS:
            case STATE_HELP:
            case STATE_CALIBRATION:
            case STATE_JOBS_LIST:
            case STATE_JOBS_NEW:
            case STATE_JOBS_DETAIL:
            case STATE_NETWORK:
            case STATE_OTA:
            case STATE_SUBMENU_INSTR:
            case STATE_SUBMENU_BANCADA:
                break;

            default:
                break;
        }

        // --- Manutencao periodica -------------------------------------------
        // A cada segundo: saude do sistema. Gravacao adiada de configuracoes.
        if ((++healthTick % 10) == 0) {
            diag_update();
        }
        settings_flush_if_needed();
        diag_feed_watchdog();

        // Servidor web e OTA. Barato quando nao ha nada pendente, e fica na
        // tarefa de medicao para nao concorrer com o desenho da tela.
        net_update();

        vTaskDelay(MEASURE_TASK_PERIOD);
    }
}

// ============================================================================
// SETUP
// ============================================================================

// Repassa o progresso do autoteste para a tela de boot.
static void boot_progress(uint8_t percent, const char* message) {
    ui_boot_progress(percent, message);
}

void setup() {
    Serial.begin(115200);
    delay(80);                    // deixa o monitor serial engatar
    LOG_SERIAL_F("");
    LOG_SERIAL_FMT("=== %s v%s (%s) ===\n", FW_NAME, FW_VERSION, FW_CODENAME);

    // ------------------------------------------------------------------
    // 0. Mutex do display - antes de qualquer coisa que possa desenhar
    // ------------------------------------------------------------------
    display_mutex_init();

    // ------------------------------------------------------------------
    // 1. Hardware de baixo nivel (ADC, LEDC, pinos, arbitragem)
    // ------------------------------------------------------------------
    hal_init();

    // ------------------------------------------------------------------
    // 2. Barramentos SPI
    // ------------------------------------------------------------------
    // O cartao SD tem pinos proprios na CYD; o touch usa HSPI dedicado.
    spiTFT_SD.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    touchSPI.begin(PIN_HSPI_SCLK, PIN_HSPI_MISO, PIN_HSPI_MOSI, PIN_TOUCH_CS);

    // ------------------------------------------------------------------
    // 3. Display
    // ------------------------------------------------------------------
    tft.init();
    tft.setRotation(SCREEN_ROTATION);
    tft.invertDisplay(false);     // cores reais, sem inversao global
    tft.setSwapBytes(true);       // byte order correto para bitmaps de 16 bits
    tft.fillScreen(TFT_BLACK);
    tftInitialized = true;

    touch.begin(touchSPI);
    touch.setRotation(SCREEN_ROTATION);

    // ------------------------------------------------------------------
    // 4. Preferencias do usuario (antes da UI, para o tema ja nascer certo)
    // ------------------------------------------------------------------
    settings_load();
    colors_update();

    // ------------------------------------------------------------------
    // 5. Avisos sonoros e visuais
    // ------------------------------------------------------------------
    buzzer_init();
    leds_init();

    // ------------------------------------------------------------------
    // 6. Armazenamento, banco de dados e trabalhos
    // ------------------------------------------------------------------
    sdCardError = !logger_init();
    db_init();
    jobs_init();

    // ------------------------------------------------------------------
    // 7. Instrumentacao
    // ------------------------------------------------------------------
    measurements_init();
    calibration_init();           // carrega os offsets das pontas
    thermal_init();
    multimeter_init(false);
    safety_init();

    // ------------------------------------------------------------------
    // 7b. Placa de expansao "Bancada" (Rev C)
    // ------------------------------------------------------------------
    // Cada item e opcional. O que nao estiver presente tem a funcao
    // desativada na interface, nunca simulada.
    expander_init();
    scope_init();
    tcam_init();
    net_init();

    // O sprite anti-flicker so e alocado se sobrar heap; caso contrario o
    // desenho cai para o modo direto sem quebrar nada.
    widget_sprite_begin();

    // ------------------------------------------------------------------
    // 8. Diagnostico e autoteste
    // ------------------------------------------------------------------
    diag_init();
    ui_boot_begin();              // logo + barra de progresso
    diag_run_selftest(boot_progress);

    // ------------------------------------------------------------------
    // 9. Interface
    // ------------------------------------------------------------------
    ui_init();

    // ------------------------------------------------------------------
    // 10. Tarefas
    // ------------------------------------------------------------------
    // O retorno importa: se uma tarefa nao for criada por falta de memoria, o
    // aparelho fica pela metade em silencio - sem interface ou sem medicao.
    BaseType_t okUI = xTaskCreatePinnedToCore(
        TaskUserInterface, "TaskUI", UI_TASK_STACK,
        nullptr, UI_TASK_PRIORITY, &gTaskUI, UI_TASK_CORE);

    BaseType_t okMeas = xTaskCreatePinnedToCore(
        TaskMeasurement, "TaskMeasure", MEASURE_TASK_STACK,
        nullptr, MEASURE_TASK_PRIORITY, &gTaskMeasure, MEASURE_TASK_CORE);

    if (okUI != pdPASS || okMeas != pdPASS) {
        LOG_SERIAL_FMT("[SYS] FALHA ao criar tarefas (UI=%d, MED=%d). "
                       "Heap livre: %lu\n",
                       (int)okUI, (int)okMeas,
                       (unsigned long)ESP.getFreeHeap());
        // Reiniciar e mais honesto que operar pela metade.
        delay(2000);
        ESP.restart();
    }

    diag_register_task_ui(gTaskUI);
    diag_register_task_measure(gTaskMeasure);

    // O watchdog so entra depois que tudo esta rodando, senao um autoteste
    // demorado seria interpretado como travamento.
    diag_watchdog_enable(20);

    // A rede so sobe se o usuario tiver habilitado. Ligar o WiFi sem
    // necessidade custa corrente, calor e tempo de boot.
    if (deviceSettings.wifiEnabled) {
        if (net_start() && net_is_connected()) {
            NetStatus ns = net_get_status();
            LOG_SERIAL_FMT("[SYS] Rede pronta em http://%s/\n", ns.ip);
        }
    }

    // Se algo essencial falhou, o usuario precisa saber agora, nao depois
    // de uma medicao errada.
    uint8_t failures = diag_selftest_failures();
    if (failures > 0) {
        LOG_SERIAL_FMT("[SYS] Autoteste concluido com %u falha(s)\n",
                       (unsigned)failures);
        widget_toast("Autoteste com falhas: veja Diagnostico",
                     TOAST_WARNING, 4000);
    } else {
        LOG_SERIAL_F("[SYS] Autoteste concluido sem falhas");
    }

    LOG_SERIAL_FMT("[SYS] Heap livre: %lu bytes\n",
                   (unsigned long)ESP.getFreeHeap());
}

// ============================================================================
// LOOP
// ============================================================================

void loop() {
    // Todo o trabalho vive nas tarefas do FreeRTOS. Suspender esta tarefa
    // devolve o tempo de CPU em vez de gastar ciclos num laco vazio.
    vTaskDelay(portMAX_DELAY);
}
