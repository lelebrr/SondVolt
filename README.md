# Component Tester PRO v2.0

Este é o projeto **Component Tester PRO v2.0** para Arduino Uno R3, desenvolvido com PlatformIO no VS Code. Ele visa ser um testador de componentes multifuncional com tela colorida, navegação por botões físicos, sonda térmica e logging em SD Card.

## 1. Visão Geral do Projeto

O Component Tester PRO v2.0 é um dispositivo projetado para testar uma variedade de componentes eletrônicos, fornecendo medições precisas e um julgamento automático (Good/Average/Bad). Ele se destaca pela interface de usuário intuitiva com gráficos pixel art e funcionalidades avançadas como sonda térmica portátil e registro de dados.

## 2. Hardware

- **Microcontrolador:** Arduino Uno R3
- **Display:** TFT 2.8" ILI9341 SPI (sem touch)
- **Armazenamento:** Módulo Micro SD SPI
- **Interface do Usuário:** 6 botões push táteis (Cima, Baixo, Esquerda, Direita, Selecionar, Voltar)
- **Feedback:** Buzzer piezoativo 5V, LED Verde (status OK), LED Vermelho (status Erro)
- **Sonda Térmica:** DS18B20 waterproof com cabo longo
- **Probes:** 2 saídas para ponteiras (agulha e garra jacaré, troca manual)
- **Componentes de Medição:** Resistores de precisão (3x 680 Ω 1%) e resistores de pull-up (3x 470 kΩ 1%) para o circuito de medição.

### Pinagem Obrigatória:

| Pino Arduino | Função                       |
|--------------|------------------------------|
| A0           | Probe 1 (+)                  |
| A1           | Probe 2 (-)                  |
| GND          | Terra comum                  |
| D2           | Botão Cima (Up)              |
| D3           | Botão Baixo (Down)           |
| D5           | Botão Esquerda (Left)        |
| D6           | Botão Direita (Right)        |
| D7           | Botão Selecionar (OK/Enter)  |
| D8           | Botão Voltar (Back/Esc)      |
| D9           | Buzzer piezoativo 5V         |
| D10          | LED Verde                    |
| D11          | LED Vermelho / MOSI (SPI)    |
| D12          | TFT CS / MISO (SPI)          |
| D13          | TFT DC / SCK (SPI)           |
| A2           | TFT RST                      |
| D4           | SD Card CS / Sonda DS18B20   |

*Nota: Todos os 6 botões usam `INPUT_PULLUP` e fecham para GND quando pressionados.*

## 3. Software (PlatformIO)

Este projeto é desenvolvido usando PlatformIO no VS Code. Certifique-se de ter o VS Code e a extensão PlatformIO instalados.

### `platformio.ini`

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    arduino-libraries/SD
    mathieucarbou/Encoder@^1.0.0
    paulstoffregen/OneWire
    milesburton/DallasTemperature
    Bounce2
