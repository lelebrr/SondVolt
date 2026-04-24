# Manual do Usuário - Component Tester PRO v2.0

## Índice

1. [Introdução](#1-introdução)
2. [Primeiro Uso](#2-primeiro-uso)
3. [Navegação nos Menus](#3-navegação-nos-menus)
4. [Funções de Medição](#4-funções-de-medição)
5. [Sonda Térmica](#5-sonda-térmica)
6. [Configurações](#6-configurações)
7. [Interpretação dos Resultados](#7-interpretação-dos-resultados)
8. [Dicas de Segurança](#8-dicas-de-segurança)
9. [Perguntas Frequentes](#9-perguntas-frequentes)

---

![UI Showcase](../assets/ui_collage.png)

## 1. Introdução

### 1.1 O Que É o Component Tester?

O Component Tester PRO v2.0 é um dispositivo eletrônico projetado para ajudá-lo a identificar e testar componentes eletrônicos. Com ele, você pode verificar se um componente está funcionando corretamente ou precisa ser substituído.

### 1.2 Quem Deve Usar Este Manual?

Este manual foi escrito principalmente para:
- �.beginners iniciantes que nunca usaram um testador de componentes
- 👨‍🔧 hobistas de eletrônica
- 👩‍🎓 estudantes aprendendo eletrônica
- 🔧 técnicos que precisam de uma referência rápida

### 1.3 O Que Você Precisa Saber Antes

Antes de usar este dispositivo, você deve saber:
- O básico de componentes eletrônicos (resistores, capacitores, diodos, etc.)
- Como conectar fios e probes
-Como ler valores básicos

**Não se preocupe se você é iniciante!** Este manual explica tudo de forma simples e passo a passo.

---

## 2. Primeiro Uso

### 2.1 Ligando o Dispositivo

```
┌────────────────────────────────────────┐
│           Component Tester v2.0          │
│               - Leandro -              │
│                                        │
│            [██████]                    │
│                                        │
│              Init...                   │
└────────────────────────────────────────┘
```

Quando você lig o dispositivo, uma tela de inicialização aparecerá por alguns segundos. Aguarde até que o menu principal apareça.

### 2.2 Conhecendo a Tela Inicial

O menu principal mostra todas as funções disponíveis:

```
┌─────────────────────────────┐
│ ► Medir                      │ ← Item selecionado (seta)
│   Termica                    │
│   Scanner                    │
│   Historico                 │
│   Estatist                  │
│   Config                    │
│   Sobre                     │
└─────────────────────────────┘
 UP/DW │ OK │ BCK           ← Instruções
```

### 2.3 Os Botões de Controle

O dispositivo possui 6 botões físicos para navegação:

| Botão | Função | Como Usar |
|-------|--------|-----------|
| **UP** | Mover para cima | Pressione para ir ao item anterior |
| **DOWN** | Mover para baixo | Pressione para ir ao próximo item |
| **OK** | Confirmar/Selecionar | Pressione para entrar em uma função |
| **BCK** | Voltar | Pressione para retornar ao menu anterior |

---

## 3. Navegação nos Menus

### 3.1 Menu Principal

O menu principal é o ponto de partida de todas as funções:

```
┌───────────────────────────────┐
│ ► Medir   ← Seleção atual   │
│   Termica                     │
│   Scanner                     │
│   Historico                   │
│   Estatist                    │
│   Config                      │
│   Sobre                       │
└───────────────────────────────┘
  UP/DW | OK | BCK
```

**Como navegar:**

1. **Para cima/baixo:** Use os botões UP e DOWN
2. **Para selecionar:** Pressione OK no item desejado
3. **Para voltar:** Pressione BCK

### 3.2 Entendendo as Setas

- **►** = Item atualmente selecionado
- ** **(sem símbolo)** = Item não selecionado

### 3.3 Fluxo de Navegação

```
Menu Principal
    │
    ├──► Medir ─────────────► Submenu Medições (13 modos)
    │                             │
    │                             ├── Capacitor
    │                             ├── Resistor
    │                             ├── Diodo/LED
    │                             ├── Transistor
    │                             ├── Indutor
    │                             ├── Volt DC
    │                             ├── Frequência
    │                             ├── PWM
    │                             ├── Optocoupler
    │                             ├── Cabo
    │                             ├── Bridge
    │                             ├── Auto
    │                             └── Continuidade
    │
    ├──► Termica ────────────► Monitor de Temperatura
    │
    ├──► Scanner ────────────► Auto-detecção Contínua
    │
    ├──► Historico ──────────► Histórico de Medições
    │
    ├──► Estatist ───────────► Estatísticas
    │
    ├──► Config ──────────────► Configurações
    │
    └──► Sobre ─────────────► Informações do Dispositivo
```

---

## 4. Funções de Medição

### 4.1 Acessando o Menu de Medições

1. No menu principal, selecione **"Medir"**
2. Pressione **OK**

Você verá 13 tipos de medições:

```
┌────────────────────────┐
│ ► Cap                  │ ← Capacitor
│   Res                  │ ← Resistor
│   Diod                 │ ← Diodo/LED
│   Trans                │ ← Transistor
│   Ind                  │ ← Indutor
│   Volt                 │ ← Volt DC
│   Freq                 │ ← Frequência
│   PWM                  │ ← PWM
│   Opto                 │ ← Optocoupler
│   Cable                │ ← Cabo
│   Bridge               │ ← Bridge
│   Auto                │ ← Auto-detectar
│   Conti                │ ← Continuidade
└────────────────────────┘
  UP/DW | OK | BCK
```

### 4.2 Ícones Visuais

À medida que você navega, ícones visuais aparecem para ajudá-lo a identificar o componente:

| Seleção | Ícone Exibido |
|---------|-------------|
| Cap | ⏤⏤ (duas linhas paralelas) |
| Res | ▬ (resistor) |
| Diod | ▶ (triângulo) |
| Trans | ▶B (transistor) |
| Ind | coils (espirais) |

### 4.3 Como Medir Cada Componente

Consulte [COMPONENTS.md](COMPONENTS.md) para instruções detalhadas para cada tipo de medição.

---

## 5. Sonda Térmica

### 5.1 O Que É a Sonda Térmica?

A sonda térmica é um sensor DS18B20 que mede a temperatura ambiente ou de componentes. É à prova d'água e pode ser usada para monitorar temperaturas.

### 5.2 Como Acessar

1. No menu principal, selecione **"Termica"**
2. Pressione **OK**

### 5.3 Tela de Temperatura

```
┌─────────────────────────────┐
│     Temperatura atual:       │
│                             │
│         25.5°C              │
│                             │
│   Estado: NORMAL            │
│                             │
│   Alerta: Nenhum            │
└─────────────────────────────┘
      BCK = Voltar
```

### 5.4 Níveis de Alerta

| Temperatura | Cor | Alerta |
|------------|-----|--------|
| < 70°C | 🟢 Verde | Nenhum |
| 70-90°C | 🟡 Amarelo | HOT |
| 90-110°C | 🟠 Laranja | DANGER |
| > 110°C | 🔴 Vermelho | CRITICAL |

### 5.5 Comportamento de Alerta

| Nível | LED | Buzzer |
|-------|-----|-------|
| NORMAL | Apagado | Silencioso |
| HOT | Piscando | Beep a cada 1 segundo |
| DANGER | Piscando rápido | Beep a cada 500ms |
| CRITICAL | Fixo | Beep a cada 200ms |

---

## 6. Configurações

### 6.1 Menu de Configurações

O menu de configurações permite personalizar o dispositivo:

```
┌─────────────────────────────┐
│     Configuracoes            │
│                             │
│ 1. Calibrar Probes          │
│ 2. Modo Escuro              │
│ 3. Modo Silencioso         │
│ 4. Timeout                 │
│ 5. Salvar Cfg               │
└─────────────────────────────┘
      UP/DW | OK | BCK
```

### 6.2 Opções de Configuração

| Opção | Função | Descrição |
|------|-------|-----------|
| **Calibrar Probes** | Calibra os probes | Ajusta a precisão das medições |
| **Modo Escuro** | Alterna cores | Troca entre tema claro/escuro |
| **Modo Silencioso** | Silencia beep | Desabilita sons de feedback |
| **Timeout** | Tempo limite | Alterna entre 30s e 60s |
| **Salvar Cfg** | Salva configurações | Salva na EEPROM |

### 6.3 Como Alterar Configurações

1. Selecione a opção desejada com **UP/DOWN**
2. Pressione **OK** para alterar
3. Para salvar, selecione **"Salvar Cfg"** e pressione **OK**

---

## 7. Interpretação dos Resultados

### 7.1 Cores de Feedback

| Cor | Significado | Ação Necessária |
|-----|-------------|-----------------|
| 🟢 Verde | Componente OK | Nenhuma |
| 🟡 Amarelo | Atenção | Verificar componente |
| 🔴 Vermelho | Problema | Substituir componente |

### 7.2 LEDs de Status

| LED | Significado |
|-----|-------------|
| **LED Verde** | Componente funcionando |
| **LED Vermelho** | Componente com problema |

### 7.3 Sons de Feedback

| Som | Significado |
|-----|------------|
| Beep curto | Seleção_confirmada |
| Beep longo | Componente OK |
| Beep duplo | Alerta de temperatura |

---

## 8. Dicas de Segurança

### 8.1 Antes de Medir

✅ **SEMPRE**:
- Desligue o circuito antes de medir
- Drene capacitores grandes antes de medir
- Use probes limpos e em bom estado

❌ **NUNCA**:
- Não meça componentes em circuitos energizados
- Não exposed a umidade extrema
- Não force os probes nos componentes

### 8.2 Limites de Segurança

| Componente | Tensão Máxima | Corrente Máxima |
|------------|---------------|-----------------|
| Probes | 5V | 40mA |

### 8.3 Armazenamento

- Armazene em local seco
- Evite exposição ao sol direto
- Mantenha os probes protegidos

---

## 9. Perguntas Frequentes

### P: O dispositivo não liga. O que fazer?
R: Verifique as conexões e certifique-se de que o Arduino está recebendo energia.

### P: As medições estão imprecisas. Como calibrar?
R: Acesse Config > Calibrar Probes e siga as instruções na tela.

### P: Como salvar as configurações?
R: Acesse Config > Salvar Cfg e pressione OK.

### P: O LED vermelho fica aceso sempre. O que significa?
R: Significa que o último componente testado apresentou problema.

### P: Posso usar o dispositivo para testar componentes SMD?
R: Sim, use probes finos ou pinças crocodilo para componentes SMD.

### P: Comouno o historico de medições?
R: Acesse Menu Principal > Historico para ver as últimas 10 medições.

### P: Qual a diferença entre Scanner e Auto-detectar?
R: Scanner faz medições contínuas automaticamente. Auto-detectar identifica o tipo de componente automaticamente.

### P: O buzzer não funciona. Está quebrado?
R: Verifique se o Modo Silencioso está ativado em Config.

---

## Glossário

| Termo | Significado |
|-------|------------|
| **Probe** | Ponta de teste (vermelho = positivo, preto = negativo) |
| **Capacitor** | Componente que armazena energia elétrica |
| **Resistor** | Componente que limita a corrente |
| **Diodo** | Componente que permite corrente em uma direção |
| **Transistor** | Componente amplifying/chave |
| **Indutor** | Componente que almacena energia em campo magnético |
| **EEPROM** | Memória não volátil para configurações |
| **SD Card** | Cartão de memória para logging |
| **PWM** | Modulação por largura de pulso |
| **ESR** | Resistência equivalente em série |

---

## Próximos Passos

Agora que você aprendeu o básico, você pode:

1. 📖 Ler mais em [COMPONENTS.md](COMPONENTS.md)
2. 🔧 Ver o hardware em [HARDWARE.md](HARDWARE.md)
3. ⚙️ Ver configurações em [CONFIG.md](CONFIG.md)
4. ❓ Ver FAQ em [FAQ.md](FAQ.md)
5. 🔧 Solucionar problemas em [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

---

## Suporte

Se você tiver dúvidas ou precisar de ajuda:
- Revise este manual
- Consulta [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- Verifique as conexões em [PINOUT.md](PINOUT.md)

---

**Documento criado para usuários iniciantes**
*Component Tester PRO v2.0 - Desenvolvido por Leandro*