# 🔌 Guia de Ligação de Componentes (Wiring Guide)

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

Este guia detalha a conexão de todos os componentes externos ao **Sondvolt v3.2** baseado na placa **ESP32-2432S028R (Cheap Yellow Display)**.

---

## 1. Visão Geral das Conexões

### Diagrama de Blocos

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                         ESP32-2432S028R (CYD)                           │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │
│  │   Display  │  │   Touch    │  │  SD Card   │  │  Speaker   │      │
│  │   TFT      │  │   Screen   │  │           │  │   (Buzzer) │      │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘      │
│         │                │                │                │              │
│    ┌────┴────┐     ┌────┴────┐     ┌────┴────┐     ┌────┴────┐        │
│    │  VSPI   │     │ SoftSPI │     │  HSPI   │     │   DAC   │        │
│    │ GPIO   │     │ GPIO    │     │ GPIO    │     │ GPIO 26 │        │
│    │15/2/14/ │     │33/25/32/│     │ 5/18/23│     │        │        │
│    │13/12   │     │39/36    │     │  /19    │     │        │        │
│    └─────────┘     └─────────┘     └─────────┘     └─────────┘        │
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐   │
│  │                      BARRAMENTOS DE EXPANSÃO                    │   │
│  │                                                                │   │
│  │   CN1 (Analógico)     P3/J3 (Digital)      питаção               │   │
│  │   ┌────┬────┐       ┌────┬────┬────┐      ┌────┬────┬────┐    │   │
│  │   │ 35 │ 34 │       │ GND│ 27 │ 22 │ 4  │   │ GND│ 5V │3.3V│    │   │
│  │   │    │    │       │    │ SDA│ SCL│ OW │    │    │    │    │    │   │
│  │   └────┴────┘       └────┴────┴────┘      └────┴────┴────┘    │   │
│  └───────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────┘

         │                  │                  │                  │
         ▼                  ▼                  ▼                  ▼
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│   ZMPT101B      │ │     INA219      │ │    DS18B20      │ │    Probes       │
│  (Sensor AC)    │ │ (Medidor DC)    │ │  (Temperatura)  │ │  (Teste Compon) │
│                 │ │                 │ │                 │ │                 │
│  OUT → GPIO 36  │ │  SDA → GPIO 27  │ │  DQ → GPIO 4    │ │  P1 → GPIO 35   │
│  VCC → 5V       │ │  SCL → GPIO 22  │ │  VCC → 3.3V    │ │  P2 → GPIO 34   │
│  GND → GND       │ │  VCC → 3.3V    │ │  GND → GND      │ │  GND → GND      │
│                 │ │  GND → GND      │ │                 │ │                 │
└─────────────────┘ └─────────────────┘ └─────────────────┘ └─────────────────┘
```

---

## 2. Conectores da Placa

### 2.1 CN1 — Conector Analógico (Probes)

Conector de 4 pinos localizado na lateral esquerda da placa.

| Pino | Função | GPIO | Descrição |
|:---:|:---:|:---:|:---|
| 1 | **Probe 1** | GPIO 35 | Entrada analógica principal (Port IO1) |
| 2 | **Probe 2 / ZMPT** | GPIO 36 | Entrada sensor AC (Shared with Touch IRQ) |
| 3 | **GND** | — | Terra comum |
| 4 | **5V / VIN** | — | Entrada de alimentação |

### 2.2 P3 / J3 — Conector Digital (I2C + OneWire)

Conector de 4 pinos para módulos digitais.

| Pino | Função | GPIO | Descrição |
|:---:|:---:|:---:|:---|
| 1 | **GND** | — | Terra comum |
| 2 | **I2C SDA** | GPIO 27 | Dados I2C (INA219) |
| 3 | **I2C SCL** | GPIO 22 | Clock I2C (INA219) |
| 4 | **OneWire DQ** | GPIO 4 | Dados OneWire (DS18B20) |

> [!INFO]
> O conector P3/J3 utiliza um plugue JST PH de 4 pinos (pitch 2.0mm).

---

## 3. Ligação dos Módulos

### 3.1 ZMPT101B — Sensor de Tensão AC (COM PROTEÇÃO)

Módulo transformador para medição de tensão alternada. O circuito abaixo é **obrigatório** para segurança em 220V.

#### Esquema de Ligação Protegido

```text
ENTRADA AC (220V)                                MÓDULO ZMPT101B
─────────────────                                ───────────────
                                            ┌───────────┐
