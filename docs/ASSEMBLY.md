# 📦 Guia de Montagem Completa

Este guia passo-a-passo detalha como montar o **Sondvolt** utilizando a placa **ESP32-2432S028R (Cheap Yellow Display)**.

---

## 1. Visão Geral da Montagem

### 1.1 Componentes Necessários

#### Placa Base
| Componente | Quantidade | Observação |
|:---|:---:|:---|
| ESP32-2432S028R (CYD) | 1 | Placa principal com display touchscreen |
| Cabo USB-C | 1 | Alimentação e programação |
| Cartão microSD | 1 | Opcional, para dados/logs |

#### Módulos de Medição
| Componente | Quantidade | Modelo Recomendado |
|:---|:---:|:---|
| Sensor tensão AC | 1 | ZMPT101B (transformador de corrente) |
| Medidor DC | 1 | INA219 (I2C) |
| Sonda térmica | 1 | DS18B20 à prova d'água |
| Resistor pull-down | 1 | 10kΩ (para ZMPT) |
| Resistor pull-up | 1 | 4.7kΩ (para DS18B20) |

#### Sistema de Probes
| Componente | Quantidade | Observação |
|:---|:---:|:---|
| Ponta de prova vermelha | 1 | Sinal/Positivo |
| Ponta de prova preta | 1 | GND/Negativo |
| Caixa de probes | 1 | Opcional (kit completo) |

#### Cabos e Conectores
| Componente | Quantidade | Especificação |
|:---|:---:|:---|
| Cabo Dupont M-F | 10 | 20cm para ZMPT |
| Cabo JST PH 4 vias | 1 | Para conectores P3 |
| Fio de conexão | 20cm | Cores variadas |
| Termo-retrátil | 10 | Isolamento |

#### Proteção AC (Obrigatório p/ 220V)
| Componente | Quantidade | Modelo |
|:---|:---:|:---|
| Fusível Rápido | 1 | 5A (com porta-fusível) |
| Varistor | 1 | 14D431 ou 10D431 |
| Diodo TVS | 1 | P6KE400A ou P6KE440A |
| Capacitor Cerâmico | 1 | 100nF (Filtro Saída) |
| Capacitor Eletro. | 1 | 10µF (Filtro Fonte) |
| Resistor de Carga | 1 | 10kΩ (Estabilização) |

---

## 2. Ferramentas Necessárias

### 2.1 Ferramentas Básicas
- 🔧 Ferro de solda (25-40W)
- 🔧 Support de solda / "terceira mão"
- 🔧 Corta-fio / alicate de corte
- 🔧 Decapante de solda
- 🔧 Multímetro (para verificação)

### 2.2 Ferramentas Opcionais
- 🔧 Pasta de solda (fluxo)
- 🔧 Sugador de solda
- 🔧 Osciloscópio (avançado)
- 🔧 Fonte variáveis (teste)

---

## 3. Preparação da Placa

### 3.1 Limpeza Inicial

> [!WARNING]
> Antes de soldar, limpe a placa com-isopropyl alcohol (IPA) para remover resíduos de verificação.

1. **Inspecione a placa** — Verifique se não há danos visíveis
2. **Limpe os pads** — Use IPA e cotton swabs
3. **Verifique soldas** — As já应用adas devem estar brilhante e sem bolhas

### 3.2 Teste da Placa (Pré-montagem)

```
! (images/assembly_pretest.png)
*[Foto da CYD limpa e pronta para montagem]*
```

1. Conecte o cabo USB-C
2. O display deve ascender mostrando a tela inicial
3. O touchscreen deve responder ao toque
4. Se tudo OK, prossiga para próxima etapa

---

## 4. Instalação dos Pull-resistors

### 4.1 Resistor Pull-down (GPIO 34)

**Função:** Mantém o pino do ZMPT em nível baixo quando não há sinal.

```
Instalação:
┌─────────────────────────────────────────────────┐
│                                                 │
│    GPIO 34 ────────●─────── GND                 │
│                    │                             │
│                   10kΩ                          │
│                                                 │
└─────────────────────────────────────────────────┘
```

