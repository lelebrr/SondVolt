// ============================================================================
// Sondvolt v4.0 - Pinagem da Placa Cheap Yellow Display
// Hardware: ESP32-2432S028R (CYD)
// ============================================================================
// Arquivo : pins.h
// Objetivo: Fonte unica de verdade para TODA a pinagem do projeto.
//
// REGRA DE OURO DO ESP32
// ----------------------
//   GPIO 34, 35, 36, 37, 38, 39 sao ENTRADA APENAS (input-only).
//   Eles NAO possuem driver de saida nem resistores de pull interno.
//   Chamar pinMode(35, OUTPUT) / digitalWrite(35, x) NAO TEM EFEITO ALGUM.
//   Toda excitacao de circuito precisa sair de um GPIO com driver de saida.
//
// Este arquivo declara explicitamente quais pinos podem acionar cargas
// (IS_OUTPUT_CAPABLE_PIN) para que o firmware nunca tente o impossivel.
// ============================================================================

#ifndef PINS_H
#define PINS_H

#include <driver/gpio.h>

// ============================================================================
// 0. IDENTIFICACAO DA PLACA
// ============================================================================
#define BOARD_NAME            "ESP32-2432S028R (Cheap Yellow Display)"
#define BOARD_VARIANT         "CYD 2.8\" ILI9341 + XPT2046"
#define BOARD_REVISION        "Rev A (fiacao original)"

// Dimensoes do painel em pixels, na rotacao 3 (paisagem).
#define SCREEN_W_PX           320
#define SCREEN_H_PX           240

// Selecione a revisao de hardware montada.
//   0 = Rev A: fiacao original, pinos compartilhados (padrao, compativel)
//   1 = Rev B: fiacao recomendada, sem compartilhamento (veja docs/PINOUT.md)
//   2 = Rev C: Rev B + placa de expansao "Bancada" (expansor, boost, scope)
//
// A Rev C existe por um motivo simples: a CYD NAO TEM NENHUM GPIO LIVRE.
// Somados TFT, touch, cartao, ADCs, LEDs, buzzer e I2C, os 24 pinos
// utilizaveis do ESP32-WROOM estao todos ocupados. Qualquer recurso novo
// precisa ou compartilhar um pino existente, ou entrar por um expansor no
// barramento I2C. A Rev C adota a segunda opcao, com um PCF8574.
#ifndef SONDVOLT_HW_REV
#define SONDVOLT_HW_REV       0
#endif

// ============================================================================
// 1. DISPLAY TFT ILI9341 - barramento SPI proprio da CYD
// ============================================================================
// Estes pinos sao fixos na placa e sao consumidos pela TFT_eSPI atraves das
// build_flags do platformio.ini. Estao repetidos aqui apenas para consulta e
// para as verificacoes de integridade em tempo de compilacao.
#define PIN_TFT_MOSI          13      // VSPI MOSI
#define PIN_TFT_MISO          12      // VSPI MISO
#define PIN_TFT_SCLK          14      // VSPI SCK
#define PIN_TFT_CS            15      // Chip Select (ativo baixo)
#define PIN_TFT_DC            2       // Data/Command
#define PIN_TFT_RST           0       // Reset por software
#define PIN_TFT_BL            21      // Backlight (PWM via LEDC)

// Canal LEDC reservado para o backlight
#define LEDC_CH_BACKLIGHT     0
#define BACKLIGHT_PWM_FREQ    5000
#define BACKLIGHT_PWM_BITS    8
#define BACKLIGHT_PWM_MAX     255

// Aliases historicos
#define PIN_VSPI_MOSI         PIN_TFT_MOSI
#define PIN_VSPI_MISO         PIN_TFT_MISO
#define PIN_VSPI_SCLK         PIN_TFT_SCLK

// ============================================================================
// 2. TOUCHSCREEN XPT2046 - barramento SPI dedicado (HSPI)
// ============================================================================
#define PIN_HSPI_MOSI         32
#define PIN_HSPI_MISO         39      // input-only, correto para MISO
#define PIN_HSPI_SCLK         25
#define PIN_TOUCH_CS          33

// ATENCAO: na CYD o pino IRQ do XPT2046 chega no GPIO36, que e o MESMO pino
// usado pela entrada analogica do ZMPT101B. O firmware NAO usa a IRQ do touch
// (faz polling via touch.touched()), portanto GPIO36 fica livre para o ADC.
// Nunca habilite attachInterrupt(PIN_TOUCH_IRQ, ...) sem antes ler
// docs/PINOUT.md, secao "Conflitos conhecidos".
#define PIN_TOUCH_IRQ         36
#define TOUCH_IRQ_IS_USED     0       // 0 = polling (obrigatorio na Rev A)

