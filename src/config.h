// ============================================================================
// Sondvolt v3.2 — Configurações Gerais e Pinagem (CYD)
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: config.h
// Descrição: Definições de hardware, constantes de UI e limites de segurança
// ============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "pins.h"

// ============================================================================
// 1. INFORMAÇÕES DO FIRMWARE
// ============================================================================
#define FW_NAME             "Sondvolt"
#define FW_VERSION          "3.2.1"
#define FW_AUTHOR           "Eletrônica DIY"
#define FW_YEAR             "2026"

// ============================================================================
// 2. CONFIGURAÇÕES DE INTERFACE (UI)
// ============================================================================
#define SCREEN_WIDTH        320
#define SCREEN_HEIGHT       240
#define SCREEN_W            SCREEN_WIDTH
#define SCREEN_H            SCREEN_HEIGHT
#define SCREEN_ROTATION     3   // Rotação Paisagem (Compatível com ILI9341_2)

// Cores Dinâmicas (RGB565) - Mapeadas para variáveis globais
extern uint16_t clr_back;
extern uint16_t clr_surf;
extern uint16_t clr_text;
extern uint16_t clr_dim;
extern uint16_t clr_primary;

#define COLOR_PRIMARY       clr_primary
#define COLOR_ACCENT        0x001F  // Azul
#define COLOR_BACKGROUND    clr_back
#define COLOR_SURFACE       clr_surf
#define COLOR_TEXT          clr_text
#define COLOR_TEXT_DIM      clr_dim
#define COLOR_GOOD          0x07E0  // Verde
#define COLOR_SUSPECT       0xFFE0  // Amarelo
#define COLOR_BAD           0xF800  // Vermelho

// Cores de Cartões (Menu)
#define C_CARD_BG           0x1082  // Cinza Superfície
#define C_CARD_SEL          0x07E0  // Verde Seleção
#define C_DIVIDER           0x2104  // Cinza Divisória

// Conjunto 2 (C_*) para compatibilidade
#define C_PRIMARY           COLOR_PRIMARY
#define C_ACCENT            COLOR_ACCENT
#define C_BACKGROUND        COLOR_BACKGROUND
#define C_SURFACE           COLOR_SURFACE
#define C_TEXT              COLOR_TEXT
#define C_TEXT_SECONDARY    COLOR_TEXT_DIM
#define C_SUCCESS           COLOR_GOOD
#define C_WARNING           COLOR_SUSPECT
#define C_ERROR             COLOR_BAD

// Cores Básicas (Compatibilidade)
#define C_WHITE             0xFFFF
#define C_BLACK             0x0000
#define C_GREY              0x8410
#define C_RED               0xF800
#define C_GREEN             0x07E0
#define C_BLUE              0x001F
#define C_YELLOW            0xFFE0
#define C_CYAN              0x07FF
#define C_PURPLE            0x780F
#define C_ORANGE            0xFD20

// Tempos
#define BACKLIGHT_TIMEOUT   45000   // 45 segundos para desligar
#define TIME_BACKLIGHT_OFF  BACKLIGHT_TIMEOUT
#define SPLASH_DURATION     2000    // 2 segundos de Splash Screen

// ============================================================================
// 3. LIMITES E PARÂMETROS DE MEDIÇÃO
// ============================================================================
#define ADC_MAX_VAL         4095
#define ADC_REF_VOLT        3.3f
#define ZMPT_CALIBRATION    1.0f

// ============================================================================
// 4. PARÂMETROS DO MULTÍMETRO E SENSORES
// ============================================================================
#define ZMPT_FILTER_SIZE        10
#define ZMPT_ZERO_POINT         2048
#define ZMPT_SAMPLE_RATE_US     500
#define ZMPT_SCALE_FACTOR       1.0f
#define ZMPT_NUM_SAMPLES        128
#define TRUE_RMS_SAMPLES        128

#define MULTI_DC_RANGE_20V      20.0f
#define MULTI_DC_RANGE_200V     200.0f
#define MULTI_DC_RANGE_600V     600.0f

#define RESISTANCE_MAX          999999.0f
#define SHORT_CIRCUIT_OHMS      10.0f
#define HIGH_VOLTAGE_THRESHOLD  50.0f
#define MAX_VOLTAGE_AC          250.0f

// Timings (Base)
#define TIME_REFRESH_MEAS       250     // ms
#define TIME_CONFIRM_TIMEOUT    10000   // 10 segundos
#define HISTORY_SIZE            20

// Layout da UI (Legacy/Base - will be overriden by CYD specifics if needed)
#define HEADER_H                35
#define FOOTER_H                30
#define CONTENT_Y               HEADER_H
#define CONTENT_H               (SCREEN_HEIGHT - HEADER_H - FOOTER_H)

// ============================================================================
// 5. SISTEMA DE ARQUIVOS
// ============================================================================
#define LOG_FILE_PATH       "/measurements.csv"
#define DB_FILE_CSV         "/database.csv"

// ============================================================================
// 5. MACROS DE DEPURAÇÃO (Serial)
// ============================================================================
#define LOG_SERIAL_F(x)      Serial.println(F(x))
#define LOG_SERIAL_FMT(x, ...) Serial.printf(x, ##__VA_ARGS__)

// Macros de Debug (usadas em sdcard.cpp e outros)
#define DBG(x)              LOG_SERIAL_F(x)
#define DBG_VAL(lbl, val)   LOG_SERIAL_FMT("%s: %d\n", lbl, (int)val)

// Alias para Buzzer/Speaker
#define PIN_SPEAKER         PIN_BUZZER

#define COLOR_WARNING       COLOR_SUSPECT
#define COLOR_ALERT         COLOR_BAD

// Timings
#define UPDATE_DISP         100
#define UPDATE_MEAS         50
#define UPDATE_PERIOD_MS    200
#define TIME_SAVE_HISTORY   5000
#define TIME_REFRESH_UI     100

// Layout (CYD 320x240)

// Fontes
#define FONT_VALUE          NULL
#define FONT_SMALL          NULL

#endif // CONFIG_H