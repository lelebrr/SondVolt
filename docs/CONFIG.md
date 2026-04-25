# ⚙️ Configurações — Sondvolt v3.2

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

Este documento detalha todas as opções de configuração do firmware do Sondvolt, incluindo calibração, persistência NVS e ajustes do sistema.

---

## 🏗️ Arquitetura de Configuração

```
┌─────────────────────────────────────────────────────────────────┐
│                  SISTEMA DE CONFIGURAÇÃO                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Interface    │    │   NVS Storage  │    │   Calibração│ │
│  │   (Touch/UI)   │◄──►│   (Flash)      │◄──►│   (ADC/AC)  │ │
│  └─────────────────┘    └─────────────────┘    └─────────────┘ │
│         │                └─────────────────┘                   │
│         │                                                     │
│         ▼                                                     │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Sistema      │    │   Segurança    │    │   Display   │ │
│  │   (Geral)      │    │   (Hardware)   │    │   (Brilho)  │ │
│  └─────────────────┘    └─────────────────┘    └─────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### ⚡ Performance do Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                    PERFORMANCE DE CONFIGURAÇÃO                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Tempo de Gravação:     ████████████████████████ 50ms         │
│  Tempo de Leitura:      █████████████████████████ 10ms        │
│  Capacidade NVS:        ███████████████████████████ 256KB      │
│  Ciclos de Gravação:    ███████████████████████████ 100.000    │
│  Uso Memória:           ██████████ 15% (config)               │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 🔧 Sistema de Persistência NVS

O ESP32 utiliza a **NVS (Non-Volatile Storage)** para salvar configurações na memória Flash, oferecendo vantagens sobre a EEPROM tradicional:

### 🎯 Vantagens do NVS

| Recurso | Descrição | Benefício |
|:---|:---|:---|
| **Auto-save** | Alterações são salvas automaticamente | Sem perda de dados |
| **Durabilidade** | Gerenciamento de desgaste integrado | Maior vida útil |
| **Capacidade** | Suporte a muito mais dados | Configurações complexas |
| **Rapidez** | Acesso rápido (ms) | Interface responsiva |

### 📋 Tabela de Chaves de Configuração

| Chave | Tipo | Descrição | Padrão | Faixa |
|:---|:---:|:---|:---:|:---:|
| `offset1` | Float | Calibração ADC (Probe) | 0.0 | ±5.0 |
| `offset2` | Float | Calibração ADC (Reservado) | 0.0 | ±5.0 |
| `silentMode` | Bool | Silenciar buzzer | false | true/false |
| `backlight` | Int | Brilho (0-255) | 180 | 0-255 |
| `zmptScale` | Float | Escala sensor AC | 1.0 | 0.1-2.0 |
| `safetyAck` | Bool | Hardware de segurança | false | true/false |
| `tempUnit` | Bool | Unidade: 0=°C, 1=°F | 0 | 0-1 |
| `total` | ULong | Total de medições | 0 | 0-4.2B |
| `faulty` | ULong | Componentes defeituosos | 0 | 0-4.2B |

### 🔍 Fluxo de Configuração

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Interface     │    │   NVS Manager   │    │   Armazenamento │
│   (Usuário)     │───►│   (Sistema)     │───►│   (Flash)       │
│                 │    │                 │    │                 │
│ [Ajustar]       │    │ [Validar]       │    │ [Salvar]        │
│ [Confirmar]     │    │ [Criptografar]  │    │ [Backup]        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 🖥️ Configurações do Display

### 🎯 Controle de Brilho

```
┌─────────────────────────────────────────────────────────────────┐
│                    CONTROLE DE BRILHO                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Brilho Atual: ████████████████████████████ 70% (180/255)      │
│  Economia:     ████████████ 30% economia energética           │
│  Vida Útil:   █████████████████████████████ +50% estimada     │
│                                                                 │
│  [0%]    [25%]    [50%]    [75%]    [100%]                    │
│  🔘      🔘       🔘       🔘       🔘                       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 📊 Especificações Técnicas

| Parâmetro | Valor | Descrição |
|:---|:---|:---|
| **Range** | 0% a 100% | Controle PWM (0-255) |
| **GPIO** | 21 | Saída PWM do backlight |
| **Padrão** | 70% (~180) | Ótimo equilíbrio |
| **Máximo** | 100% (255) | Máximo brilho |
| **Mínimo** | 10% (26) | Evita queima |

### 🔄 Orientação e Layout

A rotação automática não é suportada nativamente pelo driver ILI9341. Use rotação de 0° (retrato) para melhor compatibilidade.

#### 📐 Layout da Tela