build_flags = -Os
monitor_speed = 115200
```

### Estrutura de Arquivos

```
Component-Tester-PRO/
├── platformio.ini
├── src/
│   ├── main.cpp
│   ├── config.h
│   ├── globals.h
│   ├── buttons.h
│   ├── buttons.cpp
│   ├── leds.h
│   ├── leds.cpp
│   ├── menu.h
│   ├── menu.cpp
│   ├── measurements.h
│   ├── measurements.cpp
│   ├── thermal.h
│   ├── thermal.cpp
│   ├── drawings.h
│   ├── drawings.cpp
│   ├── icons/
│   │   ├── icon_resistor.h
│   │   ├── icon_cap_ceramic.h
│   │   ├── icon_cap_electrolytic.h
│   │   ├── icon_diode.h
│   │   ├── icon_led.h
│   │   ├── icon_transistor_to92.h
│   │   ├── icon_inductor.h
│   │   └── icon_unknown.h
│   ├── database.h
│   ├── logger.h
│   ├── logger.cpp
│   ├── buzzer.h
│   ├── buzzer.cpp
│   └── utils.h
│   └── utils.cpp
├── include/
│   └── TFT_eSPI/
│       └── User_Setup.h
└── data/                         // Pasta para arquivos SD (ex: LOG.TXT)
```

## 4. Funcionalidades

### Navegação

- **6 Botões Físicos:**
    - `Up`/`Down`: Rolar menus ou ajustar valores.
    - `Left`/`Right`: Ajustar valores ou navegar entre abas.
    - `Select` (OK/Enter): Entrar em um menu ou confirmar uma ação.
    - `Back` (Esc): Sair de um menu ou cancelar uma ação.
- **Rodapé:** Sempre exibe a legenda dos botões para facilitar a navegação.
- **Debounce:** Implementado para todos os botões para evitar leituras falsas.

### Desenhos dos Componentes

- **Estilo Pixel Art:** Ícones realistas de componentes em baixa resolução (máx. 32x32 pixels, RGB565).
- **Cores Reais:** Resistor bege com faixas EIA, capacitor cerâmico laranja/bege, eletrolítico azul escuro, diodo cinza com faixa prateada, LED transparente, transistor TO-92 preto, indutor marrom com bobina visível, etc.
- **Feedback Visual:** Ícone grande exibido ao lado do valor medido, acompanhado de um julgamento (verde para Good, amarelo para Average, vermelho para Bad).

### LEDs de Status (Verde e Vermelho)

- **Verde Fixo:** Medição OK / Componente Good / Pronto para uso.
- **Vermelho Fixo:** Componente Bad / Sobrecarga / Curto-circuito / Perigo térmico.
- **Ambos Piscando Rápido:** Medição em andamento.
- **Verde Piscando Lento:** Função Hold ativa.
- **Vermelho Piscando Rápido + Buzzer:** Temperatura >80°C ou sobrecarga.

### Sonda Térmica DS18B20

- **Modo Dedicado:** Acessível via menu principal.
- **Leitura Contínua:** Atualização a cada 500 ms.
- **Uso:** O usuário encosta a sonda manualmente nos componentes.
- **Alertas Visuais e Sonoros:**
    - `Temp: XX°C – Normal` (verde)
    - `Temp: XX°C – Quente! Cuidado` (amarelo)
    - `Temp: XX°C – Muito Quente! Perigo` (laranja)
    - `Temp: XX°C – PERIGO! DESLIGUE!` (vermelho + LED vermelho fixo + beep longo)
- **Limites Configuráveis:** Definidos em `config.h`.

### Medições (com apenas 2 probes)

- Capacitor + ESR + leakage + julgamento
- Resistor
- Diodo / LED / Zener
- Transistor (NPN/PNP/MOSFET) – pinout manual
- Indutor
- Voltímetro DC
- Frequency Counter
- PWM Generator
- Teste de optoacoplador
- Cable Tester / continuidade estendida
- Ponte retificadora
- Auto Detect
- Modo Continuidade com buzzer

### Outras Funcionalidades (39 leves acumuladas)

- Modo repetido, beep progressivo, hold, comparador, contador de medições/defeituosos, modo escuro, silent mode, timeout, versão do firmware, etc.

### Logging SD

- **Registro Automático:** Todo resultado de medição é salvo em `LOG.TXT`.
- **Formato:** `Time: [millis]ms, Type: [tipo], Value: [valor], Temp: [temperatura], Judgment: [julgamento]`.

## 5. Requisitos de Código e Otimização

- **Strings:** Todas as strings de texto usam a macro `F()` para economizar RAM.
- **Ícones:** Armazenados em arquivos `.h` separados na pasta `src/icons/` como arrays `PROGMEM` RGB565.
- **Comentários:** Código 100% comentado em português.
- **Otimização:** Compilação com `-Os`, evitar a classe `String`, usar `char arrays`.
- **Tratamento de Erros:** Completo para sobrecarga, curto-circuito, timeout.
- **Splash Screen:** Exibe a versão "Component Tester PRO v2.0 – Leandro" na inicialização.

## 6. Como Compilar e Fazer Upload

1.  Abra o projeto no VS Code com a extensão PlatformIO instalada.
2.  Conecte seu Arduino Uno R3 ao computador.
3.  No PlatformIO, clique no ícone de "Upload" (seta para a direita) na barra inferior para compilar e carregar o código para o Arduino.
4.  Para monitorar a saída serial, clique no ícone de "Monitor Serial" (plug) na barra inferior.

---

**Desenvolvido por:** Manus AI (com base nas especificações de Leandro)
**Data:** Mar 04, 2026
