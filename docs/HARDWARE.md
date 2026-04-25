# 🔧 Guia de Hardware — Component Tester PRO v3.2

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

<p align="center">
  <img src="../assets/hardware_layout.png" alt="Layout do Hardware" width="300">
</p>

Este guia detalha todos os componentes de hardware, esquemas de ligação e especificações técnicas do **Component Tester PRO v3.2**, construído sobre a placa ESP32-2432S028R (Cheap Yellow Display).

---

## 🏗️ Arquitetura de Hardware

```
┌─────────────────────────────────────────────────────────────────┐
│                COMPONENT TESTER PRO v3.2 - ARQUITETURA             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   ESP32-        │    │   Sensores      │    │   Display   │ │
│  │   2432S028R     │◄──►│   (ZMPT101B,    │◄──►│   (TFT      │ │
│  │   (CPU)        │    │   INA219,       │    │   2.8")     │ │
│  └─────────────────┘    │   DS18B20)      │    └─────────────┘ │
│         │                └─────────────────┘                   │
│         │                                                     │
│         ▼                                                     │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Armazenamento │    │   Interface     │    │   Sistema   │ │
│  │   (SD Card)    │    │   (Touch,       │    │   Segurança │ │
│  └─────────────────┘    │   Botões,       │    │   (True RMS)│ │
│                        │   Buzzer)        │    └─────────────┘ │
│                        └─────────────────┘                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### ⚡ Performance do Hardware

```
┌─────────────────────────────────────────────────────────────────┐
│                    PERFORMANCE DO HARDWARE v3.2                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Velocidade CPU:      ████████████████████████████ 240MHz      │
│  Memória RAM:        ████████████████████████ 520KB           │
│  Velocidade SPI:     ████████████████████████████ 80MHz        │
│  Touch Response:     █████████████████████████████ 0.1s       │
│  Display Refresh:    ████████████████████████████ 60fps       │
│  Uso Energia:        ████████████ 350mA (médio)                │
│  Precisão AC:       ████████████████████████████ ±1.5%        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 📱 Placa Base: ESP32-2432S028R (CYD)

A placa **ESP32-2432S028R**, conhecida como "Cheap Yellow Display", é uma plataforma de desenvolvimento Tudo-em-Um que combina:

### 🎯 Especificações Principais

| Recurso | Especificação | Status | Descrição |
|:---|:---|:---:|:---|
| **Microcontrolador** | ESP32-WROOM-32 | ✅ | Dual Core @ 240MHz |
| **Memória RAM** | 520KB SRAM | ✅ | Memória volátil |
| **Memória Flash** | 4MB SPI Flash | ✅ | Armazenamento firmware |
| **Display** | TFT 2.8" ILI9341 | ✅ | 320x240 pixels |
| **Touchscreen** | XPT2046 Resistivo | ✅ | Contato por pressão |
| **Slot MicroSD** | Integrado | ✅ | Até 32GB FAT32 |
| **LED RGB** | Integrado | ✅ | Indicador de status |
| **Amplificador** | PAM8002 | ✅ | GPIO 26 para speaker |
| **Alimentação** | Micro USB | ✅ | 5V DC entrada |

### 🔌 Sensores e Módulos Suportados

### 1. Sensor ZMPT101B (Tensão AC)

O **ZMPT101B** é um transformador de tensão isolado com amplificador operacional integrado, projetado para medições seguras de tensão de rede.

| Especificação | Valor | Status |
|:---|:---:|:---:|
| **Tensão de Entrada** | 0 a 250V RMS | ✅ |
| **Tensão de Saída** | 0 a 5V ADC | ✅ |
| **Precisão** | ±1.5% (True RMS) | ✅ |
| **Frequência de Resposta** | 40Hz a 60Hz | ✅ |
| **Interface** | GPIO 34 (ADC) | ✅ |

#### 📋 Conexão