**Passo-a-passo:**

| Passo | Ação | Imagem |
|:---:|:---|:---|
| 1 | Curve o resistor de 10kΩ para formato de "U" | ! (images/assembly_resistor_step1.png) |
| 2 | Posicione no furo do GPIO 34 | ! (images/assembly_resistor_step2.png) |
| 3 | Solde um terminal ao GPIO 34 | ! (images/assembly_resistor_step3.png) |
| 4 | Solde o outro terminal ao GND | ! (images/assembly_resistor_step4.png) |
| 5 | Corte o excesso de perna | ! (images/assembly_resistor_step5.png) |

### 4.2 Resistor Pull-up (GPIO 4 - OneWire)

**Função:** Necessário para comunicação OneWire com DS18B20.

```
Instalação:
┌─────────────────────────────────────────────────┐
│                                                 │
│    GPIO 4 ────────●─────── 3.3V                  │
│                    │                             │
│                   4.7kΩ                          │
│                                                 │
└─────────────────────────────────────────────────┘
```

| Passo | Ação | Imagem |
|:---:|:---:|:---|
| 1 | Prepare resistor 4.7kΩ | ! (images/assembly_pullup_step1.png) |
| 2 | Solde entre GPIO 4 e 3.3V | ! (images/assembly_pullup_step2.png) |
| 3 | Verifique com multímetro | ! (images/assembly_pullup_step3.png) |

> [!INFO]
> Alguns módulos DS18B20 jás incluem resistor pull-up interno. Verifique o datasheet antes de adicionar.

---

## 5. Circuito de Proteção AC (220V)

### 5.1 Esquema de Proteção de Entrada

Este circuito deve ser montado **antes** do sensor ZMPT101B para garantir que transientes não destruam o transformador ou a ESP32.

```
LINHA (AC) ────[ FUSÍVEL 5A ]────┬───────┬─────── (ZMPT IN 1)
                                 │       │
                              [VARISTOR] [TVS]
                                 │       │
NEUTRO (AC) ─────────────────────┴───────┴─────── (ZMPT IN 2)
```

### 5.2 Filtragem de Saída (ZMPT101B)

Adicione os capacitores diretamente nos pinos do módulo ZMPT:

1. **Capacitor 100nF**: Em paralelo com a saída `OUT` e `GND`.
2. **Capacitor 10µF**: Entre `VCC` e `GND` do módulo.
3. **Resistor 10kΩ**: Entre `OUT` e `GND` (Carga estável).

| Passo | Ação | Finalidade |
|:---:|:---|:---|
| 1 | Solde o 100nF entre OUT e GND | Filtro de ruído de alta frequência |
| 2 | Solde o 10µF entre VCC e GND | Estabilização da alimentação |
| 3 | Solde o 10kΩ em paralelo com a saída | Impedância de carga correta |

---

## 6. Conexão do ZMPT101B

### 5.1 Localização dos Pinos

O ZMPT101B possui 4 pinos:
- **VCC** — Alimentação 5V
- **GND** — Terra
- **OUT** — Saída analógica
- **NC** — Não conectado (ou VREF)

```
ZMPT101B (Vista Superior)
┌─────────────────────────┐
│  VCC  │  GND  │  OUT  │ NC │
│   ●   │   ●   │   ●   │ ●  │
│ RED   │ BLACK │ YELLOW│-  │
└─────────────────────────┘
```

### 5.2 Esquema de Conexão

```
┌────────────────┐         ┌─────────────────────┐
│    ZMPT101B     │         │   ESP32-2432S028R    │
├────────────────┤         ├─────────────────────┤
│ VCC   ─────────┼─────────│ 5V (VIN)            │
│ GND   ─────────┼─────────│ GND                 │
│ OUT   ─────────┼─────────│ GPIO 34 (CN1 pino 2) │
│ NC    ─────────┤         │                     │
└────────────────┘         └─────────────────────┘

Detalhe do resistor pull-down:
GPIO 34 ──┬────●──── GND
          │   10kΩ
          └──┘
```

