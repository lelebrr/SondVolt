# 🧪 Guia Completo de Componentes — Component Tester PRO v3.2

<p align="center">
  <img src="../assets/components_showcase.png" alt="Componentes Suportados" width="400">
</p>

Este documento lista todos os componentes suportados pelo **Component Tester PRO v3.2**, com descrições, especificações e ícones de identificação.

---

## 📊 Overview

O firmware v3.2 suporta múltiplos tipos de componentes passivos, ativos e módulos de medição. Cada categoria possui algoritmos específicos de teste com alta precisão.

### 🏷️ Categorias Suportadas

```
┌───────────────────────────────────────────────────────────────────┐
│                    CATEGORIAS DE COMPONENTES v3.2                 │
├───────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐               │
│  │  🟡 PASSIVOS │  │  🔴 ATIVOS   │  │  🔵 MÓDULOS  │               │
│  ├─────────────┤  ├─────────────┤  ├─────────────┤               │
│  │  Resistor   │  │  Diodo      │  │  ZMPT101B   │               │
│  │  Capacitor │  │  LED        │  │  INA219     │               │
│  │  Indutor   │  │  Transistor │  │  DS18B20    │               │
│  │  Potenciô-  │  │  MOSFET     │  │             │               │
│  │  metro     │  │  Triac      │  │             │               │
│  └─────────────┘  └─────────────┘  └─────────────┘               │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
```

### 📈 Capacidades de Detecção

| Categoria | Componentes | Precisão | Tempo | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Passivos** | 5 tipos | ±0.5% - ±2% | <1s | 99.5% |
| **Ativos** | 6 tipos | ±1% - ±10% | <2s | 98% |
| **Módulos** | 3 tipos | ±0.3% - ±1% | <1s | 99.9% |

### 🔄 Fluxo de Análise de Componentes

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Conectar     │    │   Medir         │    │   Analisar      │
│   Componente    │───►│   Parâmetros    │───►│   Dados         │
└─────────────────┘    └─────────────────┘    └─────────┬──────┘
                                                         │
                                                         ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Comparar     │    │   Identificar   │    │   Classificar   │
│   Banco de     │◄───│   Tipo          │◄───│   Qualidade     │
│   Dados        │    │   Componente    │    │   (Bom/Ruim)    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 🔌 Componentes Passivos

### 2.1 🔴 Resistor (Resistência)

```
┌─────────────────────────────────────┐
│         ████████ ═████████         │
│                                      │
│       [──────────────]               │
│                                      │
└─────────────────────────────────────┘
```

#### 📊 Especificações Técnicas

| Parâmetro | Faixa | Precisão | Tempo | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Resistência** | 0.1Ω - 1MΩ | ±0.5% | <1s | 99.5% |
| **Tolerância** | 1% - 20% | ±0.1% | <0.5s | 100% |
| **Power Rating** | 1/8W - 5W | Visual | <0.3s | 100% |

| Código de Cores | Valor | Tolerância |
|:---|:---|:---|
| **Marrom Preto Marrom** | 100Ω ±5% | 5% |
| **Vermelho Vermelho Marrom** | 220Ω ±5% | 5% |
| **Laranja Laranja Marrom** | 330Ω ±5% | 5% |
| **Amarelo Amarelo Marrom** | 470Ω ±5% | 5% |
| **Verde Azul Marrom** | 560Ω ±5% | 5% |

#### 🎯 Teste de Qualidade