```
ZMPT101B          ESP32 CYD
────────────────────────────────────────
VCC (Azul)   →    5V (VIN)
GND          →    GND
OUT          →    GPIO 34 (CN1-2)
L (Fase)    →    🔌 Fase da Rede (Via Proteção)
N (Neutro)   →    🔌 Neutro da Rede (Via Proteção)
```

### 🛡️ Circuito de Proteção 220V (Obrigatório)

Para operação segura em redes de 220V e suporte a picos de tensão, os seguintes componentes **DEVEM** ser instalados na entrada AC:

1. **Fusível Rápido 5A**: Instalado em série com a Fase (L). Protege contra curtos catastróficos.
2. **Varistor 14D431 (ou 10D431)**: Instalado em paralelo entre Fase (L) e Neutro (N). Ceifa picos de tensão acima de 275V AC.
3. **TVS Diode P6KE400A**: Instalado em paralelo após o varistor para proteção ultra-rápida contra transientes.
4. **Capacitor 100nF (Cerâmico)**: Em paralelo na saída do sinal do ZMPT101B (OUT para GND) para filtrar ruído de alta frequência.
5. **Capacitor 10µF (Eletrolítico)**: Na alimentação do ZMPT101B (VCC para GND) para estabilizar a referência do OpAmp.
6. **Resistor de Carga 10kΩ**: Entre a saída OUT do ZMPT101B e o GND para garantir estabilidade do ADC.

### 2. Sensor INA219 (Tensão/Corrente DC)

O **INA219** é um sensor de corrente e tensão DC com interface I2C, capaz de medir tensão, corrente e calcular potência.

| Especificação | Valor | Status |
|:---|:---:|:---:|
| **Tensão Máxima** | 0 a 26V | ✅ |
| **Corrente Máxima** | ±3.2A | ✅ |
| **Resolução** | 12-bit (0.8mA) | ✅ |
| **Precisão** | ±1.0% | ✅ |
| **Endereço I2C** | 0x40 (fixo) | ✅ |
| **Interface** | I2C (GPIO 27=SDA, GPIO 22=SCL) | ✅ |

#### 📋 Conexão

```
INA219            ESP32 CYD
────────────────────────────────────────
VCC           →    3.3V
GND           →    GND
SDA           →    GPIO 27 (P3-2)
SCL           →    GPIO 22 (P3-3)
Vin+          →    ➕ (Positivo da Carga)
Vin-          →    ➖ (Negativo da Carga)
```

### 3. Sonda DS18B20 (Temperatura)

A sonda **DS18B20** é um sensor de temperatura digital à prova d'água, utilizando o protocolo OneWire.

| Especificação | Valor | Status |
|:---|:---:|:---:|
| **Faixa de Temperatura** | -55°C a +125°C | ✅ |
| **Precisão** | ±0.5°C (±0.25°C típico) | ✅ |
| **Resolução** | 9-bit a 12-bit (configurável) | ✅ |
| **Distância Máxima** | até 100m | ✅ |
| **Interface** | OneWire (GPIO 4) | ✅ |
| **Pull-up Necessário** | 4.7kΩ entre DATA e 3.3V | ✅ |

#### 📋 Conexão

```
DS18B20           ESP32 CYD
────────────────────────────────────────
VCC (Vermelho) →   3.3V
DATA (Amarelo)→   GPIO 4 (P3-4)
GND (Preto)    →   GND
+ Pull-up 4.7kΩ entre DATA e 3.3V
```

---

## 📐 Diagramas de Ligação

### 🗂️ Diagrama em Blocos