FASE (L) ───[ FUSÍVEL 5A ]────┬─────────────┤ L         │
                              │             │           │
                              ▼             │           │
                          [VARISTOR]        │           │
                          [ 14D431 ]        │           │
                              ▲             │           │
                              │             │           │
NEUTRO (N) ───────────────────┴─────────────┤ N         │
                                            └───────────┘

                                                 SAÍDA DC
                                            ┌───────────┐
      ESP32 CYD (5V) <──────────────────────┤ VCC       │
                                            │           │
      ESP32 CYD (GND) <────────┬────────────┤ GND       │
                               │            │           │
                               │            │           │
      ESP32 CYD (GPIO 36) <────┼──┬─────────┤ OUT       │
                               │  │         └───────────┘
                               │  │
                        [CAP 100nF] [RES 10kΩ]
                               │  │
      GND <────────────────────┴──┴─────────────────────
```

> [!CAUTION]
> **PERIGO DE CHOQUE ELÉTRICO**: Nunca manipule o circuito de entrada (L/N) enquanto o equipamento estiver conectado à tomada. O Fusível e o Varistor devem ser acomodados em suporte isolado.

#### Esquema de Ligação

```text
ZMPT101B                    ESP32-2432S028R
────────                    ─────────────────
  VCC ────────────────────────► 5V (VIN)
  GND ────────────────────────► GND
  OUT ────────────────────────► GPIO 36 (CN1 pino 2)
                             ──► Resistor 10kΩ (pull-down)
                                 │
                                 ▼
                               GND
```

#### Especificações

| Parâmetro | Valor |
|:---|:---|
| Tensão de entrada | 0 - 250V AC |
| Tensão de saída | 0 - 3.3V AC (offset 1.65V) |
| Frequência de operação | 50-60Hz |
| Alimentação | 5V DC |

#### Cabo Recomendado

- Use Cabo Dupont macho-fêmea de 20cm
- Fio vermelho: VCC → 5V
- Fio preto: GND → GND
- Fio amarelo: OUT → GPIO 36

> [!WARNING]
> **Isolamento:** O ZMPT101B fornece isolação galvânica, mas manuseie com cuidado durante medições em alta tensão. Sempre desconecte a alimentação antes de manipular as conexões.

---

### 3.2 INA219 — Medidor de Tensão/Corrente DC

Sensor I2C para medição de tensão (0-26V), corrente (até 3.2A) e potência.

#### Esquema de Ligação

```text
INA219                       ESP32-2432S028R
──────                       ─────────────────
  VCC ────────────────────────► 3.3V
  GND ────────────────────────► GND
  SDA ────────────────────────► GPIO 27 (P3 pino 2)
  SCL ────────────────────────► GPIO 22 (P3 pino 3)
  A+ (V+) ───────────────────► (+) Carga/Bateria
  A- (V-) ───────────────────► (-) Carga/Bateria
```

#### Diagrama de Conexão em Série

```text
         ┌─────────────────────────────────────┐
         │           FONTE/BATERIA              │
         │           (+)    (-)               │
         └──────────┬────────────────────────┘
                    │                         │
                    ▼                         │
              ┌─────────┐                    │
              │  INA219 │◄────────────┐       │
              │ A+   A- │             │       │
              └───┬�─────┘             │       │
                  │                   │       │
                  │   ┌───────────────┘       │
                  │   │                       │
                  ▼   ▼                    ┌──┘
              ┌─────────┐                  │
              │ CARGA/  │◄──────────────────┘
              │ DISPO-  │
              │ Sitivo  │
              └─────────┘
