# 🚀 Component Tester PRO v3.0 — CYD Edition

## 🌟 Visão Geral

O **Component Tester PRO v3.0** é a evolução definitiva em diagnóstico de componentes eletrônicos. Agora rodando no poderoso **ESP32 com display touchscreen integrado**, o projeto combina precisão técnica com uma interface visual premium e controle por toque.

> [!IMPORTANT]
> **Versão CYD Edition:** Esta versão v3.0 roda na placa Cheap Yellow Display (ESP32-2432S028R) com TFT 2.8" integrado, touchscreen XPT2046, e slot MicroSD — tudo já soldado na placa!

---

## ✨ O que há de novo na v3.0

| 🆕 Feature | Descrição |
|:---|:---|
| **ESP32 Dual Core** | 240MHz, 520KB RAM, 4MB Flash (vs 16MHz/2KB do Uno) |
| **Touchscreen** | Navegação por toque no XPT2046 resistivo integrado |
| **Multímetro AC** | Tensão AC True RMS via sensor ZMPT101B |
| **Multímetro DC** | Tensão, corrente e potência DC via INA219 (I2C) |
| **Database Otimizado** | Índice em RAM para busca O(1) por categoria |
| **NVS Storage** | Settings persistidos via Preferences (mais robusto que EEPROM) |
| **Backlight PWM** | Controle de brilho do display via LEDC |
| **TFT_eSPI** | Driver otimizado com DMA (3-5x mais rápido) |

---

## 📱 Funcionalidades

| 🛠️ Medições | 📊 Análise | 🌡️ Monitoramento | ⚡ Multímetro |
|:---|:---|:---|:---|
| 13 modos de medição | Histórico (50 registros) | Sonda DS18B20 | Tensão AC (RMS) |
| Auto-detect | Estatísticas de uso | Alertas térmicos | Tensão DC |
| Identificação por DB | Log no SD Card | LEDs RGB | Corrente DC |
| Ícones vetoriais | RAM livre no ESP32 | Buzzer/Speaker | Potência DC |

---

## 🛠️ Hardware

### Placa Base: ESP32-2432S028R (Cheap Yellow Display)

| Componente | Especificação |
|:---|:---|
| **CPU** | ESP32-WROOM-32 (Dual Core 240MHz) |
| **RAM** | 520KB SRAM |
| **Flash** | 4MB |
| **Display** | TFT 2.8" ILI9341 320×240 (integrado) |
| **Touch** | XPT2046 Resistivo (integrado) |
| **SD Card** | Slot MicroSD (integrado, HSPI) |
| **Speaker** | Conector de speaker (GPIO 26) |
| **LED** | RGB integrado (GPIO 4, 16, 17) |
| **USB** | Micro USB (alimentação + programação) |

### Módulos Externos

| Módulo | Função | Conexão |
|:---|:---|:---|
| **ZMPT101B** | Sensor de tensão AC | GPIO 34 (ADC) |
| **INA219** | Sensor de corrente/tensão DC | GPIO 27 (SDA) + GPIO 22 (SCL) |
| **DS18B20** | Sonda de temperatura | GPIO 4 (OneWire) |
| Resistor 10kΩ | Referência para medições | Divisor no GPIO 35 |

---

## 📍 Pinagem Completa (CYD)

### Pinos Fixos da Placa (NÃO alterar)

```
TFT (VSPI):          SD Card (HSPI):       Touch (Soft SPI):
  CS  → GPIO 15        CS  → GPIO 5         CS   → GPIO 33
  DC  → GPIO 2         SCK → GPIO 18        CLK  → GPIO 25
  SCK → GPIO 14        MOSI→ GPIO 23        MOSI → GPIO 32
  MOSI→ GPIO 13        MISO→ GPIO 19        MISO → GPIO 39
  MISO→ GPIO 12                              IRQ  → GPIO 36
  BL  → GPIO 21
```

### Pinos dos Módulos Externos

```
Probe Principal  → GPIO 35 (ADC, input-only)
ZMPT101B (AC)    → GPIO 34 (ADC, input-only)
INA219 SDA       → GPIO 27
INA219 SCL       → GPIO 22
DS18B20          → GPIO 4
Buzzer/Speaker   → GPIO 26
LED Verde        → GPIO 16
LED Vermelho     → GPIO 17
```

---

## 🚀 Como Iniciar

### Requisitos
- [PlatformIO IDE](https://platformio.org/) (VSCode)
- Placa ESP32-2432S028R (Cheap Yellow Display)
- Cabo Micro USB
- Cartão MicroSD com o arquivo `COMPBD.CSV`

### Instalação

```bash
# Clone o repositório
git clone https://github.com/lelebrr/Component_Tester.git
cd Component_Tester

# Compile e faça upload para a CYD
pio run -e cyd --target upload

# Monitor serial
pio device monitor
```

### Preparação do SD Card

1. Formate o MicroSD em **FAT32**
2. Copie o arquivo `sd_files/COMPBD.CSV` para a raiz do cartão
3. Insira o cartão no slot da CYD

---

## 📂 Estrutura do Projeto

```
Component_Tester/
├── platformio.ini          # Config ESP32 CYD + TFT_eSPI
├── src/
│   ├── config.h            # Pinagem + constantes CYD
│   ├── globals.h           # Estados + variáveis globais
│   ├── main.cpp            # Setup + loop (TFT_eSPI + NVS)
│   ├── menu.cpp/h          # Menu grid + settings + about
│   ├── measurements.cpp/h  # 13 modos de medição
│   ├── multimeter.cpp/h    # NOVO: ZMPT101B + INA219
│   ├── database.cpp/h      # SD Card + índice em RAM
│   ├── drawings.cpp/h      # UI + ícones vetoriais
│   ├── buttons.cpp/h       # Touchscreen + botões
│   ├── buzzer.cpp/h        # Speaker via LEDC PWM
│   ├── leds.cpp/h          # LED RGB integrado
│   ├── thermal.cpp/h       # DS18B20 (OneWire)
│   ├── logger.cpp/h        # Log no SD Card
│   └── utils.cpp/h         # Utilitários
├── sd_files/
│   └── COMPBD.CSV          # Database de componentes
├── docs/                   # Documentação técnica
└── README.md               # Este arquivo
```

---

## 📜 Licença

Distribuído sob a licença MIT.

Distribuído sob a licença MIT.

---

<p align="center">
  Desenvolvido com ❤️ por <b>Leandro</b> — CYD Edition v3.0
</p>