```
┌─────────────────────────────────────────────────────────────────┐
│                  COMPONENT TESTER PRO v3.2                     │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │   TFT    │  │   Touch   │  │   SD    │  │  LED RGB │        │
│  │  2.8"    │  │  XPT2046  │  │  Card   │  │         │        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
│       │             │             │             │                │
│       ▼             ▼             ▼             ▼                │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    ESP32-WROOM-32                        │   │
│  │    Dual Core 240MHz │ 520KB RAM │ 4MB Flash               │   │
│  └──────────────────────────────────────────────────────────┘   │
│                              │                                  │
│       ┌──────────────────────┼──────────────────────┐         │
│       ▼                      ▼                      ▼           │
│  ┌─────────┐          ┌─────────┐          ┌─────────┐        │
│  │ GPIO 34 │          │ GPIO 27 │          │ GPIO 4  │         │
│  │  (ADC)  │          │  (SDA)  │          │(OneWire)│        │
│  └────┬────┘          └────┬────┘          └────┬────┘        │
│       │                     │                     │            │
│       ▼                     ▼                     ▼            │
│  ┌──────────┐        ┌──────────┐        ┌──────────┐         │
│  │ZMPT101B  │        │  INA219   │        │ DS18B20  │         │
│  │   AC     │        │    DC     │        │  Temp    │         │
│  │  True RMS│        │  Corrente │        │          │         │
│  └──────────┘        └──────────┘        └──────────┘         │
└─────────────────────────────────────────────────────────────────┘
```

### 🔌 Ligação dos Módulos Externos

```
                    ESP32-2432S028R (Vista Traseira)

    ┌────────────────────────────────────────────────────────┐
    │                                                        │
    │  CN1                              P3                    │
    │ ┌──────┐                        ┌──────┐              │
    │ │ 1  2 │                        │1  2 3│              │
    │ │GND 5V│                        │GND SDA│              │
    │ │     │                        │      SCL│             │
    │ │34  GND│                        │       4│             │
    │ └──────┘                        └──────┘              │
    │                                                        │
    │  ┌────┐  ┌────┐                                        │
    │  │ USB│  │Boot│                                        │
    │  └────┘  └────┘                                        │
    │                                                        │
    └────────────────────────────────────────────────────────┘
                         │
                         ▼
    ┌─────────────────────────────────────────────────────────┐
    │                    MÓDULOS EXTERNOS                        │
    │                                                         │
    │  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   │
    │  │  ZMPT101B   │   │    INA219   │   │   DS18B20   │   │
    │  │             │   │             │   │             │   │
    │  │ VCC +5V     │   │ VCC +3.3V   │   │ VCC +3.3V   │   │
    │  │ GND         │   │ GND         │   │ GND         │   │
    │  │ OUT →GPIO34│   │ SDA→GPIO27 │   │ DATA→GPIO4 │   │
    │  │             │   │ SCL→GPIO22 │   │ (Pull 4.7k)│   │
    │  │ L →Fase    │   │ Vin+       │   │             │   │
    │  │ N →Neutro   │   │ Vin-       │   │             │   │
    │  └─────────────┘   └─────────────┘   └─────────────┘   │
    └─────────────────────────────────────────────────────────┘
```

---

## 🎯 Pinagem de Referência Completa

### 📡 Pinos Utilizados pelo Firmware

