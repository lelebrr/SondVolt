# Pinagem Detalhada - Component Tester PRO v3.0 (CYD Edition)

## 1. Visão Geral da Placa (ESP32-2432S028R)

A placa **Cheap Yellow Display (CYD)** integra um ESP32-WROOM-32 com display TFT, touchscreen e slot SD em um único PCB. A pinagem é fixa para os componentes integrados.

### Resumo de Barramentos
- **VSPI:** Usado pelo display TFT.
- **HSPI:** Usado pelo slot de cartão MicroSD (separado para evitar conflitos).
- **Soft SPI:** Usado pelo controlador de Touch XPT2046.
- **I2C:** Usado para sensores externos (INA219).

---

## 2. Conexões Internas (Fixas)

### Display TFT (ST7789/ILI9341)
| Pino Lógico | Função | GPIO |
|:---|:---|:---|
| **TFT_CS** | Chip Select | 15 |
| **TFT_DC** | Data/Command | 2 |
| **TFT_SCK** | Serial Clock | 14 |
| **TFT_MOSI** | Master Out | 13 |
| **TFT_MISO** | Master In | 12 |
| **TFT_BL** | Backlight PWM | 21 |

### Cartão MicroSD (HSPI)
| Pino Lógico | Função | GPIO |
|:---|:---|:---|
| **SD_CS** | Chip Select | 5 |
| **SD_SCK** | Serial Clock | 18 |
| **SD_MOSI** | Master Out | 23 |
| **SD_MISO** | Master In | 19 |

### Touchscreen (XPT2046)
| Pino Lógico | Função | GPIO |
|:---|:---|:---|
| **TOUCH_CS** | Chip Select | 33 |
| **TOUCH_CLK** | Serial Clock | 25 |
| **TOUCH_MOSI** | Master Out | 32 |
| **TOUCH_MISO** | Master In | 39 |
| **TOUCH_IRQ** | Interrupt | 36 |

---

## 3. Conexões Externas (Probes e Sensores)

### Probes de Medição
| Nome | Função | GPIO | Tipo |
|:---|:---|:---|:---|
| **PIN_PROBE_MAIN** | Entrada de medição principal | 35 | ADC (Input-Only) |

> [!CAUTION]
> O GPIO 35 é **apenas entrada**. Para medições ativas (como carga de capacitores), um resistor de pull-up deve ser conectado a um pino de saída ou ao 3.3V constante.

### Multímetro (ZMPT101B & INA219)
| Módulo | Sinal | GPIO | Nota |
|:---|:---|:---|:---|
| **ZMPT101B** | Saída Analógica AC | 34 | ADC (Input-Only) |
| **INA219** | I2C SDA | 27 | Barramento I2C |
| **INA219** | I2C SCL | 22 | Barramento I2C |

### Outros Periféricos
| Componente | Função | GPIO | Nota |
|:---|:---|:---|:---|
| **DS18B20** | Sonda Térmica | 4 | Protocolo OneWire |
### Pinos dos Módulos Externos e Probes

```
SISTEMA DE PROBES (Garras de Teste):
  Probe 1 (B)  → GPIO 35 (ADC, input-only)
  Probe 2 (C)  → GPIO 27 (I/O, compartilhado com SDA)
  Probe 3 (E)  → GPIO 22 (I/O, compartilhado com SCL)

SENSORES:
  ZMPT101B (AC)  → GPIO 34 (ADC, input-only)
  INA219 SDA     → GPIO 27
  INA219 SCL     → GPIO 22
  DS18B20 (Temp) → GPIO 4  (Nota: Desativa o LED Azul integrado)

PERIFÉRICOS:
  Buzzer/Speaker → GPIO 26
  LED Verde      → GPIO 16
  LED Vermelho   → GPIO 17
```

> [!WARNING]
> **Conflito I2C:** Os Probes 2 e 3 compartilham os pinos do barramento I2C. Se você estiver usando o sensor INA219, certifique-se de não haver curto-circuito nos probes durante a medição DC, ou use conectores removíveis.

---

## 4. Diagrama de Conexão Recomendado

```text
       +-----------------------+
       |   ESP32 CYD BOARD     |
       |                       |
       |  [P3 Connector]       |
       |   SDA (27) -----------+---> [INA219 SDA]
       |   SCL (22) -----------+---> [INA219 SCL]
       |   GND      -----------+---> [GND]
       |   3.3V     -----------+---> [VCC]
       |                       |
       |  [CN1 Connector]      |
       |   IO 35 --------------+---> [PROBE PRINCIPAL]
       |   IO 34 --------------+---> [ZMPT101B OUT]
       |                       |
       |  [Outros Pinos]       |
       |   IO 4  --------------+---> [DS18B20 DQ]
       |   IO 26 --------------+---> [BUZZER/SPEAKER]
       +-----------------------+
```

---

## 5. Notas Técnicas

1. **ADC 12-bit:** O ESP32 possui resolução de 4096 níveis (0-4095) para 0-3.3V.
2. **Referência:** A tensão de referência é 3.3V. Não conecte sinais de 5V diretamente aos GPIOs do ESP32!
3. **Pull-ups:** O sensor DS18B20 requer um resistor de 4.7kΩ entre o VCC (3.3V) e o pino de dados (GPIO 4).
4. **I2C:** O INA219 deve ser alimentado com 3.3V para manter compatibilidade lógica com o ESP32.