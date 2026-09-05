// ============================================================================
// Sondvolt v5.1 - Configuracoes Gerais
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo : config.h
// Objetivo: Constantes de firmware, interface, medicao e seguranca.
//
// Regra deste arquivo: cada constante e definida UMA unica vez, aqui ou em
// pins.h, nunca nos dois. Definicoes duplicadas geravam avisos silenciados
// pela flag -w e faziam o valor efetivo depender da ordem dos includes.
// ============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "pins.h"

// ============================================================================
// 1. IDENTIFICACAO DO FIRMWARE
// ============================================================================
#define FW_NAME               "Sondvolt"
#define FW_VERSION            "5.1.0"
#define FW_AUTHOR             "Eletronica DIY"
#define FW_YEAR               "2026"
#define FW_CODENAME           "Bancada"

// ============================================================================
// 2. INTERFACE
// ============================================================================
#define SCREEN_WIDTH          320
#define SCREEN_HEIGHT         240
#define SCREEN_W              SCREEN_WIDTH
#define SCREEN_H              SCREEN_HEIGHT
#define SCREEN_ROTATION       3       // paisagem, compativel com ILI9341_2

// Paleta dinamica (definida em globals.cpp, alternada por colors_update()).
extern uint16_t clr_back;
extern uint16_t clr_surf;
extern uint16_t clr_text;
extern uint16_t clr_dim;
extern uint16_t clr_primary;

#define COLOR_PRIMARY         clr_primary
#define COLOR_BACKGROUND      clr_back
#define COLOR_SURFACE         clr_surf
#define COLOR_TEXT            clr_text
#define COLOR_TEXT_DIM        clr_dim
#define COLOR_ACCENT          0x001F   // azul
#define COLOR_GOOD            0x07E0   // verde
#define COLOR_SUSPECT         0xFFE0   // amarelo
#define COLOR_BAD             0xF800   // vermelho
#define COLOR_WARNING         COLOR_SUSPECT
#define COLOR_ALERT           COLOR_BAD

// Aliases curtos (conjunto C_*)
#define C_PRIMARY             COLOR_PRIMARY
#define C_ACCENT              COLOR_ACCENT
#define C_BACKGROUND          COLOR_BACKGROUND
#define C_SURFACE             COLOR_SURFACE
#define C_TEXT                COLOR_TEXT
#define C_TEXT_SECONDARY      COLOR_TEXT_DIM
#define C_SUCCESS             COLOR_GOOD
#define C_WARNING             COLOR_SUSPECT
#define C_ERROR               COLOR_BAD
#define C_CARD_BG             0x1082
#define C_CARD_SEL            0x07E0
#define C_DIVIDER             0x2104

// Cores fixas
#define C_WHITE               0xFFFF
#define C_BLACK               0x0000
#define C_GREY                0x8410
#define C_RED                 0xF800
#define C_GREEN               0x07E0
#define C_BLUE                0x001F
#define C_YELLOW              0xFFE0
#define C_CYAN                0x07FF
#define C_PURPLE              0x780F
#define C_ORANGE              0xFD20

// Layout base
#define HEADER_H              35
#define FOOTER_H              30
#define CONTENT_Y             HEADER_H
#define CONTENT_H             (SCREEN_HEIGHT - HEADER_H - FOOTER_H)

// ============================================================================
// 3. TEMPOS
// ============================================================================
#define BACKLIGHT_TIMEOUT     45000UL  // escurece a tela apos 45 s parado
#define TIME_BACKLIGHT_OFF    BACKLIGHT_TIMEOUT
#define BACKLIGHT_DIM_LEVEL   12       // brilho no modo economia (0-255)
#define SPLASH_DURATION       2000

#define TIME_REFRESH_MEAS     250      // periodo de refresh das medicoes
#define TIME_REFRESH_UI       100      // periodo de refresh da interface
#define TIME_CONFIRM_TIMEOUT  10000UL  // caducidade das telas de confirmacao
#define TIME_SAVE_HISTORY     5000UL
#define TIME_TOAST_DEFAULT    2200     // duracao padrao das notificacoes

#define UPDATE_DISP           100
#define UPDATE_MEAS           50
#define UPDATE_PERIOD_MS      200