| Função | GPIO | Conector | Tipo | Descrição | Status |
|:---|:---:|:---:|:---:|:---|:---:|
| TFT_CS | 15 | Interno | SPI | Chip Select do Display | ✅ |
| TFT_DC | 2 | Interno | SPI | Data/Command do TFT | ✅ |
| TFT_SCK | 14 | Interno | SPI | Clock SPI | ✅ |
| TFT_MOSI | 13 | Interno | SPI | MOSI do Display | ✅ |
| TFT_MISO | 12 | Interno | SPI | MISO do Display | ✅ |
| TFT_Backlight | 21 | Interno | PWM | Controle de brilho | ✅ |
| SD_CS | 5 | Interno | SPI | Chip Select do SD | ✅ |
| SD_SCK | 18 | Interno | SPI | Clock do SD | ✅ |
| SD_MOSI | 23 | Interno | SPI | MOSI do SD | ✅ |
| SD_MISO | 19 | Interno | SPI | MISO do SD | ✅ |
| Touch_CS | 33 | Interno | SPI | Chip Select Touch | ✅ |
| Touch_IRQ | 36 | Interno | Input | IRQ do Touch | ✅ |
| LED_Green | 16 | Interno | Output | LED Verde | ✅ |
| LED_Red | 17 | Interno | Output | LED Vermelho | ✅ |
| LED_Blue | 4 | Interno | Output | LED Azul / OneWire | ✅ |
| Buzzer | 26 | Interno | DAC | Speaker / Buzzer | ✅ |
| **ZMPT_IN** | **34** | CN1 | **ADC** | Entrada AC (ZMPT101B) | ✅ |
| **Probe_IN** | **35** | CN1 | **ADC** | Probe de componentes | ✅ |
| **I2C_SDA** | **27** | P3 | **I2C** | Dados I2C (INA219) | ✅ |
| **I2C_SCL** | **22** | P3 | **I2C** | Clock I2C (INA219) | ✅ |
| **OneWire** | **4** | P3 | **1-Wire** | Temperatura (DS18B20) | ✅ |

### 🔌 Conectores da Placa CYD

```
CN1 (4 pinos)          P3 (4 pinos)
┌────┬────┬────┬────┐  ┌────┬────┬────┬────┐
│ 1  │ 2  │ 3  │ 4  │  │ 1  │ 2  │ 3  │ 4  │
│35  │ 34 │GND │ 5V │  │GND │ 27 │ 22 │ 4  │
│ADC │ADC│    │    │  │    │SDA │SCL │OW │
└────┴────┴────┴────┘  └────┴────┴────┴────┘
```

---

## ⚡ Especificações Elétricas

| Parâmetro | Valor | Notas | Status |
|:---|:---:|:---|:---:|
| **Tensão de Alimentação** | 5V USB ou VIN | Recomendado 5V/2A | ✅ |
| **Consumo em Repouso** | ~150mA | Display ligado | ✅ |
| **Consumo em Medição** | ~250mA | Brilho máximo + sensores | ✅ |
| **Tensão Máxima AC (Entrada)** | 250V RMS | Via ZMPT101B | ⚠️ |
| **Tensão Máxima DC (Entrada)** | 26V | Via INA219 | ✅ |
| **Corrente Máxima DC** | ±3.2A | Via INA219 | ✅ |
| **Corrente Máxima GPIO** | 12mA | Por pino | ✅ |

---

## 📦 Lista de Materiais (BOM)

### 🖥️ Placa Base
1. **ESP32-2432S028R (CYD)** — Placa de desenvolvimento principal

### 🔍 Sensores (Opcionais para modo completo)
2. **ZMPT101B** — Módulo sensor de tensão AC
3. **INA219** — Módulo sensor de corrente/tensão DC (I2C)
4. **DS18B20** — Sonda de temperatura waterproof

### 📶 Componentes Passivos
5. **Resistor 4.7kΩ 1/4W** — Pull-up OneWire
6. **Resistor 10kΩ 1/4W** — Divisor de tensão (opcional)

### 🎧 Acessórios
7. **Cartão MicroSD** — 8GB+ (FAT32)
8. **Fonte USB 5V/2A** — Alimentação
9. **Cabos e jumpers** — Conexões

---

## ⚠️ Avisos de Segurança

> <span style="color:red">**PERIGO:**</span> Este projeto trabalha com tensão de rede elétrica (110V/220V). Sempre desconecte a energia antes de manipular as conexões dos sensores AC.

> <span style="color:orange">**ATENÇÃO:**</span> Os pinos do ESP32 operam em 3.3V. Nunca aplique tensões superiores diretamente nos GPIOs sem proteção adequada.

> <span style="color:blue">**IMPORTANTE:**</span> Use fusível de 100mA na entrada do ZMPT101B para proteção adicional. Sempre verifique as conexões antes de energizar o circuito.

