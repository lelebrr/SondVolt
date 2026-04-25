# 💻 Guia do Desenvolvedor — Sondvolt v3.2

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

Este guia é destinado a desenvolvedores que desejam modificar, estender ou contribuir para o firmware do Sondvolt.

---

## 1. Arquitetura do Sistema

O firmware v3.0 foi desenvolvido especificamente para o **ESP32 (CYD)**, aproveitando sua arquitetura dual-core de 240MHz e barramentos SPI dedicados para máxima performance.

### Stack Tecnológico

| Componente | Tecnologia |
|:---|:---|
| **Framework** | Arduino Framework para ESP32 |
| **Display** | TFT_eSPI com DMA (ILI9341) |
| **Touchscreen** | XPT2046 (resistivo) |
| **Armazenamento** | SD Card via HSPI |
| **Persistência** | NVS (Preferences) |
| **Sensores** | I2C (INA219), OneWire (DS18B20), ADC (ZMPT101B) |

---

## 2. Estrutura de Diretórios

```
Component_Tester/
├── src/
│   ├── main.cpp              # Ponto de entrada, loop e splash
│   ├── config.h              # Definições de pinagem e constantes
│   ├── globals.h            # Estados da máquina e variáveis globais
│   ├── menu.cpp/h            # Sistema de menu grid e navegação
│   ├── measurements.cpp/h  # Rotinas de medição de componentes
│   ├── multimeter.cpp/h     # Multímetro AC/DC (ZMPT + INA219)
│   ├── database.cpp/h        # Gerenciamento SD e índice em RAM
│   ├── drawings.cpp/h       # Primitivas gráficas e ícones
│   ├── buttons.cpp/h        # Handler de toque e debouncing
│   ├── buzzer.cpp/h         # Geração de tons
│   ├── leds.cpp/h          # Controle do LED RGB
│   ├── thermal.cpp/h        # Driver OneWire para DS18B20
│   └── utils.cpp/h          # Funções matemáticas e calibração
├── sd_files/
│   └── COMPBD.CSV           # Banco de dados de componentes
├── platformio.ini           # Configuração PlatformIO
└── README.md               # Visão geral do projeto
```

---

## 3. Máquina de Estados

O sistema opera em uma máquina de estados centralizada em `globals.h`:

| Estado | Descrição |
|:---|:---|
| `STATE_MENU` | Menu principal grid |
| `STATE_MEASURE` | Menu de medição de componentes |
| `STATE_MULTIMETER` | Multímetro AC/DC |
| `STATE_THERMAL` | Monitoramento de temperatura |
| `STATE_SCANNER` | Auto-detecção contínua |
| `STATE_HISTORY` | Visualização de logs |
| `STATE_SETTINGS` | Configurações e calibração |

---

## 4. Pinagem used by Firmware

### Pinos Reservados

| Função | GPIO | Barramento | Notas |
|:---|:---:|:---:|:---|
| TFT_CS | 15 | VSPI | Chip Select |
| TFT_DC | 2 | VSPI | Data/Command |
| TFT_SCK | 14 | VSPI | Clock 40MHz |
| TFT_MOSI | 13 | VSPI | Dados → Display |
| SD_CS | 5 | HSPI | Chip Select |
| Touch_CS | 33 | SoftSPI | Chip Select |
| **Probes** | 35 | ADC | Probe de componentes |
| **AC (ZMPT)** | 34 | ADC | Tensão AC |
| **I2C SDA** | 27 | I2C | Dados |
| **I2C SCL** | 22 | I2C | Clock |
| **OneWire** | 4 | 1-Wire | DS18B20 |
| **Buzzer** | 26 | DAC | Áudio |

---

## 5. Sistema de Database (COMPBD.CSV)

### Carregamento no Boot

1. O sistema abre `COMPBD.CSV` uma vez durante o boot
2. Cada linha é parseada e os índices são armazenados em RAM
3. Categorias são indexadas para busca rápida

### Estrutura do Database em RAM

```cpp
struct ComponentEntry {
    char type[8];        // NPN, PNP, DIODE, LED
    uint16_t hFE_min;
    uint16_t hFE_max;
    float Vf_min;
    float Vf_max;
    char partNumber[16];
};
```

