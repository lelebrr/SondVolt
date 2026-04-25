// ============================================================================
// Sondvolt v3.0 — Pinagem da Placa Cheap Yellow Display
// Hardware: ESP32-2432S028R (CYD)
// ============================================================================
// Arquivo: pins.h
// Descricao: Definicoes completas de pinagem para a CYD
// ============================================================================

#ifndef PINS_H
#define PINS_H

#include <driver/gpio.h>

// ============================================================================
// VERSAO E IDENTIFICACAO
// ============================================================================
#define BOARD_NAME           "ESP32-2432S028R (Cheap Yellow Display)"
#define BOARD_VARIANT         "CYD 2.8\""
#define BOARD_MANUFACTURER    "Makerfabs / ESP32-2432S028R"

// ============================================================================
// DISPLAY TFT ILI9341 (Barramento VSPI - Fixo na CYD)
// ============================================================================
// O display usa o barramento VSPI que e fixo no hardware da CYD
// Pinosfisicos no ESP32:
// ------------------------------------------------------------------------

// Barramento VSPI (Fixed hardware)
#define PIN_VSPI_MOSI         13      // VSPI MOSI (GPIO13) - Dados para TFT
#define PIN_VSPI_MISO         12      // VSPI MISO (GPIO12) - Leitura (nao usado)
#define PIN_VSPI_SCLK         14      // VSPI Clock (GPIO14) - Clock SCK

// Controle do TFT
#define PIN_TFT_CS            15      // Chip Select (GPIO15) - ATIVO BAIXO
#define PIN_TFT_DC            2       // Data/Command (GPIO2) - 0=comando, 1=dados
#define PIN_TFT_RST           0       // Reset (GPIO0) - Controlado por software
#define PIN_TFT_BL            21      // Backlight (GPIO21) - PWM controlavel

// Configuracoes do barramento
#define TFT_VSPI_HOST         VSPI_HOST
#define TFT_DMA_CH            1
#define TFT_PIN_NUM_MOSI      PIN_VSPI_MOSI
#define TFT_PIN_NUM_MISO      PIN_VSPI_MISO
#define TFT_PIN_NUM_CLK       PIN_VSPI_SCLK
#define TFT_PIN_NUM_CS        PIN_TFT_CS

// ============================================================================
// TOUCH SCREEN XPT2046 (Barramento HSPI - Fixo na CYD)
// ============================================================================
// O touch usa o barramento HSPI para n~ao conflitar com o TFT
// ------------------------------------------------------------------------

// Barramento HSPI (Fixed hardware)
#define PIN_HSPI_MOSI         32      // HSPI MOSI (GPIO32)
#define PIN_HSPI_MISO         39      // HSPI MISO (GPIO39) - Leitura do touch
#define PIN_HSPI_SCLK         25      // HSPI Clock (GPIO25)

// Controle do Touch
#define PIN_TOUCH_CS          33      // Chip Select (GPIO33) - ATIVO BAIXO
#define PIN_TOUCH_IRQ         36      // IRQ (GPIO36 / SENSOR_VP)

// Configuracoes do barramento
#define TOUCH_HSPI_HOST       HSPI_HOST
#define TOUCH_PIN_NUM_MOSI    PIN_HSPI_MOSI
#define TOUCH_PIN_NUM_MISO    PIN_HSPI_MISO
#define TOUCH_PIN_NUM_CLK    PIN_HSPI_SCLK
#define TOUCH_PIN_NUM_CS     PIN_TOUCH_CS

// Configuracoes do touch (Calibradas para CYD - Versão Original)
#define TOUCH_MIN_X           300     // Valor ADCmin X
#define TOUCH_MAX_X           3700    // Valor ADCmax X
#define TOUCH_MIN_Y           600     // Valor ADCmin Y
#define TOUCH_MAX_Y           3600    // Valor ADCmax Y
#define TOUCH_MIN_PRESSURE    100     // Pressao minima valida
#define TOUCH_MAX_PRESSURE    1000    // Pressao maxima

// ============================================================================
// CARTAO SD CARD (Barramento HSPI - Compartilhado com Touch!)
// ============================================================================
// IMPORTANTE: O SD card usa os mesmos pinos fisicos do touch!
// Para evitar conflitos, o CS do touch deve ser desabilitado quando usar SD
// ------------------------------------------------------------------------

// Pinos do SD (Barramento VSPI padrão na CYD para SD)
#define PIN_SD_MOSI            23      // GPIO23
#define PIN_SD_MISO           19      // GPIO19
#define PIN_SD_SCLK           18      // GPIO18

