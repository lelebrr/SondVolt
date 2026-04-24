# Manual do Usuário — Sondvolt v3.1

## Component Tester Pro

**Versão do Firmware:** 3.1.0
**Placa Base:** ESP32-2432S028R (Cheap Yellow Display)
**Ano de Fabricação:** 2025-2026

---

Este documento contém todas as instruções necessárias para operar o **Sondvolt** de forma segura, eficiente e profissional. Leia este manual completamente antes de operar o equipamento.

---

## 1. Introdução

### 1.1 O que é o Sondvolt

O **Sondvolt** é um instrumento de medição eletrônica multifuncional de última geração, desenvolvido sobre a plataforma ESP32-2432S028R (Cheap Yellow Display). Este equipamento combina as funcionalidades de um testador de componentes eletrônico e um multímetro digital em um único dispositivo compacto, com interface gráfica colorida touchscreen de 2,8 polegadas.

O Sondvolt foi projetado para atender engenheiros, técnicos eletrônicos, estudantes de eletrônica e entusiastas de tecnologia que precisam de uma ferramenta versátil para diagnóstico, teste e medição de componentes e circuitos eletrônicos. O dispositivo oferece uma solução tudo-em-um para oficinas de eletrônica, laboratórios de ensino e aplicações de campo.

### 1.2 Principais Funcionalidades

O Sondvolt oferece as seguintes funcionalidades principais:

- **Auto-Detecção Inteligente:** Identificação automática de mais de 8 tipos diferentes de componentes eletrônicos, incluindo resistores, capacitores, diodos, transistores, LEDs, MOSFETs, indutores e componentes未知 (desconhecidos). O sistema analisa as características elétricas do componente e determina o tipo automaticamente, sem necessidade de seleção manual de modo.

- **Modo Teste de Componentes:** Teste específico para cada tipo de componente com parâmetros detalhados. Para resistores, mede o valor em ohms com compensação de tolerância. Para capacitores, mede capacitância em pF/nF/µF/mF e ESR (Equivalent Series Resistance). Para diodos, mede tensão direta (Vf) e polaridade. Para transistores, determina tipo (NPN/PNP), ganho hFE e identificação dos terminais.

- **Modo Multímetro AC/DC:** Medição de tensão alternada (AC) e tensão contínua (DC) com True RMS, utilizando sensores dedicados (ZMPT101B para AC, ADC interno para DC). O modo DC também inclui medição de corrente através do sensor INA219, com cálculo automático de potência em watts.

- **Modo Temperatura:** Suporte para sonda térmica DS18B20 one-wire, permitindo medições de temperatura ambiente ou de componentes específicos durante operação.

- **Modo Histórico e Estatísticas:** Armazenamento de todas as medições em cartão SD, permitindo consulta posterior e análise estatística de componentes testados.

- **Sistema de Segurança Integrado:** Proteção automática contra tensões perigosas (220V AC), com detecção em tempo real, alertas sonoros e visuais, e bloqueio temporário do equipamento quando tensões perigosas são detectadas.

- **Interface Gráfica Touchscreen:** Display TFT colorido de 320x240 pixels com interface intuitiva baseada em ícones, botões de navegação e feedback visual imediato através de LEDs e buzzer.

- **Banco de Dados de Componentes:** Arquivo COMPBD.CSV no cartão SD para identificação automática de componentes porpart number, permitindo reconhecimento de transistores, diodos e outros componentes populares.

### 1.3 Vantagens sobre Multímetros Comuns

O Medivolt apresenta diversas vantagens em relação multímetros digitais convencionais:

| Característica | Multímetro Comum | Medivolt |
|:---|:---:|:---:|
| **Tipos de medição** | 2-3 (tensão, corrente, resistência) | 8+ (resistência, capacitância, diodo, transistor, temperatura, AC, DC, corrente) |
| **Auto-detecção** | Não | Sim (identificação automática de componentes) |
| **Interface** | Display de 7 segmentos ou LCD básico | Display TFT colorido touchscreen de 3,2" |
| **Armazenamento** | Geralmente não | Cartão SD com histórico completo |
| **Segurança AC** | básica (fusível) | Detecção automática com alerta e bloqueio |
| **Identificação de componentes** | Não | Sim (banco de dados CSV) |
| **Teste de transistores** | Limitado ou nenhum | Completo (tipo, hFE, pinagem) |
| **Teste de capacitores** | Limitado | Completo (capacitância + ESR) |
| **Custo-benefício** | Medio-alto | Excelente (múltiplas funções em um equipamento) |

O principal diferencial do Medivolt está na capacidade de auto-detecção e teste de componentes passivos e ativos, função que multímetros convencionais não oferecem. Enquanto um multímetro padrão mede apenas resistência, tensão e corrente, o Medivolt identifica o componente, mede seus parâmetros específicos e julga se o componente está bom, suspeito ou ruim com base em um banco de dados configurável.

---

## 2. Especificações Técnicas

### 2.1 Capacidades de Medição

A tabela abaixo apresenta todas as capacidades de medição do Medivolt:

