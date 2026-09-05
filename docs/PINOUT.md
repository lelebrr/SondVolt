# Pinagem — ESP32-2432S028R (Cheap Yellow Display)

Este documento é a referência de fiação do Sondvolt. A fonte de verdade no código é `src/pins.h`, que contém `static_assert` verificando estas regras em tempo de compilação.

---

## A regra que explica quase tudo

> **GPIO 34, 35, 36, 37, 38 e 39 do ESP32 são ENTRADA APENAS.**
>
> Não possuem driver de saída nem resistores de pull internos. `pinMode(35, OUTPUT)` compila sem erro, executa sem erro e **não tem efeito algum**.

A v3.2 tentava usar GPIO34 e GPIO35 como saída para excitar as pontas de prova. É por isso que resistência e capacitância nunca funcionaram, e é a razão de existir o circuito de excitação descrito abaixo.

O código expressa essa regra em macros:

```c
#define IS_OUTPUT_CAPABLE_PIN(g)  ((g) >= 0 && (g) <= 33 && !((g) >= 6 && (g) <= 11))
#define IS_INPUT_ONLY_PIN(g)      ((g) >= 34 && (g) <= 39)
#define IS_ADC1_PIN(g)            ((g) >= 32 && (g) <= 39)
```

GPIO6 a GPIO11 estão ligados à flash SPI interna e nunca devem ser usados.

---

## Mapa completo (Rev A — fiação original)

### Display TFT ILI9341

| Sinal | GPIO | Observação |
| :--- | :--- | :--- |
| MOSI | 13 | fixo na placa |
| MISO | 12 | fixo na placa |
| SCK | 14 | fixo na placa |
| CS | 15 | ativo baixo |
| DC | 2 | 0 = comando, 1 = dado |
| RST | 0 | reset por software |
| BL | 21 | backlight por PWM, canal LEDC 0 |

Estes pinos são consumidos pela TFT_eSPI através das `build_flags` do `platformio.ini`. Estão repetidos em `pins.h` apenas para consulta.

### Touchscreen XPT2046

| Sinal | GPIO | Observação |
| :--- | :--- | :--- |
| MOSI | 32 | barramento HSPI dedicado |
| MISO | 39 | entrada apenas — correto para MISO |
| SCK | 25 | |
| CS | 33 | ativo baixo |
| IRQ | 36 | **não usado** — veja conflitos |

O firmware faz *polling* (`touch.touched()`) em vez de usar interrupção. Isso libera o GPIO36 para o ADC do ZMPT101B.

### Cartão MicroSD

| Sinal | GPIO |
| :--- | :--- |
| MOSI | 23 |
| MISO | 19 |
| SCK | 18 |
| CS | 5 |

> A documentação da v3.2 afirmava que o SD compartilhava barramento com a TFT e que era preciso desativar o CS do display antes de cada acesso. **Isso está errado** — o slot MicroSD da CYD tem pinos exclusivos. O mutex de SPI continua sendo usado, mas por causa da concorrência entre tarefas, não por compartilhamento de pinos.

O firmware tenta 10 MHz e cai automaticamente para 4 MHz em cartões que não aguentam.

### Entradas analógicas

Todas em **ADC1**. O ADC2 fica indisponível quando o WiFi está ativo, então nunca deve ser usado para medição.

| Função | GPIO | Canal |
| :--- | :--- | :--- |
| Ponta 1 (leitura) | 35 | ADC1_CH7 |
| Ponta 2 (leitura) | 34 | ADC1_CH6 |
| ZMPT101B (rede AC) | 36 | ADC1_CH0 |

### Excitação das pontas — novo na v4.0

| Função | GPIO | Resistor de referência |
| :--- | :--- | :--- |
| Drive faixa alta | 27 | 10 kΩ 1% |
| Drive faixa baixa | 22 | 470 Ω 1% |
| Descarga de capacitor | 17 | dreno de MOSFET |

Sem estes pinos não há como medir resistência nem capacitância. O esquema de ligação está em [WIRING.md](WIRING.md).

### I²C

| Sinal | GPIO |
| :--- | :--- |
| SDA | 27 |
| SCL | 22 |

Frequência de 400 kHz. INA219 no endereço 0x40.

### Saídas digitais

| Função | GPIO | Observação |
| :--- | :--- | :--- |
| LED vermelho | 4 | **ânodo comum: LOW acende** |
| LED verde | 16 | ânodo comum |
| LED azul | 17 | ânodo comum |
| Buzzer | 26 | PWM, canal LEDC 2 |
| OneWire (DS18B20) | 4 | |

---

## Conflitos conhecidos (Rev A)

Quatro pinos têm dois donos na fiação original. O firmware resolve todos em software, mas é importante saber que existem.

### GPIO4 — LED vermelho **e** barramento OneWire

**Sintoma sem tratamento:** com o LED vermelho aceso, o pino fica em nível alto e o DS18B20 não consegue responder. O sensor de temperatura "some" exatamente quando um alerta está ativo — o pior momento possível.

**Tratamento:** `hal_bus_acquire(HAL_BUS_ONEWIRE)` apaga o LED, memoriza que ele estava aceso, empresta o pino, e `hal_bus_release()` restaura o LED ao estado anterior.

### GPIO17 — LED azul **e** descarga de capacitor

**Sintoma sem tratamento:** acender o LED azul aciona o MOSFET de descarga, aterrando a ponta 1 durante uma medição.

**Tratamento:** mesma arbitragem, via `HAL_BUS_DISCHARGE`.

### GPIO27 e GPIO22 — I²C **e** excitação das pontas

**Sintoma sem tratamento:** acionar o drive das pontas no meio de uma transação com o INA219 corrompe os dois.