// Controle do SD
#define PIN_SD_CS             5       // Chip Select (GPIO5) - ATIVO BAIXO
#define PIN_SD_DETECT         GPIO_NUM_NC  // Detect (nao conectado)

// Configuracoes do barramento
#define SD_SPI_HOST           VSPI_HOST  // Mesmo host do TFT
#define SD_PIN_NUM_MOSI       PIN_SD_MOSI
#define SD_PIN_NUM_MISO       PIN_SD_MISO
#define SD_PIN_NUM_CLK        PIN_SD_SCLK
#define SD_PIN_NUM_CS         PIN_SD_CS

// Configuracoes do SD
#define SD_FREQ_MAX           4000000 // 4MHz maximo (estavel)
#define SD_FREQ_DEFAULT       2000000 // 2MHz default
#define SD_MOUNT_POINT       "/sdcard"
#define SD_MAX_FILES          10

// ============================================================================
// MEDICOES ADC (Barramento ADC1)
// ============================================================================
// O ESP32 tem 8 canais ADC1 (GPIO32-39) acessiveis
// ------------------------------------------------------------------------

// Pinos de entrada ADC
#define PIN_ADC_PROBE1        35      // GPIO35 - Sonda 1 (ADC1_CH7)
#define PIN_ADC_PROBE2         34      // GPIO34 - Sonda 2 (ADC1_CH6)
#define PIN_ADC_ZMPT          36      // GPIO36 - Sensor AC ZMPT101B (ADC1_CH0)

// Canais ADC
#define ADC_CH_PROBE1         ADC1_CHANNEL_7
#define ADC_CH_PROBE2         ADC1_CHANNEL_6
#define ADC_CH_ZMPT           ADC1_CHANNEL_0

// Configuracoes ADC
#define ADC_WIDTH            ADC_WIDTH_12BIT  // 4095 valores (0-4095)
#define ADC_ATTENUATION       ADC_ATTEN_DB_11  // 0-3.3V range
#define ADC_TIMEOUT_MS       1000

// ============================================================================
// SENSOR DE TENSÃO AC (ZMPT101B)
// ============================================================================
// Modulo transformador de tens~ao AC
// ------------------------------------------------------------------------

#define PIN_ZMPT_OUT           PIN_ADC_ZMPT  // Saida analogica do ZMPT
#define ZMPT_SAMPLE_RATE      1000        // Amostragens por ciclo
#define ZMPT_NUM_SAMPLES     50          // Amostras para media
#define ZMPT_CALIBRATION      1.0f       // Fator de calibra~ao
#define ZMPT_TOLERANCE        5.0f        // Tolerancia (V)

// ============================================================================
// SENSOR DE CORRENTE (INA219)
// ============================================================================
// Sensor I2C de corrente com shunt resistor
// ------------------------------------------------------------------------

#define PIN_INA_SDA           27      // I2C SDA (GPIO27) - Expansion IO2
#define PIN_INA_SCL           22      // I2C SCL (GPIO22) - Expansion IO1/IO2
#define INA_I2C_PORT          I2C_NUM_0
#define INA_I2C_ADDR          0x40        // Endereco I2C padrao
#define INA_SHUNT_OHMS        0.1f       // Resistor shunt (ohms)
#define INA_MAX_AMPS          3.2f       // Corrente maxima (A)
#define INA_CALIBRATION       1.0f       // Fator de calibraao
#define INA_VBUS_MAX          32.0f      // Tensao maxima do bus (V)

// ============================================================================
// SAIDAS DIGITAIS (LEDs e Buzzer)
// ============================================================================
// pinos de saida para indicadores visuais e sonoros
// ------------------------------------------------------------------------

#define PIN_LED_RED           4       // LED Vermelho (GPIO4) - Standard CYD
#define PIN_LED_GREEN         16      // LED Verde (GPIO16)
#define PIN_LED_BLUE          17      // LED Azul (GPIO17)

// Controle de Descarga de Capacitor
#define PIN_CAP_DISCHARGE     17      // GPIO17 (Compartilhado com LED Azul)

// Buzzer ativo (oscilador interno)
#define PIN_BUZZER            26      // Buzzer PWM (GPIO26) - AUDIO: IO26

// ============================================================================
// COMUNICACAO ONEWIRE (DS18B20)
// ============================================================================
// Sensor de temperatura digital
// ------------------------------------------------------------------------