---

<p align="center">
  <i>📱 Sondvolt v3.2 — Guia de Hardware Completo</i>
</p>

Este guia detalha todos os componentes de hardware, esquemas de ligação e especificações técnicas do Component Tester PRO v3.0, construído sobre a placa ESP32-2432S028R (Cheap Yellow Display).

---

## Placa Base: ESP32-2432S028R (CYD)

A plaque **ESP32-2432S028R**, conhecida como "Cheap Yellow Display", é uma plataforma de desenvolvimento Tudo-em-Um que combina:

| Recurso | Especificação |
|:---|:---|
| **Microcontrolador** | ESP32-WROOM-32 Dual Core @ 240MHz |
| **Memória RAM** | 520KB SRAM |
| **Memória Flash** | 4MB SPI Flash |
| **Display** | TFT 2.8" ILI9341 (320x240) |
| **Touchscreen** | XPT2046 Resistivo |
| **Slot MicroSD** | Integrado (até 32GB FAT32) |
| **LED RGB** | Integrado (Verde=GPIO16, Vermelho=GPIO17, Azul=GPIO4) |
| **Amplificador** | PAM8002 (GPIO 26) para speaker |
| **Alimentação** | Micro USB 5V ou pino VIN externo |

---

## Sensores e Módulos Suportados

### 1. Sensor ZMPT101B (Tensão AC)

O **ZMPT101B** é um transformador de tensão isolado com amplificador operacional integrado, projetado para medições seguras de tensão de rede.

| Especificação | Valor |
|:---|:---|
| **Tensão de Entrada** | 0 a 250V RMS |
| **Tensão de Saída** | 0 a 5V ADC |
| **Precisão** | ±2% (calibrado) |
| **Frequência de Resposta** | 40Hz a 60Hz |
| **Interface** | GPIO 34 (ADC) |

#### Conexão

```
ZMPT101B          ESP32 CYD
─────────────────────────────────
VCC (Azul)   →    5V (VIN)
GND          →    GND
OUT          →    GPIO 34 (CN1-2)
L (Fase)    →    Fase da Rede 110V/220V
N (Neutro)   →    Neutro da Rede
```

### 2. Sensor INA219 (Tensão/Corrente DC)

O **INA219** é um sensor de corrente e tensão DC com interface I2C, capaz de medir tensão, corrente e calcular potência.

| Especificação | Valor |
|:---|:---|
| **Tensão Máxima** | 0 a 26V |
| **Corrente Máxima** | ±3.2A |
| **Resolução** | 12-bit (0.8mA) |
| **Endereço I2C** | 0x40 (fixo) |
| **Interface** | I2C (GPIO 27=SDA, GPIO 22=SCL) |

#### Conexão

```
INA219            ESP32 CYD
─────────────────────────────────
VCC           →    3.3V
GND           →    GND
SDA           →    GPIO 27 (P3-2)
SCL           →    GPIO 22 (P3-3)
Vin+          →    + (Positivo da Carga)
Vin-          →    - (Negativo da Carga)
```

### 3. Sonda DS18B20 (Temperatura)

A sonda **DS18B20** é um sensor de temperatura digital à prova d'água, utilizando o protocolo OneWire.

| Especificação | Valor |
|:---|:---|
| **Faixa de Temperatura** | -55°C a +125°C |
| **Precisão** | ±0.5°C (±0.25°C típico) |
| **Resolução** | 9-bit a 12-bit (configurável) |
| **Distância Máxima** | até 100m |
| **Interface** | OneWire (GPIO 4) |
| **Pull-up Necessário** | 4.7kΩ entre DATA e 3.3V |

#### Conexão

```
DS18B20           ESP32 CYD
─────────────────────────────────
VCC (Vermelho) →   3.3V
DATA (Amarelo)→   GPIO 4 (P3-4)
GND (Preto)    →   GND
+ Pull-up 4.7kΩ entre DATA e 3.3V
```