**Tratamento:** `HAL_BUS_PROBE_DRIVE` e `HAL_BUS_I2C` se excluem mutuamente. Nunca acione o drive com uma leitura do INA219 em andamento.

### GPIO36 — ZMPT101B **e** IRQ do touchscreen

**Sintoma sem tratamento:** habilitar `attachInterrupt()` no touch tornaria as leituras do sensor AC inúteis.

**Tratamento:** o firmware faz *polling* do touch. `TOUCH_IRQ_IS_USED` está definido como 0 em `pins.h` e não deve ser alterado na Rev A.

---

## As três revisões

```mermaid
flowchart LR
    A["<b>Rev A</b><br/>fiação original"] --> B["<b>Rev B</b><br/>OneWire e descarga<br/>em pinos próprios"]
    B --> C["<b>Rev C</b><br/>+ expansor PCF8574<br/>+ boost 12V<br/>+ atenuador<br/>+ acoplamento AC"]

    style A fill:#1f2937,stroke:#6b7280,color:#e5e7eb
    style B fill:#164e63,stroke:#22d3ee,color:#e5e7eb
    style C fill:#14532d,stroke:#4ade80,color:#e5e7eb
```

| | Rev A | Rev B | Rev C |
|:--|:--|:--|:--|
| OneWire | GPIO4 *(com LED)* | GPIO32 | GPIO32 |
| Descarga | GPIO17 *(com LED)* | GPIO16 | GPIO16 |
| Expansor I²C | — | — | PCF8574 0x20 |
| Osciloscópio | 1x apenas | 1x apenas | 1x e 10x |
| Zener | até 3,3 V | até 3,3 V | até 11 V |
| Ripple | — | — | sim |
| Câmera térmica | — | — | MLX90640 |
| Compilar com | `-e cyd` | `-e cyd-revb` | `-e cyd-revc` |

Um firmware da Rev C **roda nas outras duas**: o que não encontra hardware se desativa sozinho.

---

## O expansor de linhas (Rev C)

> [!IMPORTANT]
> **A CYD não tem nenhum GPIO livre.** Depois de display, touch, cartão, ADCs, LEDs, buzzer e I²C, os 24 pinos utilizáveis do ESP32-WROOM acabaram. Qualquer recurso novo precisa entrar pelo I²C.

O **PCF8574** em 0x20 dá 8 linhas de controle sem gastar pino:

| Linha | Função | Aciona |
|:--|:--|:--|
| P0 | Habilita a fonte de 12 V | EN do MT3608 |
| P1 | Insere o acoplamento AC | JFET ou relé |
| P2 | Conecta a saída do gerador | 2N7000 |
| P3 | Atenuador 1x / 10x | relé de sinal |
| P4 | Insere o shunt do traçador | 2N7000 |
| P5 | Isola as pontas | relé |
| P6, P7 | Reserva | — |

> As saídas são **dreno aberto** com pull-up fraco. Acionam transistor, nunca carga direta.

### Endereços I²C ocupados

| Endereço | Dispositivo | Obrigatório? |
|:--|:--|:--|
| 0x20 | PCF8574 (expansor) | Rev C |
| 0x33 | MLX90640 (câmera térmica) | opcional |
| 0x40 | INA219 (corrente) | opcional |
| 0x68 | DS3231 (RTC) | opcional, reservado |

---

## Rev B — fiação recomendada para montagens novas

Se você está montando do zero, não reproduza os conflitos. A Rev B move dois sinais para pinos livres:

| Sinal | Rev A | Rev B | Motivo |
| :--- | :--- | :--- | :--- |
| OneWire | 4 | 32 | libera o LED vermelho |
| Descarga de capacitor | 17 | 16 | libera o LED azul |

Os conflitos de GPIO27/22 (I²C vs excitação) e GPIO36 (ZMPT vs IRQ) permanecem, porque são limitação dos conectores de expansão da própria CYD — mas ambos são inofensivos com a arbitragem em software.

Para compilar:

```bash
pio run -t upload -e cyd-revb    # Rev B
pio run -t upload -e cyd-revc    # Rev C, com a placa Bancada
```

Ou defina `-DSONDVOLT_HW_REV=1` nas suas próprias `build_flags`. O `pins.h` seleciona os pinos automaticamente:

```c
#if SONDVOLT_HW_REV >= 1
  #define PIN_ONEWIRE         32
  #define ONEWIRE_SHARED_WITH_LED 0
#else
  #define PIN_ONEWIRE         4
  #define ONEWIRE_SHARED_WITH_LED 1
#endif
```

Com `SONDVOLT_HW_REV=1`, `hal_init()` registra os barramentos sem rival e a arbitragem vira uma operação de custo zero.

---

## Pinos livres

Depois de tudo alocado, sobram poucos GPIOs na CYD:

| GPIO | Situação |
| :--- | :--- |
| 1, 3 | UART0 (serial de depuração) — evite |
| 16 | livre na Rev A, usado na Rev B |
| 32 | usado pelo touch (MOSI) na Rev A |
| 34 | ponta 2, entrada apenas |

Os conectores CN1 e P3 da CYD são o caminho prático para acessar os pinos de expansão.

---

## Verificação em tempo de compilação

`pins.h` termina com asserções que quebram o build se a pinagem for reintroduzida de forma impossível:

```c
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
```

Se você mudar a pinagem e o build falhar em uma destas linhas, o compilador está impedindo a repetição do bug que travou a v3.2.

---

## Diagnóstico na tela

`Mais > Diagnóstico` mostra o resultado do autoteste de cada subsistema. Se **Pontas de prova** aparecer como FALHA, o circuito de excitação não está montado ou está com problema — o firmware detecta isso alternando o nível de `PIN_PROBE_DRIVE` e verificando se a leitura da ponta 1 acompanha.