| Parâmetro | Símbolo | Faixa Mínima | Faixa Máxima | Resolução | Precisão |
|:---|:---:|:---:|:---:|:---:|:---:|
| **Resistência** | R | 0,1 Ω | 10 MΩ | 0,1 Ω | ±2% |
| **Capacitância** | C | 1 pF | 100.000 µF | 1 pF | ±5% |
| **Indutância** | L | 0,1 µH | 10.000 mH | 0,1 µH | ±10% |
| **Tensão DC** | VDC | 0 V | 600 V | 1 mV | ±1% |
| **Tensão AC (True RMS)** | VAC | 0 V | 250 V | 1 mV | ±2% |
| **Corrente DC** | I | 0 mA | 3.200 mA | 1 mA | ±2% |
| **Temperatura** | °C | -55°C | +125°C | 0,1°C | ±0,5°C |
| **Tensão Díodo** | Vf | 0,1 V | 1,5 V | 10 mV | ±5% |
| **Ganho Transistor** | hFE | 0 | 1000 | 1 | ±10% |
| **ESR Capacitor** | ESR | 0,01 Ω | 10 Ω | 0,01 Ω | ±10% |

### 2.2 Especificações Elétricas

| Parâmetro | Valor |
|:---|:---|
| **Tensão de alimentação** | 5V DC (USB-C) |
| **Consumo típico** | 350 mA |
| **Consumo máximo** | 500 mA |
| **Bateria interna** | Não inclusa (alimentação USB) |
| **Display** | TFT 3,2" ILI9341 320x240 RGB65K |
| **Processador** | ESP32 (Dual-core 240MHz) |
| **ADC** | 12-bit (4095 níveis) |
| **Memória Flash** | 4 MB |
| **Cartão SD** | MicroSD até 32GB (FAT16/FAT32) |

### 2.3 Especificações Mecânicas

| Parâmetro | Valor |
|:---|:---|
| **Dimensões do conjunto** | 85mm x 55mm x 25mm (montado) |
| **Peso** | aproximadamente 120g |
| **Material da caixa** | Acrílico ou ABS (caseDIY) |
| **Conectores de teste** | jacks banana de 4mm ou Borne tipo CN1 |
| **Sensor temperatura** | Conector H header 3 pinos |

### 2.4 Condições de Operação

| Parâmetro | Valor |
|:---|:---|
| **Temperatura de operação** | 0°C a 40°C |
| **Temperatura de armazenamento** | -20°C a 70°C |
| **Umidade relativa** | 10% a 80% (sem condensação) |
| **Altitude máxima** | 2000m |
| **Grau de poluição** | Grau 1 |
| **Categoria de sobretensão** | CAT II 250V |

### 2.5 Pinos e Conexões

A tabela abaixo apresenta a_pinagem do konekt do Medivolt:

| Pino/Função | GPIO | Direction | Descrição |
|:---|:---:|:---:|:---|
| **PIN_PROBE_1** | GPIO 35 | Input | Probe positivo (sinal analógico) |
| **PIN_PROBE_2** | GPIO 34 | Input/Output | Probe negativo (referência) |
| **PIN_ADC_ZMPT** | GPIO 36 | Input | Sensor tensão AC (ZMPT101B) |
| **PIN_SDA** | GPIO 21 | Input/Output | I2C SDA (INA219, DS18B20) |
| **PIN_SCL** | GPIO 22 | Output | I2C SCL (INA219, DS18B20) |
| **PIN_LED_GREEN** | GPIO 16 | Output | LED Verde (status OK) |
| **PIN_LED_RED** | GPIO 17 | Output | LED Vermelho (alerta/perigo) |
| **PIN_BUZZER** | GPIO 26 | Output | Buzzer piezelétrico |
| **PIN_TFT_CS** | GPIO 5 | Output | Chip Select TFT |
| **PIN_TFT_DC** | GPIO 4 | Output | Data/Command TFT |
| **PIN_TFT_RST** | GPIO 0 | Output | Reset TFT |
| **PIN_TFT_BL** | GPIO 21 | Output | Backlight TFT |
| **PIN_SD_CS** | GPIO 15 | Output | Chip Select SD Card |

---

## 3. Como Montar

### 3.1 Materiais Necessários

Antes de iniciar a montagem, verifique se você possui os seguintes componentes:

- Placa ESP32-2432S028R (Cheap Yellow Display) ja montada com firmware Medivolt
- Cartão MicroSD (opcional, recomendado 4-32GB)
- Sonda DS18B20 (opcional)
- Módulo ZMPT101B (opcional, necessária para medição AC)
- Módulo INA219 (opcional, necessária para medição de corrente)
- Cabos de conexão (jumpers fêmea-macho, Dupont)
- Pontas de prova com garras jacaré ou plugs banana 4mm
- Fonte USB 5V/2A (não inclusa, use fonte de celular)
- Cabo USB-C (não incluso)

### 3.2 Verificação Inicial

Siga estes passos para verificar se o equipamento está funcionando corretamente:

**Passo 1:** Conecte a placa à fonte USB usando o cabo USB-C. A tela deve ascender imediatamente.

**Passo 2:** Observe a splash screen de segurança que aparece nos primeiros 3 segundos. Esta tela_WARNING mostra um aviso importante sobre medição de tensões elevadas.

**Passo 3:** Após a splash screen, o menu principal deve aparecer com os ícones de navegação.

**Passo 4:**Toque nos ícones para navegar. O beep do buzzer deve soar em cada toque. O LED verde deve permanecer acceso continuamente quando o sistema está em modo seguro.

**Passo 5:** Se todos os indicadores estiverem funcionando, o equipamento está pronto para uso.

### 3.3 Conexão dos Módulos Opcionais

Para habilitar todas as funções, conecte os módulos externos conforme as instruções abaixo:

#### Conectando o Módulo ZMPT101B (Medição AC)

O módulo ZMPT101B é um transformador de corrente para medição de tensão AC não invasiva. Para conectá-lo:

1. Localize os pinos I2C na parte inferior da placa (GPIO 21 = SDA, GPIO 22 = SCL)
2. Conecte o pino SDA do ZMPT ao GPIO 21
3. Conecte o pino SCL do ZMPT ao GPIO 22
4. Conecte o pino VCC ao 3V3
5. Conecte o pino GND ao GND
6. Conecte os fios de entrada AC às garras do módulo (ATENÇÃO: 220V!)

```
ZMPT101B          ESP32-CYD
┌─────────┐        ┌─────────┐
│   SDA   ├───────►│ GPIO 21 │
│   SCL   ├───────►│ GPIO 22 │
│   VCC   ├───────►│  3V3    │
│   GND   ├───────►│  GND    │
└─────────┘        └─────────┘
```

#### Conectando o Módulo INA219 (Medição Corrente DC)

O módulo INA219 permite medição de corrente contínua e tensão de barramento. Para conectá-lo:

1. Conecte o pino SDA do INA219 ao GPIO 21
2. Conecte o pino SCL do INA219 ao GPIO 22
3. Conecte o pino VCC ao 3V3
4. Conecte o pino GND ao GND
5. Conecte Vin+ ao positivo da carga
6. Conecte Vin- ao negativo da carga

```
INA219            ESP32-CYD
┌─────────┐        ┌─────────┐
│   SDA   ├───────►│ GPIO 21 │
│   SCL   ├───────►│ GPIO 22 │
│   VCC   ├───────►│  3V3    │
│   GND   ├───────►│  GND    │
└─────────┘        └─────────┘
```

#### Conectando a Sonda DS18B20 (Temperatura)

A sonda DS18B20 é um sensor de temperatura digital one-wire. Para conectá-la:

1. Conecte o pino DQ (dados) ao GPIO 4
2. Conecte o pino VDD ao 3V3
3. Conecte o pino GND ao GND

```
DS18B20            ESP32-CYD
┌─────────┐        ┌─────────┐
│   DQ    ├───────►│ GPIO 4  │
│   VDD   ├───────►│  3V3    │
│   GND   ├───────►│  GND    │
└─────────┘        └─────────┘
```

### 3.4 Montagem em Caixa (Opcional)

Para uma montagem permanente e profissional,_instale a placa em uma caixa. Siga estes passos:

1. Marque os furos de montagem da placa CYD na caixa
2. Corte os rasgos para acesso aos conectores USB-C, cartão SD e jumpers
3. Fixe a placa com parafusos M2 ou M3
4. Instale os conectores banana de 4mm na frente da caixa
5. Conecte os puntas de prova aos jumpers internamente
6. Feche a caixa e fixe com parafusos

---

## 4. Primeiros Passos

### 4.1 Como Ligar o Dispositivo

Ligar o Medivolt é extremamente simples:

**Passo 1:** Conecte o cabo USB-C à entrada da placa CYD

**Passo 2:** Conecte a outra extremidade do cabo a uma fonte USB de 5V/2A (como um carregador de celular)

**Passo 3:** O dispositivo inicia automaticamente em aproximadamente 2-3 segundos

**Passo 4:** A splash screen de segurança aparece primeiro, exibindo um aviso sobre tensões elevadas

**Passo 5:** Após 3 segundos, o menu principal é exibido automaticamente

**Para desligar:** Simply desconecte o cabo USB. Não há botão de power, pois o dispositivo funciona como um periférico USB (como uma webcam ou mouse).

### 4.2 Inserindo o Cartão SD

O cartão SD é opcional mas altamente recomendável para desbloquear todos os recursos do Medivolt, incluindo histórico de medições e banco de dados de componentes. Para inserir o cartão:

**Passo 1:** Localize o leitor de cartão SD na parte inferior da placa (slot microSD)

**Passo 2:** Insira o cartão microSD com o lado de contato dourado facing down (na direção da placa)

**Passo 3:** Empurre suavemente até ouvir um click de engate

**Passo 4:** Verifique se o sistema reconhece o cartão (áparecerá na splash screen "SD: OK" ou "SD: FAIL")

**Para remover:** Pressione o cartão suavemente até liberar o mecanismo de ejeção e puxe para fora.

**Importante:** Sempre formate o cartão em FAT16 ou FAT32 antes do primeiro uso. Use o utilitário deformatação do seu sistema operacional (Windows: Disk Management, macOS: Disk Utility, Linux: gparted). Não use exFAT.

### 4.3 Calibragem Inicial

A calibração é essencial para obter medições precisas. O Medivolt vem calibrado de fábrica, mas recomenda-se verificar e ajustar após transporte ou uso prolongado.

#### Calibraçãoohms (Resistência)

Para calibrar a medição de resistência:

1.短路 as pontas de prova (junte os dois pólos)
2. Acesse Ajustes > Calibragem > Resistência
3. O sistema mostra o valor medido com as pontas emcurto
4. Se o valor for diferente de 0.0Ω, ajuste o offset usando as setas para mais/menos
5. Confirme com OK

#### Calibração de Capacitância

Para calibrar a medição de capacitância:

1. Deixe as pontas de prova abertas (sem conectado nada)
2. Acesse Ajustes > Calibragem > Capacitância
3. O sistema mostra o valor residual (capacitância parasita)
4. Ajuste o offset para mostrar próximo de 0
5. Confirme com OK

#### Calibração Tensão AC (ZMPT101B)

Para calibrar o sensor de tensão AC:

1. Acesse Ajustes > Calibragem > AC
2. Use um multímetro de referência para medir a tensão da rede
3. Ajuste o fator de escala até que os valores coincidam
4. Confirme com OK

#### Calibração Corrente DC (INA219)

Para calibrar o sensor de corrente:

1. Acesse Ajustes > Calibragem > Corrente
2. Deixe o circuito aberto (sem corrente)
3. Ajuste o offset de corrente para zero
4. Confirme com OK

---

## 5. Modos de Operação

### 5.1 Modo Auto-Detecção (Teste de Componentes)

O modo Auto-Detecção é a função mais poderosa do Medivolt. Neste modo, o sistema automaticamente identifica e mede o componente conectado às pontas de prova, sem necessidade de selecionar o tipo manualmente.

**Como usar:**

1. Toque no ícone "Medir" no menu principal (ou use o botão direcional UP/DOWN e confirme com OK)
2. Conecte o componente às pontas de prova (verifique a polaridade quando aplicável)
3. O resultado aparece automaticamente na tela após 1-3 segundos
4. O display mostra: tipo do componente, valor medido, unidade, status (bom/ruim/suspeito)

**O que o sistema detecta automaticamente:**

| Componente | Parámetros Medidos | Display |
|:---|:---|:---|
| Resistor | Resistência (Ω), tolerância | Valor em Ω/kΩ/MΩ |
| Capacitor | Capacitância (pF/nF/µF), ESR | Valor com unidade |
| Diodo | Tensão direta Vf (V), polaridade | Vf em V + Polo |
| LED | Vf (V), cor identificada | Cor + Vf |
| Transistor NPN | Tipo, hFE, pinagem EBC | hFE + tipo |
| Transistor PNP | Tipo, hFE, pinagem EBC | hFE + tipo |
| MOSFET N | Tipo, Vgs limiar | Tipo N-Channel |
| MOSFET P | Tipo, Vgs limiar | Tipo P-Channel |
| Indutor | Indutância (µH/mH) | Valor com unidade |
| Curto-circuito | Resistência < 1Ω | "CURTO" + valor |
| Aberto | Resistência > 10MΩ | "ABERTO" |

**Limitações da auto-detecção:**

- Componentes SMD muito pequenos podem não ser detectados
- Componentes queimados ou parcialmente Danificados podem ser mal classificados
- Capacitores de valor muito baixo (< 10pF) podem não ser distinguidos de capacitores parasitos

### 5.2 Modo Multímetro AC/DC

O modo Multímetro permite medições precisa de tensão e corrente. Este é o modo mais perigoso do equipamento e requer atenção especial com segurança.

#### Tensãocontínua (DC)

**Como medir tensão DC:**

1. Toque no ícone "Multimetro" no menu principal
2. Selecione "Tensão DC" usando as setas
3. Conecte: positivo (vermelho) ao ponto de medição positivo, negativo (preto) ao negativo/massa
4. O valor é exibido em tempo real na tela
5.Para alterar a faixa, use os botões de range (200mV, 2V, 20V, 200V, 600V)

#### Tensão Alternada (AC) com True RMS

> ⚠️ **AVISO DE SEGURANÇA — TENSÃO AC PERIGOSA**
>
> Este modo pode medir tensões de rede elétrica (110V/220V). A exposição a estas tensões pode ser FATAL. Sempre desconecte a energia antes de manipular as conexões. Use equipamentos de proteção individual (EPIs) adequados. Não toque nos fios nus durante a medição.

**O que é True RMS?**

True RMS (Root Mean Square verdadeiro) é um método de medir tensão alternada que calcula o valor eficaz real da onda, independentemente da forma de onda (senoidal, quadrada, triangular, etc.). Multímetros mais simples usam medição de valor médio, que fornece resultados incorretos para ondas não senoidais.

O Medivolt utiliza o sensor ZMPT101B com processamento True RMS.no firmware para medições precisas de tensão AC em todas as condições.

**Como medir tensão AC:**

1. Toque no ícone "Multimetro" no menu principal
2. Selecione "Tensão AC" usando as setas
3. Verifique se o módulo ZMPT101B está conectado corretamente
4. Conecte as pontas de prova aos pontos de medição (não requer polaridade)
5. O valor RMS é exibido em tempo real
6. Para segurança, o sistema solicita confirmação antes de habilitar este modo

**Observações importantes sobre segurança AC:**

- O módulo ZMPT101BIsola galvánicamente a medição, tornando-a mais segura
- Mesmo assim, nunca segure os fios durante a medição de 220V
- Use luvas isolantes e óculos de proteção
- Mantenha uma distância segura do circuito
- Se houver dúvida, chame um profissional habilitado

#### Corrente DC

Para medir corrente, é necessário usar o módulo INA219:

1. Toque no ícone "Multimetro" no menu principal
2. Selecione "Corrente DC"
3. Abra o circuito onde deseja medir
4. Conecte: positiva da fonte → Vin+ (INA219) → carga → Vin- (INA219) → negativa da fonte
5. O display mostra: tensão, corrente e potência calculada

### 5.3 Modo Histórico

O modo Histórico exibe todas as medições anteriores armazenadas no cartão SD.

**Como acessar:**

1. Toque no ícone "Historico" no menu principal
2. Navegue pela lista de medições usando UP/DOWN
3. Toque em uma medição para ver os detalhes
4. Para exportar, remova o cartão SD e Lea o arquivo HISTORY.CSV no computador

**Formato do arquivo HISTORY.CSV:**

```
Timestamp;Data;Hora;Componente;Valor;Unidade;Status
2026-04-24;24/04/2026;14:30:25;Resistor;1000;Ω;BOM
2026-04-24;24/04/2026;14:31:10;Capacitor;100;µF;BOM
2026-04-24;24/04/2026;14:32:05;Diodo;0.65;V;BOM
```

