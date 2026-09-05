# Histórico de versões

---

## [4.0.0] — Setembro de 2026

Revisão de correção. O foco não foi adicionar recursos, e sim fazer o aparelho medir de verdade: a v3.2 tinha módulos inteiros escritos, corretos e **nunca executados**, medições fisicamente impossíveis com a pinagem publicada, e valores de tela que eram texto fixo em vez de medição.

### Corrigido — defeitos críticos

#### 1. Excitação das pontas era impossível no ESP32

`measurements.cpp` e `multimeter.cpp` chamavam `pinMode(GPIO35, OUTPUT)` e `pinMode(GPIO34, OUTPUT)` para carregar o capacitor e polarizar o divisor de resistência. **GPIO34 a 39 do ESP32 são entrada apenas** — não têm driver de saída nem resistor de pull interno. As chamadas não tinham efeito algum.

Consequência: a "capacitância" medida era o tempo que o ADC levava para ler, e a resistência dependia de o pino estar flutuando.

Correção: novo pino de excitação (`PIN_PROBE_DRIVE`, GPIO27) alimentando as pontas através de um resistor de referência de 10 kΩ, mais um segundo caminho de 470 Ω (GPIO22) para a faixa de baixa impedância. `pins.h` agora tem `static_assert` que falha o build se alguém reintroduzir uma pinagem impossível.

#### 2. A proteção elétrica nunca disparava

`safety_detect_danger()` calculava:

```c
voltage = (adc - 2048) * ZMPT_SCALE_FACTOR / 2048;   // ZMPT_SCALE_FACTOR = 1.0
```

O resultado é um número entre 0 e 1. Esse valor era comparado com limiares de 50 V, 180 V e 250 V. Como 1,0 nunca chega a 50, `safety_check_voltage()` **sempre** devolvia "seguro" e o bloqueio automático jamais era acionado.

Havia um segundo defeito no mesmo caminho: em `safety_check_voltage()` o primeiro ramo capturava tudo acima de 50 V como CRÍTICO, tornando os ramos de 220 V e 127 V inalcançáveis.

Correção: a detecção usa o motor True RMS calibrado do multímetro, que devolve volts reais, e os limiares são testados do maior para o menor.

#### 3. Faixa DC padrão multiplicava por 181

`multimeter_read_dc_voltage()` escolhia o fator de escala com `if/else if/else`, sem tratar `RANGE_AUTO` — que é justamente o valor inicial. A execução caía no `else`, aplicando o fator de 600 V (`600/3.3 = 181`). Uma pilha AA de 1,5 V era exibida como 272 V.

#### 4. Metade dos subsistemas nunca era inicializada

`setup()` chamava cinco funções de init. Nunca chamava:

| Função | Consequência |
| :--- | :--- |
| `buzzer_init()` | canal LEDC nunca configurado — nenhum som saía da placa |
| `buzzer_update()` | um tom iniciado nunca era desligado |
| `leds_init()` | pinos dos LEDs nunca viravam saída |
| `leds_update()` | padrões de pisca-pisca nunca avançavam |
| `db_init()` | banco de componentes nunca carregado |
| `calibration_init()` | offsets gravados na NVS nunca lidos — cada boot começava sem calibração |
| `settings_load()` | preferências do usuário nunca restauradas |
| `logger_write()` | **nada era gravado no cartão SD**, apesar de o manual prometer histórico automático |

#### 5. Valores falsos exibidos como medição

A tela de instrumentos mostrava strings constantes onde o usuário lê números: `ESR: 0.12 Ohms`, `hFE: 245`, `Vbe: 642mV`, `Q: 4.2 @ 1kHz`, `Rdc: 0.8 Ohms`, `Ir: < 10nA`, `Vloss: 0.8%`. Nenhum desses valores era medido.

Correção: todo valor vem do motor de análise. O que não pode ser medido aparece como `---` com o motivo ao lado.

#### 6. `src/ui.cpp` estava corrompido no repositório

O arquivo tinha 50.856 bytes **todos zerados** e nesse estado foi commitado (`3dc56b7`). Como `main.cpp` chama `ui_init()`, `ui_update()` e `ui_handle_touch()`, o projeto não linkava. Restaurado a partir de `36ad4f8`.

#### 7. INA219 sem protocolo I²C

