# ⚡ Guia de Segurança Elétrica

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

Este documento estabelece diretrizes críticas de segurança para montagem e operação do **Sondvolt**.

---

> [!DANGER]
> **AVISO CRÍTICO DE SEGURANÇA**
>
> A eletricidade pode causar ferimentos graves ou fatais!
> Este projeto envolve medições em alta tensão (250V AC). Siga TODAS as precauções!
>
> - ⚠️ Nunca trabalhe sozinho
> - ⚠️ Sempre use equipamentos de proteção (EPI)
> - ⚠️ Use o Sistema de Proteção Ativa (Fusível/Varistor/TVS)

---

## 1. Classificação de Riscos

### 1.1 Níveis de Risco

| Nível | Símbolo | Tensão | Perigo |
|:---:|:---:|:---|:---|
| **🔴 CRÍTICO** | ⚡ | >100V AC | Paralisia / Morte |
| **🟠 ALTO** | ⚠️ | 50-100V AC | Queimadura grave |
| **🟡 MÉDIO** | ⚡ | 25-50V DC | Choque perceptível |
| **🟢 BAIXO** | ℹ️ | <25V DC | Seguro |

### 1.2 Legenda Visual

```
┌─────────────────────────────────────────────────────────────┐
│                    ÍCONES DE PERIGO                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ⚡ CRÍTICO   │  Morte ou ferimento grave iminente       │
│  ⚠️  ALTO     │  Ferimento sério possibile             │
│  ⚡  MÉDIO    │  Pode causar desconforto               │
│  ℹ️  BAIXO    │  Risco mínimo (ação_NORMAL)            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Zonas de Perigo

### 2.1 Mapa de Zonas

```
┌─────────────────────────────────────────────────────────────────┐
│                    ZONAS DE PERIGO                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  ZONA AZUL - SEGURA (baixa tensão)                        │  │
│  │  • display CYD (5V)                                      │  │
│  │  • DS18B20 (3.3V)                                       │  │
│  │  • INA219 (3.3V)                                         │  │
│  │  • Probes (baixa tensão)                                 │  │
│  └──────────────────────────────────────────────────────────┘  │
│                           │                                    │
│                           │ Isolação Galvânica                 │
│                           ▼                                    │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  ZONA AMARELA - MÉDIA RISCO                               │  │
│  │  • ZMPT101B (entrada 250V AC)                             │  │
│  │    ⚠️ Isolamento requerido                               │  │
│  └──────────────────────────────────────────────────────────┘  │
│                           │                                    │
│                           │ Sem isolamento direto              │
│                           ▼                                    │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  ZONA VERMELHA - ALTO RISCO (diretamente)                 │  │
│  │  • Fios mains de energia                                  │  │
│  │  • Conexões sem isolamento                               │  │
│  │  ⚡ PERIGO DE MORTE                                      │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

### 2.2 Identificação visual de riscos

```
! (images/safety_zones_diagram.png)
*[Diagrama das zonas de危険 com cores codificadas]*
```

---

### 3.1 Alta Tensão AC (ZMPT101B)

O Sondvolt utiliza um sistema de proteção multinível para garantir que transientes e picos da rede elétrica não danifiquem o equipamento ou causem choques ao usuário:

1. **Proteção Termomagnética (Fusível 5A)**: Atua em caso de falha catastrófica ou curto-circuito interno.
2. **Proteção contra Surtos (Varistor 14D431)**: Absorve picos de tensão acima de 275V AC (comuns em descargas atmosféricas indiretas ou chaveamento de motores).
3. **Proteção Transiente Ultra-rápida (TVS P6KE400A)**: Ceifa transientes na escala de nanosegundos que o varistor possa deixar passar.
4. **Isolação Galvânica**: O ZMPT101B isola fisicamente a rede elétrica da placa ESP32 por meio de um transformador interno.

> [!IMPORTANT]
> **O uso do Fusível de 5A é OBRIGATÓRIO**. Substituir por valores maiores ou ignorar o fusível anula toda a segurança do sistema.

```
! (images/safety_high_voltage_warning.png)
*[Símbolo de perigo de alta tensão]*                   

> [!DANGER]
> O ZMPT101B mede напряжение de 250V AC da rede!
>
> - NÃO toque nos fios de medição quando a energia estiver ligada
> - Use luvas isolantes de borracha (classe 2 ou superior)
> - Mantenha uma distância segura dos fios desprotegidos
> - NÃO tente medir tensões maiores que 250V
```

