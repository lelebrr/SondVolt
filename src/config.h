// ============================================================================
// Component Tester PRO v3.0 — Configuração de Hardware (CYD Edition)
// ============================================================================
// Placa alvo: ESP32-2432S028R (Cheap Yellow Display)
// Processador: ESP32-WROOM-32 (Dual Core 240MHz, 520KB RAM, 4MB Flash)
// Display: TFT 2.8" ILI9341 320x240 integrado
// Touchscreen: XPT2046 resistivo integrado
// ============================================================================
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// VERSÃO DO FIRMWARE
// ============================================================================
#define FW_VERSION      "3.0.0"
#define FW_CODENAME     "CYD Edition"
#define FW_BOARD        "ESP32-2432S028R"

// ============================================================================
// PINOS DO DISPLAY TFT (VSPI — fixos na placa, NÃO alterar)
// ============================================================================
// Nota: Estes pinos são configurados via build_flags no platformio.ini
// para o TFT_eSPI. Mantidos aqui como referência.
#define PIN_TFT_CS      15
#define PIN_TFT_DC      2
#define PIN_TFT_SCK     14
#define PIN_TFT_MOSI    13
#define PIN_TFT_MISO    12
#define PIN_TFT_BL      21    // Backlight (PWM para controle de brilho)

// ============================================================================
// PINOS DO SD CARD (HSPI — fixos na placa, NÃO alterar)
// ============================================================================
#define PIN_SD_CS       5
#define PIN_SD_SCK      18
#define PIN_SD_MOSI     23
#define PIN_SD_MISO     19

// ============================================================================
// PINOS DO TOUCHSCREEN (Software SPI — fixos na placa, NÃO alterar)
// ============================================================================
// Nota: Configurados via build_flags para o TFT_eSPI touch driver
#define PIN_TOUCH_CS    33
#define PIN_TOUCH_CLK   25
#define PIN_TOUCH_MOSI  32
#define PIN_TOUCH_MISO  39    // Input-only (VP)
#define PIN_TOUCH_IRQ   36    // Input-only (VP/SENSOR_VP)

// ============================================================================
// PINOS DE MEDIÇÃO (módulos externos)
// ============================================================================
// Sistema de Probes (3 terminais para Transistores/Diodos)
#define PIN_PROBE_1     35    // ADC1_CH7 — Probe 1 (Input-only, principal)
#define PIN_PROBE_2     27    // GPIO27   — Probe 2 (Shared with I2C SDA)
#define PIN_PROBE_3     22    // GPIO22   — Probe 3 (Shared with I2C SCL)
#define PIN_PROBE_MAIN  PIN_PROBE_1
#define PIN_I2C_SDA     PIN_PROBE_2
#define PIN_I2C_SCL     PIN_PROBE_3

// Sensor de tensão AC (ZMPT101B)
#define PIN_ZMPT_AC     34    // ADC1_CH6 — Tensão AC via ZMPT101B

// ============================================================================
// PINOS DE PERIFÉRICOS
// ============================================================================
// Sonda térmica DS18B20 (protocolo OneWire)
#define PIN_ONEWIRE     4     // GPIO4 — DS18B20

// Buzzer/Speaker (conector de speaker já presente na placa CYD)
#define PIN_BUZZER      26    // GPIO26 — Speaker/Buzzer (via DAC ou LEDC)

// LEDs indicadores (usando o LED RGB integrado da CYD)
#define PIN_LED_GREEN   16    // LED RGB — Canal Verde
#define PIN_LED_RED     17    // LED RGB — Canal Vermelho

// ============================================================================
// CONFIGURAÇÃO DO LEDC (PWM do ESP32)
// ============================================================================
// O ESP32 não tem analogWrite() nativo — usa canais LEDC
#define LEDC_CH_BUZZER      0   // Canal LEDC para o buzzer
#define LEDC_CH_BACKLIGHT   1   // Canal LEDC para o backlight do TFT
#define LEDC_FREQ_BUZZER    2000 // Frequência do tom do buzzer (Hz)
#define LEDC_FREQ_BL        5000 // Frequência PWM do backlight (Hz)
#define LEDC_RESOLUTION     8    // Resolução PWM (8 bits = 0-255)

// ============================================================================
// CONFIGURAÇÃO DO ADC (ESP32 = 12 bits, 3.3V)
// ============================================================================
#define ADC_RESOLUTION      12          // 12 bits (0-4095)
#define ADC_MAX_VALUE       4095        // Valor máximo do ADC
#define ADC_REF_VOLTAGE     3.3f        // Tensão de referência (V)
#define ADC_VOLTS_PER_BIT   (ADC_REF_VOLTAGE / ADC_MAX_VALUE)  // ~0.000806V

// Fator de conversão analógico otimizado para 12 bits
#define ADC_TO_VOLTS(raw)   ((float)(raw) * ADC_VOLTS_PER_BIT)

// ============================================================================
// CONFIGURAÇÃO DO INA219 (Sensor de Corrente/Tensão DC)
// ============================================================================
#define INA219_I2C_ADDR     0x40    // Endereço padrão do INA219
#define INA219_SHUNT_R      0.1f   // Resistor shunt em ohms (100mΩ)