```

#### Especificações

| Parâmetro | Valor |
|:---|:---|
| Tensão máxima (Vbus) | 26V DC |
| corrente máxima | 3.2A ( shunt 0.1Ω ) |
| Resolução | 12-bit (4mA) |
| Endereço I2C | 0x40 |
|shunt Resistor | 0.1Ω (1W) integrado |

#### Cabo Recomendado

- Use Cabo JST PH de 4 vias ou Dupont
- Configure Pull-ups internos: ativados no firmware

> [!INFO]
> O INA219 utiliza endereçamento I2C fixo em **0x40**. Não conecte outros dispositivos no barramento I2C com mesmo endereço.

---

### 3.3 DS18B20 — Sonda Térmica Digital

Sensor de temperatura OneWire à prova d'água.

#### Esquema de Ligação

```text
DS18B20                     ESP32-2432S028R
───────                     ─────────────────
  VCC (Vermelho) ────────────► 3.3V
  GND (Preto)   ────────────► GND
  DQ (Amarelo/ ────────────► GPIO 4 (P3 pino 4)
  Branco)                │
                        │
                        ▼ Resistor 4.7kΩ pull-up
                        │
                        ▼
                       3.3V
```

#### Especificações

| Parâmetro | Valor |
|:---|:---|
| Faixa de temperatura | -55°C a +125°C |
| Precisão | ±0.5°C (-10°C a +85°C) |
| Resolução | 9-12 bit (configurável) |
| Protocolo | Dallas OneWire |

#### Cabo Recomendado

- Cabo de 3 vias com connectors JST ou Dupont
- Comprimento máximo: até 100 metros (cabos torcidos)
- Importante: resistor pull-up de 4.7kΩ entre DQ e 3.3V

> [!WARNING]
> O GPIO 4 é **compartilhado** com o LED azul integrado da CYD. Durante comunicação OneWire, o LED pode apresentar comportamentos inesperados (piscar/aceso). Isso é normal e não afeta a leitura.

---

## 4. Sistema de Probes

### 4.1 Probe de Teste de Componentes

Sistema de pontas de prova para teste de resistores, capacitores, diodos, transistores, etc.

#### Conexão padrão

```text
Probes                       ESP32-2432S028R
───────                      ─────────────────
  Probe Vermelho (Sinal) ────► GPIO 35 (CN1 pino 1)
  Probe Preto (GND)    ────► GND (CN1 pino 3)
```

#### Configuração de Teste

| Componente | Probe Vermelho | Probe Preto |
|:---|:---:|:---:|
| Resistor | Uma ponta | Outra ponta |
| Capacitor | Pino + | Pino - |
| Diodo | Ânodo (+) | Cátodo (-) |
| LED | Ânodo (+) | Cátodo (-) |
| Transistor (NPN) | Coletor | Emissor |

> [!IMPORTANT]
> **Descarge capacitores** antes de testar! Capacitores eletrolíticos armazename energia dangerous. Use uma resistor de descarga ou espere tempo suficiente.

---

## 5. Botões de Controle

### 5.1 Botões Integrados (se instalados)

A CYD possui 2 botões de usuário (GPIO 0 e GPIO 35) acessíveis via furos na placa.

| Botão | GPIO | Função |
|:---:|:---:|:---|
| BOOT | GPIO 0 | Boot / Reset |
| GPIO 35 | GPIO 35 | Button (definido pelo usuário) |

### 5.2 Botões Externos (opcional)

Conecte botões tactis externos ao GPIO appropriate com resistor pull-down de 10kΩ.

```text
Botão externo              ESP32-2432S028R
────────────              ─────────────────
  Pino 1 ─────────────────► GPIO (ex: 13)
  Pino 2 ─────────────────► GND
  
  ┌─────────────────────────────────────┐
  │           Resistor 10kΩ              │
  │           (pull-down)               │
  └─────────────────────────────────────┘