### Busca de Componentes

```cpp
// Pseudocódigo de busca
for (entry : database) {
    if (entry.type == measured.type &&
        entry.hFE_min <= measured.hFE && measured.hFE <= entry.hFE_max &&
        entry.Vf_min <= measured.Vf && measured.Vf <= entry.Vf_max) {
        return entry.partNumber;
    }
}
```

---

## 6. Sensores Externos

### ZMPT101B (Tensão AC True RMS)

- **GPIO:** 34 (ADC)
- **Cálculo:** `Vrms = LeituraADC × (EscalaZMPT / 4095) × 250`
- **True RMS:** Implementado via algoritmo de integração numérica

### INA219 (Tensão/Corrente DC)

- **Endereço I2C:** 0x40
- **Biblioteca:** `Adafruit_INA219`
- **Cálculo:** `Potência = Tensão × Corrente`

### DS18B20 (Temperatura)

- **GPIO:** 4 (OneWire)
- **Biblioteca:** `OneWire` + `DallasTemperature`

---

## 7. Logging no SD Card

### Arquivo LOG.TXT

Cada medição é logged no formato:

```
[YYYY-MM-DD HH:MM:SS] Tipo: Valor [PartNumber]
[2026-04-24 14:30:25] Resistor: 10kΩ
[2026-04-24 14:31:10] Multímetro DC: V=5.02V I=0.45A P=2.26W
```

### Implementação

```cpp
void logMeasurement(const char* type, const char* value) {
    File logFile = SD.open("/LOG.TXT", FILE_APPEND);
    logFile.print("[");
    logFile.print(getTimestamp());
    logFile.print("] ");
    logFile.print(type);
    logFile.print(": ");
    logFile.println(value);
    logFile.close();
}
```

---

## 8. Otimizações ESP32

### Gerenciamento de Memória

- **Database Index:** Carregado uma vez no boot, índices em RAM
- **Buffers:** 512 bytes para minimizar acessos SPI
- **Heap:** Mínimo de 50KB reservado para medições

### Performance Gráfica

- **Driver:** TFT_eSPI configurado para VSPI 40MHz
- **DMA:** Habilitado para transferência background
- **Ícones:** Primitivas GFX (sem bitmaps)

### Barramentos SPI Separados

- **VSPI:** Display TFT (15, 2, 14, 13, 12)
- **HSPI:** SD Card (5, 18, 23, 19)
- **SoftSPI:** Touchscreen (33, 25, 32, 39)

---

## 9. Adicionando Novos Modos de Medição

### Passo 1: Adicionar ao Menu

Edite `menu.cpp` para incluir o novo modo:

```cpp
const char* modes[] = {
    "Resistor",
    "Capacitor",
    "NovoModo",  // Novo modo aqui
    // ...
};
```

### Passo 2: Implementar a Função

Em `measurements.cpp`:

```cpp
float measureNovoModo() {
    // Sua lógica de medição aqui
    return result;
}
```

### Passo 3: Integrar ao Handler

Em `measurements_handle()`:

```cpp
case MODE_NOVO:
    value = measureNovoModo();
    break;
```

---

## 10. Contribuição

### Boas Práticas

1. **Modularidade:** Mantenha cada funcionalidade em seu próprio arquivo
2. **Documentação:** Comente funções exportadas
3. **Testes:** Teste em hardware real antes de submeter
4. **Estilo:** Siga o padrão do código existente

### Como Contribuir

1. Fork o repositório
2. Crie uma branch: `git checkout -b feature/nova-funcao`
3. Faça commit das alterações
4. Envie para revisão

---

## Especificações Técnicas (Resumo)

| Recurso | Valor |
|:---|:---|
| **Clock** | 240MHz |
| **RAM** | 520KB |
| **Flash** | 4MB |
| **ADC** | 12-bit (4095 níveis) |
| **SPI Display** | 40MHz |
| **SPI SD** | 20MHz |

---

<p align="center">
<i>💻 Sondvolt v3.2 — Guia do Desenvolvedor</i>
</p>