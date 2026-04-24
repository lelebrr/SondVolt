// ============================================================================
// Sondvolt v3.1 — Main (TFT_eSPI)
// Hardware: ESP32-2432S028R (CYD)
// ============================================================================

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Preferences.h>

#include "config.h"
#include "pins.h"
#include "globals.h"
#include "display_globals.h"
#include "ui.h"
#include "graphics.h"
#include "buzzer.h"
#include "safety.h"
#include "leds.h"
#include "buttons.h"
#include "sdcard.h"
#include "database.h"
#include "multimeter.h"
#include "measurements.h"
#include "thermal.h"
#include "logger.h"
#include "task_manager.h"
#include "display_mutex.h"

// ============================================================================
// INICIALIZAÇÃO MÍNIMA (fallback quando componentes externos falham)
// ============================================================================

void system_init_minimal() {
    // Inicialização absolutamente essencial (nunca falha)
    Serial.begin(115200);
    delay(1000);
    
    // LEDs e buzzer (pinos GPIO básicos)
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    
    // Buzzer básico
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    
    // Indica que o sistema está funcionando
    set_green_led(true);
    buzzer_beep(1000, 100);
    delay(200);
    set_green_led(false);
    
    LOG_SERIAL_F("Sistema iniciado em modo mínimo");
}


// Objeto global do display (definido em display_globals.h)
TFT_eSPI tft = TFT_eSPI();
Preferences nvs;

void setup() {
    // Tenta inicialização normal primeiro
    bool initSuccess = true;
    
    Serial.begin(115200);
    delay(500); // Aumenta delay para estabilização
    LOG_SERIAL_F("=== SONDVOLT v3.1 CYD ===");

    // Inicializa watchdog timer global
    // Observação: watchdog timer será configurado via platformio.ini
    // esp_task_wdt_init(15, true); // 15 segundos timeout, panic em caso de timeout
    // esp_task_wdt_add(NULL); // Adiciona a tarefa principal ao watchdog

    // Inicialização básica e obrigatória (nunca falha)
    leds_init();
    buzzer_init();

    // Inicialização do display com tratamento de erros
    graphics_init();
    bool displayOk = tftInitialized;
    if (!displayOk) {
        LOG_SERIAL_F("ERRO: Falha na inicialização do display");
        initSuccess = false;
    }

    // Inicialização de segurança (não crítica)
    safety_init();

    // Mostra splash screen básica mesmo com falhas
    draw_splash_screen();

    // Inicialização de componentes opcionais (com tratamento de erros)
    bool sdInitialized = false;
    if (sd_init()) {
        sdInitialized = true;
        db_load_index();
        logger_init();
    } else {
        LOG_SERIAL_F("AVISO: SD card não disponível, usando modo offline");
    }

    // Inicialização do gerenciador de tarefas
    if (!task_manager_init()) {
        LOG_SERIAL_F("ERRO: Falha na inicialização do gerenciador de tarefas");
        initSuccess = false;
    }

    task_manager_create_all();

    // Exibe menu principal com tratamento de erros
    LOCK_TFT();
    if (tftInitialized) {
        ui_menu_show();
    } else {
        draw_splash_screen();
        tft.setTextColor(C_ERROR);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Display não disponível", SCREEN_W/2, SCREEN_H/2 + 60);
    }
    UNLOCK_TFT();

    DBG("Setup concluido com sucesso");
    
    // Se a inicialização normal falhar, usa modo mínimo
    if (!initSuccess) {
        LOG_SERIAL_F("FALLBACK: Inicialização mínima ativada");
        system_init_minimal();
        
        // Indica falha por LED e buzzer
        set_red_led(true);
        buzzer_beep(500, 200);
        delay(100);
        buzzer_beep(500, 200);
        set_red_led(false);
        
        // Espera por reinicialização ou comando do usuário
        while (true) {
            delay(1000);
            set_red_led(true);
            delay(500);
            set_red_led(false);
        }
    }
}

void loop() {
    // No FreeRTOS, o loop principal deve apenas tratar
    // entradas de usuario e acoes de alto nivel da UI.
    // As medicoes e seguranca estao em tarefas separadas.

    buttons_update();
    ui_menu_update();

    // Alimenta watchdog periodicamente
    // Comentado devido a problemas de compilação - watchdog será gerenciado pelo FreeRTOS
    // esp_task_wdt_reset();

    vTaskDelay(pdMS_TO_TICKS(10)); // Yield para outras tarefas
}