`multimeter_read_dc_current()` chamava `Wire.requestFrom()` sem antes escrever o ponteiro de registrador, não aplicava as escalas do datasheet (4 mV/bit no barramento, 10 µV/bit no shunt) e reinicializava o I²C a cada leitura. Os números eram aleatórios.

Correção: escrita do ponteiro seguida de *repeated start*, escalas corretas, registrador de calibração configurado uma vez no init, e leitura do shunt tratada como inteiro com sinal.

#### 8. LEDs invertidos

O LED RGB da CYD é de **ânodo comum**: nível baixo acende. O código escrevia `HIGH` para acender. Todos os indicadores operavam ao contrário.

### Corrigido — defeitos de robustez

- **True RMS com zero fixo.** O offset do ZMPT era assumido em 2048; qualquer desvio do trimpot virava tensão fantasma. Agora o zero é a média das próprias amostras.
- **Filtro compartilhado entre modos.** Um único buffer de média móvel atendia tensão, corrente e resistência. Trocar de modo contaminava a leitura nova por vários ciclos. Agora há um filtro independente por modo.
- **Dois mapeamentos de toque divergentes.** `main.cpp` e `buttons.cpp` convertiam coordenadas com constantes e orientações diferentes; `safety.cpp` tinha um terceiro. Os botões das telas de segurança ficavam espelhados. Agora existe uma única conversão, em `hal_touch_read()`.
- **`db_init()` não fazia parse.** Contava linhas do CSV e incrementava um contador; o array de componentes ficava vazio. Além disso, `DB_FILE_CSV` apontava para `/database.csv` enquanto o arquivo real é `COMPBD.CSV`.
- **Log sem rotação.** O CSV crescia sem limite até encher o cartão, e a gravação passava a falhar em silêncio. Agora rotaciona em 256 KB.
- **`logger_get_recent()` fragmentava o heap** alocando um objeto `String` por linha, e deslocava o array inteiro a cada registro lido.
- **`measurements_discharge_capacitor()` travava a tarefa** com onze `delay(100)` seguidos, e reportava progresso simulado em vez do real.
- **`safety_update()` redesenhava a tela de bloqueio** a cada 100 ms a partir da tarefa de medição, disputando o barramento SPI com a interface.
- **`safety_alert_led_flash()` bloqueava a tarefa** por 200 ms a cada chamada.
- **`draw_safety_alert_screen()` nunca era chamada.** A tela de alerta de alta tensão existia desde a v3.1 e jamais aparecia.
- **Botão CALIBRAR não calibrava.** Copiava a última leitura para os offsets, gravando como "erro das pontas" o valor do componente que estivesse conectado.
- **Sequência de boot fingia.** Mostrava "Montando SD Card..." com um `delay(400)` e seguia adiante sem montar nada.
- **DS18B20 bloqueava 800 ms** por leitura, dentro da tarefa de medição.
- **Leitura térmica falhava com LED aceso.** O OneWire divide o GPIO4 com o LED vermelho; um LED aceso mantém o pino alto e o sensor "sumia" justamente durante um alerta.
- **Macros duplicadas com valores diferentes.** `ZMPT_NUM_SAMPLES` valia 50 em `pins.h` e 128 em `config.h`; toda a paleta `THEME_*` estava definida em `theme.h` e `visual.h` com cores diferentes. Qual valor prevalecia dependia da ordem dos includes — e a flag `-w` escondia o aviso.
- **`task_manager.cpp`** (686 linhas) implementava um sistema completo de cinco tarefas e filas que `main.cpp` nunca usava. Removido.
- **Calibração sem validação.** Offsets absurdos eram gravados e passavam a estragar todas as medições seguintes. O campo `checksum` existia mas era sempre zero.

### Adicionado

#### Módulos novos

| Arquivo | Papel |
| :--- | :--- |
| `hal.h` / `hal.cpp` | Abstração de hardware: compatibilidade LEDC entre core 2.x e 3.x, arbitragem dos pinos compartilhados, ADC com sobreamostragem e correção de não linearidade, conversão única do toque |
| `analysis.h` / `analysis.cpp` | Motor de medição e identificação de componentes |
| `diagnostics.h` / `diagnostics.cpp` | Autoteste, saúde do sistema, estatísticas, persistência na NVS |
| `uiwidgets.h` / `uiwidgets.cpp` | Notificações, gráficos, medidores, diálogos, teclado, listas |