```
┌─────────────────────────────────────────────────────────────────┐
│                      TELA 320x240 (0°)                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    BARRA SUPERIOR                         │   │
│  │ [Bateria: 85%] [SD: 2.1GB] [WiFi: OFF] [Segurança: OK]  │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    ÁREA PRINCIPAL                       │   │
│  │                                                         │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │   │
│  │  │  Componente  │  │   Resultados │  │   Gráfico    │     │   │
│  │  │  Detectado  │  │   Medição    │  │   Tendência  │     │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘     │   │
│  │                                                         │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    BARRA INFERIOR                         │   │
│  │ [Testar] [Multímetro] [Config] [Histórico] [Ajuda]     │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 🎨 Temas e Cores

| Tema | Cor Principal | Cor Secundária | Contraste |
|:---|:---|:---|:---:|
| **Padrão** | Azul #007BFF | Branco #FFFFFF | ✅ Excelente |
| **Escuro** | Cinza #2C3E50 | Branco #ECF0F1 | ✅ Bom |
| **Alto Contraste** | Preto #000000 | Amarelo #FFD700 | ✅ Excelente |
| **Claro** | Azul Claro #5DADE2 | Preto #2C3E50 | ✅ Bom |

> [!NOTE]
> O brilho do display é controlado por PWM para máxima eficiência energética e vida útil do painel.

> [!TIP]
> Reduzir o brilho em 30% aumenta a vida útil do display em aproximadamente 50%.

> [!WARNING]
> Nunca mantenha o brilho em 100% por longos períodos para evitar queimaduras na tela.

---

## Calibração de Sensores

### Calibração ADC (Probe Principal)

O ajuste de **offset** corrige erros de medição em aberto:

1. Deixe os probes desconectados (circuito aberto)
2. Acesse **Ajustes → Calibração → ADC Offset**
3. Ajuste até que a leitura mostre **0.0** (ou OPEN)
4. Confirme a seleção

### Calibração ZMPT101B (True RMS AC)

> **🔴 PERIGO:** Este ajuste envolve tensão de rede. Desconecte antes de manipular!

O fator de escala corrige variações entre módulos ZMPT101B. O Sondvolt v3.2 utiliza amostragem de 128 pontos (True RMS).

1. Meça uma tensão de referência (tomada 127V ou 220V).
2. Compare com um multímetro de referência.
3. Acesse **Ajustes → Calibração → ZMPT Scale**.
4. Ajuste até igualar a leitura do multímetro.
5. Fórmula: `NovaEscala = LeituraMultímetro / LeituraCYD`.

**Parâmetros Técnicos:**
- **Samples**: 128 (Fixo em `config.h`).
- **Offset**: 1.65V (Centralizado via software).
- **Janela**: ~2 ciclos de 60Hz por medição.

### Calibração INA219 (Corrente DC)

O sensor INA219 é pré-calibrado, mas pode necessitar de ajuste:

| Parâmetro | Ajuste |
|:---|:---|
| **Shunt** | 0.1Ω (padrão) |
| **Calibração** | Automática na inicialização |

---

## Configurações de Áudio

### Modo Silencioso

| Opção | Descrição |
|:---|:---|
| **OFF** (Padrão) | Bipes ativos |
| **ON** | Silencioso (apenas alertas visuais) |

### Volume do Buzzer

- **GPIO:** 26 (DAC)
- **Frequência:** 1kHz a 4kHz (tom)

---

## Banco de Dados (COMPBD.CSV)

### Localização

O arquivo deve estar na **raiz** do cartão SD:

```
/COMPBD.CSV
```

### Formato do Arquivo

```
Tipo,hFE_Min,hFE_Max,Vf_Min,Vf_Max,PartNumber
NPN,100,200,0.6,0.7,BC547
NPN,200,400,0.6,0.7,BC548
PNP,100,200,0.6,0.7,BC557
DIODE,0.6,0.7,,,1N4148
LED_RED,1.8,2.0,,,LED_5MM_RED
```

### Campos

| Campo | Descrição | Exemplo |
|:---|:---|:---|
| Tipo | Tipo do componente | NPN, PNP, DIODE, LED |
| hFE_Min | Ganho mínimo | 100 |
| hFE_Max | Ganho máximo | 200 |
| Vf_Min | Tensão direta mínima | 0.6 |
| Vf_Max | Tensão direta máxima | 0.7 |
| PartNumber | Identificação | BC547 |

---

## Logging e Dados

### Arquivos no SD Card

| Arquivo | Descrição | Formato |
|:---|:---|:---|
| **COMPBD.CSV** | Banco de dados componentes | CSV |
| **LOG.TXT** | Histórico de medições | Texto |
| **CONFIG.DAT** | Configuraç��es (backup) | Binário |

### Formato do LOG.TXT

```
[2026-04-24 14:30:25] Auto-Detect: Resistor 10kΩ
[2026-04-24 14:31:10] Multímetro DC: V=5.02V I=0.45A
[2026-04-24 14:32:05] Temperatura: 25.3°C
```

---

## Reset de Fábrica

### Método 1: Menu de Configurações

1. Acesse **Ajustes → Limpar Tudo**
2. Confirme a ação
3. O dispositivo reiniciará

### Método 2: PlatformIO (Completo)

```bash
# Apagar flash antes de enviar
pio run -e cyd --target erase
pio run -e cyd --target upload
```

> ⚠️ Isso apaga TODAS as configurações e dados.

---

## Timeout e Economia de Energia

### Tempo de Espera

O display permanece ligado enquanto houver atividade. Para economia:

| Ajuste | Descrição |
|:---|:---|
| **Timeout** | Tempo para dimmer automático |
| **Sleep** | Modo de baixo consumo |

---

## Atualização do Firmware

### Via USB (UART)

```bash
pio run -e cyd --target upload
```

### Via OTA (Wireless)

```bash
pio device monitor --upload-port 192.168.1.100
```

---

## Avisos de Segurança

> **🔴 PERIGO:** A calibração do ZMPT101B envolve tensão de rede. Sempre desconecte antes de manipular!

> **⚠️ ATENÇÃO:** Use apenas ferramentas isoladas ao trabalhar com tensões superiores a 50V.

---

<p align="center">
<i>⚙️ Sondvolt v3.2 — Configurações</i>
</p>