### 5.3 Passo-a-Passo de Conexão

| Passo | Ação | Imagem |
|:---:|:---|:---|
| 1 | Conecte fio vermelho (VCC → 5V) | ! (images/assembly_zmpt_step1.png) |
| 2 | Conecte fio preto (GND → GND) | ! (images/assembly_zmpt_step2.png) |
| 3 | Conecte fio amarelo (OUT → GPIO 34) | ! (images/assembly_zmpt_step3.png) |
| 4 | Verifique conexões com multímetro | ! (images/assembly_zmpt_step4.png) |

> [!CAUTION]
> **Verifique polaridade:** O ZMPT101B pode ser danificado se conectado com polaridade invertida. Sempre teste continuidade antes de energizar.

---

## 6. Conexão do INA219

### 6.1 Localização dos Pinos

```
INA219 (Vista Superior)
┌──────────────────────────────────────┐
│  VCC  │  GND  │  SDA  │  SCL  │  A+  │
│ PWR   │ GND   │  I2C  │  I2C  │ Bus │
├──────┼───────┼───────┼───────┼─────┤
│  ●   │   ●   │   ●   │   ●   │  ●  │
└──────────────────────────────────────┘

Pinout:
• VCC = 3.3V
• GND = Terra
• SDA = Dados I2C
• SCL = Clock I2C
• A+ = V+ (Medição tensão positiva)
• A- = V- (Medição tensão negativa)
```

### 6.2 Conexão I2C

```
┌────────────────┐         ┌─────────────────────┐
│     INA219     │         │   ESP32-2432S028R    │
├────────────────┤         ├─────────────────────┤
│ VCC   ─────────┼─────────│ 3.3V                │
│ GND   ─────────┼─────────│ GND                 │
│ SDA   ─────────┼─────────│ GPIO 27 (P3 pino 2) │
│ SCL   ─────────┼─────────│ GPIO 22 (P3 pino 3)  │
└────────────────┘         └─────────────────────┘
```

### 6.3 Conexão de Medição (Barra positiva)

```
        FONTE/BATERIA (+)
              │
              ├────────────────┐
              │                │
              ▼                │
        ┌──────────┐           │
        │  A+ (V+)  │◄──────────┤
        │          │           │
        │  INA219  │           │
        │          │           │
        │  A- (V-) │◄──────────┤
        └──────────┘           │
              │               │
              │               │
              ▼               │
        ┌──────────┐           │
        │   CARGA   │◄──────────┘
        │  (+)     │           │
        └──────────┘           │
              │               │
              └───────────────┘
              (retorno comum)
              │
        FONTE/BATERIA (-)
```

### 6.4 Passo-a-Passo

| Passo | Ação | Imagem |
|:---:|:---|:---|
| 1 | Conecte VCC ao 3.3V | ! (images/assembly_ina_step1.png) |
| 2 | Conecte GND | ! (images/assembly_ina_step2.png) |
| 3 | Conecte SDA → GPIO 27 | ! (images/assembly_ina_step3.png) |
| 4 | Conecte SCL → GPIO 22 | ! (images/assembly_ina_step4.png) |
| 5 | Conecte A+ à alimentação positiva | ! (images/assembly_ina_step5.png) |
| 6 | Conecte A- à carga/dispositivo | ! (images/assembly_ina_step6.png) |

---

## 7. Conexão da DS18B20

### 7.1 Identificação dos Fios

```
DS18B20 (Cabo à prova d'água)
┌─────────────────────────────────┐
│                                 │
│   Vermelho  ────── VCC (3.3V)     │
│   Preto    ────── GND            │
│   Amarelo  ────── DQ (Dados)      │
│   Branco  ────── DQ (Dados)      │
│                                 │
└─────────────────────────────────┘

Atenção:Cores podem variar por fabricante!
Consulte o datasheet do seu módulo.
```

### 7.2 Conexão

