// ============================================================================
// Sondvolt v3.x — Configuracoes do Projeto
// Hardware: ESP32-2432S028R (Cheap Yellow Display)
// ============================================================================
// Arquivo: config.h
// Descricao: Configuracoes gerais, constantes e parametros do projeto
// ============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "pins.h"

// ============================================================================
// VERSAO DO FIRMWARE
// ============================================================================
#define FW_VERSION                "3.1.0"
#define FW_CODENAME              "Sondvolt"
#define FW_BOARD                 BOARD_NAME
#define FW_BUILD                __DATE__ " " __TIME__
#define FW_AUTHOR               "Eletrônica DIY"
#define FW_YEAR                 "2025-2026"

// ============================================================================
// TAMANHO E RESOLUCAO DE TELA
// ============================================================================
#define SCREEN_W                 320     // Largura em pixels
#define SCREEN_H                 240     // Altura em pixels
#define STATUS_BAR_H            28      // Altura da barra de status
#define FOOTER_H                20      // Altura do rodape
#define CONTENT_Y               STATUS_BAR_H
#define CONTENT_H               (SCREEN_H - STATUS_BAR_H - FOOTER_H)

// ============================================================================
// CORES DA UI (RGB565)
// ============================================================================
// Cores principais do tema visual
#define C_PRIMARY               0x07E0  // Verde principal
#define C_PRIMARY_DARK         0x0584  // Verde escuro
#define C_PRIMARY_LIGHT        0xB7BE  // Verde claro
#define C_SECONDARY            0xFD20  // Laranja
#define C_ACCENT              0x001F  // Azul
#define C_BACKGROUND          0x0000  // Preto
#define C_SURFACE             0x1082  // Superficie escura
#define C_SURFACE_LIGHT       0x3186  // Superficie clara
#define C_TEXT                0xFFFF  // Branco
#define C_TEXT_SECONDARY      0xC618  // Cinza claro
#define C_ERROR               0xF800  // Vermelho
#define C_WARNING             0xFFC0  // Amarelo
#define C_SUCCESS             0x07E0  // Verde

// Cores dos componentes (identificacao)
#define C_RESISTOR             0xD6BA  // Marrom
#define C_CAPACITOR           0x0078  // Azul
#define C_DIODE               0xF062  // Roxo
#define C_LED                 0x07E0  // Verde
#define C_TRANSISTOR          0xC80A  // Roxo escuro
#define C_INDUCTOR            0xB2B2  // Couro
#define C_CRYSTAL             0xCE59  // Quartzo
#define C_UNKNOWN             0x7BEF  // Cinza
#define C_GREY                0x8410  // Cinza claro
#define C_DARK                0x18E3  // Cinza muito escuro
#define C_DARK_GREY           0x2104  // Cinza escuro

// ============================================================================
// CONSTANTES DE TEMPO (millisegundos)
// ============================================================================
#define TIME_SPLASH            2000    // Tempo splash screen (ms)
#define TIME_DEBOUNCE           50      // Debounce botoes (ms)
#define TIME_REFRESH_DISP       100     // Refresh display (ms)
#define TIME_REFRESH_MEAS      200     // Refresh medicoes (ms)
#define TIME_BACKLIGHT_OFF     45000   // Backlight desligar (ms)
#define TIME_SAVE_HISTORY      5000    // Salvar historico (ms)
#define TIME_CALIBRATION       1000    // Duracao calibra~ao (ms)
#define TIME_SAFETY_ALERT     3000    // Alerta seguranca (ms)
#define TIME_CONFIRM_TIMEOUT  10000   // Timeout confirm (ms)

// ============================================================================
// LIMITES DE SEGURANCA ELETRICA
// ============================================================================
#define SAFETY_MAX_VOLTAGE_DC   50.0f   // Tensao DC maxima segura (V)
#define SAFETY_MAX_VOLTAGE_AC   25.0f   // Tensao AC maxima segura (V)
#define SAFETY_MAX_CURRENT      1.0f    // Corrente maxima segura (A)
#define SAFETY_ALERT_VOLTAGE  30.0f    // Alerta tensao (V)
#define SAFETY_WARNING_VOLTAGE 50.0f // Aviso tensao (V)