### 5.4 Modo Estatísticas

O modo Estatísticas mostra métricas agregadas de todas as medições:

- Total de componentes testados
- Quantidade de componentes bons
- Quantidade de componentes ruins
- Quantidade de componentes suspeitos
- Distribuição por tipo de componente
- Temperatura média durante medições

### 5.5 Modo Ajuste/Calibragem

O modo Ajuste permite configurar diversas opções do sistema:

- **Backlight:** Ajuste do brilho da tela (0-100%)
- **Buzzer:** Ativar ou desativar sons de feedback
- **Calibragem:** Ajustar offsets de medição (descrito na seção 4.3)
- **Sobre o sistema:** Versão do firmware, tempo de operação, memória livre

---

## 6. Como Testar Cada Componente

Esta seção fornece instruções detalhadas para testar cada tipo específico de componente eletrônico.

### 6.1 Testando Resistores

Resistores são os componentes mais comuns e fáceis de testar.

**Símbolo típico:**
```
──□───□───□──
  |  |  |
  |  |  └── Terminal 2
  |  └────── Corpo (faixas coloridas)
  └───────── Terminal 1
```

**Valores típicos:**

| Tipo | Faixa de Valores | Tolerância Típica |
|:---|:---|:---|
| Carbono composto | 1Ω - 1MΩ | ±5% a ±20% |
| Filme de carbono | 1Ω - 10MΩ | ±2% a ±10% |
| Filme metálico | 0,1Ω - 1MΩ | ±0,1% a ±2% |
| SMD 0402 | 1Ω - 1MΩ | ±1% a ±5% |
| SMD 1206 | 0,1Ω - 10MΩ | ±0,25% a ±5% |

**Como testar:**

1. Selecione modo "Medir" (auto-detecção) ou "Resistor"
2. Conecte as pontas aos terminais do resistor (polaridade não importa)
3. O valor é exibido na tela
4. Compare com o valor nominal (use o código de cores se necessário)

**Interpretação dos resultados:**

- Valor dentro da tolerância = BOM (verde)
- Valor próximo ao limite da tolerância = SUSPEITO (amarelo)
- Valor fora da tolerância = RUIM (vermelho)

### 6.2 Testando Capacitores

Capacitores armazenam energia elétrica em um campo eletrostático. A medição inclui capacitância e ESR.

**Símbolo típico:**
```
━━┴━━  (elétrico)     ─│─  (cerâmico)
   |                      |
```

**Valores típicos:**

| Tipo | Faixa de Valores | Tensão Máxima |
|:---|:---|:---|
| Cerâmico | 1pF - 1µF | 25V - 1kV |
| Filme | 1nF - 10µF | 50V - 1kV |
| Eletrolítico | 1µF - 10.000µF | 6,3V - 450V |
| SMD | 1pF - 1mF | 4V - 50V |
| Tântalo | 1µF - 1mF | 4V - 50V |

**⚠️ AVISO IMPORTANTE — ELETROLÍTICOS**

Capacitores eletrolíticos são polarizados. Conectar com polaridade invertida pode causar vazamento, superaquecimento ou EXPLOSÃO. Sempre identifique o terminal negativo (-) antes de conectar.

**Como testar:**

1. Selecione modo "Medir" (auto-detecção) ou "Capacitor"
2. **Descarregue o capacitor** conectando os terminais por alguns segundos através de um resistor de 100Ω-1kΩ (para evitar curto)
3. Conecte as pontas aos terminais (observe a polaridade para eletrolíticos)
4. O valor é exibido na tela (pode levar alguns segundos para capacitores grandes)
5. O ESR é exibido abaixo da capacitância

**Interpretação dos resultados:**

- Capacitância e ESR dentro da especificação = BOM
- Capacitância muitobaixo = RUIM (capacitor ressecado)
- Capacitância muito alta = RUIM (capacitor degradado)
- ESR muito alto = RUIM (capacitor ressecado ou defeituoso)

### 6.3 Testando Diodos

Diodos permitem corrente em apenas uma direç��o. A medição verifica tensão direta (Vf) e função de bloqueio.

**Símbolo típico:**
```
───|──────>
   |    seta (direção da corrente)
   |____ (catodo)
   |
  ânodo
```

**Valores típicos:**

| Tipo | Tensão Direta (Vf) | Tensão Reversa |
|:---|:---|:---|
| Silício 1N400x | 0,6V - 0,8V | 50V - 1000V |
| Germanium | 0,2V - 0,4V | 30V - 60V |
| Schottky | 0,2V - 0,5V | 20V - 150V |
| Zener | 0,6V - 180V | Especificada |
| LED | 1,8V - 3,6V | 5V típico |

**Como testar:**

1. Selecione modo "Medir" (auto-detecção) ou "Diodo"
2. Conecte: ponta positiva ao ânodo, ponta negativa ao cátodo
3. O display mostra: tensão direta em volts, identificação de polo
4. Inverta as pontas para verificar bloqueio
5. Na polarização reversa, deve mostrar "ABERTO" (tensão alta)

**Interpretação dos resultados:**

- Tensão direta dentro da faixa típica + bloqueio reverso = BOM
- Tensão direta 0V ou muitobaixa = RUIM (curto)
- Tensão direta infinita (OPEN) = RUIM (aberto)
- Condução nos dois sentidos = RUIM (diodo em curto)

### 6.4 Testando LEDs

LEDs (Light Emitting Diodes) são diodos que emitem luz ao conduzir. O teste é semelhante a diodos, mas também identifica a cor.

**Como testar:**