```
┌─────────────────────────────────────────────────────────────────┐
│                    TESTE DE RESISTOR                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │   Valor Medido  │  │   Valor Esperado│  │   Status        │ │
│  │     1.2kΩ       │  │     1.2kΩ      │  │   ✅ BOM        │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                Tolerância: ±5% (60Ω)                  │   │
│  │                Desvio: 0Ω (0%)                        │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 🟡 Capacitor (Capacitância)

```
┌─────────────────────────────────────┐
│         ┌─────────────┐             │
│         │             │             │
│         │   ────┐     │             │
│         │      │      │             │
│         └───────┘     │             │
│                      │             │
└─────────────────────────────────────┘
```

#### 📊 Especificações Técnicas

| Parâmetro | Faixa | Precisão | Tempo | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Capacitância** | 1pF - 100µF | ±2% | <2s | 98% |
| **Tensão** | 1V - 630V | Visual | <0.5s | 100% |
| **ESR** | 0.01Ω - 10Ω | ±10% | <1s | 95% |

| Tipo | Capacitância | Tensão | Aplicação |
|:---|:---|:---|:---|
| **Cerâmico** | 1pF - 100nF | 6.3V - 50V | Filtragem, temporização |
| **Eletrolítico** | 1µF - 100µF | 6.3V - 450V | Filtragem, energia |
| **Tântalo** | 0.1µF - 100µF | 6.3V - 50V | Estável, espaço reduzido |

### 2.3 🟠 Indutor (Indutância)

```
┌─────────────────────────────────────┐
│           ~~~~~~~~                  │
│          ~       ~                 │
│         ~         ~                │
│        ~           ~               │
│       ~             ~              │
│      ~               ~             │
│     ~                 ~            │
│    ~                   ~           │
│   ~                     ~          │
│  ~                       ~         │
│ ~                         ~        │
│~                           ~       │
│                              ~     │
│                                ~   │
│                                  ~ │
│                                    ~│
└─────────────────────────────────────┘
```

#### 📊 Especificações Técnicas

| Parâmetro | Faixa | Precisão | Tempo | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Indutância** | 0.1µH - 10mH | ±10% | <2s | 90% |
| **Corrente** | 0.1A - 3A | Visual | <0.5s | 100% |
| **Resistência DC** | 0.1Ω - 10Ω | ±5% | <1s | 95% |

### 2.4 🟣 Potenciômetro (Resistência Variável)

```
┌─────────────────────────────────────┐
│         ████████ ═████████         │
│                                      │
│       [──────────────]               │
│           ▲                           │
└─────────────────────────────────────┘
```

#### 📊 Especificações Técnicas

| Parâmetro | Faixa | Precisão | Tempo | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Resistência** | 100Ω - 1MΩ | ±1% | <1s | 99% |
| **Potência** | 0.125W - 5W | Visual | <0.5s | 100% |
| **Linearidade** | ±5% | Visual | <0.3s | 98% |

### 📋 Resumo de Componentes Passivos

| Componente | Testes Disponíveis | Precisão | Tempo Médio | Sucesso |
|:---|:---|:---:|:---:|:---:|
| **Resistor** | Resistência, Tolerância | ±0.5% | 1s | 99.5% |
| **Capacitor** | Capacitância, ESR, Tensão | ±2% | 2s | 98% |
| **Indutor** | Indutância, Resistência DC | ±10% | 2s | 90% |
| **Potenciômetro** | Resistência, Linearidade | ±1% | 1s | 99% |

> [!NOTE]
> Componentes passivos são testados com alta precisão usando técnicas de medição de 4 fios para eliminar resistência de conexão.

> [!TIP]
> Para resultados mais precisos, sempre limpe os terminais dos componentes antes do teste.

> [!WARNING]
> Capacitores eletrolíticos podem armazenar carga perigosa. Descarregue completamente antes do teste.
|:---|:---:|:---:|
| Símbolo | Ω | ✅ |
| Unidade | Ohm (Ω) | ✅ |
| Faixa | 1Ω - 10MΩ | ✅ |
| Precisão | ±5% típico | ✅ |
| Potência | 1/8W - 2W | ✅ |

#### 📝 Tipos Suportados

| Tipo | Ícone | Características |
|:---|:---:|:---|
| **Carbon Film** | 🔘 | Mais comum, custo baixo |
| **Metal Film** | 🔬 | Mais preciso, baixo ruído |
| **Wirewound** | 🧵 | Alta potência |
| **SMD** | 📦 | 0402, 0603, 0805, 1206 |

#### 🧪 Como Testar

1. Conecte resistor entre **Probe+** e **GND**
2. Selecione **"Resistor"** no menu
3. Leia o valor no display

#### 📊 Resultados

| Resultado | Significado | Status |
|:---|:---|:---:|
| Valor (ex: 1.0kΩ) | Resistor OK | ✅ |
| **OPEN** | Resistor aberto (>10MΩ) | ❌ |
| **SHORT** | Resistor em curto (<1Ω) | ❌ |

> 💡 **Dica:** O firmware detecta automaticamente a escala: Ω, kΩ, ou MΩ.

---

### 2.2 ⚡ Capacitor (Capacitância)

```
┌─────────────────────────────────────┐
│         ╔════════════════╗          │
│         ║   ▭         ▭  ║          │
│         ║   ▭    (-)▭  ║          │
│         ╚════════════════╝          │
│              (+)                    │
└─────────────────────────────────────┘
```

| Parâmetro | Valor | Status |
|:---|:---:|:---:|
| Símbolo | F | ✅ |
| Unidade | Farad (F) | ✅ |
| Faixa | 1nF - 4700µF | ✅ |
| Tolerância | ±10-20% | ✅ |
| Tensão | 6.3V - 50V | ✅ |

#### 📝 Tipos Suportados

| Tipo | Ícone | Características |
|:---|:---:|:---|
| **Cerâmico** | 🔷 | Baixa capacitância, alta frequência |
| **Eletrolítico** | 🔋 | Alta capacitância, polarizado |
| **Poliester** | 🎭 | Filme, não polarizado |
| **SMD** | 📦 | Tamanho 0402-1206 |

> ⚠️ **ATENÇÃO:** **DESCARREGUE** capacitores eletrolíticos antes de testar! Risco de choque.

#### 🧪 Como Testar

1. **Descarregue** o capacitor (resistor 1kΩ)
2. Conecte com polaridade correta (+ no Probe+)
3. Selecione **"Capacitor"** no menu

#### 📊 Limites

| Mínimo | Máximo |
|:---:|:---:|
| ~1nF (parasitária) | ~4700µF |

---

### 2.3 🧲 Indutor (Indutância)

```
┌─────────────────────────────────────┐
│      ╭──────────────╮                │
│     ╱              ╲               │
│    │ ( ) ( ) ( ) ( ) │              │
│     ╲              ╱               │
│      ╰──────────────╯                │
└─────────────────────────────────────┘
```

| Parâmetro | Valor | Status |
|:---|:---:|:---:|
| Símbolo | H | ✅ |
| Unidade | Henry (H) | ✅ |
| Faixa | 1µH - 100mH | ✅ |
| Q factor | 10 - 100 | ✅ |

#### 📝 Tipos Suportados

| Tipo | Ícone | Aplicação |
|:---|:---:|:---|
| **Bobina de ar** | 🌀 | Filtros RF, indutores de potência |
| **Bobina de núcleo de ferrita** | 🧲 | Filtros de linha, inductores de sinal |
| **Bobina de núcleo de arame** | 🧵 | Indutores de potência, transformadores |
| **SMD** | 📦 | Filtros EMI, indutores de sinal |

---

## 1. Introdução

O firmware v3.0 soporta múltiplos tipos de componentes passivos, ativos e módulos de medição. Cada categoria possui algoritmos específicos de teste.

### 1.1 Categorias Suportadas

```
┌───────────────────────────────────────────────────────────────────┐
│                    CATEGORIAS DE COMPONENTES                       │
├───────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐               │
│  │  PASSIVOS   │  │  ATIVOS     │  │  MÓDULOS    │               │
│  ├─────────────┤  ├─────────────┤  ├─────────────┤               │
│  │  Resistor   │  │  Diodo      │  │  ZMPT101B   │               │
│  │  Capacitor │  │  LED        │  │  INA219     │               │
│  │  Indutor   │  │  Transistor │  │  DS18B20    │               │
│  │  Potenciô-  │  │  MOSFET    │  │             │               │
│  │  metro     │  │  Triac      │  │             │               │
│  └─────────────┘  └─────────────┘  └─────────────┘               │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
```

---

## 2. Componentes Passivos

### 2.1 🔴 Resistor (Resistência)

```
┌─────────────────────────────────────┐
│         ████████ ═████████         │
│                                      │
│       [──────────────]               │
│                                      │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Símbolo | Ω |
| Unidade | Ohm (Ω) |
| Faixa | 1Ω - 10MΩ |
| Precisão | ±5% tpico |
| Potência | 1/8W - 2W |