// ============================================================================
// LIMITES DE MEDICAO
// ============================================================================
#define MEAS_MIN_RESISTANCE   0.1f    // Res min (ohms)
#define MEAS_MAX_RESISTANCE  10000000.0f // Res max (ohms)
#define MEAS_MIN_CAPACITANCE 1.0f     // Cap min (pF)
#define MEAS_MAX_CAPACITANCE  100000.0f // Cap max (uF)
#define MEAS_MIN_INDUCTANCE   0.1f     // Ind min (uH)
#define MEAS_MAX_INDUCTANCE   10000.0f // Ind max (mH)
#define MEAS_MIN_VOLTAGE      0.0f     // Tensao min (V)
#define MEAS_MAX_VOLTAGE      250.0f   // Tensao max (V)
#define MEAS_MIN_CURRENT     0.0f     // Corrente min (A)
#define MEAS_MAX_CURRENT      5.0f     // Corrente max (A)

// ============================================================================
// CONSTANTES ADC
// ============================================================================
#define ADC_MAX_VALUE         4095    // Valor maximo ADC 12-bit
#define ADC_REF_VOLTAGE       3.3f    // Tensao referencia (V)
#define ADC_VREF             1.1f    // Tensao referencia interna (V)
#define ADC_RESOLUTION         12       // Bits de resolucao
#define ADC_SCALE            (ADC_REF_VOLTAGE / (float)ADC_MAX_VALUE)

// ============================================================================
// CONSTANTES ZMPT101B (Sensor Tensao AC)
// ============================================================================
// ZMPT_SAMPLE_RATE_US definido em multimeter.h
#define ZMPT_NUM_SAMPLES     50      // Amostras para media
#define ZMPT_ZERO_POINT      2048    // Ponto zero (meio do ADC)
#define ZMPT_SCALE_FACTOR    311.0f  // Fator para 220V RMS
#define ZMPT_SCALE           ZMPT_SCALE_FACTOR  // Alias
#define ZMPT_FILTER_SIZE    5       // Tamanho filtro media movel
#define ZMPT_CALIBRATION    1.0f

// ============================================================================
// SEGURANCA — Definicoes movidas para safety.h
// ============================================================================
// (ver safety.h para todas as constantes SAFETY_*)

// ============================================================================
// CONSTANTES INA219 (Sensor Corrente)
// ============================================================================
#define INA219_ADDR           0x40    // Endereco I2C padrao
#define INA219_SHUNT          0.1f    // Shunt resistor (ohms)
#define INA219_RSHUNT        INA219_SHUNT // Alias
#define INA219_MAX_AMPS       3.2f    // Corrente maxima (A)
#define INA219_VBUS_MAX      32.0f   // Tensao maxima bus (V)
#define INA_CALIBRATION      1.0f

// ============================================================================
// CALIBRACAO
// ============================================================================
#define CALIB_DEFAULT_RES     0.0f   // Offset resistencia padrao
#define CALIB_DEFAULT_CAP     0.0f   // Offset capacitancia padrao
#define CALIB_DEFAULT_VOLT   0.0f   // Offset tensao padrao
#define CALIB_DEFAULT_CURR    0.0f   // Offset corrente padrao
#define CALIB_TOLERANCE       0.01f  // Tolerancia calibracao

// ============================================================================
// BANCO DE DADOS
// ============================================================================
#define DB_MAX_COMPONENTS     200    // Maximo componentes
#define DB_ENTRY_NAME_LEN    32      // Comprimento nome
#define DB_ENTRY_CODE_LEN     16      // Comprimento codigo
#define DB_FILE_CSV          "/sdcard/COMPBD.CSV"
#define DB_FILE_TXT          "/sdcard/database.txt"
#define DB_DELIMITER         ','     // Delimitador CSV

// ============================================================================
// HISTORICO
// ============================================================================
#define HISTORY_MAX_ENTRIES  100    // Maximo entradas
#define HISTORY_FILE         "/sdcard/HISTORY.CSV"
#define HISTORY_HEADER       "Timestamp,Data,Hora,Componente,Valor,Unidade,Status"
#define HISTORY_DELIMITER    ';'

// ============================================================================
// MULTIMETRO
// ============================================================================
#define MULTI_DC_RANGE_200MV    0.2f    // 200mV range
#define MULTI_DC_RANGE_2V     2.0f    // 2V range
#define MULTI_DC_RANGE_20V   20.0f   // 20V range
#define MULTI_DC_RANGE_200V  200.0f  // 200V range
#define MULTI_DC_RANGE_600V  600.0f  // 600V range
#define MULTI_AC_RANGE_200V  200.0f  // 200V range
#define MULTI_AC_RANGE_600V  600.0f    // 600V range

// Fontes GFX (TFT_eSPI) — usar fontes built-in do TFT_eSPI
#include <TFT_eSPI.h>