1. Selecione modo "Medir" (auto-detecção) ou "LED"
2. Conecte: ponta positiva ao ânodo (perna longa), negativa ao cátodo (perna curta)
3. O display mostra: tensão direta (Vf) e cor identificada

**Identificação de cor pela tensão:**

| Cor | Tensão Direta Típica |
|:---|:---|
| Infravermelho | 1,2V - 1,5V |
| Vermelho | 1,8V - 2,2V |
| Laranja | 2,0V - 2,2V |
| Amarelo | 2,1V - 2,4V |
| Verde | 2,0V - 2,8V |
| Azul | 2,5V - 3,6V |
| Branco | 2,8V - 3,6V |
| UV | 3,1V - 4,0V |

**Teste funcional:** Alguns LEDs podem ser testados observando-se o brilho/frente. Se não brilhar mas a tensão estiver correta, pode haver problema interno (filamento rompido mas junção intacta).

### 6.5 Testando Transistores BJT

Transistores bipolares (BJT) amplificamcorente. O teste verifica tipo (NPN/PNP), ganho (hFE) e identificação dos terminais.

**Símbolo típico:**
```
NPN:  ────|>── (E)      PNP:  ────|<── (E)
         |                        |
         C                      C
         B                      B
```

**Como testar:**

1. Selecione modo "Medir" (auto-detecção) ou "Transistor"
2. Toque em cada combinação de terminais para identificar E-B-C
3. Aperte em "Testar" quando os três terminais estiverem conectados
4. O display mostra: tipo (NPN/PNP), hFE, pinagem

**Identificação de terminais por packaging:**

| Package | Emissor | Base | Coletor |
|:---|:---:|:---:|:---|
| TO-92 (plástico) | ESQ | meio | DIR |
| TO-220 (metal) | meio | -- | DIR |
| SMD SOT-23 | ESQ | meio | DIR |

**Cuidados:** Transistores em circuito (conectados a outros componentes) podem fornecer resultados incorretos. Sempre dessolde pelo menos um terminal antes de testar.

### 6.6 Testando MOSFETs

 MOSFETs (Metal-Oxide-Semiconductor Field Effect Transistors) são transistores de efeito de campo. O teste verifica tipo (N-Channel/P-Channel) e resistência de condução (Rds).

**Como testar:**

1. Selecione modo "Medir" (auto-deteção) ou "MOSFET"
2. Toque nos três terminais (S, G, D)
3. O display mostra: tipo, Rds(on)
4. Alguns MOSFETs precisam de tensão de gate e podem não testarporn completo em aberto

### 6.7 Testando Indutores

 Indutores almacenam energia em um campo magnético. O medidor mede indutância.

**Símbolo típico:**
```
──────(())──────
      ||
      || (núcleo)
```

**Faixas típicas:**

| Tipo | Faixa de Valores |
|:---|:---|
| Radial | 1µH - 10mH |
| SMD | 0,1µH - 100µH |
| Toroidal | 1mH - 100mH |
| RF (ar) | 1nH - 1µH |

**Como testar:**

1. Selecione modo "Medir" (auto-deteção) ou "Indutor"
2. Conecte as pontas aos terminais (polaridade não importa)
3. O valor é exibido na tela

### 6.8 Tabela Resumo de Teste

| Componente | Conectar | Não Conectar | Resultado BOM |
|:---|:---|:---|:---|
| Resistor | Polo 1 - Polo 2 | Nada mais | Valor ± tolerância |
| Capacitor | Polo 1 - Polo 2 | Carregado | C nominal + ESR baixa |
| Diodo | + no Ânodo, - no Cátodo | Polaridade invertida | Vf típica + bloqueio |
| LED | + no Ânodo, - no Cátodo | Polaridade invertida | Vf típica + luz |
| Transistor | E, B, C identificados | Circuitovizinho | hFE dentro da faixa |
| MOSFET | S, G, D identificados | Circuitovizinho | Rds(on)baixa |
| Indutor | Pino 1 - Pino 2 | Nada mais | L dentro da faixa |

---

## 7. Sistema de Segurança

O Medivolt possui um sistema de segurança integrado sophistcado para proteção do usuário e do equipamento contra tensões perigosas.

### 7.1 Visão Geral do Sistema

O sistema de segurança monitora continuamente as tensões presentes nas pontas de prova e bloqueiaautomaticamente o equipamento quando tensões perigosas são detectadas. Este é um recurso CRÍTICO para sua segurança.

**Componentes do sistema de segurança:**

- Monitoramento em temporeal de tensão AC e DC
- Detecção automática de tensões > 50V
- Alertas visuais (LED vermelho piscante, tela de alerta)
- Alertas sonoros (buzzer beep aggressivo)
- Bloqueio temporário do equipamento (10 segundos)
- Tela de confirmação para medição AC
- Proteção contra curto-circuito

### 7.2 Níveis de Alerta

O sistema define quatro níveis de alerta:

| Nível | Tensão Detectada | Ação Automática |
|:---|:---|:---|
| **Nenhum** | < 50V AC | Operação normal, LED verde acesso |
| **Baixo** | 50-100V AC | Alerta amarelo, LED verde/piscante |
| **Médio** | 100-180V AC | Alerta laranja, som de aviso |
| **Alto** | 180-250V AC | ALERTA VERMELHO, bloqueio |
| **Crítico** | > 250V AC | BLOQUEIO TOTAL, som de emergência |

### 7.3 Tela de Aviso de Perigo

Quando o sistema detecta tensão perigosa, a tela de alerta aparece com:

- Fundo vermelho escuro
- Mensagem "ALERTA DE SEGURANÇA"
- Tensão detectada (em volts)
- Mensagem específica (ex: "PERIGO! 220V DETECTADO")

Dois botões aparecem na tela:

- "CONTINUAR IGUALMENTE" - prossegue mediante confirmação (para usuários experientes)
- "CANCELAR" - retorna ao menu, proteção máxima

### 7.4 Tela de Bloqueio

Após três detecções consecutivas de tensão perigosa, o equipamento entra em modo de BLOQUEIO:

- A tela mostra "BLOQUEADO" e tempo restante
- Um countdown de 10 segundos é iniciado
- Todos os modos de medição são desabilitados
- Após timeout, o sistema retorna ao modo verificar

### 7.5 Confirmação AC

Antes de habilitar o modo multímetro AC, o sistema solicita confirmação:

**Tela exibida:**

```
┌────────────────────────┐
│    AVISO IMPORTANTE    │
│                        │
│ Para medir 220V AC,    │
│ voce deve ter:        │
│                        │
│ * Fusivel 1A       │
│ * Varistor 275V     │
│ * Circuitos protecao  │
│                        │
│ [TENHO PROTECAO]    │
│   [CANCELAR]        │
└────────────────────────┘
```

**Botões:**

- TENHO PROTECAO: confirma que tem proteção instalada, habilita modo AC
- CANCELAR: retorna ao menu, modo AC permanece bloqueado

### 7.6 Avisos Fortes de Segurança

> ⚠️ **PERIGO DE CHOQUE ELÉTRICO**
>
> A tensão de rede elétrica (110V/220V) pode causarmorte por choqueelétrico. Nunca toque nos fios ou componentes energizados durante a medição. Use luvas isolantes, óculos de proteção e calçados isolantes. Mantenha uma distância segura do circuito. Não use o equipamento com as mãos molhadas ou em ambientes úmidos.

> ⚠️ **IMPORTANTE: PROTECÃO OBRIGATÓRIA**
>
> Para medir tensão AC de rede, você DEVE instalar: Fusível de 1A na entrada (para proteção contra curto), Varistor de 275V (para proteção contra surto), Resistores limitadores (para proteção contra sobrecarga). Sem estes componentes, não tente medir 220V AC.

> ⚠️ **RISCO DE INCÊNDIO**
>
> Não use o equipamento próximo a materiais inflamáveis. Não deixe o equipamentoDES ligando quando não estiver em uso. Não exceda as especificações de tensão. Capacitor carregados podem explodir se curto-circuitados.

> ⚠️ **DANOS AO EQUIPAMENTO**
>
> Não conecte o instrumento a tensões AC acima de 250V. Não conecte tensão DC acima de 600V. Não use para testar baterias de alta tensão. Não use para testar capacitores carregados sem primeiroosdescartar. A violação destas especificações pode danificar permanentemente o equipamento e INVALIDAR A GARANTIA.

> ✅ **DICA DE SEGURANÇA SEMPRE**
>
> Quando em dúvida, não meça. Consulte um profissional ou use equipamentocapacitado para medições de alta tensão. A segurança vale mais do que uma medição.

### 7.7 Configurações de Segurança

O sistema de segurança pode ser configurado via menu Ajustes:

| Opção | Padrão | Descrição |
|:---|:---:|:---|
| Verificação 자동ica | Ativada | Monitora tensãocontinuamente |
| Som de alerta | Ativado | Emite som em detectartensão perigosa |
| LED de alerta | Ativado | LED vermelho pisca em detectartensão |
| Bloqueio automatico | Ativado | Bloqueia após 3 detecções |

Recomenda-se manter todas as opções ativadas para máxima proteção.

---

## 8. Manutenção e Atualização

### 8.1 Manutenção Preventiva

Siga estas práticas para manter o equipo em boa condições:

- **Limpeza:** Limpe a tela com pano macio e levemente úmido. Não use produtos químicos agressivos.
- **Armazenamento:** Guarde em local seco, protegido de poeira e luz solar direta.
- **Verificação periódica:** Execute a calibração de resistência a cada 3 meses ou após transporte.
- **Inspeção visual:** Verifique se há danos nos cabos, pontas de prova e conectores.

### 8.2 Atualização do Banco de Dados (COMPBD.CSV)

O arquivo COMPBD.CSV no cartão SD contém o banco de dados de identificação de componentes. Você pode atualizar este arquivo para adicionar novos componentes ou corrigir valores.

#### Formato do Arquivo

```
Tipo,hFE_Min,hFE_Max,Vf_Min,Vf_Max,PartNumber
NPN,100,200,0.6,0.7,BC547
NPN,200,400,0.6,0.7,BC548
PNP,100,200,0.6,0.7,BC557
PNP,200,400,0.6,0.7,BC558
NPN,100,250,0.6,0.75,2N2222
NPN,60,200,0.6,0.7,2N3904
N-Channel,Rds<0.5,--,--,--,IRF540
P-Channel,Rds<0.5,--,--,--,IRF9540
```

#### Como Atualizar

1. Remova o cartão SD do equipamento
2. Insira o cartão nocomputador
3. Abra o arquivo COMPBD.CSV com um editor de planilhas (Excel, LibreOffice Calc) ou editor de texto (Notepad++, VS Code)
4. Adicione novas linhas seguindo o formato existente (Tipo, valor mínimo, valor máximo, Part Number)
5. Salve o arquivo em formato CSV (separado por vírgulas, UTF-8)
6. Remonte o cartão no equipamento
7. Reinicie o Medivolt para carregar o novo banco de dados

### 8.3 Atualização do Firmware (Avançado)

