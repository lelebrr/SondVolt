# 📍 Referência de Pinagem (CYD Pinout)

Este guia detalha a pinagem da placa **ESP32-2432S028R** e como ela é utilizada no firmware v3.0.

---

## 🎨 Diagrama da Placa (Vista Traseira)

```text
       _______________________________________
      |   [ USB ] [ Boot ] [ Reset ] [ SD ]   |
      |                                       |
      |   [ CN1 ]             [ LDR ]         |
      |    (1) (2) (3) (4)                    |
      |     |   |   |   |                     |
      |    35  34  GND 5V                     |
      |                                       |
      |   [ P3 / J3 ]                         |
      |    (1) (2) (3) (4)                    |
      |     |   |   |   |                     |
      |    GND 27  22  4                      |
      |                                       |
      |   [ Speaker ] [ RGB LED ] [ Batt ]    |
      |_______________________________________|
```

---

## 🛠️ Mapeamento de Pinos

### 1. Periféricos Integrados (Fixos)

| Periférico | Função | GPIO | Barramento |
|:---|:---|:---:|:---:|
| **Display TFT** | CS / DC / SCK / MOSI / MISO | 15 / 2 / 14 / 13 / 12 | VSPI |
| **Backlight** | Controle de Brilho (PWM) | 21 | - |
| **SD Card** | CS / SCK / MOSI / MISO | 5 / 18 / 23 / 19 | HSPI |
| **Touchscreen** | CS / CLK / MOSI / MISO / IRQ | 33 / 25 / 32 / 39 / 36 | SoftSPI |
| **LED RGB** | Green / Red (Blue em GPIO 4) | 16 / 17 | - |
| **Buzzer** | Áudio / Alertas | 26 | DAC |

### 2. Sensores e Probes (Expansão)

| Uso no Projeto | Módulo / Componente | GPIO | Conector |
|:---|:---|:---:|:---:|
| **Medição AC** | Saída ZMPT101B | **34** | CN1 - Pin 2 |
| **Probe Principal** | Entrada de Componentes | **35** | CN1 - Pin 1 |
| **I2C SDA** | INA219 / Outros I2C | **27** | P3 - Pin 2 |
| **I2C SCL** | INA219 / Outros I2C | **22** | P3 - Pin 3 |
| **OneWire** | Sonda Térmica DS18B20 | **4** | P3 - Pin 4 |

---

## ⚠️ Observações Importantes

### Pinos de Apenas Entrada (Input-Only)
Os GPIOs **34, 35, 36 e 39** são entradas puras no ESP32. Eles não possuem resistores de pull-up internos e não podem ser usados como saídas.
- **GPIO 34/35:** Usados como entradas analógicas (ADC) para os sensores AC e Probe.

### Conflito do LED Azul
O **LED Azul** integrado da CYD compartilha o **GPIO 4** com o barramento **OneWire**. 
- Ao conectar a sonda DS18B20, o LED azul pode piscar ou permanecer aceso dependendo do tráfego de dados. Isso é normal.

### Barramento I2C
O firmware configura o barramento I2C nos pinos **27 (SDA)** e **22 (SCL)**. Estes pinos estão expostos no conector **P3**. Se você conectar outros dispositivos I2C, certifique-se de que eles tenham endereços diferentes do INA219 (0x40).

---

## 🔌 Alimentação

| Pino | Tensão | Recomendação |
|:---:|:---:|:---|
| **5V / VIN** | 4.5V - 6V | Use para alimentar módulos que requerem 5V (ZMPT101B). |
| **3.3V** | 3.3V Estável | Use para a sonda DS18B20 e INA219. |
| **GND** | 0V | Ponto comum de aterramento para todos os módulos. |

> [!CAUTION]
> **Nunca** alimente a placa simultaneamente via USB e via pino 5V/VIN a menos que você saiba o que está fazendo (risco de retorno de corrente).