| Tensão | Tempo de exposição | Efeito |
|:---|:---|:---|
| 50V | >1s | Paralisia muscular |
| 100V | Instâneo | fibrilação cardíaca |
| 250V | Instantâneo | Morte |

### 3.2 Descarga de Capacitores

```
! (images/safety_capacitor_warning.png)
*[Símbolo de capacitor carregado]*

> [!DANGER]
> Capacitores eletrolíticos armazenam energia perigosa!
>                                   
> - SEMPRE descargue antes de testar (use resistor ~1kΩ)
> - Não segure os terminais ao descarregar
> - Depois de conectado, esperar 30 segundos antes de tocar
```

**Procedimento de Descarga Seguro:**

```
┌──────────────────────────────────────────────────┐
│           PROCEDIMENTO DE DESCARGA                │
├──────────────────────────────────────────────────┤
│                                                   │
│  1. Desligue a alimentação                       │
│                                                   │
│  2. Conecte resistor 1kΩ (5W) entre terminais:   │
│                                                   │
│     C+ ───[1kΩ 5W]─── C-                        │
│                                                   │
│  3. Aguarde 30 segundos                          │
│                                                   │
│  4. Meça tensão com multímetro (deve ser 0V)    │
│                                                   │
│  5. Só então remova o capacitor                │
│                                                   │
└──────────────────────────────────────────────────┘
```

### 3.3 Curto-circuitos

```
> [!WARNING]
> Ao testar componentes, NUNCA crie curto-circuitos diretos!
>
> Resistores baixo valor ( <10Ω ) podem causar:
> - Aquecimento excessivo
> - Danos ao ESP32
> - Incêndio (raro, mas possível)
> - Queimaduras
```

| Resistência | Corrente (5V) | Risco |
|:---|:---|:---|
| 1Ω | 5A | Perigo de incêndio |
| 10Ω | 500mA | Aquecimento |
| 100Ω | 50mA | Seguro |

### 3.4 Polarização Incorreta

```
> [!CAUTION]
> Componentes polarizados podem explodir se conectados incorretamente!
>
> - Capacitores eletrolíticos (explodem/gotejam)
> - Diodos Zener (queimam)
> - LEDs (param irreversible)
> - Transistores (danificam permanentemente)
```

---

## 4. Equipamentos de Proteção Individual (EPI)

### 4.1 Equipamentos Obrigatórios

| EPI | Função | Classificação |
|:---|:---|:---|
| **Óculos de proteção** | Proteger olhos de faíscas | ANSI Z87.1 |
| **Luvas de borracha** | Isolamento | Classe 00 (500V) mín |
| **Calçado isolante** | Isolamento do solo | ASTM F1114 |
| **Não use jewelry** | Evitar contato | Anéis, relógios |

### 4.2 Equipamentos Recomendados

| EPI | Função |
|:---|:---|
| **Máscara facial** | Proteção facial completa |
| **Avental de cotton** | Roupa natural, não sintética |
| **Fones auditivos** | proteção sonora |

### 4.3 Como Usar os EPIs

```
! (images/safety_ppe_guide.png)
*[Guia visual de como vestir EPIs corretamente]*

1. ÓCULOS: Ajustar antes de iniciar
2. LUVAS: Verificar integridade (sem furos/rasgos)
3. CALÇADO: Sapatos fechados, secos
4. REMOVER: Anéis, relógios, pulseiras
```

---

## 5. Procedimentos de Operação Segura

### 5.1 Conexão dos Módulos

> [!WARNING]
> **SEMPRE desligue a alimentação antes de conectar/desconectar módulos!**

```
┌──────────────────────────────────────────────────┐
│           PROCEDIMENTO DE CONEXÃO              │
├──────────────────────────────────────────────────┤
│                                                   │
│  ANTES DE CONECTAR:                               │
│  ┌────────────────────────────────────────────┐  │
│  │  [ ] Desligar fonte de alimentação         │  │
│  │  [ ] Desconectar cabo USB                   │  │
│  │  [ ] Esperar 30 segundos                    │  │
│  │  [ ] Verificar se LEDs estão apagados        │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
│  CONECTAR:                                        │
│  ┌────────────────────────────────────────────┐  │
│  │  [ ] Conectar módulos um a um                  │  │
│  │  [ ] Verificar polaridade                   │  │
│  │  [ ] Verificar conexõesfirmes               │  │
│  │  [ ] Organizar cabos                      │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
│  DEPOIS:                                         │
│  ┌────────────────────────────────���─���─────────┐  │
│  │  [ ] Conferir todas as conexões            │  │
│  │  [ ] Ligar alimentação                  │  │
│  │  [ ] Testar sem carga primeiro           │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
└──────────────────────────────────────────────────┘
```