O firmware pode ser atualizado via OTA (Over-The-Air) ou via USB. Para atualizar:

1. Baixe a nova versão do firmware (arquivo .bin)
2. Conecte o dispositivo ao computador via USB
3. Use o ferramenta de flash (esptool ou PlatformIO)
4. Execute o comando de flash
5. Aguarde a conclusão e reinicie

Consulte a documentação de desenvolvimento (DEVELOP.md) para instruções detalhadas.

### 8.4 Recalibração

Recalibre o equipamento se:

- As medições estiverem imprecisas
- Após queda ou impacto
- Após longo período sem uso (> 6 meses)
- Após substituição de componentes

**Procedimento de recalibração:**

1. Acesse menu Ajustes > Calibragem
2. Execute cada calibração conforme seção 4.3
3. Anote os valores de offset
4. Execute testes de verificação
5. Repita se necessário

---

## 9. Solução Rápida de Problemas

### 9.1 Problemas Comuns e Soluções

| Sintoma | Causa Provável | Solução |
|:---|:---|:---|
| Display nãoacende | Cabo USB solto/fonte ruim | Verifique conexão e fonte 5V/2A |
| Tela táctil não responde | Calibração perdida | Calibre a tela em Ajustes |
| Leituras incorretas | Descalibrado |Execute calibração |
| Cartão SD não reconhecido | Formato incompatível | Formate em FAT16/FAT32 |
| Erro ao medir resistor | Componente em circuito | Dessolde um terminal |
| Erro ao medir capacitor | Capacitor carregado | Descarregue primeiro |
| Bloqueio constante | Tensão parasita | Verifique circuito |
| Buzzer sem som | Desativado | Ative em Ajustes |
| LED nãoacende | LED queimado | Substitua LED |
| Medição AC não funciona | ZMPT não conectado | Conecte o módulo |

### 9.2 Mensagens de Erro

| Mensagem | Significado | Ação |
|:---|:---|:---|
| "SD: FAIL" | Cartão não reconhecido | Verifique formato, reinsira |
| "COMP: CURTO" | Componente em curto | Verifique conexões |
| "COMP: ABERTO" | Componente aberto | Verifique continuidade |
| "SOBRECARGA" | Valor muito alto | Reduza faixa |
| "TENSÃO PERIGOSA" | Tensão alta detectada | Desconecte imediatamente |
| "BLOQUEADO" | Segurança ativada | Aguarde 10 segundos |

### 9.3 Suporte Técnico

Para suporte adicional:

- Consulte a documentação completa em docs/
- Verifique o CHANGELOG.md para atualizações
- Reporte problemas em https://github.com/anomalyco/opencode/issues

---

## 10. Garantia e Suporte

### 10.1 Garantia

O Medivolt possui garantia de 90 dias contra defeitos de fabricação. A garantía não cobre:

- Danos causados por uso indevido
- Danos causados por sobretensão
- Danos físicos (quedas, impactos)
- Modificações no firmware
- Componentes consumíveis (pontas de prova, cabos)

### 10.2 Suporte

Para suporte técnico, registre um issue no repositório do projeto com:

- Descrição detalhada do problema
- Passos para reprodução
- Fotos/vídeos (se aplicável)
- Versão do firmware
- Modelo da placa

---

## Apêndice A: Glossário

| Termo | Significado |
|:---|:---|
| AC | Corrente Alternada (Alternating Current) |
| ADC | Conversor Analógico-Digital (Analog-to-Digital Converter) |
| BJT | Transistor de Junção Bipolar (Bipolar Junction Transistor) |
| CYD | Cheap Yellow Display |
| DC | Corrente Contínua (Direct Current) |
| ESR | Resistência Série Equivalente (Equivalent Series Resistance) |
| GPIO | Pino de Entrada/Saída de Propósito Geral (General Purpose Input/Output) |
| hFE | Ganho de corrente DC do transistor |
| I2C | Inter-Integrated Circuit (barramento serial) |
| LED | Diodo Emissor de Luz (Light-Emitting Diode) |
| MOSFET | Metal-Oxide-Semiconductor Field-Effect Transistor |
| RMS | Valor Quadrático Médio (Root Mean Square) |
| SD | Secure Digital (cartão de memória) |
| Vf | Tensão Direta (Forward Voltage) |

---

## Apêndice B: Referências Rápidas

### Navegação

| Botão | Função |
|:---|:---|
| UP/DOWN | Navegar itens |
| LEFT/RIGHT | Navegar colunas |
| OK/ENTER | Selecionar |
| BACK | Voltar/Cancelar |

### Códigos de Cores de Resistores

| Cor | Valor | Multiplicador | Tolerância |
|:---|:---:|:---:|:---:|
| Preto | 0 | 1Ω | -- |
| Marrom | 1 | 10Ω | ±1% |
| Vermelho | 2 | 100Ω | ±2% |
| Laranja | 3 | 1kΩ | ±3% |
| Amarelo | 4 | 10kΩ | ±4% |
| Verde | 5 | 100kΩ | ±0,5% |
| Azul | 6 | 1MΩ | ±0,25% |
| Violeta | 7 | 10MΩ | ±0,1% |
| Cinza | 8 | 100MΩ | ±0,05% |
| Branco | 9 | 1GΩ | -- |
| Dourado | -- | 0,1Ω | ±5% |
| Prata | -- | 0,01Ω | ±10% |

---

<p align="center">

**Sondvolt v3.1 — Component Tester Pro**

*Manual do Usuário*

*Desenvolvido pela Comunidade Eletrônica DIY*

*2025-2026*

</p>