// Limites brutos do ADC do touch, calibrados para a CYD em rotacao 3.
#define TOUCH_RAW_X_MIN       200
#define TOUCH_RAW_X_MAX       3700
#define TOUCH_RAW_Y_MIN       240
#define TOUCH_RAW_Y_MAX       3800
#define TOUCH_MIN_PRESSURE    200     // Z minimo para considerar toque valido
#define TOUCH_MAX_PRESSURE    4000

// Aliases historicos (mantidos para nao quebrar codigo antigo)
#define TOUCH_MIN_X           TOUCH_RAW_X_MIN
#define TOUCH_MAX_X           TOUCH_RAW_X_MAX
#define TOUCH_MIN_Y           TOUCH_RAW_Y_MIN
#define TOUCH_MAX_Y           TOUCH_RAW_Y_MAX

// ============================================================================
// 3. CARTAO MICROSD - barramento SPI proprio (NAO compartilhado com a TFT)
// ============================================================================
// Ao contrario do que a documentacao antiga afirmava, na CYD o slot MicroSD
// tem pinos exclusivos. Nao ha necessidade de desativar o CS da TFT antes de
// acessar o cartao, embora o mutex de SPI continue sendo usado por seguranca.
#define PIN_SD_MOSI           23
#define PIN_SD_MISO           19
#define PIN_SD_SCLK           18
#define PIN_SD_CS             5
#define SD_SPI_SPEED_MHZ      10      // 10 MHz e estavel com cabos curtos
#define SD_SPI_SPEED_SAFE_MHZ 4       // fallback automatico se 10 MHz falhar

// ============================================================================
// 4. ENTRADAS ANALOGICAS (todas em ADC1 - ADC2 conflita com WiFi)
// ============================================================================
#define PIN_ADC_PROBE1        35      // Ponta 1  - INPUT ONLY
#define PIN_ADC_PROBE2        34      // Ponta 2  - INPUT ONLY
#define PIN_ADC_ZMPT          36      // ZMPT101B - INPUT ONLY

#define ADC_CH_PROBE1         ADC1_CHANNEL_7
#define ADC_CH_PROBE2         ADC1_CHANNEL_6
#define ADC_CH_ZMPT           ADC1_CHANNEL_0

#define ADC_RESOLUTION_BITS   12
#define ADC_MAX_COUNT         4095

// ============================================================================
// 5. PINO DE EXCITACAO DAS PONTAS  (a correcao mais importante desta revisao)
// ============================================================================
// As pontas de prova estao em GPIO34/35, que sao input-only. Portanto o
// firmware NAO consegue aplicar tensao nelas. Para medir resistencia e
// capacitancia e obrigatorio um pino com driver de saida alimentando o
// divisor atraves de um resistor de referencia conhecido.
//
//   +3V3 --[ PIN_PROBE_DRIVE ]--[ R_REF 10k 1% ]--+-- PROBE 1 (GPIO35, leitura)
//                                                 |
//                                            componente
//                                                 |
//                                                GND
//
// GPIO27 sai no conector de expansao P3 da CYD e possui driver completo.
#define PIN_PROBE_DRIVE       27
#define PROBE_REF_RESISTOR    10000.0f   // ohms, 1% recomendado

// Segundo pino de excitacao, usado nas faixas de baixa resistencia e no
// teste de transistores. Opcional: se nao estiver soldado, o firmware desliga
// automaticamente as funcoes que dependem dele (veja hal_probe_available()).
#define PIN_PROBE_DRIVE_LOW   22
#define PROBE_REF_RESISTOR_LOW 470.0f    // ohms, faixa de baixa impedancia

// Descarga de capacitor: dreno de MOSFET/transistor que aterra a ponta 1.
#if SONDVOLT_HW_REV >= 1
  #define PIN_CAP_DISCHARGE   16        // Rev B: pino dedicado
  #define CAP_DISCHARGE_SHARED_WITH_LED 0
#else
  #define PIN_CAP_DISCHARGE   17        // Rev A: compartilhado com LED azul
  #define CAP_DISCHARGE_SHARED_WITH_LED 1
#endif

// ============================================================================
// 6. BARRAMENTO I2C (INA219, e futuramente MLX90640)
// ============================================================================
#define PIN_I2C_SDA           27
#define PIN_I2C_SCL           22
#define I2C_FREQ_HZ           400000

#define INA219_ADDR           0x40
#define INA219_SHUNT_OHMS     0.1f
#define INA219_MAX_AMPS       3.2f
#define INA219_VBUS_MAX       26.0f

// Aliases historicos
#define PIN_INA_SDA           PIN_I2C_SDA
#define PIN_INA_SCL           PIN_I2C_SCL
#define INA_I2C_ADDR          INA219_ADDR
#define INA_SHUNT_OHMS        INA219_SHUNT_OHMS
#define INA_MAX_AMPS          INA219_MAX_AMPS
#define INA_VBUS_MAX          INA219_VBUS_MAX