#define PIN_ONEWIRE           4       // OneWire bus (GPIO4) - Compartilhado com LED Vermelho
#define ONEWIRE_MAX_DEVICES   3       // Maximo dispositivos
#define DS18B20_FAMILY       0x28      // Familia DS18B20

// ============================================================================
// I2C EXPANSAO (FUTURO)
// ============================================================================
// Barramento I2C para sensores externos
// ------------------------------------------------------------------------

#define PIN_I2C_SDA           27      // I2C SDA (GPIO27) - Expansion IO2
#define PIN_I2C_SCL           22      // I2C SCL (GPIO22) - Expansion IO1/IO2
#define I2C_PORT             I2C_NUM_0
#define I2C_FREQ             100000   // 100kHz padrao

// ============================================================================
// BOTOES (Direct wire to GPIO, sem divisor!)
// ============================================================================
// ATENCAO: A CYD tem botoes wired direct, sem resistores!
// ------------------------------------------------------------------------

// Mapeamento dos botoes (depende da biblioteca TFT_eSPI)
// O Touch e usado para botoes virtuais na tela

#define PIN_BTN_UP            GPIO_NUM_NC  // Nao conectado
#define PIN_BTN_DOWN          GPIO_NUM_NC  // Nao conectado
#define PIN_BTN_LEFT          GPIO_NUM_NC  // Nao conectado
#define PIN_BTN_RIGHT         GPIO_NUM_NC  // Nao conectado
#define PIN_BTN_OK            GPIO_NUM_NC  // Nao conectado
#define PIN_BTN_BACK         GPIO_NUM_NC  // Nao conectado

// ============================================================================
// PINOS NAO USADOS (Reserved)
// ============================================================================
// Pinos reservados ou nao conectados
// ------------------------------------------------------------------------

#define PIN_RESERVED_1       1       // UART0 TX (debug)
#define PIN_RESERVED_2        3       // UART0 RX (debug)
#define PIN_RESERVED_3        5       // SD CS (compartilhado!)
// 9, 10, 11, 12 - Reserved for future use
// 24, 28, 30, 31 - Nao disponiveis

// ============================================================================
// MAPA DE INTERRUPÇÕES
// ============================================================================
// Definicoes de interrupcoes por evento
// ------------------------------------------------------------------------

#define IRQ_ADC_PROBE1        ADC1_CHANNEL_7
#define IRQ_ADC_PROBE2        ADC1_CHANNEL_6
#define IRQ_ADC_ZMPT          ADC1_CHANNEL_0
#define IRQ_TOUCH             TOUCH_HSPI_HOST

// ============================================================================
// VERIFICACOES DE HARDWARE
// ============================================================================
// Funcoes de validacao de pinagem
// ------------------------------------------------------------------------

// Verifica se pino e valido para ADC
#define IS_ADC_PIN(gpio)      (((gpio) >= GPIO_NUM_32) && ((gpio) <= GPIO_NUM_39))

// Verifica se pino e valido para saida
#define IS_OUTPUT_PIN(gpio)   (((gpio) <= GPIO_NUM_27) && ((gpio) != GPIO_NUM_6) && \
                             ((gpio) != GPIO_NUM_7) && ((gpio) != GPIO_NUM_8) && \
                             ((gpio) != GPIO_NUM_9) && ((gpio) != GPIO_NUM_10))

// Verifica pino pode ser CS
#define IS_CS_PIN(gpio)       (((gpio) == 5) || ((gpio) == 15) || \
                             ((gpio) == 33) || ((gpio) == 27))

// ============================================================================
// COMPATIBILIDADE (Aliases para config.h)
// ============================================================================
// Mantem compatibilidade com codigo existente
// ------------------------------------------------------------------------

// Display TFT
#define PIN_TFT_CS_LEGACY    PIN_TFT_CS
#define PIN_TFT_DC_LEGACY    PIN_TFT_DC
#define PIN_TFT_MOSI         PIN_VSPI_MOSI
#define PIN_TFT_SCLK         PIN_VSPI_SCLK

// Touch
#define PIN_TOUCH_CS_LEGACY  PIN_TOUCH_CS

// SD Card
#define PIN_SD_CS_LEGACY     PIN_SD_CS

// ADC
#define PIN_PROBE_1           PIN_ADC_PROBE1
#define PIN_PROBE_2           PIN_ADC_PROBE2
#define PIN_ZMPT_AC           PIN_ADC_ZMPT

// SD Card Aliases
#define SD_CS                 PIN_SD_CS

// Thermal Sensor
#define PIN_DS18B20           PIN_ONEWIRE

#endif // PINS_H