# Guias Passo a Passo - Component Tester PRO v2.0

## Índice

1. [ Guias de Medição](#1-guias-de-medição)
2. [Medir Capacitor](#2-medir-capacitor)
3. [Medir Resistor](#3-medir-resistor)
4. [Testar Diodo/LED](#4-testar-diodoled)
5. [Identificar Transistor](#5-identificar-transistor)
6. [Medir Indutor](#6-medir-indutor)
7. [Medir Tensão DC](#7-medir-tensão-dc)
8. [Contar Frequência](#8-contar-frequência)
9. [Gerar PWM](#9-gerar-pwm)
10. [Testar Optocoupler](#10-testar-optocoupler)
11. [Testar Cabo](#11-testar-cabo)
12. [Testar Bridge](#12-testar-bridge)
13. [Detecção Automática](#13-detecção-automática)
14. [Teste de Continuidade](#14-teste-de-continuidade)
15. [Usar Sonda Térmica](#15-usar-sonda-térmica)
16. [Usar Scanner](#16-usar-scanner)
17. [Ver Histórico](#17-ver-histórico)
18. [Ver Estatísticas](#18-ver-estatísticas)
19. [Configurar Dispositivo](#19-configurar-dispositivo)

---

## 1. Guias de Medição

Estes guias mostram passo a passo como usar cada função de medição.

---

## 2. Medir Capacitor

### O que você precisa saber

**Para que serve:** Medir a capacitância de capacitores em microfarads (µF).

**Quando usar:** Quando você precisa saber o valor de um capacitor ou verificar se está bom.

**O que você precisa:**
- 1 Capacitor (qualquer valor entre 1µF e 1000µF)
-Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o capacitor aos probes:      │
│                                         │
│   Probe (+) ──┬── Terminal 1            │
│              │                          │
│   Probe (-) ─┴── Terminal 2            │
│                                         │
│   (A polaridade não importa para        │
│    capacitores não polarizados)           │
└─────────────────────────────────────────┘
```

⚠️ **IMPORTANTE:** Para capacitores polarizados (eletrolíticos), o terminal mais longo é o (+).

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│   ► Cap                             │
│     Res                             │
│     Diod                            │
│     Trans                           │
│     Ind                             │
│     Volt                            │
│     Freq                            │
│     PWM                             │
│     Opto                            │
│     Cable                           │
│     Bridge                          │
│     Auto                            │
│     Conti                           │
│                                         │
│   Use UP/DW para selecionar "Cap"       │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                    │
│                                         │
│   Capacitor:                          │
│                                         │
│   100.00 uF                         │
│                                         │
│   [Verde = OK | Vermelho = Ruim]        │
│                                         │
│   BCK = Voltar                      │
└─────────────────────────────────────────┘
```

### Interpretação dos Resultados

| Valor Mostrado | Significado |
|---------------|--------------|
| 1.00 - 1000.00 µF | Capacitor OK |
| 0.00 ou valor muito baixo | Capacitor aberto/ruim |
| Valor muito alto | Capacitor em curto |

### Dicas

💡 **Dica 1:** Capacitores pequenos (< 1µF) podem não ser detectados.

💡 **Dica 2:** Sempre descargue capacitores grandes antes de testar.

💡 **Dica 3:** O valor medido pode ter uma tolerância de ±20%.

---

## 3. Medir Resistor

### O que você precisa saber

**Para que serve:** Medir a resistência de resistores em Ohms (Ω).

**Quando usar:** Para verificar o valor ou se o resistor está queimado.

**O que você precisa:**
- 1 Resistor (qualquer valor entre 1Ω e 1MΩ)
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o resistor aos probes:       │
│                                         │
│   Probe (+) ──┬── Terminal 1            │
│              │                          │
│   Probe (-) ─┴── Terminal 2            │
│                                         │
│   (A polaridade não importa)            │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Cap                              │
│   ► Res                             │
│     Diod                            │
│     Trans                           │
│     Ind                             │
│     Volt                            │
│     Freq                            │
│     PWM                             │
│     Opto                            │
│     Cable                           │
│     Bridge                          │
│     Auto                            │
│     Conti                           │
│                                         │
│   Use UP/DW para selecionar "Res"    │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                    │
│                                         │
│   Res: 1000 R    (ou 1k)             │
│                                         │
│   [Verde = OK | Vermelho = Ruim]        │
│                                         │
│   BCK = Voltar                      │
└─────────────────────────────────────────┘
```

### Interpretação dos Resultados

| Valor Mostrado | Significado |
|---------------|--------------|
| Valor > 0 | Resistor OK |
| OPEN/HIGH | Resistor aberto (queimado) |
| 0Ω | Resistor em curto |

### Códigos de Cores de Resitores

Se você precisar verificar o valor manualmente:

| Cor | 1ª Faixa | 2ª Faixa | Multiplicador | Tolerância |
|-----|----------|----------|---------------|------------|
| Preto | 0 | 0 | 1Ω | |
| Marrom | 1 | 1 | 10Ω | ±1% |
| Vermelho | 2 | 2 | 100Ω | ±2% |
| Laranja | 3 | 3 | 1kΩ | ±3% |
| Amarelo | 4 | 4 | 10kΩ | ±4% |
| Verde | 5 | 5 | 100kΩ | ±0.5% |
| Azul | 6 | 6 | 1MΩ | ±0.25% |
| Violeta | 7 | 7 | 10MΩ | ±0.1% |
| Cinza | 8 | 8 | | ±0.05% |
| Branco | 9 | 9 | | |

---

## 4. Testar Diodo/LED

### O que você precisa saber

**Para que serve:** Verificar se um diodo ou LED está funcionando e identificar seus terminais (ânodo e cátodo).

**Quando usar:** Para testar diodos, LEDs, ou identificar a polaridade.

**O que você precisa:**
- 1 Diodo ou LED
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Para DIODO:                           │
│                                         │
│   Probe (+) ──┬── Ânodo (A)            │
│              │                          │
│   Probe (-) ─┴── Cátodo (K)             │
│                                         │
│   Para LED:                            │
│                                         │
│   Probe (+) ──┬── Ânodo (perna +)      │
│   (perna longa)│                          │
│   Probe (-) ─┴── Cátodo (perna -)      │
│   (perna curta)                         │
└─────────────────────────────────────────┘
```

💡 **Dica:** LED = Light Emitting Diode. A perna mais longa é o ânodo (+).

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Cap                              │
│     Res                              │
│   ► Diod                             │
│     Trans                            │
│     Ind                              │
│     Volt                             │
│     Freq                             │
│     PWM                              │
│     Opto                             │
│     Cable                            │
│     Bridge                           │
│     Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "Diod"        │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   Se OK, a tela mostra:                │
│                                         │
│   Anodo P1, Katodo P2                  │
│                                         │
│   [Verde] = Funcionando                │
│                                         │
│   Se ruim, mostra:                     │
│                                         │
│   No Diode                             │
│                                         │
│   [Vermelho] = Problema                │
└─────────────────────────────────────────┘
```

### Interpretação dos Resultados

| Resultado | Significado |
|----------|--------------|
| "Anodo P1, Katodo P2" | Diodo/LED OK, ânodo no probe 1 |
| "Anodo P2, Katodo P1" | Diodo/LED OK, ânodo no probe 2 |
| "No Diode" | Aberto ou invertido |

### Tensão de Forward (VF) Típica

| Componente | Tensão Típica |
|-----------|--------------|
| Diodo обычный | 0.6 - 0.7V |
| LED Vermelho | 1.8 - 2.0V |
| LED Verde | 2.0 - 2.2V |
| LED Azul | 2.5 - 3.0V |
| LED Branco | 3.0 - 3.5V |

---

## 5. Identificar Transistor

### O que você precisa saber

**Para que serve:** Identificar transistores BJT NPN e verificar se estão funcionando.

**Quando usar:** Para identificar transistores desconhecidos ou testar transistores.

**O que você precisa:**
- 1 Transistor BJT (NPN)
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Não importa a conexão inicial -      │
│   o dispositivo testa ambos:          │
│                                         │
│   Opção 1:                             │
│   Probe 1 → C (Coletor)                │
│   Probe 2 → E (Emissor)               │
│                                         │
│   Opção 2:                             │
│   Probe 1 → E (Emissor)                │
│   Probe 2 → C (Coletor)                │
│                                         │
│   O dispositivo encontra a Base!       │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Cap                              │
│     Res                              │
│     Diod                            │
│   ► Trans                           │
│     ...                            │
│                                         │
│   Use UP/DW para selecionar "Trans"    │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   Se OK, a tela mostra:                │
│                                         │
│   Transistor:                          │
│   Identifying...                       │
│                                         │
│   BJT NPN                            │
│   Beta: 100                          │
│                                         │
│   [Verde] = OK                       │
│                                         │
│   Se ruim, mostra:                     │
│                                         │
│   Not Found                          │
│                                         │
│   [Vermelho] = Problema               │
└─────────────────────────────────────────┘
```

### Interpretação dos Resultados

| Resultado | Significado |
|-----------|-------------|
| "BJT NPN" + Beta | Transistor OK |
| "Not Found" | Transistor ruim ou não detectável |

### O que é Beta?

**Beta (β)** é o ganho de corrente do transistor. Valores típicos:
- transistors comuns: 50-200
- transistores de áudio: 100-400

### Pinagem Típica de Transistores

Para transistores TO-92 (mais comuns):

```
┌─────────────────────────────┐
│         E  B  C             │
│         │  │  │             │
│         ○  ○  ○             │
│              C              │
│                            │
│   (Vista de baixo)         │
└─────────────────────────────┘
```

---

## 6. Medir Indutor

### O que você precisa saber

**Para que serve:** Medir a indutância de indutores em microhenries (µH) ou milihenries (mH).

**Quando usar:** Para verificar o valor de indutores ou bobinas.

**O que você precisa:**
- 1 Indutor
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o indutor aos probes:          │
│                                         │
│   Probe (+) ──┬── Terminal 1            │
│              │                          │
│   Probe (-) ─┴── Terminal 2            │
│                                         │
│   (A polaridade não importa)           │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Trans                           │
│   ► Ind                             │
│     Volt                            │
│     Freq                            │
│     PWM                             │
│     Opto                            │
│     Cable                           │
│     Bridge                          │
│     Auto                            │
│     Conti                           │
│                                         │
│   Use UP/DW para selecionar "Ind"      │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                    │
│                                         │
│   Indutor:                             │
│   Identifying...                      │
│                                         │
│   [Valor] uH  (ou mH)                │
│                                         │
│   [Verde] = OK                       │
│                                         │
│   Ou:                                  │
│                                         │
│   OPEN                                 │
│                                         │
│   [Vermelho] = Problema               │
└─────────────────────────────────────────┘
```

### Interpretação dos Resultados

| Valor Mostrado | Significado |
|----------------|--------------|
| 1 - 1000000 µH | Indutor OK |
| "OPEN" | Indutor aberto (queimado) |

---

## 7. Medir Tensão DC

### O que você precisa saber

**Para que serve:** Medir tensão contínua de 0V a 5V.

**Quando usar:** Para verificar tensões em circuitos.

**O que você precisa:**
- Fonte de tensão (ou circuito)
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Probe Vermelha → Positivo (+)         │
│   Probe Preta → Negativo (-)            │
│                                         │
│   IMPORTANTE: Não ultrapasse 5V!       │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Ind                              │
│   ► Volt                             │
│     Freq                             │
│     PWM                              │
│     Opto                             │
│     Cable                            │
│     Bridge                           │
│     Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "Volt"     │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Volt DC:                             │
│                                         │
│   3.30 V                              │
│                                         │
│   [Verde] = Mediçãook                 │
│                                         │
│   BCK = Voltar                        │
└─────────────────────────────────────────┘
```

### Limites

⚠️ **IMPORTANTE:** Nunca aplique mais de 5V nos probes! Isso pode danificar o Arduino.

---

## 8. Contar Frequência

### O que você precisa saber

**Para que serve:** Medir frequência de 1Hz a 1MHz.

**Quando usar:** Para verificar sinais de clock, PWM, ou频率.

**O que você precisa:**
- Fonte de sinal
- Fio de conexão

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o sinal ao Pino 5:           │
│                                         │
│   Pino 5 → Sinal de entrada            │
│   GND → Ground do circuito              │
│                                         │
│   Não use os probes para esta função!  │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Volt                             │
│   ► Freq                             │
│     PWM                              │
│     Opto                             │
│     Cable                            │
│     Bridge                           │
│     Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "Freq"     │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Frequency:                           │
│   Pin 5 input                          │
│                                         │
│   1000 Hz                              │
│                                         │
│   [Verde] = Sinal detectado            │
│                                         │
│   Ou:                                  │
│                                         │
│   No Signal                            │
│                                         │
│   [Vermelho] = Sem sinal               │
└─────────────────────────────────────────┘
```

---

## 9. Gerar PWM

### O que você precisa saber

**Para que serve:** Gerar um sinal PWM de 1kHz a 50% de ciclo de trabalho.

**Quando usar:** Para testar motores, LEDs, ou outros componentes controllable por PWM.

**O que você precisa:**
- Carga a testar (LED, motor, etc.)
- Fios de conexão

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o componente ao Pino 9:       │
│                                         │
│   Pino 9 → Componente (+)              │
│   GND → Componente (-)                │
│                                         │
│   Não use os probes para esta função!   │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Freq                             │
│   ► PWM                              │
│     Opto                             │
│     Cable                            │
│     Bridge                           │
│     Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "PWM"      │
│   e pressione OK                     │
└───────────────────────────────────────��─��
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   PWM: 1kHz 50%                        │
│                                         │
│   [ATIVADO] Pino 9 gerando PWM         │
│                                         │
│   BCK = Voltar (desliga)               │
└─────────────────────────────────────────┘
```

---

## 10. Testar Optocoupler

### O que você precisa saber

**Para que serve:** Verificar se um optocoupler (acoplador óptico) está funcionando.

**Quando usar:** Para testar隔离.

**O que você precisa:**
- 1 Optocoupler
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte o optocoupler:                │
│                                         │
│   Probe 1 → LED Anodo (pino 1 ou 2)    │
│   Probe 2 → LED Cátodo (pino 2 ou 1)    │
│                                         │
│   Os terminais do transistor no lado      │
│   do circuito integrado não são         │
│   conectados aos probes!               │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     PWM                              │
│   ► Opto                             │
│     Cable                            │
│     Bridge                           │
│     Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "Opto"       │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Optocoupler:                         │
│                                         │
│   WORKING                             │
│                                         │
│   [Verde] = Isolamento OK               │
│                                         │
│   Ou:                                  │
│                                         │
│   FAILED                             │
│                                         │
│   [Vermelho] = Problema               │
└─────────────────────────────────────────┘
```

---

## 11. Testar Cabo

### O que você precisa saber

**Para que serve:** Verificar continuidade de cabos e fios.

**Quando usar:** Para testar cabos de conexão, jumpers, ou fios.

**O que você precisa:**
- Cabo ou fio a testar

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte as pontas do cabo:            │
│                                         │
│   Probe 1 → Uma ponta                  │
│   Probe 2 → Outra ponta                │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Opto                             │
│   ► Cable                           │
│     Bridge                          │
│     Auto                            │
│     Conti                           │
│                                         │
│   Use UP/DW para selecionar "Cable"      │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Cable:                              │
│                                         │
│   CONNECTED                           │
│                                         │
│   [Verde] + Beep = Cabo OK              │
│                                         │
│   Ou:                                  │
│                                         │
│   OPEN                                 │
│                                         │
│   [Vermelho] = Cabo rompido              │
└─────────────────────────────────────────┘
```

---

## 12. Testar Bridge

### O que você precisa saber

**Para que serve:** Testar uma ponte retificadora (4 diodos).

**Quando usar:** Para verificar pontes retificadoras.

**O que você precisa:**
- 1 Ponte retificadora
- Probe vermelha e probe preta

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte a ponte:                      │
│                                         │
│   Probe 1 → terminal AC (+)            │
│   Probe 2 → terminal AC (-)            │
│                                         │
│   A ordem não importa inicialmente       │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Cable                           │
│   ► Bridge                          │
│     Auto                            │
│     Conti                           │
│                                         │
│   Use UP/DW para selecionar "Bridge"      │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Bridge Rect:                         │
│   Testing diodes...                    │
│                                         │
│   GOOD    (4 diodos OK)                │
│   PARTIAL (2-3 diodos OK)              │
│   FAILED  (0-1 diodos OK)              │
└─────────────────────────────────────────┘
```

---

## 13. Detecção Automática

### O que você precisa saber

**Para que serve:** Automaticamente detectar o tipo de componente conectado.

**Quando usar:** Quando você não sabe que tipo de componente está testando.

**O que você precisa:**
- Qualquer componente

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte qualquer componente:            │
│                                         │
│   Probe 1 → Terminal 1                  │
│   Probe 2 → Terminal 2                  │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Bridge                          │
│   ► Auto                             │
│     Conti                            │
│                                         │
│   Use UP/DW para selecionar "Auto"     │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Auto...                              │
│                                         │
│   [Tipo identificado]                  │
│                                         │
│   Valores e status                    │
│                                         │
│   [Verde] = Componente detectado       │
│                                         │
│   Ou:                                  │
│                                         │
│   None                                 │
│                                         │
│   [Vermelho] = Nenhum componente      │
└─────────────────────────────────────────┘
```

---

## 14. Teste de Continuidade

### O que você precisa saber

**Para que serve:** Verificar continuidade com feedback sonoro (beep).

**Quando usar:** Para encontrar pontos conectados em circuit boards.

**O que você precisa:**
- Qualquer thing a testar

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Conectar            │
│                                         │
│   Conecte os pontos:                    │
│                                         │
│   Probe 1 → Ponto 1                     │
│   Probe 2 → Ponto 2                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Selecionar           │
│                                         │
│   No menu Medir:                        │
│                                         │
│     Auto                             │
│   ► Conti                            │
│                                         │
│   Use UP/DW para selecionar "Conti"    │
│   e pressione OK                     │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Resultado           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Continuidade:                         │
│   Probing...                           │
│                                         │
│   Resistência em Ohms                  │
│                                         │
│   Beep curta = Baixa resistência (<10Ω)  │
│   Beep longa = OK (<50Ω)               │
│   Sem beep = Alta resistência          │
└─────────────────────────────────────────┘
```

---

## 15. Usar Sonda Térmica

### O que você precisa saber

**Para que serve:** Monitorar temperatura ambiente ou de componentes.

**Quando usar:** Para monitorar temperatura durante medições.

**O que você precisa:**
- Sonda DS18B20 já conectada

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Acessar             │
│                                         │
│   No menu principal:                   │
│                                         │
│     Medir                             │
│     Termica                            │
│     Scanner                            │
│     Historico                          │
│     Estatist                           │
│     Config                             │
│     Sobre                              │
│                                         │
│   Selecione "Termica" e pressione OK   │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Monitorar           │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Temperatura atual:                   │
│                                         │
│   25.5°C                               │
│                                         │
│   Estado: NORMAL                       │
│                                         │
│   Alerta: Nenhum                       │
│                                         │
│   BCK = Voltar                        │
└─────────────────────────────────────────┘
```

### Níveis de Alerta

| Temperatura | Cor | LED | Buzzer |
|-------------|-----|-----|-------|
| < 70°C | Verde | Apagado | Silencioso |
| 70-90°C | Amarelo | Piscando | 1s |
| 90-110°C | Laranja | Piscando | 500ms |
| > 110°C | Vermelho | Fixo | 200ms |

---

## 16. Usar Scanner

### O que você precisa saber

**Para que serve:** Auto-detecção contínua a cada 2 segundos.

**Quando usar:** Para monitoramento continuo.

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Acessar             │
│                                         │
│   No menu principal:                   │
│                                         │
│     Medir                             │
│     Termica                            │
│   ► Scanner                            │
│     Historico                          │
│     Estatist                           │
│     Config                             │
│     Sobre                              │
│                                         │
│   Selecione "Scanner" e pressione OK   │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 2: Ativar             │
│                                         │
│   A tela mostrará:                      │
│                                         │
│   Scanner                             │
│                                         │
│   OK = auto medicao                    │
│   BCK = sair                          │
│                                         │
│   Pressione OK para iniciar           │
└─────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────┐
│           Passo 3: Escaneando          │
│                                         │
│   Escaneando...                        │
│                                         │
│   Componente detectado a cada 2s      │
│                                         │
│   OK=parar BCK=sair                    │
└─────────────────────────────────────────┘
```

---

## 17. Ver Histórico

### Passo a Passo

1. Selecione **"Historico"** no menu principal
2. Pressione **OK**

As últimas 10 medições serão mostradas com:
- Nome do tipo de medição
- Valor medido
- Cor (verde = OK, vermelho = ruim)

---

## 18. Ver Estatísticas

### Passo a Passo

1. Selecione **"Estatist"** no menu principal
2. Pressione **OK**

Serão mostrados:
- Total de medições
- Componentes/ruins
- Taxa de falha (%)
- Temperatura atual

---

## 19. Configurar Dispositivo

### Passo a Passo

```
┌─────────────────────────────────────────┐
│           Passo 1: Acessar             │
│                                         │
│   No menu principal:                   │
│                                         │
│     Historico                          │
│     Estatist                           │
│     Config                             │
│     Sobre                              │
│                                         │
│   Selecione "Config" e pressione OK   │
└─────────────────────────────────────────┘
```

### Opções

| Opção | Função |
|------|-------|
| Calibrar Probes | Ajustar precisão |
| Modo Escuro | Mudar cores |
| Modo Silencioso | Desabilitar sons |
| Timeout | 30s ou 60s |
| Salvar Cfg | Salvar na EEPROM |

### Como Configurar

1. Use **UP/DW** para selecionar
2. Pressione **OK** para alterar
3. Selecione **"Salvar Cfg"** para salvar

---

**Continue learning:**
- [COMPONENTS.md](COMPONENTS.md) - Detalhes técnicos
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solução de problemas
- [FAQ.md](FAQ.md) - Perguntas frequentes