// NOTA DE CONFLITO: PIN_I2C_SDA (27) coincide com PIN_PROBE_DRIVE e
// PIN_I2C_SCL (22) coincide com PIN_PROBE_DRIVE_LOW. Isso e intencional na
// Rev A: os dois recursos compartilham o conector de expansao P3 e o firmware
// faz arbitragem temporal (hal_bus_acquire / hal_bus_release em hal.h).
// Nunca acione o drive das pontas com o INA219 no meio de uma transacao.

// ============================================================================
// 7. SAIDAS DIGITAIS - LEDs indicadores e buzzer
// ============================================================================
#define PIN_LED_RED           4
#define PIN_LED_GREEN         16
#define PIN_LED_BLUE          17

// Na CYD o LED RGB e de ANODO COMUM: nivel BAIXO acende.
#define LED_ACTIVE_LEVEL      LOW
#define LED_IDLE_LEVEL        HIGH

#define PIN_BUZZER            26
#define LEDC_CH_BUZZER        2
#define BUZZER_PWM_BITS       10
#define PIN_SPEAKER           PIN_BUZZER

// ============================================================================
// 8. ONEWIRE (DS18B20)
// ============================================================================
#if SONDVOLT_HW_REV >= 1
  #define PIN_ONEWIRE         32        // Rev B: pino dedicado
  #define ONEWIRE_SHARED_WITH_LED 0
#else
  #define PIN_ONEWIRE         4         // Rev A: compartilhado com LED vermelho
  #define ONEWIRE_SHARED_WITH_LED 1
#endif
#define ONEWIRE_MAX_DEVICES   4
#define DS18B20_FAMILY        0x28
#define PIN_DS18B20           PIN_ONEWIRE

// ============================================================================
// 9. PLACA DE EXPANSAO "BANCADA" (Rev C)
// ============================================================================
// Todos os recursos abaixo entram pelo barramento I2C ja existente. Nenhum
// deles consome um GPIO do ESP32, porque nao ha nenhum sobrando.
//
//   PCF8574   0x20   8 linhas de controle digitais
//   MLX90640  0x33   matriz termica 32x24
//   DS3231    0x68   relogio de tempo real com bateria (opcional)
//
// O firmware detecta cada um no boot. O que nao estiver presente tem a
// funcao correspondente desativada, nunca simulada.

#define PCF8574_ADDR          0x20    // A0=A1=A2 em GND
#define MLX90640_ADDR         0x33
#define DS3231_ADDR           0x68

// ----------------------------------------------------------------------------
// Mapa das 8 linhas do expansor
// ----------------------------------------------------------------------------
// Todas ativas em nivel ALTO, exceto onde indicado. O PCF8574 tem saida
// dreno-aberto com pull-up fraco interno: use-o para acionar transistores e
// entradas logicas, nunca para alimentar carga diretamente.
#define EXP_BOOST_ENABLE      0   // liga o MT3608 (fonte de 12 V para Zener)
#define EXP_RIPPLE_COUPLE     1   // insere o capacitor de acoplamento AC
#define EXP_SIGGEN_OUTPUT     2   // conecta o gerador de sinal na ponta 1
#define EXP_SCOPE_ATTEN       3   // atenuador do osciloscopio: 0=1x, 1=10x
#define EXP_CURVE_SENSE       4   // insere o resistor de 100R do tracador
#define EXP_PROBE_ISOLATE     5   // isola as pontas durante testes de rede
#define EXP_AUX_1             6   // reserva
#define EXP_AUX_2             7   // reserva

// ----------------------------------------------------------------------------
// Gerador de sinal
// ----------------------------------------------------------------------------
// Sai pelo GPIO22, o mesmo da excitacao de baixa impedancia, arbitrado pela
// HAL. Onda quadrada apenas: os dois DACs do ESP32 (GPIO25 e GPIO26) estao
// ocupados pelo clock do touch e pelo buzzer.
#define PIN_SIGGEN_OUT        PIN_PROBE_DRIVE_LOW
#define LEDC_CH_SIGGEN        4
#define SIGGEN_MIN_HZ         1
#define SIGGEN_MAX_HZ         100000
#define SIGGEN_PWM_BITS       8