// ============================================================================
// 4. ADC E MEDICAO
// ============================================================================
#define ADC_MAX_VAL           ADC_MAX_COUNT   // definido em pins.h
#define ADC_REF_VOLT          3.30f

// Limiares de julgamento nas pontas
#define ADC_OPEN_CIRCUIT_V    (ADC_REF_VOLT - 0.04f)
#define ADC_SHORT_CIRCUIT_V   0.012f

// Resistores de referencia (valores reais vem de pins.h)
#define PULLUP_RESISTANCE     PROBE_REF_RESISTOR

// Capacitor
#define CAP_CHARGE_THRESHOLD  2588     // 63.2 % de 4095
#define CAP_CHARGE_TIMEOUT_US 3000000UL
#define CAP_DISCHARGE_MAX_MS  1500

// Resistencia
#define RESISTANCE_MAX        999999.0f
#define SHORT_CIRCUIT_OHMS    10.0f

// Temperatura
#define THERMAL_WARNING_TEMP  70.0f
#define THERMAL_CRITICAL_TEMP 90.0f
#define THERMAL_INVALID_TEMP  -127.0f

// ============================================================================
// 5. MULTIMETRO
// ============================================================================

// ZMPT101B - sensor de tensao AC
#define ZMPT_ZERO_POINT       2048     // meia escala do ADC de 12 bits
#define ZMPT_SAMPLE_RATE_US   200      // 5 kHz: 83 amostras por ciclo de 60 Hz
#define ZMPT_NUM_SAMPLES      256      // 4 ciclos completos em 60 Hz
#define TRUE_RMS_SAMPLES      ZMPT_NUM_SAMPLES
#define ZMPT_FILTER_SIZE      8
#define ZMPT_DEFAULT_GAIN     0.3707f  // volts por conta, calibrado de fabrica

// Faixas de tensao DC (divisores externos no conector CN1)
#define MULTI_DC_DIVIDER_1X   1.0f     // leitura direta, ate 3.3 V
#define MULTI_DC_DIVIDER_11X  11.0f    // divisor 10:1, ate 26 V
#define MULTI_DC_RANGE_3V     3.3f
#define MULTI_DC_RANGE_26V    26.0f

// Corrente
#define CURRENT_MAX_AMPS      INA219_MAX_AMPS

// Seguranca eletrica
#define MAX_VOLTAGE_AC        250.0f
#define HIGH_VOLTAGE_THRESHOLD 50.0f
#define SURGE_MULTIPLIER      1.414f   // raiz de 2
#define FILTER_ALPHA_DEFAULT  0.2f

// Historico circular do multimetro
#define HISTORY_SIZE          20

// ============================================================================
// 6. ARQUIVOS NO CARTAO SD
// ============================================================================
#define LOG_FILE_PATH         "/MEASURE.CSV"
#define LOG_FILE_BACKUP       "/MEASURE.OLD"
#define LOG_MAX_BYTES         262144UL   // 256 KB antes de rotacionar
#define DB_FILE_CSV           "/COMPBD.CSV"
#define SETTINGS_FILE         "/SETTINGS.CFG"
#define REPORT_DIR            "/RELATOR"

// ============================================================================
// 7. DEPURACAO
// ============================================================================
#ifndef SONDVOLT_VERBOSE
#define SONDVOLT_VERBOSE      1
#endif

#if SONDVOLT_VERBOSE
  #define LOG_SERIAL_F(x)        Serial.println(F(x))
  #define LOG_SERIAL_FMT(x, ...) Serial.printf(x, ##__VA_ARGS__)
#else
  #define LOG_SERIAL_F(x)        do {} while (0)
  #define LOG_SERIAL_FMT(x, ...) do {} while (0)
#endif

#define DBG(x)                LOG_SERIAL_F(x)
#define DBG_VAL(lbl, val)     LOG_SERIAL_FMT("%s: %d\n", lbl, (int)(val))

// ============================================================================
// 8. FONTES (stubs de compatibilidade com codigo antigo)
// ============================================================================
#define FONT_VALUE            NULL
#define FONT_SMALL            NULL

#endif // CONFIG_H
