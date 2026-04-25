# 📍 Referência Completa de Pinagem

Este documento é a referência completa de todos os pinos da placa **ESP32-2432S028R (Cheap Yellow Display)** utilizada no **Sondvolt**.

---

## 1. Visão Geral da Placa

### 1.1 Diagrama da Placa (Vista Traseira)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                    ESP32-2432S028R (Vista Traseira)                        │
│                                                                             │
│    ┌─────────────────────────────────────────────────────────────────┐     │
│    │                                                                  │     │
│    │   ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐        ┌─────────┐        │     │
│    │   │ USB │  │BOOT │  │RST │  │ SD │        │  LDR    │        │     │
│    │   │ C   │  │ BTN │  │    │  │card│        │ (Light) │        │     │
│    │   └─────┘  └─────┘  └─────┘  └─────┘        └─────────┘        │     │
│    │                                                                  │     │
│    │   ┌─────────────────┐                    ┌────────────┐      │     │
│    │   │ Expansion IO1   │                    │ Expansion IO2  │      │     │
│    │   │  ┌──┬──┬──┬──┐ │                    │  ┌──┬──┬──┬──┐│      │     │
│    │   │  │21│22│35│ G│ │                    │  │3V│27│22│ G││      │     │
│    │   │  └──┴──┴──┴──┘ │                    │  └──┴──┴──┴──┘│      │     │
│    │   └─────────────────┘                    └────────────┘      │     │
│    │                                                                  │     │
│    │                    ┌──────────────────────┐                    │     │
│    │                    │   ESP32-WROOM-32      │                    │     │
│    │                    │   (Módulo Principal)  │                    │     │
│    │                    └──────────────────────┘                    │     │
│    │                                                                  │     │
│    │   ┌────────┐  ┌────────┐  ┌────────┐                             │     │
│    │   │Speaker│  │RGB LED │  │Batt   │                             │     │
│    │   │ Buzzer │  │   ●   │  │  conn │                             │     │
│    │   └────────┘  └────────┘  └────────┘                             │     │
│    │                                                                  │     │
│    └─────────────────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Identificação dos Conectores

| Conector | Função | Tipo | Pitch |
|:---|:---|:---|:---|
| **IO1** | Analógico/I2C (Probe 1/SCL) | JST 1.25mm 4p | 1.25mm |
| **IO2** | I2C (SDA/SCL) | JST 1.25mm 4p | 1.25mm |
| **USB-C** | Alimentação/Programação | USB-C | — |
| **USB-micro**| Alimentação/Programação | USB-micro | — |
| **SD Card** | Leitor microSD | Slot push-push | — |
| **Speaker** | Conector Áudio | JST 1.25mm 2p | 1.25mm |

---

## 2. Mapeamento de GPIOs

### 2.1 Periféricos Fixos ( pré-definidos)

| Periférico | Função | GPIO |Barramento| Observação |
|:---|:---|:---|:---|:---|
| **Display TFT** | CS | GPIO 15 | VSPI | Chip Select |
| | DC | GPIO 2 | VSPI | Data/Command |
| | SCK | GPIO 14 | VSPI | Clock |
| | MOSI | GPIO 13 | VSPI | Master Out |
| | MISO | GPIO 12 | VSPI | Master In |
| **Backlight** | Brilho | GPIO 21 | PWM | Controle PWM |
| **SD Card** | CS | GPIO 5 | HSPI | Chip Select |
| | SCK | GPIO 18 | HSPI | Clock |
| | MOSI | GPIO 23 | HSPI | Master Out |
| | MISO | GPIO 19 | HSPI | Master In |
| **Touchscreen** | CS | GPIO 33 | SoftSPI | Chip Select |
| | CLK | GPIO 25 | SoftSPI | Clock |
| | MOSI | GPIO 32 | SoftSPI | Master Out |
| | MISO | GPIO 39 | SoftSPI | Master In |
| | IRQ | GPIO 36 | — | Interrupção |
| **LED RGB** | Vermelho | GPIO 4 | — | LED integrado (Comp. OneWire) |
| | Verde | GPIO 16 | — | LED integrado |
| | Azul | GPIO 17 | — | LED integrado (Comp. Discharge) |
| **Buzzer** | Áudio | GPIO 26 | DAC | AUDIO: IO26 |
| **ADC Probes** | Probe 1 | GPIO 35 | ADC1_CH7 | Expansion IO1 |
| | Probe 2 | GPIO 34 | ADC1_CH6 | Internal Solder |
| | ZMPT AC | GPIO 36 | ADC1_CH0 | Shared with Touch IRQ |