```
┌────────────────┐         ┌─────────────────────┐
│    DS18B20      │         │   ESP32-2432S028R    │
├────────────────┤         ├─────────────────────┤
│ VCC  ──────────┼─────────│ 3.3V                │
│ GND  ──────────┼─────────│ GND                 │
│ DQ   ──────────┼─────────│ GPIO 4 (P3 pino 4)  │
│                │         │    + Pull-up 4.7kΩ  │
└────────────────┘         └─────────────────────┘
```

**Detalhe do Pull-up:**
```
3.3V ──┬─ Pull-up 4.7kΩ ── GPIO 4 ── DQ (DS18B20)
       │
       GND
```

### 7.3 Passo-a-Passo

| Passo | Ação | Imagem |
|:---:|:---:|:---|
| 1 | Conecte fio vermelho (VCC → 3.3V) | ! (images/assembly_ds18_step1.png) |
| 2 | Conecte fio preto (GND) | ! (images/assembly_ds18_step2.png) |
| 3 | Conecte fio dados (DQ → GPIO 4) | ! (images/assembly_ds18_step3.png) |
| 4 | Instale resistor pull-up | ! (images/assembly_ds18_step4.png) |
| 5 | Aplique termo-retrátil | ! (images/assembly_ds18_step5.png) |

---

## 8. Sistema de Probes

### 8.1 Preparação dos Cabos

```
Probe Simples (Kit DIY)
┌─────────────────────────────────────────┐
│                                           │
│  ┌─────────┐    Cabo    ┌──────────────┐    │
│  │  Cabo   │───────────│  Ponta probe │    │
│  │  20cm   │  Vermelho │   Vermelha  │    │
│  └─────────┘           └──────────────┘    │
│                                           │
│  ┌─────────┐    Cabo    ┌──────────────┐    │
│  │  Cabo   │───────────│  Ponta probe │    │
│  │  20cm   │  Preto   │    Preta     │    │
│  └─────────┘           └──────────────┘    │
│                                           │
└───────────────────────────────────────────┘
```

### 8.2 Conexão dos Probes

| Step | Action | Image |
|:---:|:---|:---|
| 1 | Conecte probe vermelho ao GPIO 35 | ! (images/assembly_probe_step1.png) |
| 2 | Conecte probe preto ao GND | ! (images/assembly_probe_step2.png) |
| 3 | Fixe os cabos com fita | ! (images/assembly_probe_step3.png) |
| 4 |Organize com presilhas | ! (images/assembly_probe_step4.png) |

### 8.3 Diagrama Final dos Probes

```
┌─────────────────────────────────────────────────┐
│                                                 │
│   Probe Vermelho (Sinal)  ──────────────────┐   │
│                                          │   │
│                                          ▼   │
│                                   ┌─────────┐  │
│                                   │ GPIO 35 │  │
│                                   └─────────┘  │
│                                                 │
│   Probe Preto (GND)      ──────────────────┐   │
│                                          │   │
│                                          ▼   │
│                                   ┌─────────┐  │
│                                   │   GND   │  │
│                                   └─────────┘  │
│                                                 │
└─────────────────────────────────────────────────┘
```

---

## 9. Botões (Opcional)

### 9.1 Botão de Reset Externo

```
┌────────────────┐         ┌─────────────────────┐
│   Botão Tact   │         │   ESP32-2432S028R   │
├────────────────┤         ├─────────────────────┤
│ Pino 1 ────────┼─────────│ GPIO 0 (para BOOT)  │
│ Pino 2 ────────┼─────────│ GND                 │
│                │         │     + Pull-up 10kΩ │
└────────────────┘         └─────────────────────┘
```

### 9.2 Botões de Função (Adicionais)

| Botão | GPIO | Função |
|:---:|:---:|:---|
| BOOT | GPIO 0 | Reset / Bootloader |
| FUN1 | GPIO 13 | Definido pelo usuário |
| FUN2 | GPIO 14 | Definido pelo usuário |

---

## 10. Organização dos Cabos