---

## Diagramas de Ligação

### Diagrama em Blocos

```
┌─────────────────────────────────────────────────────────────────┐
│                      ESP32-2432S028R (CYD)                      │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │   TFT    │  │   Touch   │  │   SD    │  │  LED RGB │        │
│  │  2.8"    │  │  XPT2046  │  │  Card   │  │         │        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
│       │             │             │             │                │
│       ▼             ▼             ▼             ▼                │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    ESP32-WROOM-32                        │   │
│  │    Dual Core 240MHz │ 520KB RAM │ 4MB Flash               │   │
│  └──────────────────────────────────────────────────────────┘   │
│                              │                                  │
│       ┌──────────────────────┼──────────────────────┐         │
│       ▼                      ▼                      ▼           │
│  ┌─────────┐          ┌─────────┐          ┌─────────┐        │
│  │ GPIO 34 │          │ GPIO 27 │          │ GPIO 4  │         │
│  │  (ADC)  │          │  (SDA)  │          │(OneWire)│        │
│  └────┬────┘          └────┬────┘          └────┬────┘        │
│       │                     │                     │            │
│       ▼                     ▼                     ▼            │
│  ┌──────────┐        ┌──────────┐        ┌──────────┐         │
│  │ZMPT101B  │        │  INA219   │        │ DS18B20  │         │
│  │   AC     │        │    DC     │        │  Temp    │         │
│  └──────────┘        └──────────┘        └──────────┘         │
└─────────────────────────────────────────────────────────────────┘
```

### Ligação dos Módulos Externos

```
                    ESP32-2432S028R (Vista Traseira)

    ┌────────────────────────────────────────────────────────┐
    │                                                        │
    │  CN1                              P3                    │
    │ ┌──────┐                        ┌──────┐              │
    │ │ 1  2 │                        │1  2 3│              │
    │ │GND 5V│                        │GND SDA│              │
    │ │     │                        │      SCL│             │
    │ │34  GND│                        │       4│             │
    │ └──────┘                        └──────┘              │
    │                                                        │
    │  ┌────┐  ┌────┐                                        │
    │  │ USB│  │Boot│                                        │
    │  └────┘  └────┘                                        │
    │                                                        │
    └───────────────────────────���─��──────────────────────────┘
                         │
                         ▼
    ┌─────────────────────────────────────────────────────────┐
    │                    MÓDULOS EXTERNOS                        │
    │                                                         │
    │  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   │
    │  │  ZMPT101B   │   │    INA219   │   │   DS18B20   │   │
    │  │             │   │             │   │             │   │
    │  │ VCC +5V     │   │ VCC +3.3V   │   │ VCC +3.3V   │   │
    │  │ GND         │   │ GND         │   │ GND         │   │
    │  │ OUT →GPIO34│   │ SDA→GPIO27 │   │ DATA→GPIO4 │   │
    │  │             │   │ SCL→GPIO22 │   │ (Pull 4.7k)│   │
    │  │ L →Fase    │   │ Vin+       │   │             │   │
    │  │ N →Neutro   │   │ Vin-       │   │             │   │
    │  └─────────────┘   └─────────────┘   └─────────────┘   │
    └─────────────────────────────────────────────────────────┘
```

---

## Pinagem de Referência Completa

### Pinos Utilizados pelo Firmware