### 5.2 Medição de Tensão AC

> [!DANGER]
> **NUNCA segure os fios de probe com as mãos nuas ao medir AC!**

```
┌──────────────────────────────────────────────────┐
│        PROCEDIMENTO DE MEDIÇÃO AC               │
├──────────────────────────────────────────────────┤
│                                                   │
│  ANTES:                                           │
│  ┌────────────────────────────────────────────┐  │
│  │  [ ] Vestir EPI completo                   │  │
│  │  [ ] Usar luvas classe 2+                 │  │
│  │  [ ] Não trabalhar sozinho               │  │
│  │  [ ] Ter alguém por perto                 │  │
│  │  [ ] area de trabalho limpa             │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
│  DURANTE:                                         │
│  ┌────────────────────────────────────────────┐  │
│  │  [ ] Usar apenas probes isolados             │  │
│  │  [ ] Não tocar nos fios descobertos       │  │
│  │  [ ] Manter distância do circuito        │  │
│  │  [ ] Evitar movimentos bruscos           │  │
│  │  [ ] Ficar imóvel durante a leitura   │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
│  DEPOIS:                                          │
│  ┌────────────────────────────────────────────┐  │
│  │  [ ] Desligar fonte ANTES de desconectar   │  │
│  │  [ ] Descargar capacitores                 │  │
│  │  [ ] Guardar equipamentos                │  │
│  └────────────────────────────────────────────┘  │
│                                                   │
└──────────────────────────────────────────────────┘
```

### 5.3 Remoção de Componentes

```
┌──────────────────────────────────────────────────┐
│          PROCEDIMENTO DE REMOÇÃO                 │
├──────────────────────────────────────────────────┤
│                                                   │
│  1. Desligar e aguardar 30 segundos             │
│                                                   │
│  2. Descargar capacitores (se aplicável)         │
│                                                   │
│  3. Desconectar probes lentamente               │
│                                                   │
│  4. Nãopuxar pelos fios — Segurar pelo corpo     │
│                                                   │
│  5. Verificar temperatura (se foi aquecido)      │
│                                                   │
│  6. Componente ainda quente? Aguardar          │
│                                                   │
└──────────────────────────────────────────────────┘
```

---

## 6. Ambiente de Trabalho

### 6.1 Área de Trabalho Segura

```
! (images/safety_workspace_setup.png)
*[Layout ideal de área de trabalho]*

- [ ] Mesa seca e limpa
- [ ] Boa iluminação
- [ ] Sem umidade
- [ ] Sem materiais inflamáveis por perto
- [ ] Extintor por perto (classe C)
- [ ] Telefone acessível para emergências
```

### 6.2 Condições Ambientais

| Condição | Aceitável | Ideal |
|:---|:---|:---|
| Temperatura | 15-30°C | 20-25°C |
| Umidade | <70% | 40-50% |
| Iluminação | >300 lux | >500 lux |
| Ventilação | Natural | Forçada |

---

## 7. Primeiro Socorros

### 7.1 Em Caso de Acidente

```
┌──────────────────────────────────────────────────┐
│              PROCEDIMENTO DE EMERGÊNCIA          │
├──────────────────────────────────────────────────┤
│                                                   │
│  1. PARE! Não entre em pânico                      │
│                                                   │
│  2. DESLIGUE a fonte imediatamente              │
│                                                   │
│  3. Se a vítima estiver tocando o circuito:     │
│     → NÃO toque na vítima!                       │
���     → Desligue a fonte primeiro                │
│     → Se impossível, empurre com material      │
│       isolante (madeira seca)                  │
│                                                   │
│  4. Chame ajuda: SAMU 192 / BOMBEI 193         │
│                                                   │
│  5. Verifique respiração e pulso               │
│                                                   │
│  6. Se necessário, inicie RCP (apenas se treinado) │
│                                                   │
│  7. Mantenha a vítima aquecida e em repouso      │
│                                                   │
│  8. Aguarde chegada do socorro                │
│                                                   │
└──────────────────────────────────────────────────┘
```

### 7.2 Informações para Emergência

| Informação | Dados |
|:---|:---|
| **Local do acidente** | [SEU ENDEREÇO] |
| **Tipo de acidente** | Choque elétrica |
| **Tensão envolvida** | 250V AC máx |
| **Nome do projeto** | Sondvolt v3.2 |
| **Placa** | ESP32-2432S028R |

