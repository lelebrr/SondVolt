# Solução de Problemas - Component Tester PRO v2.0

## Índice

1. [Problemas Gerais](#1-problemas-gerais)
2. [Problemas de Display](#2-problemas-de-display)
3. [Problemas de Medição](#3-problemas-de-medição)
4. [Problemas de Botões](#4-problemas-de-botões)
5. [Problemas de LEDs/Buzzer](#5-problemas-de-ledsbuzzer)
6. [Problemas de Sonda Térmica](#6-problemas-de-sonda-térmica)
7. [Problemas de SD Card](#7-problemas-de-sd-card)
8. [Problemas de Software](#8-problemas-de-software)
9. [Códigos de Erro](#9-códigos-de-erro)

---

## 1. Problemas Gerais

### 🔴 Dispositivo Não Liga

**Sintoma:** A tela fica preta aoligar.

**Causas possíveis:**
1. Falta de energia
2. Conexões soltas
3. Firmware não carregado

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  SOLUÇÃO 1: Verificar Energia                          │
│                                                          │
│  ✅ Conecte o Arduino ao PC ou fonte de 7-12V            │
│  ✅ Verifique o LED PWR do Arduino (verde)                 │
│  ✅ Use um cabo USB em boas condições                    │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  SOLUÇÃO 2: Verificar Conexões                         │
│                                                          │
│  ✅ Todos os fios estão bem conectados?                 │
│  ✅ Não há fios soltos ou quebrados?                  │
│  ✅ As conexões correspondem ao pino correto?         │
│                                                          │
│  Verifique especialmente:                              │
│  - TFT: D12, D13, A2                                   │
│  - Botões: D2, D3, D5, D6, D7, D8                    │
│  - LEDs: D10, D11                                       │
│  - Buzzer: D9                                           │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  SOLUÇÃO 3: Carregar o Firmware                         │
│                                                          │
│  Execute:                                               │
│                                                          │
│  pio run --target upload                                │
│                                                          │
│  Verifique se o upload foi bem-sucedido                 │
└─────────────────────────────────────────────────────────────┘
```

---

### 🔴 Tela Congela

**Sintoma:** O dispositivo para de responder.

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  SOLUÇÃO: Reset do Sistema                              │
│                                                          │
│  1. Desligue o dispositivo                            │
│  2. Aguarde 5 segundos                                │
│  3. Ligue novamente                                   │
│                                                          │
│  Se persistir:                                          │
│  4. Recarregue o firmware                              │
│  5. Verifique conexões dos botones (pinos D2-D8)      │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Problemas de Display

### 🟡 Display Sem Imagem

**Sintoma:** TFT mostra apenas branco ou nada.

**Causas:**
1. Conexão SPI incorreta
2. Backlight não conectado
3. Conflito de pinos

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  VERIFIQUE: Conexões do TFT                              │
│                                                          │
│  Pino TFT          Pino Arduino                           │
│  ─────────────────────────────────────────────           │
│  VCC              3.3V                                   │
│  GND              GND                                     │
│  CS               D12                                    │
│  DC               D13                                    │
│  RST              A2                                     │
│  MOSI             D11                                    │
│  SCK              D13                                    │
│  LED (backlight)  3.3V via resistor 150Ω                  │
│                                                          │
│  ⚠️ Não conecte 5V diretamente no TFT!                │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  SE AINDA NÃO FUNCIONAR:                                 │
│                                                          │
│  1. Teste o backlight:                                   │
│     Conecte 3.3V diretamente ao pino LED do TFT        │
│                                                          │
│  2. Verifique level shifter:                              │
│     TFT opera em 3.3V - use level shifter se necessário  │
│                                                          │
│  3. Teste com programa simples:                          │
│     Carregue exemplo da biblioteca Adafruit            │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 Display Com Defeitos Visuais

**Sintoma:** Linhas, pontos, ou cores erradas.

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  LINHAS NA TELA:                                          │
│  - Problema de conexão SPI                              │
│  - Verifique fios MOSI, SCK                            │
│                                                          │
│  CORES ERRADAS:                                         │
│  - Bibliotecas desatualizadas                            │
│  - Execute pio lib update                               │
│                                                          │
│  PONTOS BRANCOS:                                        │
│  - Defeito físico do display                          │
│  - Substitua o TFT                                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Problemas de Medição

### 🟡 Medições Imprecisas

**Sintoma:** Valores diferentes do esperado.

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  PASSO 1: Calibração                                     │
│                                                          │
│  Menu > Config > Calibrar Probes                         │
│                                                          │
│  Siga as instruções na tela                            │
│  para ajustar os offsets de medição                   │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  PASSO 2: Verifique os Probes                           │
│                                                          │
│  ✅ Probes estão limpos?                                 │
│  ✅ Pontas de prova em bom estado?                      │
│  ✅ Fios semquebrados?                                  │
│  ✅ Conexão A0 e A1 está firme?                        │
│                                                          │
│  Limpe as pontas com álcool isopropílico               │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  PASSO 3: Temperatura Ambiente                          │
│                                                          │
│  A precisão pode variar com temperatura                  │
│                                                          │
│  Tabela de correção aproximada:                        │
│  - 20°C: ±1%                                            │
│  - 25°C: ±2%                                            │
│  - 30°C: ±3%                                            │
│  - >35°C: considerar mais erro                         │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 "OPEN" em Componentes Ok

**Sintoma:** Mostra "OPEN" mas componente está bom.

**Causas:**
1. Conexão ruim
2. Componente fora do range
3. Probe mal conectado

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  VERIFIQUE:                                              │
│                                                          │
│  1. Conexão física:                                       │
│     - Probes tocam bem o componente?                     │
│     - Fios não estão quebrados?                          │
│                                                          │
│  2. Componente:                                          │
│     - Está dentro do range de medição?                  │
│                                                          │
│  3. Tente novamente:                                     │
│     - Desconecte e reconecte                           │
│     - Limpe as pontas                                   │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 Resistor Shows 0 Ohms (Mas está bom)

**Solução:**

```
┌─────────────────────────────────────────────────────────────┐
│  Isso geralmente indica CURTO, não 0Ω real               │
│                                                          │
│  POSSÍVEIS CAUSAS:                                       │
│  - Resistor queimado                                      │
│  - Poeira entre terminais                               │
│  - Umidade                                              │
│                                                          │
│  TENTE:                                                  │
│  - Limpe o resistor com álcool                         │
│  - Seque completamente                                   │
│  - Substitua se necessário                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. Problemas de Botões

### 🟡 Botão Não Responde

**Sintoma:** Pressionar botão não faz nada.

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  BOTÃO NÃO RESPONDE:                                      │
│                                                          │
│  1. Identifique qual botão não funciona:                │
│     UP → D2    DOWN → D3    LEFT → D5                   │
│     RIGHT → D6    OK → D7    BCK → D8                    │
│                                                          │
│  2. Verifique conexão:                                   │
│     +5V ──[10kΩ]── Pino ──[Botão]── GND                  │
│                                                          │
│  3. Teste com multímetro:                                 │
│     - Sem pressionar: Alto (5V)                        │
│     - Pressionado: Baixo (0V)                          │
│                                                          │
│  4. Pino solto?                                           │
│     - Verifique conexão no Arduino                       │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 Botão Apertado Sempre

**Sintoma:** Botão funciona mesmo sem pressionar.

**Solução:**

```
┌─────────────────────────────────────────────────────────────┐
│  BOTÃO APERTADO SEMPRE:                                   │
│                                                          │
│  Causa: resistor de pull-up solto ou queimado            │
│                                                          │
│  Solução:                                                │
│  1. Substitua resistor 10kΩ                             │
│  2. Verifique conexão                                  │
│  3. Use pull-up interno: pinoMode(pino, INPUT_PULLUP)  │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. Problemas de LEDs/Buzzer

### 🔴 LEDs Não Acendem

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  LED NÃO ACENDE:                                          │
│                                                          │
│  1. Identifique:                                          │
│     Verde → D10    Vermelho → D11                        │
│                                                          │
│  2. Teste simples:                                        │
│     digitalWrite(D10, HIGH);  // Verde                 │
│     digitalWrite(D11, HIGH);  // Vermelho              │
│                                                          │
│  3. Verifique hardware:                                    │
│     LED(+) ──[220Ω]── Pino ── LED(-)── GND               │
│                                                          │
│  4. Substitua LED se necessário                        │
└─────────────────────────────────────────────────────────────┘
```

### 🔴 Buzzer Não Toca

**Soluções:**

```
┌───────────────────────────────────────────────���─���───────────┐
│  BUZZER NÃO TOCA:                                          │
│                                                          │
│  1. Verifique pino: D9 (PWM)                             │
│                                                          │
│  2. Teste: tone(D9, 1000); delay(500); noTone(D9);        │
│                                                          │
│  3. Verifique modo silencioso:                            │
│     Menu > Config > Modo Silencioso                       │
│                                                          │
│  4. Verifique conexão:                                    │
│     D9 ──[100Ω]── Buzzer(+) ── Buzzer(-)── GND           │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Problemas de Sonda Térmica

### 🟡 Temperatura Errada ou "--"

**Sintoma:** Mostra "--" ou temperatura impossível.

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  SONDA NÃO DETECTADA:                                       │
│                                                          │
│  1. Verifique pino: A3 (ONEWIRE_BUS)                     │
│                                                          │
│  2. Verifique conexões:                                   │
│     5V ──[4.7kΩ]── A3 ── Sonda DQ (amarelo)             │
│     GND ──────────── Sonda GND (preto)                   │
│                                                          │
│  3. Biblioteca:                                           │
│     Verifique OneWire e DallasTemperature                 │
│                                                          │
│  4. Endereço:                                             │
│     Use scanner OneWire para verificar endereço          │
└─────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│  TEMPERATURA ERRADA:                                       │
│                                                          │
│  -55°C ou 85°C fixo: sensor com problema                │
│  127°C: sensorOW parasitic power ok                     │
│                                                          │
│  Substitua a sonda DS18B20 se necessário               │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 Alertas Não Disparam

**Verificar:**
1. Menu > Config > Mode Silencioso = OFF?
2. Sonda conectada corretamente?
3. temperatura acima de threshold?

---

## 7. Problemas de SD Card

### 🔴 SD Card Não Funciona

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  SD CARD PROBLEMAS:                                       │
│                                                          │
│  1. Verifique pino CS: D4                                │
│                                                          │
│  2. Formato: FAT16 ou FAT32                             │
│                                                          │
│  3. Tamanho: até 32GB                                   │
│                                                          │
│  4. Biblioteca correta:                                 │
│     SD.h (built-in PlatformIO)                          │
│                                                          │
│  5. Teste simples:                                       │
│     File dataFile = SD.open("test.txt", FILE_WRITE);    │
│     if(dataFile) { dataFile.close(); }                   │
└─────────────────────────────────────────────────────────────┘
```

---

## 8. Problemas de Software

### 🔴 Erro de Compilação

**Soluções:**

```
┌─────────────────────────────────────────────────────────────┐
│  ERRO DE COMPILAÇÃO:                                      │
│                                                          │
│  1. Bibliotecas faltando:                                 │
│     pio lib install "adafruit/Adafruit ILI9341"          │
│                                                         │
│  2. Versão PlatformIO:                                   │
│     Verifique platformio.ini                            │
│                                                          │
│  3. Arquivos faltando:                                  │
│     Verifique src/                                     │
│                                                          │
│  4. Limpe e recompile:                                   │
│     pio run --target clean                              │
│     pio run                                            │
└─────────────────────────────────────────────────────────────┘
```

### 🟡 Memória Insuficiente

**Sintoma:** "Low memory" ou comportamentos inesperados.

```
┌─────────────────────────────────────────────────────────────┐
│  MEMÓRIA:                                                 │
│                                                          │
│  Uso atual:                                               │
│  - Flash: 96.1% (31,002 bytes)                           │
│  - RAM: 73.9% (1,513 bytes)                             │
│                                                          │
│  ⚠️ MUITO PERTO DO LIMITE!                                │
│                                                          │
│  Se houver problemas:                                      │
│  - Reduza strings no código                             │
│  - Use PROGMEM para strings fixas                        │
│  - Evite variáveis grandes locais                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 9. Códigos de Erro

### Tabela de Erros

| Código | Mensagem | Causa | Solução |
|--------|---------|-------|--------|
| E01 | "No Diode" | Diodo não detectado | Verificar conexão |
| E02 | "OPEN" | Componente aberto | Verificar componente |
| E03 | "Not Found" | Transistor não achado | Verificar transistor |
| E04 | "No Signal" | Sem frequência | Verificar sinal |
| E05 | "FAILED" | Teste falhou | Verificar componente |
| E06 | "--" | Sonda não detectada | Verificar DS18B20 |

---

## Checklist de Diagnóstico

```
┌─────────────────────────────────────────────────────────────────────┐
│              CHECKLIST DE DIAGNÓSTICO                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  [ ] Arduino recebe energia? (LED PWR acesso)                       │
│  [ ] Display TFT funciona? (imagem normal)                         │
│  [ ] Botões funcionam? (navegação ok)                             │
│  [ ] Medições fazem sentido? (componentes conhecidos)               │
│  [ ] LEDs acendem? (verde/vermelho)                                │
│  [ ] Buzzer toca? (sons audíveis)                                 │
│  [ ] Sonda térmica mede? (temperatura ambiente)                   │
│  [ ] SD Card funciona? (le/gravação)                            │
│  [ ] EEPROM salva? ( configurações persistem)                   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Quando Substituir Componentes

| Componente | Quando Substituir |
|-----------|-------------------|
| Arduino Uno | Se não liga após verificações |
| TFT Display | Se não mostra imagem |
| Sonda DS18B20 | Se retorna "--" ou values impossíveis |
| Probes | Se danificados ou oxidados |
| Botões | Se não respondem após reparo |

---

**Precisa de mais ajuda?** Leia o [FAQ.md](FAQ.md) ou entre em contato com o desenvolvedor.