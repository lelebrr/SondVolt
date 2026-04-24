# Especificações de Hardware - Component Tester PRO v3.0 (CYD Edition)

## 1. Placa Principal (ESP32-2432S028R)

A placa **Cheap Yellow Display (CYD)** é o coração do sistema, baseada no microcontrolador ESP32.

| Especificação | Valor |
|:---|:---|
| **Microcontrolador** | ESP32-WROOM-32 (Dual Core) |
| **Velocidade de Clock** | 240 MHz |
| **Memória RAM** | 520 KB SRAM |
| **Memória Flash** | 4 MB |
| **Tensão de Operação** | 3.3V Interna (5V USB) |
| **Conectividade** | Wi-Fi & Bluetooth (disponível para expansão) |
| **Armazenamento** | Slot MicroSD integrado |

---

## 2. Display e Interface

### Tela TFT Integrada
- **Tamanho:** 2.8 polegadas
- **Controlador:** ILI9341 ou ST7789
- **Resolução:** 320 x 240 pixels
- **Cores:** 16-bit High Color (TFT_eSPI otimizado)
- **Interface:** SPI Dedicado (VSPI)

### Touchscreen
- **Controlador:** XPT2046 Resistivo
- **Interface:** SPI (Software SPI / SoftSPI)
- **Uso:** Navegação completa por zonas de toque (Zonas: UP, DOWN, LEFT, RIGHT, OK, BACK)

---

## 3. Sensores do Multímetro

### ZMPT101B (Tensão AC)
- **Função:** Medição de Tensão AC True RMS.
- **Isolação:** Transformador de alta precisão.
- **Range:** 0-250V AC (ajustável via potenciômetro no módulo).
- **Conexão:** GPIO 34 (ADC).

### INA219 (Tensão/Corrente/Potência DC)
- **Interface:** I2C (GPIO 27/22).
- **Range de Tensão:** 0-26V DC.
- **Range de Corrente:** ±3.2A.
- **Resolução:** 0.8mA (configurável).

---

## 4. Periféricos Integrados

### Áudio e Alertas
- **Buzzer:** Speaker integrado no conector lateral (GPIO 26).
- **LED RGB:** LED integrado na parte traseira da placa (GPIO 4, 16, 17).

### Armazenamento de Dados
- **SD Card:** Slot MicroSD integrado (HSPI).
- **NVS (Non-Volatile Storage):** Persistência de configurações via Preferences API.

---

## 5. Sonda Térmica (Externo)

### DS18B20
- **Range:** -55°C a +125°C.
- **Conexão:** GPIO 4 (OneWire).
- **Requisito:** Resistor de pull-up 4.7kΩ entre VCC e Data.

---

## 6. Especificações Elétricas

| Parâmetro | Valor |
|:---|:---|
| **Alimentação** | 5V via Micro USB ou pinos VIN |
| **Consumo Típico** | 200mA (com display em 100%) |
| **ADC Resolution** | 12-bit (0-4095) |
| **ADC Voltage Range** | 0 - 3.3V (Não exceder!) |

---

## 7. Lista de Materiais (BOM)

| Qtd | Item | Função |
|:---:|:---|:---|
| 1 | ESP32-2432S028R (CYD) | Unidade Central + Display |
| 1 | Módulo ZMPT101B | Medição de Tensão AC |
| 1 | Módulo INA219 | Medição DC (V/I/P) |
| 1 | Sonda DS18B20 | Medição de Temperatura |
| 1 | Cartão MicroSD | Database e Logs |
| 1 | Resistor 4.7kΩ | Pull-up OneWire |
| 1 | Resistor 10kΩ | Divisor para Probe Principal |
| 1 | Speaker 1W (Opcional) | Conectado ao GPIO 26 |