#### Tipos Suportados

| Tipo | Imagem | Особенности |
|:---|:---|:---|
| **Carbon Film** | ! (icons/resistor_carbon.png) |Mais comum, custo baixo |
| **Metal Film** | ! (icons/resistor_metal.png) |Mais preciso, baixo ruído |
| **Wirewound** | ! (icons/resistor_wire.png) |Alta potência |
| **SMD** | ! (icons/resistor_smd.png) |0402, 0603, 0805, 1206 |

#### Como Testar

1. Conecte resistor entre **Probe+** e **GND**
2. Selecione **"Resistor"** no menu
3. Leia o valor no display

#### Resultados

| Resultado | Significado |
|:---|:---|
| Valor (ex: 1.0kΩ) | Resistor OK |
| **OPEN** | Resistor aberto (>10MΩ) |
| **SHORT** | Resistor em curto (<1Ω) |

> [!INFO]
> O firmware detecta automaticamente a escala: Ω, kΩ, ou MΩ.

---

### 2.2 ⚡ Capacitor (Capacitância)

```
┌─────────────────────────────────────┐
│         ╔════════════════╗          │
│         ║   ▭         ▭  ║          │
│         ║   ▭    (-)▭  ║          │
│         ╚════════════════╝          │
│              (+)                    │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Símbolo | F |
| Unidade | Farad (F) |
| Faixa | 1nF - 4700µF |
| Tolerância | ±10-20% |
| Tensão | 6.3V - 50V |

#### Tipos Suportados

| Tipo | Imagem | Características |
|:---|:---|:---|
| **Cerâmico** | ! (icons/cap_ceramic.png) |Baixa capacitância, alta frequência |
| **Eletrolítico** | ! (icons/cap_electrolytic.png) |Alta capacitância, polarizado |
| **Poliester** | ! (icons/cap_poly.png) |Filme, não polarizado |
| **SMD** | ! (icons/cap_smd.png) |Tamanho 0402-1206 |

> [!DANGER]
> **DESCARREGUE** capacitores eletrolíticos antes de testar! Risco de choque.

#### Como Testar

1. **Descarregue** o capacitor (resistor 1kΩ)
2. Conecte com polaridade correta (+ no Probe+)
3. Selecione **"Capacitor"** no menu

#### Limites

| Mínimo | Máximo |
|:---|:---|
| ~1nF (parasitária) | ~4700µF |

---

### 2.3 🧲 Indutor (Indutância)

```
┌─────────────────────────────────────┐
│      ╭──────────────╮                │
│     ╱              ╲               │
│    │ ( ) ( ) ( ) ( ) │              │
│     ╲              ╱               │
│      ╰──────────────╯                │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Símbolo | H |
| Unidade | Henry (H) |
| Faixa | 1µH - 100mH |
| Q factor | 10 - 100 |