### 10.1 Sugestão de Layout

```
! (images/assembly_final_layout.png)
*[Foto do layout Sugerido dos componentes]*

Legenda:
┌───────────────────────────────────────────┐
│  1. CYD (Central)                        │
│  2. ZMPT101B (Esquerda)                  │
│  3. INA219 (Direita)                     │
│  4. DS18B20 (Frente/Atrás)               │
│  5. Probes (Acesso fácil)               │
│  6. Cabos organizados com velcro         │
└───────────────────────────────────────────┘
```

### 10.2 Dicas de Organização

1. **Use presilhas** para agrupar cabos por função
2. **Etiquete os cabos** com fita ou caneta permanente
3. **Mantenha os probes** acessíveis para troca rápida
4. **Guarde os módulos** em local ventilado

---

## 11. Verificação Final

### 11.1 Checklist de Montagem

```
┌──────────────────────────────────────────────────┐
│              VERIFICAÇÃO PRÉ-ENERGIA               │
├───────────────────────────��─��────────────────────┤
│                                                  │
│ [ ] Todos os resistores pull/pull-up soldados     │
│ [ ] ZMPT101B conectado: VCC→5V, GND→GND,        │
│     OUT→GPIO34                                    │
│ [ ] INA219 conectado: VCC→3.3V, SDA→27, SCL→22  │
│ [ ] DS18B20 conectado: VCC→3.3V, DQ→GPIO4       │
│ [ ] Probes conectados: SINAL→GPIO35, GND→GND    │
│ [ ] Sem.curtocircuitos visíveismontagem          │
│ [ ] Cabos organizados e presos                  │
│ [ ] Placa limpa sem resíduos de solda          │
│                                                  │
└──────────────────────────────────────────────────┘
```

### 11.2 Teste Inicial

| Passo | Ação | Resultado Esperado |
|:---:|:---|:---|
| 1 | Conecte USB-C | Display acende |
| 2 | Toque na tela | Touch responde |
| 3 | Acesse menu ZMPT | Leitura ~0V (sem tensão) |
| 4 | Acesse menu INA219 | Leitura ~0A (sem carga) |
| 5 | Acesse menu DS18B20 | Temperatura ambiente |

---

## 12. Imagens de Referência

### 12.1 Montagem Completa (Vista Superior)

```
! (images/assembly_full_top.png)
*[Foto aérea da montagem completa]*
```

### 12.2 Detalhe das Conexões

```
! (images/assembly_connections_detail.png)
*[Close-up das conexões solderingadas]*
```

### 12.3 Case / Gabinete (Opcional)

```
! (images/assembly_enclosure.png)
*[Foto com case/gavernecimento impresso em 3D]*
```

---

## 13. Troubleshooting de Montagem

| Problema | Causa | Solução |
|:---|:---|:---|
| Display não acende | Cabo USB solto | Verifique conexão USB-C |
| Touch não responde |touchscreen não calibrado | Recalice no menu |
| ZMPT lê valor errado | Pull-down отсутствует | Adicione resistor 10kΩ |
| INA219 não detectado | Endereço I2C conflito | Verifique conexões SDA/SCL |
| DS18B20 não detectado | Pull-up ausente | Adicione resistor 4.7kΩ |
| Leituras erráticas | GNDs não conectados | Verifique terra comum |
| Probes não funcionam | GPIO trocado | Verifique GPIO 35 |

---

## 14. Tempo Estimado de Montagem

| Etapa | Tempo |
|:---|:---|
| Preparação e limpeza | 15 min |
| Pull-resistores | 10 min |
| ZMPT101B | 10 min |
| INA219 | 15 min |
| DS18B20 | 10 min |
| Probes | 10 min |
| Verificação | 15 min |
| **Total** | **~1h 25min** |

---

_Fim do Guia de Montagem_

---

> [!TIP]
> Para assistênciavideo completa, assista ao tutorial de montagem no canal: [LINK]
> 
> Para suporte adicional, consulte o FAQ em `docs/FAQ.md`.