// ============================================================================
// CONFIGURAÇÃO DO ZMPT101B (Sensor de Tensão AC)
// ============================================================================
#define ZMPT_SAMPLES        1000    // Número de amostras para cálculo RMS
#define ZMPT_SAMPLE_PERIOD  1000    // Período total de amostragem (µs por ciclo)
#define ZMPT_ZERO_POINT     2048    // Ponto zero do ADC (meio da escala 12-bit)
#define ZMPT_SCALE_FACTOR   311.0f  // Fator de escala (calibrar com multímetro)
// True RMS: Vrms = sqrt(sum(Vi²)/N) * fator_escala

// ============================================================================
// LIMITES DE TEMPERATURA (sonda térmica DS18B20)
// ============================================================================
#define TEMP_NORMAL_THRESHOLD   70.0f   // Abaixo = operação normal (verde)
#define TEMP_HOT_THRESHOLD      90.0f   // Acima = aquecimento (laranja)
#define TEMP_DANGER_THRESHOLD   110.0f  // Acima = perigo térmico! (vermelho)

// ============================================================================
// CORES DA UI (formato RGB565 para ILI9341)
// ============================================================================
// Paleta moderna "Dark Cyber" — otimizada para legibilidade em TFT 2.8"
#define UI_COLOR_BG         0x000F  // Navy Blue profundo
#define UI_COLOR_ACCENT     0x07FF  // Cyan vibrante (destaque principal)
#define UI_COLOR_TEXT       0xFFFF  // Branco puro
#define UI_COLOR_HILIGHT    0x03EF  // Azul médio (seleção de itens)
#define UI_COLOR_RED        0xF800  // Vermelho (erro/perigo)
#define UI_COLOR_GREEN      0x07E0  // Verde (OK/sucesso)
#define UI_COLOR_ORANGE     0xFD20  // Laranja (aviso/aquecimento)
#define UI_COLOR_GREY       0x7BEF  // Cinza (texto secundário)
#define UI_COLOR_DARK       0x0008  // Navy mais escuro (cards inativos)
#define UI_COLOR_BORDER     0x0841  // Cinza-azulado (bordas de cards)
#define UI_COLOR_STATUS_BG  0x0014  // Background da barra de status
#define UI_COLOR_YELLOW     0xFFE0  // Amarelo (AC voltage)

// ============================================================================
// LAYOUT DA UI
// ============================================================================
// Dimensões da tela (com rotação = 1, landscape)
#define SCREEN_WIDTH        320
#define SCREEN_HEIGHT       240
#define TFT_ROTATION        1       // Landscape (conector USB à esquerda)

// Grid de Menu
#define MENU_GRID_COLS      2       // Colunas do grid principal
#define MENU_GRID_ROWS      3       // Linhas do grid principal
#define MENU_ICON_SIZE      32      // Tamanho base dos ícones (pixels)

// Barra de Status (topo)
#define STATUS_BAR_HEIGHT   28      // Altura da barra de status (pixels)

// Footer (rodapé)
#define FOOTER_HEIGHT       20      // Altura do rodapé (pixels)

// ============================================================================
// CONFIGURAÇÕES GERAIS
// ============================================================================
#define HISTORY_SIZE        50      // Histórico de medições (mais RAM no ESP32!)
#define MAX_MEASUREMENT_NAME 16     // Tamanho máximo do nome de medição
#define SERIAL_BAUD         115200  // Velocidade da porta serial

// Timeout padrão para medições
#define DEFAULT_TIMEOUT_MS  30000   // 30 segundos

// Backlight padrão (0-255)
#define DEFAULT_BACKLIGHT   200     // ~78% de brilho

// ============================================================================
// DEBOUNCE E TOUCH
// ============================================================================
#define BTN_DEBOUNCE_MS     50      // Debounce para botões físicos (ms)
#define TOUCH_DEBOUNCE_MS   200     // Debounce para touchscreen (ms)
#define TOUCH_THRESHOLD     300     // Limiar de pressão do touch (0-4095)

// ============================================================================
// CONFIGURAÇÃO DO SD CARD (HSPI separado)
// ============================================================================
// O SD Card da CYD usa o barramento HSPI (separado do VSPI do TFT)
// Isso evita conflitos de SPI e permite acesso simultâneo
#define SD_SPI_FREQ         4000000 // 4MHz (seguro para MicroSD)

// Caminhos de arquivos no SD Card
#define DB_FILE_PATH        "/COMPBD.CSV"   // Base de dados de componentes
#define LOG_FILE_PATH       "/LOG.TXT"      // Arquivo de log de medições

// ============================================================================
// MACROS DE DEBUG
// ============================================================================
#define SERIAL_DEBUG 1
#if SERIAL_DEBUG
  #define LOG_SERIAL(x)     Serial.println(x)
  #define LOG_SERIAL_F(x)   Serial.println(F(x))
  #define LOG_BEGIN(x)       Serial.begin(x)
#else
  #define LOG_SERIAL(x)
  #define LOG_SERIAL_F(x)
  #define LOG_BEGIN(x)
#endif

#endif // CONFIG_H
