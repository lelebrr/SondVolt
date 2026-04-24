# Pinagem Detalhada - Component Tester PRO v2.0

## Índice

1. [Visão Geral da Pinagem](#1-visão-geral-da-pinagem)
2. [Pinos Analógicos](#2-pinos-analógicos)
3. [Pinos Digitais](#3-pinos-digitais)
4. [Tabela de Pinagem Completa](#4-tabela-de-pinagem-completa)
5. [Diagrama Visual](#5-diagrama-visual)
6. [Conexões por Componente](#6-conexões-por-componente)
7. [Notas Importantes](#7-notas-importantes)

---

## 1. Visão Geral da Pinagem

### Pinos Usados

```
┌──────────────────────────────────────────────────────────────────────┐
│                    PINAGEM RESUMO                           │
├──────────────────────────────────────────────────────────────────────┤
│                                                              │
│  Pino              Função              Tipo                    │
│  ───────────────────────────────────────────────────────     │
│  A0              Probe 1 (+)         Entrada Analógica      │
│  A1              Probe 2 (-)         Entrada Analógica      │
│                                                              │
│  D2              Botão UP            Entrada (Pull-up)      │
│  D3              Botão DOWN          Entrada (Pull-up)      │
│  D4              SD Card CS         Saída (SPI)          │
│  D5              Botão LEFT         Entrada (Pull-up)      │
│  D6              Botão RIGHT        Entrada (Pull-up)      │
│  D7              Botão OK          Entrada (Pull-up)      │
│  D8              Botão BCK         Entrada (Pull-up)      │
│  D9              Buzzer PWM        Saída PWM             │
│  D10             LED Verde        Saída Digital         │
│  D11             LED Vermelho     Saída Digital         │
│  D12             TFT CS          Saída SPI             │
│  D13             TFT DC          Saída Digital         │
│                                                              │
│  A2              TFT RST          Saída Digital         │
│  A3              DS18B20          Entrada/Saída OneWire │
│                                                              │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 2. Pinos Analógicos

### A0 - Probe 1 (Positivo)

```
┌────────────────────────────────────────────────────────────┐
│                    A0 - PROBE 1 (+)                       │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  Função: Pino positivo para medições                      │
│                                                            │
│  Modos de Uso:                                           │
│  ┌─────────────────────────────────────────────────────┐  │
│  │ Modo             │ Descrição                    │  │
│  ├──────────────────────────────────────────────────┤  │
│  │ OUTPUT LOW      │ GND virtual                   │  │
│  │ OUTPUT HIGH   │ +5V (via resistor interno)   │  │
│  │ INPUT        │ Leitura analógica 0-5V      │  │
│  │ INPUT_PULLUP │ Leitura com pull-up         │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                            │
│  Conexão Física:                                          │
│  ┌─────────────────────────────────────────────────────┐  │
│  │  A0 ───────────── Probe Vermelho                  │  │
│  │                     │                         │  │
│  │              ┌────┴────┐                    │  │
│  │              │ Componente │                    │  │
│  │              └──────────┘                    │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

### A1 - Probe 2 (Negativo)

```
┌────────────────────────────────────────────────────────────┐
│                    A1 - PROBE 2 (-)                       │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  Função: Pino negativo para medições                      │
│                                                            │
│  Similar a A0, usado como retorno/terra              │
│                                                            │
│  Conexão Física:                                          │
│  ┌─────────────────────────────────────────────────────┐  │
│  │  A1 ───────────── Probe Preto                    │  │
│  │                     │                         │  │
│  │              ┌────┴────┐                    │  │
│  │              │ Componente │                    │  │
│  │              └──────────┘                    │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

## 3. Pinos Digitais

### D2 - Botão UP

```
┌────────────────────────────────────────────────────────────┐
│                       D2 - UP                            │
├─────────��──────────────────────────────────────────────────┤
│                                                            │
│  Função: Navegar para cima no menu                       │
│                                                            │
│  Configuração: INPUT_PULLUP                              │
│                                                            │
│  Circuito:                                              │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                                                  │  │
│  │    +5V                                         │  │
│  │     │                                           │  │
│  │     └�                                          │  │
│  │      │ 10kΩ                                   │  │
│  │      │                                         │  │
│  │   ┌──┴──┐                                    │  │
│  │   │ Botã │o                                     │  │
│  │   └──┬──┘                                    │  │
│  │      │                                         │  │
│  └──────┴──────────────────────────────────────────┘  │
│                                                            │
│  Estado:                                                │
│  - solto = HIGH (5V via resistor)                       │
│  - pressionado = LOW (0V)                              │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

### D3 - Botão DOWN

- Conexão idêntica ao D2
- Função: Navegar para baixo no menu

### D5 - Botão LEFT

- Conexão idêntica ao D2
- Função: Esquerda (reservado para futuras funções)

### D6 - Botão RIGHT

- Conexão idêntica ao D2
- Função: Direita (reservado para futuras funções)

### D7 - Botão OK

- Conexão idêntica ao D2
- Função: Confirmar/Selecionar

### D8 - Botão BCK

- Conexão idêntica ao D2
- Função: Voltar/Sair

---

## 4. Tabela de Pinagem Completa

### Resumo Completo

| Pino | Nome Lógico | Função | Tipo | Componente |
|------|------------|-------|------|----------|
| **A0** | PROBE1_PIN | Probe positivo | Entrada analógica | Probes |
| **A1** | PROBE2_PIN | Probe negativo | Entrada analógica | Probes |
| **D2** | BTN_UP_PIN | Botão UP | Entrada pull-up | Botões |
| **D3** | BTN_DOWN_PIN | Botão DOWN | Entrada pull-up | Botões |
| **D4** | SD_CS_PIN | SD Card select | Saída SPI | SD Card |
| **D5** | BTN_LEFT_PIN | Botão LEFT | Entrada pull-up | Botões |
| **D6** | BTN_RIGHT_PIN | Botão RIGHT | Entrada pull-up | Botões |
| **D7** | BTN_OK_PIN | Botão OK | Entrada pull-up | Botões |
| **D8** | BTN_BACK_PIN | Botão BACK | Entrada pull-up | Botões |
| **D9** | BUZZER_PIN | Buzzer PWM | Saída PWM | Buzzer |
| **D10** | LED_GREEN_PIN | LED verde | Saída digital | LEDs |
| **D11** | LED_RED_PIN | LED vermelho | Saída digital | LEDs |
| **D12** | TFT_CS_PIN | TFT chip select | Saída SPI | TFT |
| **D13** | TFT_DC_PIN | TFT data/cmd | Saída digital | TFT |
| **A2** | TFT_RST_PIN | TFT reset | Saída digital | TFT |
| **A3** | ONEWIRE_BUS | OneWire bus | Entrada/saída | DS18B20 |

### Pinos Não Usados

| Pino | Disponibilidade |
|------|--------------|
| A4 | Disponível (TWI SDA) |
| A5 | Disponível (TWI SCL) |

---

## 5. Diagrama Visual

### Arduino Uno - Vista Superior

```
                    Arduino Uno R3 - Pinout
    ┌────────────────────────────────────────────────────────┐
    │                                                        │
    │   [A0] [A1] [A2] [A3] [A4] [A5]              │
    │    │    │    │    │    │    │                        │
    │    │    │    │    │    └─► SDA (I2C)            │
    │    │    │    │    └─────► SCL (I2C)            │
    │    │    │    └──────────► TFT RST              │
    │    │    └────────────────► DS18B20 (OneWire)         │
    │    └─────────────────────► Probe (-)                │
    │                                            │
    │   Probe (+) ◄────────── A0                     │
    │                                                        │
    │   ┌─────────────────────────────────────────────────────┐  │
    │   │                                                 │  │
    │   │   D13 D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 │  │
    │   │    │   │   │  │  │  │  │  │  │  │  │  │  │  │  │
    │   │ TFT  │LED │LED │   │  │  │  │  │  │  │  │  │
    │   │ DC  │V |V  │PWM│BCK│OK │RT │LF│SD │DW│UP│  │
    │   │     │   │   │  │  │  │  │CS│  │  │  │
    │   └──┴───┴───┴───┴──┴──┴──┴──┴──┴──┴──┴──┘  │
    │                                                        │
    │   5V  GND  GND  Vin                                     │
    │    │    │    │                                        │
    └────┴────┴────┴────────────────────────────────────────┘
```

### Mapeamento Visual

```
┌─────────────────────────────────────────────────────────────────────┐
│                    MAPA DE PINOS                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   FUNÇÃO              │ PINO FISICO │ NOME LÓGICO                 │
│   ────────────────────────────────────────────────────────────        │
│   PROBES                                                     │
│   ├─ Probe (+)      │   A0      │   PROBE1_PIN                │
│   └─ Probe (-)      │   A1      │   PROBE2_PIN                │
│                                                                     │
│   BOTÕES                                                        │
│   ├─ UP           │   D2      │   BTN_UP_PIN               │
│   ├─ DOWN         │   D3      │   BTN_DOWN_PIN             │
│   ├─ LEFT        │   D5      │   BTN_LEFT_PIN              │
│   ├─ RIGHT       │   D6      │   BTN_RIGHT_PIN             │
│   ├─ OK          │   D7      │   BTN_OK_PIN               │
│   └─ BACK        │   D8      │   BTN_BACK_PIN              │
│                                                                     │
│   DISPLAY TFT                                                     │
│   ├─ CS           │   D12     │   TFT_CS_PIN               │
│   ├─ DC           │   D13     │   TFT_DC_PIN               │
│   └─ RST          │   A2      │   TFT_RST_PIN               │
│                                                                     │
│   LEDs                                                           │
│   ├─ Verde        │   D10     │   LED_GREEN_PIN             │
│   └─ Vermelho    │   D11     │   LED_RED_PIN               │
│                                                                     │
│   OUTROS                                                        │
│   ├─ Buzzer       │   D9      │   BUZZER_PIN                │
│   ├─ SD CS       │   D4      │   SD_CS_PIN                │
│   └─ OneWire     │   A3      │   ONEWIRE_BUS_PIN         │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 6. Conexões por Componente

### TFT Display

```
┌─────────────────────────────────────────────────────────────────────┐
│                      TFT ILI9341                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  Pino TFT        │  Pino Arduino  │  Função                       │
│  ─────────────────���─���─────────────────────────────────────────       │
│  VCC            │  3.3V         │  Alimentação                  │
│  GND            │  GND          │  Terra                         │
│  CS             │  D12          │  Chip Select                  │
│  DC/RS          │  D13          │  Data/Command                 │
│  RST            │  A2           │  Reset                        │
│  SDI (MOSI)     │  D11          │  Data entrada (SPI)            │
│  SCK            │  D13          │  Clock (SPI)                 │
│  LED            │  3.3V        │  Backlight                   │
│  SDO (MISO)    │  D12          │  Data saída (SPI) - não usado  │
│                                                                     │
│  Nota: Usar level shifter 5V→3.3V para linhas de dados!          │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### SD Card Module

```
┌─────────────────────────────────────────────────────────────────────┐
│                      SD CARD MODULE                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  Pino Módulo      │  Pino Arduino  │  Função                       │
│  ─────────────────────────────────────────────────────────────       │
│  VCC            │  5V ou 3.3V│  Alimentação                  │
│  GND            │  GND        │  Terra                         │
│  CS             │  D4         │  Chip Select                  │
│  MOSI           │  D11        │  Data entrada                │
│  MISO           │  D12        │  Data saída                 │
│  SCK            │  D13        │  Clock                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### DS18B20 Sonda

```
┌─────────────────────────────────────────────────────────────────────┐
│                      DS18B20                                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  Pino Sonda     │  Pino Arduino  │  Função                       │
│  ─────────────────────────────────────────────────────────────       │
│  VCC (Vermelho)│  5V com resistor 4.7kΩ│  Alimentação          │
│  DQ (Amarelo)  │  A3          │  Data (OneWire)          │
│  GND (Preto)  │  GND        │  Terra                         │
│                                                                     │
│  IMPORTANTE: Resistor pull-up 4.7kΩ entre VCC e DQ!             │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### Sistema de Botões

```
┌─────────────────────────────────────────────────────────────────────┐
│                      BOTÕES (6x)                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                      │
│  Botão      │  Pino Arduino  │  Resistor     │
│  ─────────────────────────────────────────────       │
│  UP         │  D2          │  10kΩ       │
│  DOWN       │  D3          │  10kΩ       │
│  LEFT       │  D5          │  10kΩ       │
│  RIGHT      │  D6          │  10kΩ       │
│  OK         │  D7          │  10kΩ       │
│  BCK        │  D8          │  10kΩ       │
│                                                      │
│  Conexão: +5V ──[10kΩ]── Pino ──[Botão]── GND     │
│                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### LEDs

```
┌─────────────────────────────────────────────────────────────────────┐
│                      LEDs                                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                      │
│  LED          │  Pino Arduino  │  Resistor    │
│  ───────────────────────────────────────────        │
│  Verde       │  D10         │  220Ω      │
│  Vermelho    │  D11         │  220Ω      │
│                                                      │
│  Conexão: Pino ──[220Ω]── LED(+) ── LED(-)── GND   │
│                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Buzzer

```
┌─────────────────────────────────────────────────────────────────────┐
│                      BUZZER                                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                      │
│  Buzzer       │  Pino Arduino  │  Resistor    │
│  ───────────────────��─��─────────────────────        │
│  Signal     │  D9          │  100Ω      │
│                                                      │
│  Conexão: D9 (PWM) ──[100Ω]── Buzzer ── GND       │
│                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 7. Notas Importantes

### Conflitos de Pinos

⚠️ **ATENÇÃO:**
- D4 é usado para SD Card CS mas também foi movido o DS18B20 para A3 para evitar konflikto
- Pino A3 originally era OneWire mas agora está configurado corretamente

### Pinos Reservados/Future

| Pino | Status | Uso Futuro |
|------|-------|-----------|
| A4 | Livre | I2C SDA |
| A5 | Livre | I2C SCL |
| D5 | Botão LEFT | Futuras expansões |
| D6 | Botão RIGHT | Futuras expansões |

### Level Shifters

Para TFT operando em 3.3V:
- Usar level shifter bidirecional para MOSI, SCK
- CS, DC, RST podem usar level shifter ou conexão direta (saída only)

### Resumo de Alimentação

```
┌─────────────────────────────────────────────────────────────────────┐
│                   ALIMENTAÇÃO                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                              │
│  Tensão      │  Componentes                                     │
│  ────────────────────────────────────────────────────────          │
│  5V         │  Arduino, Botões, Buzzer, SD Card (se 5V)        │
│  3.3V       │  TFT, SD Card (se 3.3V), LED backlight           │
│  GND        │ Todos componentes                                   │
│                                                              │
└─────────────────────────────────────────────────────────────────────┘
```

---

**Continue learning:**
- [HARDWARE.md](HARDWARE.md) - Especificações completas
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solução de problemas