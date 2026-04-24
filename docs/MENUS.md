# Estrutura de Menus — Sondvolt v3.1

Este documento detalha a estrutura completa de menus e navegação da interface touchscreen do Sondvolt.

---

## Menu Principal (Grid Principal)

O menu principal exibe as principais funções do dispositivo em um grid otimizado para o display de 2.8" resolução 320x240.

### Layout do Menu Principal

| 📱Tela Inicial | | | | |
|:---:|:---:|:---:|:---:|:---:|
| **Função** | | | | |
| **🔍 Auto-Detect** | **📊 Componente** | **🌡️ Temperatura** | **⚡ Multímetro** | |
| Identificação automática | Modos de teste específicos | Via sonda DS18B20 | Tensão AC/DC | |
| *(2x3 grid)* | | | | |
| **📁 Histórico** | **⚙️ Ajustes** | *(Expandir)* | *(Expandir)* | |
| Últimas medições | Configurações | | | |

### Descrição dos Itens do Menu

| Ícone | Função | Descrição |
|:---:|:---|:---|
| 🔍 | **Auto-Detect** | Identificação automática de componentes |
| 📊 | **Componente** | Menu com modos de teste específicos |
| 🌡️ | **Temperatura** | Monitoramento via sonda DS18B20 |
| ⚡ | **Multímetro** | Tensão AC (RMS), DC, Corrente |
| 📁 | **Histórico** | Últimas 50 medições |
| ⚙️ | **Ajustes** | Configurações do sistema |

---

## Menu de Componentes (Submenu)

Ao tocar em "Componente", abre-se o menu de modos de teste específicos:

### Layout (Lista Vertical)

| # | Ícone | Modo | Descrição |
|:---:|:---:|:---|:---|
| 1 | 🔴 | **Resistor** | Ohmímetro (0.1Ω a 10MΩ) |
| 2 | 🟡 | **Capacitor** | Capacímetro (1pF a 10mF) |
| 3 | 🔵 | **Diodo/LED** | Teste de diodos e LEDs |
| 4 | 🟢 | **Transistor** | hFE e pinagem (NPN/PNP) |
| 5 | 🟣 | **Indutor** | Indutômetro (0.1mH a 1H) |
| 6 | ⚪ | **Volt DC** | Voltímetro básico (0-3.3V) |
| 7 | 📟 | **Frequência** | Medição de frequência |
| 8 | 📤 | **PWM Out** | Gerador de sinais PWM |
| 9 | 🌐 | **Optoacoplador** | Teste de optoacopladores |
| 10 | 🔗 | **Continuidade** | Teste sonoro de continuidade |
| 11 | 🔄 | **Ponte Ret.** | Teste de pontes de diodo |
| 12 | 🔍 | **ESR Meter** | Resistência série equivalente |

---

## Menu Multímetro

O menu de multímetro oferece duas categorias:

### Tensão AC (ZMPT101B)

| Leitura | Descrição |
|:---|:---|
| **V RMS** | Tensão eficaz (True RMS) |
| **V Peak** | Tensão de pico |
| **Hz** | Frequência estimada |

Selecione "AC" para activar o sensor ZMPT101B.

### Tensão DC (INA219)

| Leitura | Descrição |
|:---|:---|
| **V** | Tensão DC (0-26V) |
| **I** | Corrente DC (±3.2A) |
| **P** | Potência (calculada) |

Selecione "DC" para ativar o sensor INA219.

---

## Menu de Ajustes (Configurações)

### Configurações Disponíveis

| # | Ajuste | Opções | Descrição |
|:---:|:---|:---|:---|
| 1 | **Brilho** | 0-100% | Ajuste do backlight |
| 2 | **Volume** | Mudo/Baixo/Alto | Volume do buzzer |
| 3 | **Calibração** | — | Calibração de sensores |
| 4 | **Unidade Temp.** | °C / °F | Unidade de temperatura |
| 5 | **Limpar Log** | Sim/Não | Apaga histórico |
| 6 | **Informações** | — | Dados do sistema |

### Submenu Calibração

| Ajuste | Descrição |
|:---|:---|
| **ZMPT Scale** | Fator de escala AC |
| **ADC Offset** | Ajuste de zero |
| **INA219** | Calibração DC |

---

## Zonas de Toque

A interface responde a gestos e toques em zonas específicas:

### Zonas de Navegação

```
┌───────────────────────┐
│ ▲ TOPO (Scroll Up)   │
├───────────────────────┤
│                       │
│       CENTRO          │
│    (Selecionar)       │
│                       │
├───────────────────────┤
│ ▼ BASE (Scroll Down) │
├───────────────────────┤
│ ◄ VOLTAR │          │
└───────────────────────┘
```

| Zona | Ação |
|:---|:---|
| **Topo** | Navegar para cima / Increase |
| **Base** | Navegar para baixo / Decrease |
| **Centro** | Selecionar / Confirmar |
| **Canto inferior** | Voltar / Cancelar |

---

## Barra de Status

Exibida no topo de todas as telas:

```
┌───────────────────────────────────────┐
│ v3.0.0 │ 🌡️25°C │ 💾 │ ⚡           │
└───────────────────────────────────────┘
```

| Campo | Descrição |
|:---|:---|
| **v3.0.0** | Versão do firmware |
| **🌡️25°C** | Temperatura (se DS18B20 conectada) |
| **💾** | SD Card OK (ou "✗" se erro) |
| **⚡** | Status de energia |

---

## Fluxo de Navegação

```
                    ┌─────────────────┐
                    │   Menu Inicial   │
                    └────────┬────────┘
                             │
        ┌──────────┬──────────┼──────────┬──────────┐
        ▼          ▼          ▼          ▼          ▼
   ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
   │Auto-    │ │Compo-   │ │Temp.    │ │Multí-   │ │Histó-   │
   │Detect   │ │nente    │ │         │ │metro    │ │rico     │
   └─────────┘ └────┬────┘ └─────────┘ └────┬────┘ └─────────┘
                      │                      │
             ┌────────┴────────┐    ┌────────┴────────┐
             ▼               ▼    ▼               ▼
        ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
        │ Resistor│    │Capacitor│    │ AC      │    │ DC      │
        │ Diodo   │    │Transistor│   │(ZMPT)   │   │(INA219) │
        └─────────┘    └─────────┘    └─────────┘    └─────────┘
```

---

## Atalhos de Botões Físicos

Se o touchscreen falhar, use os botões físicos:

| Botão | Função |
|:---|:---|
| **Boot** | Selecionar / Confirmar |
| **RST** | Voltar / Cancelar |

---

<p align="center">
<i>Component Tester PRO v3.0 — Estrutura de Menus</i>
</p>