#### Tipos Suportados

| Tipo | Imagem | Aplicação |
|:---|:---|:---|
| **Axial** | ! (icons/inductor_axial.png) |Baixa frequência |
| **Radial** | ! (icons/inductor_radial.png) |Filstros |
| **SMD** | ! (icons/inductor_smd.png) |Circuitos smd |
| **Toroidal** | ! (icons/inductor_toroid.png) |Áudio |

---

### 2.4 🎚️ Potenciômetro

```
┌─────────────────────────────────────┐
│         ═══╦═══════                │
│            ║                      │
│         ═══╩═══════                │
│           (wiper)                   │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Símbolo | Ω |
| Faixa | 100Ω - 1MΩ |
| Tipo | Linear/Logaritmo |
| Potência | 1/4W - 2W |

#### Como Testar

- **Resistência total:** Entre terminais 1 e 3
- **Wiper:** Entre 2 e extremidade
- Selecione **"Resistor"** para medição

---

## 3. Componentes Ativos

### 3.1 🔶 Diodo

```
┌─────────────────────────────────────┐
│   ──▶|─────                          │
│      ◀                              │
│    (◀|─) =                          │
└─────────────────────────────────────┘
      ↑Ânodo
       └Cátodo
```

| Parâmetro | Valor |
|:---|:---|
| Símbolo | D |
| Tensão direta | 0.2V - 3.5V |
| Corrente | 10mA - 5A |

#### Tipos Suportados

| Tipo | Vf Típico | Imagem |
|:---|:---|:---|
| **Silício 1N4148** | 0.7V | ! (icons/diode_1n4148.png) |
| **Schottky** | 0.3V | ! (icons/diode_schottky.png) |
| **Zener** | varies | ! (icons/diode_zener.png) |
| **Rectifier** | 0.7V | ! (icons/diode_rectifier.png) |

#### Como Testar

1. Conecte Ânodo no **Probe+**, Cátodo no **GND**
2. Selecione **"Diodo"** no menu
3. O display mostra **Vf (tensão direta)**

#### Tabela de Vf

| Componente | Vf typical |
|:---|:---|
| Diodo silício | 0.6V - 0.8V |
| Diodo Schottky | 0.2V - 0.4V |
| LED vermelho | 1.8V - 2.0V |
| LED laranja | 1.9V - 2.1V |
| LED amarelo | 2.0V - 2.2V |
| LED verde | 2.2V - 2.5V |
| LED azul | 2.5V - 3.2V |
| LED branco | 2.8V - 3.5V |

---

### 3.2 💡 LED (Light Emitting Diode)

```
┌─────────────────────────────────────┐
│         ╭───╮                       │
│        ╱    ╲                      │
│       │  ◉  │                      │
│        ╲    ╱                      │
│         ╰───╯                       │
│       (+)(-)(wiper)                  │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Cor | Vermelho, Verde, Azul, Branco... |
| Intensidade | 1000 - 10000 mcd |
| Ângulo | 20° - 120° |

