// ============================================================================
// Sondvolt v3.2 — Main Entry Point
// ============================================================================

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "display_globals.h"
#include "display_mutex.h" // Adicionado para LOCK/UNLOCK
#include "ui.h"
#include "globals.h"
#include "logger.h"

namespace {
constexpr uint16_t TOUCH_X_MIN_RAW = 200;
constexpr uint16_t TOUCH_X_MAX_RAW = 3700;
constexpr uint16_t TOUCH_Y_MIN_RAW = 240;
constexpr uint16_t TOUCH_Y_MAX_RAW = 3800;
constexpr uint16_t TOUCH_SCREEN_W = 320;
constexpr uint16_t TOUCH_SCREEN_H = 240;

constexpr TickType_t UI_TASK_DELAY = pdMS_TO_TICKS(20);
constexpr TickType_t MEASURE_TASK_DELAY = pdMS_TO_TICKS(100);

inline uint16_t map_touch_x(int16_t rawX) {
    long mapped = map(rawX, TOUCH_X_MIN_RAW, TOUCH_X_MAX_RAW, 0, TOUCH_SCREEN_W - 1);
    return (uint16_t)constrain(mapped, 0, TOUCH_SCREEN_W - 1);
}

inline uint16_t map_touch_y(int16_t rawY) {
    long mapped = map(rawY, TOUCH_Y_MIN_RAW, TOUCH_Y_MAX_RAW, 0, TOUCH_SCREEN_H - 1);
    return (uint16_t)constrain(mapped, 0, TOUCH_SCREEN_H - 1);
}
}

// Instâncias Globais — Declaradas em display_globals.h
// spiTFT_SD, touchSPI e touch são gerenciados em display_globals.cpp

void TaskUserInterface(void* pvParameters) {
    for (;;) {
        LOCK_TFT();
        if (touch.touched()) {
            TS_Point p = touch.getPoint();
            // Mapeamento de toque Landscape (320x240)
            uint16_t tx = map_touch_x(p.x);
            uint16_t ty = map_touch_y(p.y);
            
            ui_handle_touch(tx, ty);
        } else {
            ui_reset_touch_state();
        }
        
        ui_update();
        UNLOCK_TFT();
        vTaskDelay(UI_TASK_DELAY);
    }
}

#include "measurements.h"
#include "thermal.h"
#include "multimeter.h"

#include "safety.h"

void TaskMeasurement(void* pvParameters) {
    for (;;) {
        // Monitoramento de segurança em background
        safety_update();
        if (currentAppState != STATE_MULTIMETER) {
            safety_detect_danger();
        }

        // Atualiza medições globais conforme o estado
        switch (currentAppState) {
            case STATE_MEASURE_RESISTOR:
                lastResistance = measurements_get_raw_resistance();
                break;
            case STATE_MEASURE_CAPACITOR:
                lastCapacitance = measurements_get_raw_capacitance();
                break;
            case STATE_MEASURE_DIODE:
            case STATE_MULTIMETER:
                multimeter_handle();
                break;
            case STATE_THERMAL_PROBE:
            case STATE_THERMAL_CAMERA:
                lastTemperature = thermal_read();
                break;
            case STATE_MEASURE_IC:
            case STATE_SCANNER:
                measurements_update();
                break;
            case STATE_COMPARATOR:
                // Medição contínua para comparação
                measurements_update();
                lastResistance = measurements_get_raw_resistance();
                lastVoltage = measurements_get_last_value();
                break;
            case STATE_SUBMENU_TEMP:
            case STATE_SUBMENU_MAIS:
                // No measurement needed in these menus
                break;
            default:
                measurements_update(); 
                break;
        }
        
        vTaskDelay(MEASURE_TASK_DELAY);
    }
}

void setup() {
    Serial.begin(115200);
    
    // Inicialização SPI
    spiTFT_SD.begin(PIN_SD_SCLK, PIN_SD_MISO, PIN_SD_MOSI);
    touchSPI.begin(PIN_HSPI_SCLK, PIN_HSPI_MISO, PIN_HSPI_MOSI, PIN_TOUCH_CS);
    
    tft.init();
    tft.setRotation(3); 
    // Cores reais (evita distorção/inversão global)
    tft.invertDisplay(false);
    // Bitmaps 16-bit (ícones/logo) no formato correto de byte-order
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
    
    touch.begin(touchSPI);
    touch.setRotation(3);

    // Inicializa periféricos de medição
    measurements_init();
    thermal_init();
    multimeter_init(false);
    sdCardError = !logger_init();
    safety_init();

    // Inicializa Sistema de UI (Splash + Menu)
    ui_init();

    // Criação de Tasks
    xTaskCreate(TaskUserInterface, "TaskUI", 4096, NULL, 2, NULL);
    xTaskCreate(TaskMeasurement, "TaskMeasure", 4096, NULL, 1, NULL);
}

void loop() {
    // FreeRTOS assume o controle
    vTaskDelay(portMAX_DELAY);
}