> [!IMPORTANT]
> Anote estas informações e mantenha-as sempre visíveis próximo ao local de trabalho!

### 7.3 Números de Emergência

| Serviço | Número |
|:---|:---|
| SAMU (Emergência médica) | **192** |
| Corpo de Bombeiros | **193** |
| Police | **190** |
| Centro de Intoxicação | **0800 722 6001** |

---

## 8. Checklist de Segurança

### 8.1 Antes de Cada Sessão

```
┌──────────────────────────────────────────────────┐
│          CHECKLIST PRÉ-OPERAÇÃO                   │
├──────────────────────────────────────────────────┤
│                                                   │
│  [ ] EPI completovestido                           │
│  [ ] area de trabalho limpa e organizada       │
│  [ ] Extintor acessível                          │
│  [ ] Iluminação adequada                         │
│  [ ] Sem umidade no ambiente                     │
│  [ ] Sem pessoas não autorizadas                 │
│                                                    │
├──────────────────────────────────────────────────┤
│  [ ] Fontes de alimentação desligadas            │
│  [ ] Cabos USB desconectados                    │
│  [ ] Capacitores descarregados                  │
│  [ ] Verificação visual de conexões            │
│  [ ] Sem curto-circuitos inadvertidos           │
│  [ ] Conexõesfirmes                              │
│                                                    │
└──────────────────────────────────────────────────┘
```

### 8.2 Depois de Cada Sessão

```
┌──────────────────────────────────────────────────┐
│          CHECKLIST PÓS-OPERAÇÃO                   │
├──────────────────────────────────────────────────┤
│                                                   │
│  [ ] Desligar tutte as fontes                    │
│  [ ] Desconectar cabos de alimentação            │
│  [ ] Descargar capacitores                      │
│  [ ] Limpar area de trabalho                    │
│  [ ] Guardar componentes                        │
│  [ ] Guardar ferramentas                       │
│  [ ] Anotar any incidente                      │
│                                                    │
└──────────────────────────────────────────────────┘
```

---

## 9. Manutenção e Inspeção

### 9.1 Inspeção Regular

| Verificação | Frequência | Responsável |
|:---|:---|:---|
| Condição dos fios | Semanal |Usuário |
| Estado dos probes | Semanal |Usuário |
| Integridade das luvas | Antes de cada uso | Usuário |
| Cabo USB/Carregador | Mensal | Usuário |
| Soldas e conexões | Mensal | Usuário |

---

## 10. Avisos Legais

```
┌──────────────────────────────────────────────────┐
│                 AVISO LEGAL                      │
├──────────────────────────────────────────────────┤
│                                                   │
│  Este projeto envolve eletricidade de ALTA      │
│  TENÇÃO que pode causar MORTE.                  │
│                                                   │
│  AO USAR ESTE PROJETO, VOCÊ CONCORDA QUE:        │
│                                                   │
│  • Assume todos os riscos associados             │
│  • Seguirá todas as precauções de segurança      │
│  • Não responsabilizará os autores por danos    │
│  • Tem conhecimento suficiente para operar       │
│                                                   │
│  SE VOCÊ NÃO TEM CERTEZA, NÃO PROSIGA!           │
│                                                   │
│  Procure ajuda profissional qualificada        │
│                                                   │
└──────────────────────────────────────────────────┘
```

---

## 11. Resumo Visual de Segurança

```
┌─────────────────────────────────────────────────────────────┐
│                 RESUMO DE SEGURANÇA                          │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ✅ FAÇA                                                    │
│  ─────────────────────────────────────────────────────────   │
│  • Use EPIs sempre                                         │
│  • Trabalhe em área iluminada e seca                       │
│  • Descarregue capacitores antes de testar                   │
│  • Verifique conexões antes de energizar                   │
│  • Mantenha extintor por perto                            │
│                                                              │
│  ❌ NÃO FAÇA                                               │
│  ─────────────────────────────────────────────────────────   │
│  • Não trabalhe sozinho                                  │
│  • Não toque fios quando energizado                       │
│  • Não use ropa sintética                                 │
│  • Nãosegure componentes ao desquecar                     │
│  • Não use ferramentas úmidas                              │
│  • Nãof mexa com jewelry                                 │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

> [!DANGER]
> **SE VOCÊ NÃO TEM CERTEZA, NÃO PROSIGA!**
>
> Encontre um profissional qualified para ajudá-lo.

---

_Fim do Guia de Segurança_

---

> [!TIP]
> Para mais informações, consulte o manual completo em `docs/MANUAL.md`.
> Para FAQ, consulte `docs/FAQ.md`.