// ----------------------------------------------------------------------------
// Osciloscopio
// ----------------------------------------------------------------------------
// Amostra a ponta 1 por DMA do I2S, que garante espacamento exato entre
// amostras - coisa que analogRead() num laco nao consegue.
#define PIN_SCOPE_INPUT       PIN_ADC_PROBE1
#define SCOPE_ADC_UNIT        1
#define SCOPE_ADC_CHANNEL     ADC_CH_PROBE1
#define SCOPE_BUFFER_SIZE     512      // amostras por captura
#define SCOPE_MAX_RATE_HZ     200000   // limite pratico util do ADC1
#define SCOPE_MIN_RATE_HZ     1000
#define SCOPE_ATTEN_RATIO     10.0f    // divisor da ponta 10x

// ----------------------------------------------------------------------------
// Entrada acoplada em AC (medidor de ripple)
// ----------------------------------------------------------------------------
// Le a ondulacao que anda em cima de um trilho DC - o teste que denuncia
// capacitor de filtro ressecado. Usa a ponta 2, que sobrava.
#define PIN_RIPPLE_INPUT      PIN_ADC_PROBE2
#define RIPPLE_COUPLING_NF    100.0f   // capacitor de acoplamento
#define RIPPLE_BIAS_VOLTS     1.65f    // meia escala, o repouso da entrada

// ----------------------------------------------------------------------------
// Fonte auxiliar de 12 V (teste de Zener)
// ----------------------------------------------------------------------------
#define BOOST_OUTPUT_VOLTS    12.0f
#define ZENER_SERIES_RESISTOR 4700.0f  // limita a corrente de teste a ~2 mA
#define ZENER_MAX_VOLTS       11.0f    // acima disso o boost satura

// ----------------------------------------------------------------------------
// Tracador de curva I-V
// ----------------------------------------------------------------------------
#define CURVE_SENSE_RESISTOR  100.0f   // shunt do tracador
#define CURVE_STEPS           64       // pontos por varredura
#define LEDC_CH_CURVE         6

// ============================================================================
// 10. VERIFICACOES DE INTEGRIDADE
// ============================================================================

// Verdadeiro apenas para GPIOs que possuem driver de saida no ESP32.
#define IS_OUTPUT_CAPABLE_PIN(g)  ((g) >= 0 && (g) <= 33 && \
                                   !((g) >= 6 && (g) <= 11))

// Verdadeiro para pinos que so aceitam leitura.
#define IS_INPUT_ONLY_PIN(g)      ((g) >= 34 && (g) <= 39)

// Verdadeiro para pinos ligados ao ADC1 (unico utilizavel com WiFi ativo).
#define IS_ADC1_PIN(g)            (((g) >= 32 && (g) <= 39))

// Aliases historicos
#define IS_ADC_PIN(g)             IS_ADC1_PIN(g)
#define IS_OUTPUT_PIN(g)          IS_OUTPUT_CAPABLE_PIN(g)

// Falha o build cedo se alguem reintroduzir uma pinagem impossivel.
static_assert(IS_INPUT_ONLY_PIN(PIN_ADC_PROBE1),
              "PIN_ADC_PROBE1 deve ser um pino de entrada do ADC1");
static_assert(IS_OUTPUT_CAPABLE_PIN(PIN_PROBE_DRIVE),
              "PIN_PROBE_DRIVE precisa ter driver de saida");
static_assert(IS_OUTPUT_CAPABLE_PIN(PIN_CAP_DISCHARGE),
              "PIN_CAP_DISCHARGE precisa ter driver de saida");
static_assert(IS_OUTPUT_CAPABLE_PIN(PIN_BUZZER),
              "PIN_BUZZER precisa ter driver de saida");
static_assert(IS_ADC1_PIN(PIN_ADC_ZMPT),
              "O ZMPT101B precisa estar em um canal do ADC1");

static_assert(IS_ADC1_PIN(PIN_SCOPE_INPUT),
              "A entrada do osciloscopio precisa estar no ADC1");
static_assert(IS_ADC1_PIN(PIN_RIPPLE_INPUT),
              "A entrada de ripple precisa estar no ADC1");
static_assert(IS_OUTPUT_CAPABLE_PIN(PIN_SIGGEN_OUT),
              "A saida do gerador de sinal precisa ter driver de saida");
static_assert(LEDC_CH_BACKLIGHT != LEDC_CH_BUZZER &&
              LEDC_CH_BUZZER    != LEDC_CH_SIGGEN &&
              LEDC_CH_SIGGEN    != LEDC_CH_CURVE,
              "Cada periferico PWM precisa de um canal LEDC proprio");

// ============================================================================
// 11. ALIASES DE COMPATIBILIDADE
// ============================================================================
#define PIN_PROBE_1           PIN_ADC_PROBE1
#define PIN_PROBE_2           PIN_ADC_PROBE2
#define PIN_ZMPT_AC           PIN_ADC_ZMPT
#define PIN_ZMPT_OUT          PIN_ADC_ZMPT
#define SD_CS                 PIN_SD_CS

#endif // PINS_H
