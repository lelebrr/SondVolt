# Especificações de Hardware - Component Tester PRO v2.0

## Índice

1. [Componentes de Hardware](#1-componentes-de-hardware)
2. [Placa Principal](#2-placa-principal)
3. [Display TFT](#3-display-tft)
4. [Sistema de Botões](#4-sistema-de-botões)
5. [LEDs de Status](#5-leds-de-status)
6. [Buzzer](#6-buzzer)
7. [Sonda Térmica](#7-sonda-térmica)
8. [SD Card](#8-sd-card)
9. [EEPROM](#9-eeprom)
10. [Probes](#10-probes)
11. [Esquema de Conexões](#11-esquema-de-conexões)
12. [Especificações Elétricas](#12-especificações-elétricas)
13. [Dimensões Físicas](#13-dimensões-físicas)
14. [Lista de Componentes](#14-lista-de-componentes)

---

![Hardware Layout](../assets/hardware_layout.png)

## 1. Componentes de Hardware

### Visão Geral do Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                    Component Tester PRO v2.0                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐       │
│  │   Arduino   │◄──►│   TFT      │    │  SD Card   │       │
│  │    Uno      │    │  2.8"     │    │  Module    │       │
│  └─────────────┘    └─────────────┘    └─────────────┘       │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐       │
│  │   Botões    │    │    LEDs     │    │   Buzzer   │       │
│  │    6x      │    │  Verde/Verm │    │    5V     │       │
│  └─────────────┘    └─────────────┘    └─────────────┘       │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐                          │
│  │   Probes    │    │   DS18B20  │                          │
│  │   A0/A1    │    │  Sonda     │                          │
│  └─────────────┘    └─────────────┘                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Placa Principal

### Arduino Uno R3

| Especificação | Valor |
|-------------|-------|
| Microcontrolador | ATmega328P |
| Tensão de Operação | 5V |
| Tensão de Entrada | 7-12V |
| Clock | 16 MHz |
| Flash Memory | 32 KB |
| SRAM | 2 KB |
| EEPROM | 1 KB |
| Pinos Digitais | 14 |
| Pinos Analógicos | 6 |

### Pinout do Arduino Uno

```
              Arduino Uno R3
    ┌──────────────────────────────┐
    │  [SDA][SCL][AREF][GND][13]  │
    │   ○  ○   ○   ○  ○           │
    │   D13 D12 D11 D10 D9  D8   │
    │                      ○     │
    │                      D7    │
    │                      ○    │
    │   [A0] [A1] [A2] [A3]     │
    │    ○    ○    ○    ○       │
    │   A0   A1   A2   A3       │
    │                       [GND]│
    │                       ○   │
    │                       D6   │
    │                       ○   │
    │                       D5   │
    │                       ○    │
    │                       D4    │
    │                       ○    │
    │                       D3    │
    │                       ○    │
    │                       D2    │
    │   [5V] [3V3] [RST] [GND]   │
    │    ○    ○    ○    ○       │
    │ USB     Power               │
    └──────────────────────────────┘
```

---

## 3. Display TFT

### TFT 2.8" ILI9341

| Especificação | Valor |
|-------------|-------|
| Tamanho | 2.8 polegadas |
| Controlador | ILI9341 |
| Resolução | 320 x 240 pixels |
| Cores | 65,536 cores (16-bit) |
| Interface | SPI |
| Tensão | 3.3V (com level shifter) |
| Backlight | LED Branco |

### Conexões do TFT

```
┌─────────────────────────────────────────┐
│         TFT ILI9341 Pinout              │
├─────────────────────────────────────────┤
│                                         │
│  Pino TFT  →  Pino Arduino              │
│  ─────────────────────────────────     │
│  VCC       →   3.3V                   │
│  GND       →   GND                    │
│  CS        →   D12 (TFT_CS_PIN)       │
│  DC        →   D13 (TFT_DC_PIN)       │
│  RST       →   A2 (TFT_RST_PIN)       │
│  SDI/MOSI →   D11 (MOSI)              │
│  SCK       →   D13 (SCK)              │
│  LED       →   3.3V (via resistor)   │
│  SDO/MISO →   D12 (MISO - não usado)  │
│                                         │
└─────────────────────────────────────────┘
```

### Biblioteca Necessária

```
PlatformIO:
Adafruit ILI9341
Adafruit GFX Library
Adafruit TouchScreen
```

---

## 4. Sistema de Botões

### 6 Botões Push

| Botão | Pino Arduino | Função |
|-------|-------------|--------|
| UP | D2 | Navegar para cima |
| DOWN | D3 | Navegar para baixo |
| LEFT | D5 | Função adicional |
| RIGHT | D6 | Função adicional |
| OK | D7 | Confirmar seleção |
| BCK | D8 | Voltar/Exit |

### Esquema de Conexão

```
┌─────────────────────────────────────────┐
│         Sistema de Botões              │
├─────────────────────────────────────────┤
│                                         │
│   +5V ──┬──[Resistor]──┬── D2 (UP)   │
│        │    10kΩ        │             │
│        │                ├── D3 (DOWN) │
│        │                │             │
│        │                ├── D5 (LEFT) │
│        │                │             │
│        │                ├── D6 (RIGHT) │
│        │                │             │
│        │                ├── D7 (OK)    │
│        │                │             │
│        │                └── D8 (BCK)   │
│        │                                   │
│        └───────────── GND                │
│                                         │
└��────────────────────────────────────────┘
```

### Configuração de Pull-up

Os botões usam pull-up interno do Arduino:
- Modo: `INPUT_PULLUP`
- Quando pressionado: pino vai para LOW
- Quando liberado: pino vai para HIGH (via resistor interno)

---

## 5. LEDs de Status

### 2 LEDs

| LED | Pino Arduino | Cor | Função |
|-----|--------------|-----|--------|
| LED Verde | D10 | Verde | Componente OK |
| LED Vermelho | D11 | Vermelho | Componente Ruim |

### Esquema de Conexão

```
┌─────────────────────────────────────────┐
│            LEDs de Status               │
├─────────────────────────────────────────┤
│                                         │
│   D10 ──[Resistor]──┐── LED Verde    │
│        220Ω          │──(Anodo)      │
│                     │                │
│   D11 ──[Resistor]──┤── LED Vermelho │
│        220Ω          └──(Anodo)      │
│                          │            │
│           GND ───────────┴─────────── │
│                                         │
└─────────────────────────────────────────┘
```

### Resistores Recomendados

| LED | Tensão | Corrente | Resistor |
|-----|-------|----------|--------|
| Verde | 2.0V | 20mA | 150Ω |
| Vermelho | 2.0V | 20mA | 150Ω |

**Nota:** 220Ω é usado para segurança extra.

---

## 6. Buzzer

### Buzzer Passivo 5V

| Especificação | Valor |
|-------------|-------|
| Modelo | Buzzer passivo 5V |
| Pino | D9 (PWM) |
| Frequência | 2-3 kHz |
| Controle | analogWrite() |

### Esquema de Conexão

```
┌─────────────────────────────────────────┐
│              Buzzer                    │
├─────────────────────────────────────────┤
│                                         │
│   D9 ──[Resistor]──┐── Buzzer       │
│                     │                 │
│        100Ω         └──── GND          │
│                                         │
│   Controle: Tone PWM                   │
│                                         │
└─────────────────────────────────────────┘
```

### Funções de Som

| Função | Duração | Uso |
|--------|---------|-----|
| beep(100) | 100ms | Seleção |
| beep(200) | 200ms | Parar |
| beep(300) | 300ms | Baixa resistência |
| beep(500) | 500ms | Curto-circuito |

---

## 7. Sonda Térmica

### DS18B20 Waterproof

| Especificação | Valor |
|-------------|-------|
| Modelo | DS18B20 |
| Precisão | ±0.5°C |
| Range | -55°C a +125°C |
| Resolução | 9-12 bit |
| Interface | OneWire |
| Pino | A3 |

### Conexão

```
┌─────────────────────────────────────────┐
│           Sonda DS18B20                  │
├─────────────────────────────────────────┤
│                                         │
│   Fio Preto   →   GND                   │
│   Fio Amarelo →   A3 (ONEWIRE_BUS)      │
│   Fio Vermelho→   5V (via resistor 4.7kΩ)│
│                                         │
│   Pull-up: 5V ──[4.7kΩ]── A3 ── Sonda  │
│                                         │
└─────────────────────────────────────────┘
```

### Biblioteca Necessária

```
OneWire
DallasTemperature
```

---

## 8. SD Card

### Módulo SD Card SPI

| Especificação | Valor |
|-------------|-------|
| Interface | SPI |
| Pino CS | D4 (SD_CS_PIN) |
| Voltagem | 3.3V ou 5V |
| Capacidade | até 32GB |
| Sistema | FAT16/FAT32 |

### Conexão

```
┌─────────────────────────────────────────┐
│            SD Card Module                 │
├─────────────────────────────────────────┤
│                                         │
│   Pino Módulo →  Pino Arduino           │
│   ─────────────────────────────────    │
│   VCC        →   5V ou 3.3V            │
│   GND        →   GND                   │
│   CS         →   D4                    │
│   MOSI       →   D11                   │
│   MISO       →   D12                   │
│   SCK        →   D13                   │
│                                         │
└─────────────────────────────────────────┘
```

### Pinagem Completa

```
┌─────────────────────────────────────────┐
│       Pinagem de Todos os Componentes    │
├─────────────────────────────────────────┤
│                                         │
│ Pino Arduino │ Função │ Componente     │
│ ─────────────────────────────────────    │
│ A0          │ Probe 1 (+) │ Probes     │
│ A1          │ Probe 2 (-) │ Probes     │
│              │                    │
│ D2           │ Botão UP    │ Botões     │
│ D3           │ Botão DOWN  │ Botões     │
│ D4           │ SD CS      │ SD Card   │
│ D5           │ Botão LEFT │ Botões     │
│ D6           │ Botão RIGHT│ Botões     │
│ D7           │ Botão OK   │ Botões     │
│ D8           │ Botão BCK  │ Botões     │
│ D9           │ Buzzer PWM │ Buzzer     │
│ D10          │ LED Verde │ LEDs       │
│ D11          │ LED Vermelho│ LEDs      │
│ D12          │ TFT CS    │ TFT       │
│ D13          │ TFT DC    │ TFT       │
│ A2           │ TFT RST   │ TFT       │
│ A3           │ OneWire   │ DS18B20   │
│              │                    │
│ 3.3V         │ VCC       │ TFT, SD   │
│ 5V           │ VCC       │ Arduino   │
│ GND          │ GND       │ Todos     │
│                                         │
└─────────────────────────────────────────┘
```

---

## 9. EEPROM

### EEPROM Interna do ATmega328P

| Especificação | Valor |
|-------------|-------|
| Capacidade | 1024 bytes |
| Retenção | 100 anos |
| Ciclos de escrita | 100,000 |

### Uso no Projeto

```
Endereço    │ Dados
────────────────────────────
0x000-0x0FF │ Configurações (Settings)
0x100-0xFFF│ Reservado
```

### Estrutura de Dados

```cpp
struct Settings {
  float offset1;           // 0x00-0x03
  float offset2;           // 0x04-0x07
  bool darkMode;          // 0x08
  bool silentMode;        // 0x09
  unsigned long timeout;  // 0x0A-0x0D
  unsigned long total;    // 0x0E-0x11
  unsigned long faulty;   // 0x12-0x15
};
```

---

## 10. Probes

### Sistema de Medição

| Especificação | Valor |
|---------------|-------|
| Probe 1 | A0 (Positivo) |
| Probe 2 | A1 (Negativo) |
| Resistência interna | ~100kΩ |

### Tips Recomendados

- Use probes com garra jacaré
- Para SMD: use pinças crocodilo
- Mantenha pontas limpas

---

## 11. Esquema de Conexões

### Diagrama Completo

```
┌─────────────────────────────────────────────────────────────────────┐
│              Component Tester PRO v2.0 - Diagrama                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   Arduino Uno R3                                                    │
│   ┌─────────────────────────────────────────────────────────────┐   │
│   │                                                             │   │
│   │  A0 ─────────────── Probe Vermelho (+)                     │   │
│   │  A1 ─────────────── Probe Preto (-)                       │   │
│   │                                                             │   │
│   │  D2 ───[10kΩ]──┬──[Bot]── UP                             │   │
│   │                └──[Bot]── DOWN                            │   │
│   │                └──[Bot]── LEFT                            │   │
│   │                └──[Bot]── RIGHT                           │   │
│   │                └──[Bot]── OK                              │   │
│   │                └──[Bot]── BCK                             │   │
│   │                                                             │   │
│   │  D9 ──► Buzzer                                            │   │
│   │  D10 ──► LED Verde                                        │   │
│   │  D11 ──► LED Vermelho                                     │   │
│   │                                                             │   │
│   │  D12 ──► TFT CS                                          │   │
│   │  D13 ──► TFT DC                                          │   │
│   │  A2 ───► TFT RST                                         │   │
│   │                                     (TFT 3.3V)        │   │
│   │                                                             │   │
│   │  D4 ───► SD CS                                          │   │
│   │                                     (SD 3.3V)         │   │
│   │                                                             │   │
│   │  A3 ──[4.7kΩ]──► DS18B20                                 │   │
│   │                                                             │   │
│   └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 12. Especificações Elétricas

### Consumption

| Especificação | Valor |
|---------------|-------|
| Tensão de Alimentação | 7-12V DC |
| Corrente Típica | 150mA |
| Corrente Máxima | 300mA |

### Limites

| Especificação | Valor |
|---------------|-------|
| Tensão Probe (máx) | 5V |
| Corrente Probe (máx) | 40mA |
| Temperatura Operação | 0-70°C |

---

## 13. Dimensões Físicas

### Dimensões

| Especificação | Valor |
|-------------|-------|
| Comprimento | 75mm |
| Largura | 55mm |
| Altura | 20mm (sem display) |
| Peso | ~50g |

---

## 14. Lista de Componentes

### Lista de Materiais

| Item | Descrição | Quantidade |
|------|----------|----------|
| 1 | Arduino Uno R3 | 1 |
| 2 | TFT 2.8" ILI9341 | 1 |
| 3 | Módulo SD Card | 1 |
| 4 | Sonda DS18B20 | 1 |
| 5 | Botões Push | 6 |
| 6 | LED Verde 5mm | 1 |
| 7 | LED Vermelho 5mm | 1 |
| 8 | Buzzer 5V | 1 |
| 9 | Resistor 10kΩ | 6 |
| 10 | Resistor 220Ω | 2 |
| 11 | Resistor 150Ω | 1 |
| 12 | Resistor 4.7kΩ | 1 |
| 13 | Fios jumpers | 20 |
| 14 | Probe com garra | 2 |
| 15 | Case/Box | 1 |

---

**Continue learning:**
- [PINOUT.md](PINOUT.md) - Pinagem detalhada
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solução de problemas