// Fontes GFXFF disponiveis via TFT_eSPI
#define FM9   &FreeSans9pt7b
#define FMB9  &FreeSansBold9pt7b
#define FM12  &FreeSans12pt7b
#define FMB12 &FreeSansBold12pt7b
#define FMB   &FreeSansBold9pt7b
#define FMBO  &FreeSansBold9pt7b // Alias para FMBO (Bold)

// ============================================================================
// FONTES (Geral)
// ============================================================================
#define FONT_HEADER          FMB12   // Titulos grandes
#define FONT_NORMAL         FM9     // Texto normal
#define FONT_SMALL         NULL    // Texto Pequeno (GLCD)
#define FONT_VALUE         FMB12   // Valores grandes

// ============================================================================
// BUZZER (Sons)
// ============================================================================
#define BUZZER_FREQ_BTN         1000    // Botao (Hz)
#define BUZZER_FREQ_OK         2000    // Confirmacao boa (Hz)
#define BUZZER_FREQ_ERROR      500     // Erro (Hz)
#define BUZZER_FREQ_WARNING   1500    // Alerta (Hz)
#define BUZZER_FREQ_SUCCESS   2500    // Sucesso (Hz)
#define BUZZER_DURATION_BTN   30      // Botao (ms)
#define BUZZER_DURATION_OK    100     // Boa medicao (ms)
#define BUZZER_DURATION_ERROR 200     // Erro (ms)
#define BUZZER_DURATION_WARNING 150   // Alerta (ms)

// ============================================================================
// LOGGER
// ============================================================================
#define LOG_MAX_MESSAGES       50      // Mensagens buffer
#define LOG_FILE             "/sdcard/LOG.TXT"
#define LOG_LEVEL_DEBUG      0
#define LOG_LEVEL_INFO       1
#define LOG_LEVEL_WARNING    2
#define LOG_LEVEL_ERROR    3

// ============================================================================
// MEMORIA
// ============================================================================
#define HEAP_MIN_FREE         10000   // Minimo heap livre (bytes)
#define PSRAM_AVAILABLE       0       // PSRAM disponivel

// ============================================================================
// DEBUG
// ============================================================================
#define DEBUG_ENABLED        0       // 0=desligado, 1=ligado
#define DEBUG_BAUD          115200   // Baud rate serial

// Macro de log serial (usado em init de modulos)
#define LOG_SERIAL_F(x)     Serial.println(F(x))

#if DEBUG_ENABLED
  #define DBG(x)          Serial.println(x)
  #define DBGF(x)         Serial.print(F(x))
  #define DBG_VAL(x,y)    Serial.print(x); Serial.println(y)
  #define DBG_FUNC(x)    Serial.println(F(#x))
#else
  #define DBG(x)
  #define DBGF(x)
  #define DBG_VAL(x,y)
  #define DBG_FUNC(x)
#endif

// ============================================================================
// ATRIBUTOS DE MEMORIA
// ============================================================================
// No ESP32, PROGMEM ja e definido corretamente em pgmspace.h
#include <pgmspace.h>
#define RAM_ATTR            IRAM_ATTR
#define NOINIT_ATTR         __attribute__((section(".noinit")))

// ============================================================================
// CONFIGURACAO DO BUZZER (LEDC)
// ============================================================================
#define LEDC_CH_BUZZER       0
#define LEDC_FREQ_BUZZER     2000
#define LEDC_TIMER_BIT       8

// ============================================================================
// CORES ADICIONAIS (RGB565)
// ============================================================================
#define C_WHITE              0xFFFF
#define C_BLACK              0x0000
#ifndef C_GREY
#define C_GREY               0x8410
#endif
#define C_LIGHT_GREY         0xD69A
#ifndef C_DARK_GREY
#define C_DARK_GREY          0x4208
#endif
#define C_RED                0xF800
#define C_GREEN              0x07E0
#define C_BLUE               0x001F
#define C_YELLOW             0xFFE0
#define C_CYAN               0x07FF
#define C_PURPLE             0x780F
#define C_ORANGE             0xFD20
#ifndef C_DARK
#define C_DARK               0x2104
#endif
#define C_BORDER             C_GREY   // Cor padrao de borda

// ============================================================================
// COMPATIBILIDADE LEGACY
// ============================================================================
// Aliases para codigo existente

// Aliases de tempo para measurements.cpp
#define UPDATE_DISP          TIME_REFRESH_DISP
#define UPDATE_MEAS          TIME_REFRESH_MEAS
#define AUTOOFF_TIME         TIME_BACKLIGHT_OFF

#endif // CONFIG_H