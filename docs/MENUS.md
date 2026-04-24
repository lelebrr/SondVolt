# Descrição dos Menus - Component Tester PRO v2.0

## Índice

1. [Menu Principal](#1-menu-principal)
2. [Menu Medições](#2-menu-medições)
3. [Menu Configurações](#3-menu-configurações)
4. [Tela Térmica](#4-tela-térmica)
5. [Tela Scanner](#5-tela-scanner)
6. [Tela Histórico](#6-tela-histórico)
7. [Tela Estatísticas](#7-tela-estatísticas)
8. [Tela Sobre](#8-tela-sobre)
9. [Fluxo de Navegação](#9-fluxo-de-navegação)

---

![UI Showcase](../assets/ui_collage.png)

## 1. Menu Principal

### Visual

```
┌─────────────────────────────────────────────┐
│           COMPONENT TESTER PRO v2.0             │
├─────────────────────────────────────────────┤
│                                             │
│  ► Medir            │ [Selecionado]          │
│    Termica         │                       │
│    Scanner       │                       │
│    Historico    │                       │
│    Estatist     │                       │
│    Config      │                       │
│    Sobre       │                       │
│                                             │
└─────────────────────────────────────────────┘
       UP/DW │ OK │ BCK
```

### Opções

| # | Opção | Descrição | Acesso |
|---|-------|---------|--------|
| 1 | **Medir** | Menu de medições (13 tipos) | STATE_MEASURING |
| 2 | **Termica** | Monitor de temperatura | STATE_THERMAL_PROBE |
| 3 | **Scanner** | Auto-detecção contínua | STATE_SCANNER |
| 4 | **Historico** | Últimas medições | STATE_HISTORY |
| 5 | **Estatist** | Estatísticas | STATE_STATS |
| 6 | **Config** | Configurações | STATE_SETTINGS |
| 7 | **Sobre** | Informações | STATE_ABOUT |

### Navegação

- **UP/DW**: Selecionar opção
- **OK**: Entrar na função selecionada
- **BCK**: (não tem função no menu principal)

---

## 2. Menu Medições

### Visual

```
┌─────────────────────────────────────────────┐
│                 MEDIÇÕES                     │
├─────────────────────────────────────────────┤
│                                             │
│  ► Cap            │ [Ícone capacitivo]       │
│    Res          │ [Ícone resistor]        │
│    Diod        │ [Ícone diodo]          │
│    Trans       │ [Ícone transistor]    │
│    Ind        │ [Ícone indutor]       │
│    Volt        │ [Ícone voltímetro]    │
│    Freq        │ [Ícone frequêncímetro] │
│    PWM        │ [Sinal PWM]           │
│    Opto       │ [Optocoupler]        │
│    Cable      │ [Cabo]              │
│    Bridge    │ [Ponte]             │
│    Auto     │ [Auto-detectar]      │
│    Conti    │ [Continuidade]       │
│                                             │
└─────────────────────────────────────────────┘
       UP/DW │ OK │ BCK
```

### Todos os 13 Modos

| # | Nome | Badge | Descrição |
|---|------|-------|----------|
| 1 | **Cap** | 🎚 | Capacitor + ESR |
| 2 | **Res** | 𝒸 | Resistor |
| 3 | **Diod** | ◀ | Diodo/LED |
| 4 | **Trans** | ▶B | Transistor NPN |
| 5 | **Ind** |  ⟲ | Indutor |
| 6 | **Volt** | V | Voltímetro DC |
| 7 | **Freq** | ~ | Frequencímetro |
| 8 | **PWM** | ⊞ | Gerador PWM |
| 9 | **Opto** | ⊙↦ | Optocoupler |
| 10 | **Cable** | ═ | Continuidade |
| 11 | **Bridge** | ◇ | Ponte retificadora |
| 12 | **Auto** | ? | Auto-detecção |
| 13 | **Conti** | ⟿ | Continuidade c/ beep |

### Ícones Visuais

Durante a navegação, ícones são desenhados na tela:

| Modo | Ícone Desenhado |
|------|----------------|
| Cap | Duas linhas paralelas |
| Res | Retângulo com linhas |
| Diod | Triângulo com barra |
| Trans | Triângulo com base |
| Ind | Espirais/loops |
| Outros | Retângulo genérico |

### Tela de Resultado (Exemplo: Capacitor)

```
┌─────────────────────────────────────────────┐
│              CAPACITOR:                      │
├─────────────────────────────────────────────┤
│                                             │
│   100.00 uF                                 │
│                                             │
│   [Verde] = OK  [Vermelho] = Ruim           │
│                                             │
│              BCK = Voltar                   │
└─────────────────────────────────────────────┘
```

---

## 3. Menu Configurações

### Visual

```
┌─────────────────────────────────────────────┐
│            CONFIGURAÇÕES                     │
├─────────────────────────────────────────────┤
│                                             │
│  1. Calibrar Probes                         │
│  2. Modo Escuro                            │
│  3. Modo Silencioso                       │
│  4. Timeout                               │
│  5. Salvar Cfg                             │
│                                             │
└─────────────────────────────────────────────┘
       UP/DW │ OK │ BCK
```

### Opções Detalhadas

| # | Opção | Função | Comportamento |
|---|------|-------|-------------|
| 1 | **Calibrar Probes** | Calibração | Inicia rotina de calibração |
| 2 | **Modo Escuro** | Tema | Alterna ON/OFF |
| 3 | **Modo Silencioso** | Som | Alterna ON/OFF |
| 4 | **Timeout** | Tempo limite | Alterna 30s/60s |
| 5 | **Salvar Cfg** | EEPROM | Salva configurações |

### Exemplo: Modo Escuro Ativado

```
┌─────────────────────────────────────────────┐
│            CONFIGURAÇÕES                     │
├─────────────────────────────────────────────┤
│                                             │
│  1. Calibrar Probes                         │
│  2. Modo Escuro         [ON]              │
│  3. Modo Silenciloso                     │
│  4. Timeout          [30s]              │
│  5. Salvar Cfg                         │
│                                             │
└─────────────────────────────────────────────┘
       UP/DW │ OK │ BCK
```

### Feedback ao Salvar

```
┌─────────────────────────────────────────────┐
│            SALVO!                           │
├──────────────��──────────────────────────────┤
│                                             │
│   Configurações salvas na EEPROM              │
│                                             │
│   [Aguarde 1 segundo]                  │
└─────────────────────────────────────────────┘
```

---

## 4. Tela Térmica

### Visual Normal (< 70°C)

```
┌─────────────────────────────────────────────┐
│            TEMPERATURA                       │
├─────────────────────────────────────────────┤
│                                             │
│           25.5°C                            │
│                                             │
│   Estado: NORMAL                            │
│                                             │
│   Alerta: Nenhum                           │
│                                             │
│            🟢 LED apagado                  │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

### Visual HOT (70-90°C)

```
┌─────────────────────────────────────────────┐
│            TEMPERATURA                       │
├─────────────────────────────────────────────┤
│                                             │
│           80.0°C                            │
│                                             │
│   Estado: HOT ⚠️                        │
│                                             │
│   Alerta: Beep a cada 1s                   │
│                                             │
│            🟡 LED piscando                  │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

### Visual DANGER (90-110°C)

```
┌─────────────────────────────────────────────┐
│            TEMPERATURA                       │
├─────────────────────────────────────────────┤
│                                             │
│           100.0°C                           │
│                                             │
│   Estado: DANGER ⚠️                      │
│                                             │
│   Alerta: Beep a cada 500ms                 │
│                                             │
│            🟠 LED piscando rápido           │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

### Visual CRITICAL (> 110°C)

```
┌─────────────────────────────────────────────┐
│            TEMPERATURA                       │
├─────────────────────────────────────────────┤
│                                             │
│           115.0°C                           │
│                                             │
│   Estado: CRITICAL ⚠️                     │
│                                             │
│   Alerta: Beep a cada 200ms                 │
│                                             │
│            🔴 LED fixo                      │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

---

## 5. Tela Scanner

### Tela Inicial

```
┌─────────────────────────────────────────────┐
│              SCANNER                         │
├─────────────────────────────────────────────┤
│                                             │
│   OK = auto medicao                         │
│   BCK = sair                            │
│                                             │
│         Pressione OK para iniciar              │
│                                             │
└─────────────────────────────────────────────┘
       UP/DW │ OK │ BCK
```

### Modo Scanner Ativo

```
┌─────────────────────────────────────────────┐
│              SCANNER                         │
├─────────────────────────────────────────────┤
│                                             │
│   Escaneando...                             │
│                                             │
│   Tipo: [Cap] Valor: [100uF]              │
│                                             │
│   Status: OK                               │
│                                             │
│   OK=parar BCK=sair                       │
│                                             │
└─────────────────────────────────────────────┘
```

### Comportamento

- Intervalo: 2 segundos entre medições
- Pressione OK para iniciar/parar
- Pressione BCK para sair

---

## 6. Tela Histórico

### Visual

```
┌─────────────────────────────────────────────┐
│             HISTÓRICO                       │
├─────────────────────────────────────────────┤
│                                             │
│   Cap: 100.0  [🟢]                       │
│   Res: 1000   [🟢]                       │
│   Diod: OK   [🟢]                       │
│   Trans: 100 [🟢]                       │
│   Res: OPEN  [🔴]                       │
│   Ind: 1000  [🟢]                       │
│   ...                                     │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

### Se Histórico Vazio

```
┌─────────────────────────────────────────────┐
│             HISTÓRICO                       │
├─────────────────────────────────────────────┤
│                                             │
│        Nenhuma medição registrada             │
│                                             │
│        Realize medições para ver aqui         │
│                                             │
└─────────────────────────────────────────────┘
```

### Detalhes

- Armazena últimas 10 medições
- Cor verde = componenteOK
- Cor vermelho = componente ruim
- Mostra: nome, valor, status

---

## 7. Tela Estatísticas

### Visual

```
┌─────────────────────────────────────────────┐
│            ESTATÍSTICAS                      │
├─────────────────────────────────────────────┤
│                                             │
│   Total: 150                                │
│                                             │
│   Defeituosos: 15                          │
│                                             │
│   Taxa falha: 10.0%                         │
│                                             │
│   Temp: 25.5°C                              │
│                                             │
└─────────────────────────────────────────────┘
```

### Quando Sem Dados

```
┌─────────────────────────────────────────────┐
│            ESTATÍSTICAS                      │
├─────────────────────────────────────────────┤
│                                             │
│   Total: 0                                  │
│                                             │
│   Defeituosos: 0                           │
│                                             │
│   Taxa falha: --                            │
│                                             │
│   Temp: 25.5°C                              │
│                                             │
└─────────────────────────────────────────────┘
```

---

## 8. Tela Sobre

### Visual

```
┌─────────────────────────────────────────────┐
│            SOBRE                             │
├─────────────────────────────────────────────┤
│                                             │
│   Component Tester                          │
│   PRO v2.0                                  │
│                                             │
│   Desenvolvido por: Leandro                  │
│   Baseado em Arduino Uno R3                 │
│   Firmware: v2.0.1                       │
│   Data: 04/03/2026                       │
│                                             │
└─────────────────────────────────────────────┘
       BCK = Voltar
```

---

## 9. Fluxo de Navegação

### Diagrama de Estados

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      MÁQUINA DE ESTADOS                                │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   STATE_MENU                                                          │
│      │                                                                │
│      ├──[OK]──► STATE_MEASURING ──────► Menu de 13 medições            │
│      │                   │                                               │
│      │                   └──[BCK]──► STATE_MENU                       │
│      │                                                                │
│      ├──[OK]──► STATE_THERMAL_PROBE ──► Monitor temperatura          │
│      │                   │                                               │
│      │                   └──[BCK]──► STATE_MENU                       │
│      │                                                                │
│      ├──[OK]──► STATE_SCANNER ────────────► Auto-detecção contínua      │
│      │                   │                                               │
│      │                   └──[BCK]──► STATE_MENU                       │
│      │                                                                │
│      ├──[OK]──► STATE_HISTORY ────────────► Histórico de medições      │
│      │                   │                                               │
│      │                   └──[BCK]──► STATE_MENU                       │
│      │                                                                │
│      ├──[OK]──► STATE_STATS ──────────────► Estatísticas              │
│      │                   │                                               │
│      │                   └──[BCK]���─��� STATE_MENU                       │
│      │                                                                │
│      ├──[OK]──► STATE_SETTINGS ───────────► Configurações                │
│      │                   │                                               │
│      │                   └──[BCK]──► STATE_MENU                       │
│      │                                                                │
│      └──[OK]──► STATE_ABOUT ──────────────► Informações              │
│                          │                                           │
│                          └──[BCK]──► STATE_MENU                       │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Tabela de Estados

| Estado | Nome | Origem | Destino |
|--------|-----|-------|--------|
| STATE_MENU | Menu Principal | - | Todos |
| STATE_MEASURING | Menu Medições | STATE_MENU | STATE_MENU |
| STATE_THERMAL_PROBE | Térmica | STATE_MENU | STATE_MENU |
| STATE_SCANNER | Scanner | STATE_MENU | STATE_MENU |
| STATE_HISTORY | Histórico | STATE_MENU | STATE_MENU |
| STATE_STATS | Estatísticas | STATE_MENU | STATE_MENU |
| STATE_SETTINGS | Configurações | STATE_MENU | STATE_MENU |
| STATE_ABOUT | Sobre | STATE_MENU | STATE_MENU |

---

## Resumo Visual das Screens

### Todas as Screens

| Screen | Itens | Cores | Ícones |
|--------|-------|-------|--------|
| **Menu Principal** | 7 itens | BRANCO/CINZA | Não |
| **Menu Medições** | 13 itens | BRANCO/CINZA | Sim |
| **Config** | 5 itens | BRANCO/CINZA | Não |
| **Térmica** | 1 info | Verde/Amarelo/Laranja/Vermelho | Sim |
| **Scanner** | 1 info | BRANCO | Não |
| **Histórico** | 10 info | Verde/Vermelho | Sim |
| **Estatísticas** | 4 info | BRANCO | Não |
| **Sobre** | 5 info | BRANCO | Não |

---

**Continue learning:**
- [GUIDES.md](GUIDES.md) - Guias passo a passo
- [COMPONENTS.md](COMPONENTS.md) - Detalhes técnicos
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solução de problemas