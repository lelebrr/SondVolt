# Configurações - Component Tester PRO v2.0

## Índice

1. [Configs Disponíveis](#1-configs-disponíveis)
2. [Calibração](#2-calibração)
3. [Modo Escuro](#3-modo-escuro)
4. [Modo Silencioso](#4-modo-silencioso)
5. [Timeout](#5-timeout)
6. [Salvar Configurações](#6-salvar-configurações)
7. [Dados na EEPROM](#7-dados-na-eeprom)
8. [Valores Padrão](#8-valores-padrão)

---

## 1. Configs Disponíveis

### Menu de Configurações

```
┌─────────────────────────────────────────────────────────────┐
│              CONFIGURAÇÕES                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. Calibrar Probes    [Offsets]                            │
│  2. Modo Escuro       [ON/OFF]                             │
│  3. Modo Silencioso  [ON/OFF]                             │
│  4. Timeout          [30s/60s]                             │
│  5. Salvar Cfg       [OK]                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Acceso

1. Vá ao **Menu Principal**
2. Selecione **"Config"**
3. Pressione **OK**
4. Use **UP/DW** para navegar
5. Pressione **OK** para alterar
6. Pressione **BCK** para sair

---

## 2. Calibração

### O que faz

Ajusta a precisão das medições através de offsets de software.

### Como usar

```
┌─────────────────────────────────────────────────────────────┐
│           CALIBRAR PROBES                                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  PASSO 1: Menu > Config > 1. Calibrar Probes             │
│                                                             │
│  PASSO 2: Conecte um resistor de valor conhecido           │
│          (exatamente 10kΩ)                              │
│                                                             │
│  PASSO 3: Ajuste ate mostrar 10.0kΩ                        │
│                                                             │
│  PASSO 4: Pressione OK para salvar                         │
│                                                             │
│  Valores são salvos na EEPROM                             │
└─────────────────────────────────────────────────────────────┘
```

### Valores

| Ajuste | Range | Padrão |
|-------|-------|--------|
| Offset Probe 1 | -10.0 a +10.0 | 0.0 |
| Offset Probe 2 | -10.0 a +10.0 | 0.0 |

### Frequência

Recalibração recomendada:
- A cada 6 meses
- Após substituir probes
- Se medições ficarem imprecisas

---

## 3. Modo Escuro

### O que faz

Alterna entre tema claro e escuro no display.

### Comparação

```
┌────────────────────────���────────────────────────────────────┐
│              MODO CLARO (OFF)                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   Texto: BRANCO                                            │
│   Fundo: PRETO                                             │
│   Destaque: BRANCO                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│              MODO ESCURO (ON)                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   Texto: PRETO                                             │
│   Fundo: BRANCO                                           │
│   Destaque: PRETO                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Quando usar

| Situação | Recomendação |
|----------|---------------|
| Ambiente claro | OFF (modo claro) |
| Ambiente escuro | ON (modo escuro) |
| Economia de energia | ON |

### Efeito

- Alterna cores de texto e fundo
- Não afeta LEDs ou buzzer
- Salvo na EEPROM

---

## 4.Modo Silencioso

### O que faz

Desabilita todos os sons do buzzer.

### Comparação

| Modo | Beeps | Feedback Sonoro |
|------|-------|-----------------|
| OFF (padrão) | ✅ Ativo | ✅ Sim |
| ON | ❌ Desativado | ✅ LED apenas |

### Quando usar

| Situação | Recomendação |
|----------|---------------|
| Uso normal | OFF |
| Ambiente silencioso | ON |
| Teste em público | ON |

### Sounds Ativos/Desativados

| Função | Som Normal | Silencioso |
|--------|------------|-------------|
| Seleção menu | beep cur | LED apenas |
| Componente OK | beep longo | LED verde |
| Componente RUIM | beep duplo | LED vermelho |
| Temperatura | beep progressivo | LED apenas |
| Cabo continuidade | beep | LED apenas |

---

## 5. Timeout

### O que faz

Define o tempo limite antes de retornar ao menu principal.

### Opções

| Tempo | Quando usar |
|------|-------------|
| 30s (padrão) | Testes rápidos |
| 60s | Testes demoorados |

### Comportamento

```
┌─────────────────────────────────────────────────────────────┐
│                 TIMEOUT                                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Se nenhum botão for pressionado por X segundos:            │
│                                                             │
│  1. Volta ao Menu Principal                               │
│  2. LEDs desligam                                       │
│  3. Display permanece                                  │
│                                                             │
│  Pressione qualquer botão para cancelar                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. Salvar Configurações

### O que faz

Salva todas as configurações na EEPROM do Arduino.

### Como usar

```
┌─────────────────────────────────────────────────────────────┐
│              SALVAR CFG                                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  PASSO 1: Após alterar configurações                   │
│                                                             │
│  PASSO 2: Navegue até "5. Salvar Cfg"                  │
│                                                             │
│  PASSO 3: Pressione OK                                  │
│                                                             │
│  PASSO 4: Aguarde confirmação na tela                   │
│                                                             │
│  "Salvo!" aparecera por 1 segundo                      │
└─────────────────────────────────────────────────────────────┘
```

### O que é salvo

| Configuração | Endereço EEPROM |
|--------------|-----------------|
| offset1 | 0x00 |
| offset2 | 0x04 |
| darkMode | 0x08 |
| silentMode | 0x09 |
| timeoutDuration | 0x0A |
| totalMeasurements | 0x0E |
| faultyMeasurements | 0x12 |

### Importância

⚠️ **SEMPRE** salve após alterações! Configurações não salvas serão perdidas ao reiniciar.

---

## 7. Dados na EEPROM

### Estrutura de Memória

```
┌─────────────────────────────────────────────────────────────┐
│              MAPA DA EEPROM                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ Endereço  │ Tamanho  │ Descrição                          │
│ ──────────────────────────────────────────────────────      │
│ 0x00      │ 4 bytes  │ offset1 (float)                    │
│ 0x04      │ 4 bytes  │ offset2 (float)                    │
│ 0x08      │ 1 byte   │ darkMode (bool)                   │
│ 0x09      │ 1 byte   │ silentMode (bool)                  │
│ 0x0A      │ 4 bytes  │ timeoutDuration (unsigned long)   │
│ 0x0E      │ 4 bytes  │ totalMeasurements (unsigned long)│
│ 0x12      │ 4 bytes  │ faultyMeasurements (unsigned long)│
│ 0x16-0x3FF│         │ Reservado                        │
│                                                             │
│ Total usado: 22 bytes                                    │
│ Total disponível: 1024 bytes                             │
└─────────────────────────��─��─────────────────────────────────┘
```

### Retenção

- **Tempo de retenção:** ~100 anos
- **Ciclos de escrita:** 100,000

⚠️ **Cuidado:** Não escreva na EEPROM frequentemente! Cada escrita reduz a vida útil.

---

## 8. Valores Padrão

### Padrão de fábrica

| Configuração | Valor Padrão |
|--------------|-------------|
| offset1 | 0.0 |
| offset2 | 0.0 |
| darkMode | false |
| silentMode | false |
| timeoutDuration | 30000 (30s) |
| totalMeasurements | 0 |
| faultyMeasurements | 0 |

### Como restaurar padrão

```
┌─────────────────────────────────────────────────────────────┐
│              RESETAR PARA PADRÃO                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Método 1: via software                                    │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ 1. Entre em Config                                │   │
│  │ 2. Configure manualmente                        │   │
│  │    offset1 = 0.0                                 │   │
│  │    offset2 = 0.0                                 │   │
│  │    darkMode = false                              │   │
│  │    silentMode = false                            │   │
│  │    timeoutDuration = 30000                       │   │
│  │ 3. Selecione "Salvar Cfg"                        │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  Método 2: via upload de firmware                           │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ 1. Grave um novo sketch 'blinky'                 │   │
│  │ 2. Carregue o firmware do Component Tester       │   │
│  │   EEPROM será preservada                         │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  Método 3: bootloader                                     │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ 1. Use AVR mkisprg para borrar EEPROM            │   │
│  │ 2. Carregue o firmware novamente                │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Resumo

| Config | Valores | Padrão | Salvar? |
|---------|---------|--------|----------|
| Calibrar | ±10.0 | 0.0 | ✅ |
| Modo Escuro | ON/OFF | OFF | ✅ |
| Modo Silencioso | ON/OFF | OFF | ✅ |
| Timeout | 30s/60s | 30s | ✅ |
| Salvar | OK | - | - |

---

**Dica:** Sempre salve após alterações usando "5. Salvar Cfg"!

---

**Continue learning:**
- [HARDWARE.md](HARDWARE.md) - Hardware details
- [FAQ.md](FAQ.md) - Perguntas frequentes