```

---

## 6. Buzzer / Speaker

### 6.1 Buzzer Integrado

A CYD possui um buzzer/speaker conectado ao GPIO 26.

| Componente | GPIO | Função |
|:---|:---:|:---|
| Speaker | GPIO 26 | Áudio e alertas sonoros |

> [!INFO]
> O GPIO 26 também pode ser usado como **saída PWM** para gerador de sinais. Desconecte o speaker se necessário.

---

## 7. LED RGB Integrado

### 7.1 LED de Status

A CYD possui um LED RGB (na verdade, 2 LEDs: verde e vermelho — azul é opcional).

| Cor | GPIO | Observação |
|:---:|:---:|:---|
| Verde | GPIO 16 | LED verde |
| Vermelho | GPIO 17 | LED vermelho |
| Azul | GPIO 4 | Compartilhado com OneWire (ver nota) |

> [!WARNING]
> O **LED azul** compartilha o GPIO 4 com barramento OneWire da DS18B20. Ao usar a sonda, o LED azul pode apresentar comportamento inesperado.

---

## 8. Tabela Resumo de Conexões

### Resumo Geral

| Módulo | Pino Signal | Pino Alimentação | Pino Terra | Conector |
|:---|:---:|:---:|:---:|:---|
| **ZMPT101B** | GPIO 34 | 5V | GND | CN1 |
| **INA219** | GPIO 27 (SDA) / 22 (SCL) | 3.3V | GND | P3 |
| **DS18B20** | GPIO 4 | 3.3V | GND | P3 |
| **Probe Principal** | GPIO 35 | — | GND | CN1 |
| **Buzzer/Speaker** | GPIO 26 | — | GND | — |
| **LED RGB** | GPIO 16 / 17 | — | GND | — |

---

## 9. Imagens de Referência

### 9.1 Diagrama de Ligação (Visão Superior)

Adicione aqui uma imagem de diagrama de ligação visãoglobal.

```
! (images/wiring_overview.png)
*[Adicione uma foto/diagrama mostrando todos os módulos conectados]
```

### 9.2 Detalhe do Conector CN1

```
! (images/wiring_cn1_detail.png)
*[Detalhe do conector CN1 com ZMPT e Probe conectados]
```

### 9.3 Detalhe do Conector P3

```
! (images/wiring_p3_detail.png)
*[Detalhe do conector P3 com INA219 e DS18B20 conectados]
```

### 9.4 Foto da Montagem Completa

```
! (images/wiring_full_setup.png)
*[Foto da montagem completa com todos os módulos]
```

---

## 10. Checklist de Conexões

Antes de ligá-lo, verifique cada conexão:

- [ ] ZMPT101B: VCC → 5V, GND → GND, OUT → GPIO 36
- [ ] INA219: VCC → 3.3V, GND → GND, SDA → 27, SCL → 22
- [ ] DS18B20: VCC → 3.3V, GND → GND, DQ → GPIO 4 + resistor 4.7kΩ
- [ ] Probes: P1 → GPIO 35, P2 → GPIO 34
- [ ] Resistor pull-down 10kΩ no GPIO 36 (ZMPT)
- [ ] Verificar ausência de curto entre 5V e GND

> [!DANGER]
> **Nunca** inverta as conexões de alimentação! 5V no pino 3.3V pode danificar permanentemente a placa.

---

## 11. Troubleshooting de Conexões

| Problema | Causa Provável | Solução |
|:---|:---|:---|
| ZMPT não lê tensão | Pull-down ausente | Adicione resistor 10kΩ entre GPIO 36 e GND |
| INA219 não responding | Endereço I2C conflito | Verifique endereços no barramento |
| DS18B20 não detecta | Pull-up ausente | Adicione resistor 4.7kΩ entre DQ e 3.3V |
| Leituras errôneas | Gnds não comuns | Use GND único para todas as referências |
| Display não inicia | Bot BOOT pressionado | Não segure BOOT ao ligar |

---

_Fim do Guia de Ligação_