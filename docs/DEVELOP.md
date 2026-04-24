# Developer Guide - Component Tester PRO v2.0

## Índice

1. [Visão Geral do Código](#1-visão-geral-do-código)
2. [Estrutura de Arquivos](#2-estrutura-de-arquivos)
3. [Variáveis Globais](#3-variáveis-globais)
4. [Configuração de Pinos](#4-configuração-de-pinos)
5. [Menu System](#5-menu-system)
6. [Medições](#6-medições)
7. [Sonda Térmica](#7-sonda-térmica)
8. [EEPROM](#8-eeprom)
9. [Adicionar Novos Recursos](#9-adicionar-novos-recursos)
10. [Compilação](#10-compilação)
11. [Limitações](#11-limitações)

---

## 1. Visão Geral do Código

### Arquitetura

```
┌─────────────────────────────────────────────────────────────────────┐
│                     ARQUITETURA DO SISTEMA                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   main.cpp                                                          │
│   ├── setup()           → Inicialização                            │
│   │   ├── init_TFT()     → Display                                  │
│   │   ├── init_buttons() → Botões                                   │
│   │   ├── init_probes()  → Probes                                  │
│   │   ├── init_thermal()→ Sonda DS18B20                          │
│   │   └── loadSettings()→ EEPROM                                   │
│   │                                                                   │
│   └── loop()            → Loop principal                           │
│       ├── menu_handle()  → Menu principal                           │
│       ├── measurements_handle() → Medições                        │
│       ├── thermal_handle()→ Térmica                               │
│       └── (outros)                                                 │
│                                                                      │
│   Módulos:                                                          │
│   ├── config.h            → Pinos e constantes                     │
│   ├── globals.h          → Variáveis globais                       │
│   ├── menu.cpp/h         → Navegação                               │
│   ├── measurements.cpp/h→ Funções de medição                      │
│   ├── buttons.cpp/h     → Sistema de botões                       │
│   ├── leds.cpp/h        → LEDs                                    │
│   ├── buzzer.cpp/h       → Som                                      │
│   ├── drawings.cpp/h    → UI e ícones                             │
│   ├── thermal.cpp/h    → DS18B20                                 │
│   ├── logger.cpp/h      → SD Card                                 │
│   └── database.cpp/h   → Dados de componentes                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Estrutura de Arquivos

### Files Principais

```
src/
├── main.cpp           // setup(), loop()
├── config.h           // Definições de pinos e constantes
├── globals.h          // Structs Settings, History, variáveis globais
├── menu.cpp/h         // Menu e navegação
├── measurements.cpp/h // 13 funções de medição
├── buttons.cpp/h     // Leitura de botões
├── leds.cpp/h        // Controle de LEDs
├── buzzer.cpp/h      // Sounds
├── drawings.cpp/h   // UI screens
├── thermal.cpp/h    // DS18B20
├── logger.cpp/h     // SD Card logging
├── database.cpp/h   // Busca de componentes (não usado)
├── utils.cpp/h      // Funções utilitárias
└── logger.cpp/h     // SD Card
```

### platformio.ini

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
lib_deps =
    adafruit/Adafruit ILI9341@^1.10
    adafruit/Adafruit GFX Library@^1.10
    adafruit/Adafruit TouchScreen@^1.0
    paulstoffregen/OneWire@^2.3
    milesburton/DallasTemperature@^3.9
monitor_speed = 9600
upload_speed = 115200
```

---

## 3. Variáveis Globais

### Struct Settings

```cpp
struct Settings {
  float offset1;              // Calibração probe 1 (+-10)
  float offset2;              // Calibração probe 2 (+-10)
  bool darkMode;              // Tema escuro
  bool silentMode;           // Sem beeps
  unsigned long timeoutDuration; //Timeout(ms): 30000/60000
  unsigned long totalMeasurements;    // Total de medições
  unsigned long faultyMeasurements;  // Total defeituosos
};
extern Settings deviceSettings;
```

### Struct History

```cpp
struct MeasurementHistory {
  char name[12];       // "Cap", "Res", etc
  float value;         // Valor medido
  float temp;         // Temperatura no momento
  bool isGood;       // true=verde, false=vermelho
};
#define HISTORY_SIZE 10
extern MeasurementHistory measurementHistory[HISTORY_SIZE];
```

### Estados

```cpp
enum AppState {
  STATE_MENU,
  STATE_MEASURING,
  STATE_THERMAL_PROBE,
  STATE_SCANNER,
  STATE_HISTORY,
  STATE_STATS,
  STATE_SETTINGS,
  STATE_ABOUT
};
extern AppState currentAppState;
```

---

## 4. Configuração de Pinos

### config.h

```cpp
// Probes
#define PROBE1_PIN A0
#define PROBE2_PIN A1

// Botões
#define BTN_UP_PIN 2
#define BTN_DOWN_PIN 3
#define BTN_LEFT_PIN 5
#define BTN_RIGHT_PIN 6
#define BTN_OK_PIN 7
#define BTN_BACK_PIN 8

// Output
#define BUZZER_PIN 9
#define LED_GREEN_PIN 10
#define LED_RED_PIN 11

// TFT
#define TFT_CS_PIN 12
#define TFT_DC_PIN 13
#define TFT_RST_PIN A2

// SD & Thermal
#define SD_CS_PIN 4
#define ONEWIRE_BUS_PIN A3

// Temperature thresholds
#define TEMP_NORMAL_THRESHOLD 70.0
#define TEMP_HOT_THRESHOLD 90.0
#define TEMP_DANGER_THRESHOLD 110.0
```

---

## 5. Menu System

### Menu Principal

```cpp
MenuItem menuItems[] = {
  {"Medir", STATE_MEASURING},
  {"Termica", STATE_THERMAL_PROBE},
  {"Scanner", STATE_SCANNER},
  {"Historico", STATE_HISTORY},
  {"Estatist", STATE_STATS},
  {"Config", STATE_SETTINGS},
  {"Sobre", STATE_ABOUT}
};
```

### Navegação

```cpp
// Em menu.cpp
void menu_handle() {
  buttons_update();
  
  if (isUpPressed()) {
    currentMenuItem--;
    draw_menu();
  }
  if (isDownPressed()) {
    currentMenuItem++;
    draw_menu();
  }
  if (isOkPressed()) {
    currentAppState = menuItems[currentMenuItem].targetState;
    // Handler específico
  }
}
```

---

## 6. Medições

### Exemplo: Medir Resistor

```cpp
void measure_resistor() {
  // Configura pinos
  pinMode(PROBE1_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, LOW);
  pinMode(PROBE2_PIN, INPUT_PULLUP);
  
  // Mede
  delay(10);
  int raw = analogRead(PROBE2_PIN);
  
  // Calcula
  float resistance;
  if (raw < 1022) {
    resistance = 35000.0 * (float)raw / (1023.0 - (float)raw);
  } else {
    resistance = -1.0;
  }
  
  // Exibe resultado
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  if (resistance > 0) {
    tft.println("Resistor OK");
    set_green_led(true);
    addToHistory("Res", resistance, currentTemperature, true);
  } else {
    tft.println("OPEN");
    set_red_led(true);
    increment_faulty_count();
  }
  
  showBackMsg();
}
```

### All Measurement Functions

| Função | Descrição |
|-------|----------|
| measure_capacitor() | Capacitor + ESR via RC |
| measure_resistor() | Resistência via divisor |
| measure_diode() | Diodo/LED testar |
| measure_transistor() | Identificar NPN |
| measure_inductor() | Indutância via tempo |
| measure_voltmeter_dc() | Tensão 0-5V |
| measure_frequency_counter() | Frequência 1Hz-1MHz |
| generate_pwm() | PWM 1kHz 50% |
| test_optocoupler() | Optocoupler testar |
| test_cable_continuity() | Cabo testar |
| test_bridge_rectifier() | Bridge testar |
| auto_detect_component() | Auto-detecção |
| test_continuity_buzzer() | Continuidade + beep |

---

## 7. Sonda Térmica

### Inicialização

```cpp
// Em main.cpp setup()
OneWire oneWire(ONEWIRE_BUS_PIN);
sensors.setOneWire(&oneWire);
sensors.begin();
```

### Leitura

```cpp
// Em thermal.cpp
void update_temperature() {
  sensors.requestTemperatures();
  currentTemperature = sensors.getTempCByIndex(0);
  
  // Verificar alertas
  if (currentTemperature > TEMP_DANGER_THRESHOLD) {
    // Alerta crítico
    play_beep(200);
    set_red_led(true);
  } else if (currentTemperature > TEMP_HOT_THRESHOLD) {
    // Alerta quente
    // ...
  }
}
```

---

## 8. EEPROM

### Salvar

```cpp
void saveSettings() {
  EEPROM.put(0x00, deviceSettings.offset1);
  EEPROM.put(0x04, deviceSettings.offset2);
  EEPROM.put(0x08, deviceSettings.darkMode);
  EEPROM.put(0x09, deviceSettings.silentMode);
  EEPROM.put(0x0A, deviceSettings.timeoutDuration);
  EEPROM.put(0x0E, deviceSettings.totalMeasurements);
  EEPROM.put(0x12, deviceSettings.faultyMeasurements);
}
```

### Carregar

```cpp
void loadSettings() {
  EEPROM.get(0x00, deviceSettings.offset1);
  EEPROM.get(0x04, deviceSettings.offset2);
  // ... etc
}
```

---

## 9. Adicionar Novos Recursos

### Novo Tipo de Medição

```cpp
// 1. Adicionar ao array em measurements.cpp
const char m13[] PROGMEM = "Novo";

// measurementNames[] = {...}

// 2. Nome no menu (measurements.cpp)
measurementNames[13] = m13;

// 3. Adicionar função (measurements.cpp)
void measure_novo() {
  setupTela(F("Novo Teste:"));
  // Código aqui
  showBackMsg();
}

// 4. Adicionar ao switch (measurements.cpp)
case 13:
  measure_novo();
  break;
```

### Novo Menu

```cpp
// 1. Adicionar item em menu.cpp
{"NovoItem", STATE_NOVO},

// 2. Adicionar case no menu_handle()
else if (currentAppState == STATE_NOVO) {
  draw_novo_screen();
}

// 3. Criar handler
void handle_novo() {
  buttons_update();
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
  }
}
```

---

## 10. Compilação

### Comandos

```bash
# Compilar
pio run

# Upload
pio run --target upload

# Monitor Serial
pio run --target monitor

# Limpar e compilar
pio run --target clean
pio run
```

### Output Esperado

```
Building...
Linking .pio\build\uno\src\main.cpp.o
Linking .pio\build\uno\src\measurements.cpp.o
...
Linking .pio\build\uno\firmware.hex
Flash: 31002 bytes (96.1%)
RAM: 1513 bytes (73.9%)
```

---

## 11. Limitações

### Memória

| Recurso | Uso | Disponível | Porcentagem |
|---------|-----|------------|--------------|
| Flash | 31,002 bytes | 32,256 bytes | 96.1% |
| RAM | 1,513 bytes | 2,048 bytes | 73.9% |

⚠️ **ATENÇÃO:** Memória Flash está muito cheia (96%)!Qualquer nova funcionalidade deve ser pequena.

### Limites de Medição

| Componente | Mínimo | Máximo |
|------------|--------|--------|
| Resistor | 1Ω | 1MΩ |
| Capacitor | 1µF | 1000µF |
| Tensão DC | 0V | 5V |
| Frequência | 1Hz | 1MHz |

---

## API Reference

### Funções Exportadas

```cpp
// Histórico
void addToHistory(const char* name, float value, float temp, bool isGood);

// EEPROM
void loadSettings();
void saveSettings();

// Calibração
void calibrate_probes();

// Contagem
void increment_measurement_count();
void increment_faulty_count();

// LEDs
void set_green_led(bool state);
void set_red_led(bool state);

// Buzzer
void play_beep(int duration);

// Thermal
float get_current_temperature();
```

---

## Contribuindo

1. Fork o projeto
2. Crie uma branch para sua feature
3. Faça as alterações
4. Teste
5. Submit pull request

---

**Nota:** Este projeto está com memória quase cheia. Qualquer modificação deve ser cuidadosamente considerada.