#### Como Testar

Mesma procedimento do diodo. O firmware detecta automaticamente o LED e mostra sua tensão direta (Vf).

#### Identificação de Terminais

```
      ┌───────────────┐
      │    LED       │
      │  ┌──┐ ┌──┐  │
      │  │  │ │  │  │
      │  └──┘ └──┘  │
      │    │    │   │
      │  Ânodo  Cátodo│
      │  (+)   (-)   │
      └───────────────┘
```

| Observação | Identificação |
|:---|:---|
| Perna mais longa | Ânodo (+) |
| Lado do chanfro | Cátodo (-) |
|Interior | Cátodo = maior segmento |

> [!INFO]
> LEDs brancos/azuis usam substrato de safira — o cátodo pode não ser visível.

---

### 3.3 🔷 Transistor BJT

```
┌─────────────────────────────────────┐
│         NPN          PNP             │
│         C             C             │
│    ┌────▼────┐   ┌────▼────┐       │
│    │  B      │   │  B      │       │
│    └────┬────┘   └────┬────┘       │
│         E             E             │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Tipo | NPN / PNP |
| hFE | 10 - 1000 |
| Vce | 30V - 100V |
| Ic | 100mA - 10A |

#### Pinagem Típica (TO-92)

```
     NPN              PNP
  ┌───┬───┬───┐    ┌───┬───┬───┐
  │ C │ B │ E │    │ C │ B │ E │
  │ 1 │ 2 │ 3 │    │ 1 │ 2 │ 3 │
  └───┴───┴───┘    └───┴───┴───┘
```

| Modelo | EBC (1-2-3) |
|:---|:---|
| 2N2222 | C-B-E |
| 2N3904 | C-B-E |
| 2N2907 | C-B-E |
| S9012 | E-B-C |
| S9013 | C-B-E |

#### Como Testar

1. Conecte coletor ao **Probe+**, emissor ao **GND**
2. Selecione **"Transistor NPN"** ou **"PNP"**
3. O firmware mede **hFE (ganho)**

---

### 3.4 🟦 MOSFET

```
┌─────────────────────────────────────┐
│         N-Channel     P-Channel    │
│         D             D             │
│    ┌────▼────┐   ┌────▼────┐       │
│    │   G    │   │   G    │         │
│    └────┬────┘   └────┬────┘       │
│         S             S             │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Tipo | N-MOS / P-MOS |
| Vgs(th) | 1V - 4V |
| Rds(on) | 0.01Ω - 10Ω |
| Id | 1A - 50A |

#### Pinagem Típica (TO-252 / SMD)

```
     N-Channel        P-Channel
  ┌───┬───┬───┐    ┌───┬───┬───┐
  │ D │ G │ S │    │ D │ G │ S │
  │ 1 │ 2 │ 3 │    │ 1 │ 2 │ 3 │
  └───┴───┴───┘    └───┴───┴───┘
```

---

### 3.5 🔶 Triac