### 2.2 Barramentos de Expansão

| Função | GPIO | Barramento | Endereço |
|:---|:---|:---|:---|
| **I2C SDA** | GPIO 27 | I2C0 | 0x40 (INA219) |
| **I2C SCL** | GPIO 22 | I2C0 | — |
| **OneWire** | GPIO 4 | OneWire | — |

---

## 3. Detalhamento por Conector

### 3.1 CN1 — Conector Analógico

Header de 4 pinos para 连接传感器 e probes.

```
IO1 (Vista Superior) - Perto do ESP32
┌─────────────────────────────┐
│  1   │  2   │  3   │  4    │
│ GND  │ IO35 │ IO22 │ IO21  │
├──────┼──────┼──────┼──────┤
│      │PROBE1│ SCL  │ BLIGHT│
└─────────────────────────────┘
```

| Pino | GPIO | Função | Tipo | Descrição |
|:---:|:---:|:---|:---|:---|
| 1 | GND | Terra | Power | Terra comum |
| 2 | GPIO 35 | Probe 1 | Entrada ADC | Sinal principal do probe |
| 3 | GPIO 22 | I2C SCL | Output | Clock I2C |
| 4 | GPIO 21 | Backlight | Output | Controle brilho (NÃO usar p/ dados) |

> [!CAUTION]
> O pino 4 (5V/VIN) **não** é regulado! Use fonte de 5V estável ou 4.5-6V DC.

### 3.2 P3/J3 — Conector Digital

Conector JST PH de 4 pinos para módulos I2C e OneWire.

```
IO2 (Vista Superior) - Perto do SD Card
┌─────────────────────────────┐
│  1   │  2   │  3   │  4    │
│ GND  │ IO22 │ IO27 │ 3.3V  │
├──────┼──────┼──────┼──────┤
│      │ SCL  │ SDA  │ PWR   │
└─────────────────────────────┘
```

| Pino | GPIO | Função | Tipo | Descrição |
|:---:|:---:|:---|:---|:---|
| 1 | GND | Terra | Power | Terra comum |
| 2 | GPIO 22 | I2C SCL | Output | Clock I2C |
| 3 | GPIO 27 | I2C SDA | bidirecional | Dados I2C (INA219) |
| 4 | 3.3V | VCC | Power | Alimentação 3.3V |

### 3.3 Conector de Alimentação

```
┌─────────────────────────────────────┐
│         Conector USB-C              │
├─────────────────────────────────────┤
│  D+  │  D-  │  VBUS  │  GND   │
│      │      │  5V   │       │
└─────────────────────────────────────┘
```

| Pino | Função | Tensão | Corrente Máx |
|:---|:---|:---|:---|
| VBUS | 5V | 5V DC | 500mA (USB) |
| GND | Terra | 0V | — |

---

## 4. Tabela Resumo de GPIO

### 4.1 Todos os GPIOs Utilizados

| GPIO | Função | Tipo | Pull |備考 |
|:---:|:---|:---|:---|:---|
| **GPIO 0** | BOOT button | Input | Pull-up | Botão integrado |
| **GPIO 2** | TFT DC | Output | — | Display |
| **GPIO 4** | OneWire / LED Red| IO | — | **Compartilhado** |
| **GPIO 5** | SD CS | Output | — | SD Card |
| **GPIO 12** | TFT MISO | Input | — | Display |
| **GPIO 13** | TFT MOSI | Output | — | Display |
| **GPIO 14** | TFT SCK | Output | — | Display |
| **GPIO 15** | TFT CS | Output | — | Display |
| **GPIO 16** | LED Verde | Output | — | LED RGB |
| **GPIO 17** | LED Azul / Discharge| Output | — | **Compartilhado** |
| **GPIO 18** | SD SCK | Output | — | SD Card |
| **GPIO 19** | SD MISO | Input | — | SD Card |
| **GPIO 21** | Backlight PWM | Output | — | Display |
| **GPIO 22** | I2C SCL | Output | — | Expansion IO1/IO2 |
| **GPIO 23** | SD MOSI | Output | — | SD Card |
| **GPIO 25** | Touch CLK | Output | — | Touchscreen |
| **GPIO 26** | Audio / PWM | Output | DAC | AUDIO: IO26 |
| **GPIO 27** | I2C SDA | IO | — | Expansion IO2 |
| **GPIO 32** | Touch MOSI | Output | — | Touchscreen |
| **GPIO 33** | Touch CS | Output | — | Touchscreen |
| **GPIO 34** | Probe 2 Input | **Input only** | — | Internal |
| **GPIO 35** | Probe 1 Input | **Input only** | — | Expansion IO1 |
| **GPIO 36** | ZMPT / Touch IRQ | Input | — | Shared |
| **GPIO 39** | Touch MISO | Input | — | Touchscreen |