| Função | GPIO | Conector | Tipo | Descrição |
|:---|:---:|:---:|:---:|:---|
| TFT_CS | 15 | Interno | SPI | Chip Select do Display |
| TFT_DC | 2 | Interno | SPI | Data/Command do TFT |
| TFT_SCK | 14 | Interno | SPI | Clock SPI |
| TFT_MOSI | 13 | Interno | SPI | MOSI do Display |
| TFT_MISO | 12 | Interno | SPI | MISO do Display |
| TFT_Backlight | 21 | Interno | PWM | Controle de brilho |
| SD_CS | 5 | Interno | SPI | Chip Select do SD |
| SD_SCK | 18 | Interno | SPI | Clock do SD |
| SD_MOSI | 23 | Interno | SPI | MOSI do SD |
| SD_MISO | 19 | Interno | SPI | MISO do SD |
| Touch_CS | 33 | Interno | SPI | Chip Select Touch |
| Touch_IRQ | 36 | Interno | Input | IRQ do Touch |
| LED_Green | 16 | Interno | Output | LED Verde |
| LED_Red | 17 | Interno | Output | LED Vermelho |
| LED_Blue | 4 | Interno | Output | LED Azul / OneWire |
| Buzzer | 26 | Interno | DAC | Speaker / Buzzer |
| **ZMPT_IN** | **34** | CN1 | **ADC** | Entrada AC (ZMPT101B) |
| **Probe_IN** | **35** | CN1 | **ADC** | Probe de componentes |
| **I2C_SDA** | **27** | P3 | **I2C** | Dados I2C (INA219) |
| **I2C_SCL** | **22** | P3 | **I2C** | Clock I2C (INA219) |
| **OneWire** | **4** | P3 | **1-Wire** | Temperatura (DS18B20) |

### Conectores da Placa CYD

```
CN1 (4 pinos)          P3 (4 pinos)
┌────┬────┬────┬────┐  ┌────┬────┬────┬────┐
│ 1  │ 2  │ 3  │ 4  │  │ 1  │ 2  │ 3  │ 4  │
│35  │ 34 │GND │ 5V │  │GND │ 27 │ 22 │ 4  │
│ADC │ADC│    │    │  │    │SDA │SCL │OW │
└────┴────┴────┴────┘  └────┴────┴────┴────┘
```

---

## Especificações Elétricas

| Parâmetro | Valor | Notas |
|:---|:---:|:---|
| **Tensão de Alimentação** | 5V USB ou VIN | 推荐 5V/2A |
| **Consumo em Repouso** | ~150mA | Display ligado |
| **Consumo em Medição** | ~250mA | Brilho máximo + sensores |
| **Tensão Máxima AC (Entrada)** | 250V RMS | Via ZMPT101B |
| **Tensão Máxima DC (Entrada)** | 26V | Via INA219 |
| **Corrente Máxima DC** | ±3.2A | Via INA219 |
| **Corrente Máxima GPIO** | 12mA | Por pino |

---

## Lista de Materiais (BOM)

### Placa Base
1. ESP32-2432S028R (CYD) — Placa de desenvolvimento

### Sensores (Opcionais para modo completo)
2. ZMPT101B — Módulo sensor de tensão AC
3. INA219 — Módulo sensor de corrente/tensão DC (I2C)
4. DS18B20 — Sonda de temperatura waterproof

### Componentes Passivos
5. Resistor 4.7kΩ 1/4W — Pull-up OneWire
6. Resistor 10kΩ 1/4W — Divisor de tensão (opcional)

### Acessórios
7. Cartão MicroSD — 8GB+ (FAT32)
8. Fonte USB 5V/2A — Alimentação
9. Cabos e jumpers — Conexões

---

## Avisos de Segurança

> **PERIGO:** Este projeto trabalha com tensão de rede elétrica (110V/220V). Sempre desconecte a energia antes de manipular as conexões dos sensores AC.

> **ATENÇÃO:** Os pinos do ESP32 operam em 3.3V. Nunca aplique tensões superiores diretamente nos GPIOs sem proteção adequada.

> **IMPORTANTE:** Use fusível de 100mA na entrada do ZMPT101B para proteção adicional. Sempre verifique as conexões antes de energizar o circuito.

---

<p align="center">
<i>Component Tester PRO v3.0 — Guia de Hardware</i>
</p>