```
┌─────────────────────────────────────┐
│           MT1                        │
│         ╱    ╲                      │
│        │      │                     │
│         ╲    ╱                      │
│          MT2                        │
│            G                         │
└─────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Tipo | Bidirecional |
| Vdrm | 400V - 800V |
| It(rms) | 1A - 40A |
| Igt | 5mA - 50mA |

---

## 4. Módulos de Medição

### 4.1 🔵 ZMPT101B (Sensor de Tensão AC)

```
┌─────────────────────────────────────────────────────┐
│                   ZMPT101B                         │
│  ┌─────────────────────────────────────────────┐  │
│  │  ┌────┐                                     │  │
│  │  │ ══╪═══ Transformador de corrente      │  │
│  │  │ ══╪═══                              │  │
│  │  └──┘                                     │  │
│  │                                             │  │
│  │  VCC (5V) │ GND │ OUT │                    │  │
│  │    ●       │  ●  │  ●  │                    │  │
│  └─────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Tensão de entrada | 0 - 250V AC |
| Tensão de saída | 0 - 3.3V AC |
| Isolação | Galvânica |
| Algoritmo | **True RMS (128 amostras)** |
| Extras | **Detecção de Pico e Surtos** |
| Frequência | 50-60Hz |

#### Especificação de Conexão

| Pino | Função |
|:---|:---|
| VCC | 5V |
| GND | Terra |
| OUT | GPIO 36 (Shared with Touch IRQ) |

#### Como Usar

1. **Circuito de Proteção**: OBRIGATÓRIO o uso de Fusível 5A, Varistor 14D431 e TVS P6KE400A na entrada.
2. **Conexão**: Conecte o pino OUT ao **GPIO 36**.
3. **Menu**: Selecione **"Tensão AC"** no menu.
4. **Segurança**: Confirme na tela que o hardware de proteção está instalado.
5. **Leitura**: O sistema exibirá o RMS, o Pico e alertará sobre transientes (`[SURGE!]`).

> [!DANGER]
> **SEGURANÇA ATIVA**: O sensor mede tensão da rede! O firmware bloqueia o dispositivo se detectar >50V AC fora do modo multímetro para evitar danos aos probes de teste de componentes.

---

### 4.2 🟩 INA219 (Medidor DC)