### 4.2 GPIOs Input-Only (Atenção!)

```
! (images/pinout_input_only.png)
*[Destaque para GPIOs 34, 35, 36, 39 como entrada pura]*
```

Os seguintes GPIOs são **entrada pura** no ESP32:

| GPIO | Função | Advertência |
|:---:|:---|:---|
| **GPIO 34** | ADC1_CH6 | Não tem pull-up/down interno |
| **GPIO 35** | ADC1_CH7 | Não tem pull-up/down interno |
| **GPIO 36** | ADC1_CH0 | Não tem pull-up/down interno |
| **GPIO 39** | ADC1_CH3 | Não tem pull-up/down interno |

> [!WARNING]
> Estes GPIOs **não podem ser usados como saída**! Não tente controlar LEDs ou outros componentes com eles.

---

## 5. Alimentação e Energia

### 5.1 Fontes de Alimentação

| Pino | Tensão | Uso Recomendado | Corrente Máx |
|:---|:---|:---|:---|
| **5V/VIN** | 4.5V - 6V | Módulos externos (ZMPT) | 500mA |
| **3.3V** | 3.3V | Sensores I2C, DS18B20 | 500mA |
| **GND** | 0V | Terra comum | — |

### 5.2 Diagrama de Alimentação

```
┌─────────────────────────────────────────────────────────────────────┐
│                      FONTES DE ALIMENTAÇÃO                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                          │
│   USB-C (5V) ─────┬──► 5V/VIN ──► ZMPT101B           │
│                  │                                       │
│                  ├──► Regulador 3.3V ──► 3.3V ──► INA219│
│                  │                         │         DS18B20   │
│                  │                         │         Pull-ups │
│                  │                         │                  │
│                  │                         ▼                  │
│                  │                      ┌──────────┐           │
│                  │                      │   GND    │           │
│                  │                      │   (0V)   │           │
│                  └────────────────────┴──────────┘           │
│                                                          │
└──────────────────────────────────────────────────────────────┘
```

---

## 6. Cores Recomendadas para Fios

### 6.1 Padrão de Cores

| Função | Cor Recomendada | Alternativa | Bitola |
|:---|:---|:---|:---|
| **5V / VCC** | Vermelho | Laranja | 24 AWG |
| **3.3V** | Laranja | Amarelo | 26 AWG |
| **GND** | Preto | Marrom | 24 AWG |
| **Sinal Analógico** | Amarelo | Roxo | 26 AWG |
| **I2C SDA** | Verde | Azul | 26 AWG |
| **I2C SCL** | Azul | Verde | 26 AWG |
| **OneWire DQ** | Branco | Cinzento | 26 AWG |
| **Probe (+)** | Vermelho | — | 22 AWG |
| **Probe (-)** | Preto | — | 22 AWG |
| **LED/RGB** | Verde/VMelho/Azul | — | 26 AWG |

### 6.2 Legenda Visual

```
! (images/pinout_wire_colors.png)
*[Foto dos fios coloridos para cada função]*
```

---

## 7. Tabela de Conflictos

### 7.1 Conflitos Conhecidos

| GPIO | Conflito | Impacto | Solução |
|:---|:---|:---|:---|
| **GPIO 4** | LED Azul ↔ OneWire | LED pisca com dados | Use LED como indicador de dados |
| **GPIO 26** | Buzzer ↔ PWM | Speaker toca ao gerar PWM | Desconecte speaker se necessário |
| **GPIO 35** | Probe ↔ Button | — | — |
| **GPIO 36** | Touch IRQ ↔ — | — | Usado internamente |

### 7.2 Barramentos Compartilhados