São **112 funções públicas novas** nos cinco módulos.

#### Medição e análise

ESR de capacitor · hFE de transistor com detecção de tipo · tensão direta de diodo e LED · detecção de MOSFET pela retenção de carga no gate · indutância pela constante de tempo L/R · corrente de fuga · resistência interna de bateria · frequência e ciclo de trabalho · tensão pico a pico · teste de continuidade, curto, aberto e fusível · identificação automática completa.

#### Engenharia aplicada

Código de cores de resistor de quatro faixas desenhado na tela · conversão inversa (cores para valor) · valor comercial mais próximo nas séries E6, E12 e E24 · desvio percentual · tolerância sugerida · notação de engenharia com prefixo SI.

#### Banco de dados

Catálogo de **50 componentes reais** em flash, com parâmetros de datasheet, sempre disponível mesmo sem cartão. Parse real do `COMPBD.CSV` por varredura sob demanda: os 5.726 registros ficam consultáveis **sem custo de RAM**. Busca por código de peça, por valor e sugestão de equivalentes.

#### Interface

Notificações que somem sozinhas · gráfico em tempo real com auto-escala · acumulador hold/min/max/média · barra de progresso · medidor de barras com zonas · medidor de arco · diálogo de confirmação · tela de erro que explica a causa e o que fazer · teclado na tela · listas com rolagem e barra lateral · cartões de valor com fonte adaptativa · spinner.

#### Sistema

Autoteste de 10 subsistemas alimentando a barra de boot real · monitor de heap e de pilha das tarefas · temperatura do chip · watchdog · log rotativo · exportação de relatório em texto para o cartão · estatísticas de uso · persistência de configurações com número mágico e versão de formato.

### Alterado

- **`-w` removido do build.** O projeto compila limpo com `-Wall -Wextra`, nas duas revisões de hardware.
- **Versões fixadas.** `platform = espressif32 @ 6.5.0` e bibliotecas com versão explícita, para o build ser reproduzível.
- **Ambiente `cyd-revb`** para quem montar a fiação sem pinos compartilhados.
- **Dependência do Adafruit INA219 removida** — o firmware fala I²C direto.
- **Fontes da TFT_eSPI reduzidas** ao que o projeto usa, economizando flash.
- **Pilha da tarefa de interface** aumentada de 4 KB para 6 KB.
- **`ZMPT_SAMPLE_RATE_US`** de 500 µs para 200 µs e **`ZMPT_NUM_SAMPLES`** para 256: quatro ciclos completos de 60 Hz por leitura.
- **`.gitignore`** cobrindo artefatos de build. Removidos do repositório 19 arquivos `build_*.txt`, um `compile_commands.json` de 1,4 MB e um `src/platformio.ini` órfão.

### Removido

- `src/task_manager.cpp` e `src/task_manager.h` — 686 linhas de código morto
- Constantes duplicadas em `multimeter.h`, `pins.h` e `theme.h`
- `temprature_sens_read()` — função interna sem calibração, convertida com fórmula errada
- Afirmação de que o cartão SD compartilha barramento com a TFT (não compartilha: tem pinos próprios)
- Menção a "ESP32-S3" na tela de informações de uma placa com ESP32 clássico

---

## [3.2.0] — Abril de 2026

### Adicionado
- Motor True RMS com 128 amostras e detecção de tensão de pico
- Sistema de segurança multinível com bloqueio por software
- Detecção de surtos e transientes
- Tela de confirmação de hardware de proteção
- Auditoria de materiais (BOM)
- Exportação CSV

### Alterado
- Buffer de 128 amostras (potência de 2)
- Layout do multímetro exibindo RMS, pico e status
- Requisitos obrigatórios de proteção: fusível 5 A, varistor 14D431, TVS P6KE400A

> **Nota da v4.0:** vários itens acima estavam implementados mas não operavam. O motor True RMS existia; a detecção de segurança que dependia dele comparava um número normalizado com limiares em volts. Veja a seção 2 da v4.0.

---

## [3.0.0] — Abril de 2026

Porte para a placa ESP32-2432S028R (Cheap Yellow Display), migração do display para TFT_eSPI e do armazenamento para SdFat.