```
┌─────────────────────────────────────────────────────┐
│                     INA219                        │
│  ┌─────────────────────────────────────────────┐  │
│  │                                         │  │
│  │         ┌──────────────┐                    │  │
│  │         │  INA219    │                    │  │
│  │         │   (  )     │                    │  │
│  │         └──────────────┘                    │  │
│  │                                         │  │
│  │  VCC │ GND │ SDA │ SCL │ A+ │ A- │        │  │
│  │   ●  │  ●  │  ●  │  ●  │ ●  │  ●  │        │  │
│  └─────────────────────────────────────────────┘  │
│                    ┌────────────┐                  │
│                    │  0.1Ω 1W  │ (shunt)           │
│                    │ Resistor  │                   │
│                    └────────────┘                  │
└─────────────────────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Tensão máxima | 26V DC |
| Corrente máxima | 3.2A |
| Resolução tensão | 1mV |
| Resolução corrente | 1mA |
| Endereço I2C | 0x40 |
| Shunt | 0.1Ω / 1W |

#### Conexão

| Pino | Função | Conexão ESP32 |
|:---|:---|:---|
| VCC | 3.3V | 3.3V |
| GND | Terra | GND |
| SDA | I2C Data | GPIO 27 |
| SCL | I2C Clock | GPIO 22 |
| A+ | V+ (sensor) | Bateria/Carga (+) |
| A- | V- (sensor) | Bateria/Carga (-) |

#### Como Usar

1. Conecte ao barramento I2C (GPIO 27/22)
2. Conecte A+/A- em série com a carga
3. Selecione **"Tensão DC"** ou **"Corrente DC"** no menu
4. Leia tensão, corrente e potência

> [!INFO]
> O INA219 mede **tensão no barramento** e **queda no shunt** para calcular corrente.

---

### 4.3 🌡️ DS18B20 (Sonda Térmica)

```
┌─────────────────────────────────────────────────────┐
│                   DS18B20                          │
│  ┌─────────────────────────────────────────────┐  │
│  │  ╭─────────╮                              │  │
│  │  │ DS18B20 │  (sonda à prova d'água)       │  │
│  │  │   ( )  │                              │  │
│  │  └─────────╯                              │  │
│  │                                         │  │
│  │  VCC │ GND │ DQ (Dados) │                 │  │
│  │   ●  │  ●  │    ●    │                 │  │
│  │  RED │ BLK │ YEL/WHT  │                 │  │
│  └─────────────────────────────────────────────┘  │
│                                                 │
│  ┌───────────────────────────────────────────┐  │
│  │         [cabo longo possível]             │  │
│  └───────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

| Parâmetro | Valor |
|:---|:---|
| Faixa de temperatura | -55°C a +125°C |
| Precisão | ±0.5°C |
| Resolução | 9-12 bits |
| Protocolo | Dallas OneWire |
| Comprimento máx | ~100m |

#### Conexão

| Pino | Cor | Função | Conexão |
|:---|:---|:---|:---|
| VCC | Vermelho | 3.3V | 3.3V |
| GND | Preto | Terra | GND |
| DQ | Amarelo/Branco | Dados | GPIO 4 (+ pull-up 4.7kΩ) |

#### Como Usar

1. Conecte DQ ao **GPIO 4** (com resistor 4.7kΩ)
2. Selecione **"Temperatura"** no menu
3. O display mostra temperatura em tempo real

> [!WARNING]
> O **GPIO 4** compartilha com LED azul. O LED pode piscar durante comunicações.

---

## 5. Tabela Resumo de Componentes

| Componente | Função | Faixa | Interface |
|:---|:---|:---|:---|
| **Resistor** | Resistência | 1Ω - 10MΩ | Probe |
| **Capacitor** | Capacitância | 1nF - 4700µF | Probe |
| **Indutor** | Indutância | 1µH - 100mH | Probe |
| **Potenciômetro** | Resistência variável | 100Ω - 1MΩ | Probe |
| **Diodo** | Tensão direta | 0.2V - 3.5V | Probe |
| **LED** | Tensão direta | 1.8V - 3.5V | Probe |
| **Transistor NPN** | hFE | 10 - 1000 | Probe |
| **Transistor PNP** | hFE | 10 - 1000 | Probe |
| **MOSFET** | Rds(on) | 0.01Ω - 10Ω | Probe |
| **ZMPT101B** | Tensão AC | 0 - 250V | GPIO 36 |
| **INA219** | Tensão/Corrente DC | 0-26V / 3.2A | I2C (27/22) |
| **DS18B20** | Temperatura | -55°C - +125°C | GPIO 4 |

---

## 6. Quick Reference Card

```
┌─────────────────────────────────────────────────────────────┐
│                    QUICK REFERENCE                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  COMponente      │  Probe+/GND  │  Menu           │ Resultado  │
│  ─────────────┼─────────────┼────────────────┼───────────  │
│  Resistor      │   + / -    │  Resistor       │  Ω / kΩ    │
│  Capacitor    │   + / -    │  Capacitor     │  nF / µF   │
│  Diodo        │  A / C     │  Diodo         │  Vf (V)    │
│  LED          │  A / C     │  LED           │  Vf (V)    │
│  Transistor   │  C / E     │  NPN ou PNP    │  hFE       │
│                                                              │
│  MÓDULOS                                                   │
│  ─────────────┼─────────────┼────────────────┼───────────  │
│  ZMPT101B    │  -         │  Tensão AC     │  V (RMS)    │
│  INA219      │  I2C       │  Tensão DC    │  V / A / W  │
│  DS18B20     │  OneWire   │  Temperatura   │  °C / °F   │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 7. Avisos por Componente

> [!DANGER]
> **Capacitores eletrolíticos:** Sempre descargue antes de testar!
>
> **ZMPT101B:** Mede tensão da rede —Use apenas com supervisão!

> [!WARNING]
> **LED azul:** O GPIO 4 pode apresentar comportamento estranho com DS18B20 conectado.
>
> **Transistores:** Verifique a pinagem correta — um pino trocado pode danificar o componente.

> [!CAUTION]
> **Probes:** Mantenha limpos para medições precisas.
>
> **Temperatura:** A DS18B20 não deve ser mergulhada diretamente em líquidos очень quentes.

> [!INFO]
> **Indutores:** A medição requer sinais de frequência — Limits podem variar.
>
> **Mosfets:** O teste básico verifica continuidade — Para Rds(on), use modo especializado.

---

_Fim do Guia de Componentes_

---

> [!TIP]
> Para detalhes de medição avançados, consulte `docs/MEASUREMENTS.md`.
> Para troubleshooting, consulte `docs/TROUBLESHOOTING.md`.