```
┌──────────────────────────────────────────────────────────┐
│            BARRAMENTOS E COMPARTILHAMENTOS                │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  VSPI: GPIO 15(CS), 2(DC), 14(SCK), 13(MOSI), 12(MISO)  │
│    └─ Display TFT                                         │
│                                                          │
│  HSPI: GPIO 5(CS), 18(SCK), 23(MOSI), 19(MISO)        │
│    └─ SD Card                                             │
│                                                          │
│  SoftSPI: GPIO 33(CS), 25(CLK), 32(MOSI), 39(MISO),36   │
│    └─ Touchscreen                                        │
│                                                          │
│  I2C0: GPIO 27(SDA), 22(SCL)                            │
│    └─ INA219 (0x40) + outros dispositivos (0x??)        │
│                                                          │
│  OneWire: GPIO 4                                        │
│    └─ DS18B20                                            │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

---

## 8. Pinout Detalhado por Módulo

### 8.1 Mapeamento ZMPT101B

```
┌─────────────────┐         ┌──────────────────────────────┐
│    ZMPT101B      │         │         ESP32-CYD            │
├─────────────────┤         ├──────────────────────────────┤
│ VCC  ───────────┼─────────│ 5V (CN1 pino 4) + Cap 10µF  │
│ GND ────────────┼─────────│ GND (CN1 pino 3)            │
│ OUT ────────────┼─────────│ GPIO 34 + Cap 100nF         │
│                 │         │ (Resistor 10kΩ para GND)    │
└─────────────────┘         └──────────────────────────────┘
```

### 8.2 Mapeamento INA219

```
┌─────────────────┐         ┌─────────────────────┐
│     INA219      │         │   ESP32-CYD         │
├─────────────────┤         ├─────────────────────┤
│ VCC  ───────────┼─────────│ 3.3V                │
│ GND ────────────┼─────────│ GND                 │
│ SDA ────────────┼─────────│ GPIO 27             │
│ SCL ────────────┼─────────│ GPIO 22             │
└─────────────────┘         └─────────────────────┘
```

### 8.3 Mapeamento DS18B20

```
┌─────────────────┐         ┌─────────────────────┐
│    DS18B20      │         │   ESP32-CYD         │
├─────────────────┤         ├──���─���────────────────┤
│ VCC  ───────────┼─────────│ 3.3V                │
│ GND ────────────┼─────────│ GND                 │
│ DQ  ────────────┼─────────│ GPIO 4              │
│        (4.7kΩ)  │         │ (pull-up para 3.3V)│
└─────────────────┘         └─────────────────────┘
```

### 8.4 Mapeamento Probes

```
┌─────────────────┐         ┌─────────────────────┐
│      Probes     │         │   ESP32-CYD         │
├─────────────────┤         ├─────────────────────┤
│ Probe 1 (Red)   ├─────────│ GPIO 35 (IO1 p2)    │
│ Probe 2 (Black) ├─────────│ GND (IO1 pino 1)    │
└─────────────────┘         └─────────────────────┘
```

---

## 9. Referência Rápida

### 9.1 Pinagem Resumida

```
┌────────────────────────────────────────────────────────────────┐
│                    REFERÊNCIA RÁPIDA                           │
├─────────────────┬──────────┬─────────────────────────────────┤
│ Módulo          │ Pino CYD │ Função                          │
├─────────────────┼──────────┼─────────────────────────────────┤
│ Display TFT     │ 15,2,14, │ CS,DC,SCK,MOSI,MISO            │
│                 │ 13,12   │                                 │
│ Touchscreen     │ 33,25,32,│ CS,CLK,MOSI,MISO,IRQ           │
│                 │ 39,36   │                                 │
│ SD Card        │ 5,18,23, │ CS,SCK,MOSI,MISO              │
│                 │ 19      │                                 │
│ Backlight     │ 21       │ PWM                            │
│ LED RGB       │ 16,17,4  │ Verde,Vermelho,Azul            │
│ Buzzer        │ 26       │ DAC/PWM                        │
│ ZMPT101B      │ 34       │ Entrada ADC                    │
│ Probe        │ 35       │ Entrada ADC                   │
│ INA219 (I2C) │ 27,22   │ SDA,SCL                      │
│ DS18B20       │ 4       │ OneWire DQ                   │
└─────────────────┴──────────┴───────────────────────────────┘
```

### 9.2 Imagem Resumo

```
! (images/pinout_cheatsheet.png)
*[Imagem resumo paraimpressão]*
```

---

## 10. Notas Importantes

> [!IMPORTANT]
> 1. **GPIO 34/35** são entrada pura — não use como saída!
> 2. **GPIO 4** controla LED azul E OneWire — pode haver interferência visual
> 3. Não conecte dispositivos I2C com endereço **0x40** (INA219)
> 4. Use **3.3V** para DS18B20 e INA219 — **não** 5V!
> 5. O pino **5V/VIN** é direto — use fonte estável

---

_Fim da Referência de Pinagem_

---

> [!TIP]
> Paravisualização interativa, consulte o diagrama interactivo em: `docs/INDEX.md`
> Para troubleshooting, consulte `docs/TROUBLESHOOTING.md`