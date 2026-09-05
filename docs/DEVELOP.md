# Guia do desenvolvedor

Arquitetura do firmware Sondvolt v4.0, para quem vai mexer no código.

---

## Estrutura de camadas

O projeto tem quatro camadas. A regra é que cada uma só conheça a de baixo.

```
┌──────────────────────────────────────────────────────────┐
│  APRESENTAÇÃO   ui.cpp · menu.cpp · uiwidgets.cpp        │
│                 graphics.cpp · splash.cpp · help.cpp     │
├──────────────────────────────────────────────────────────┤
│  DOMÍNIO        analysis.cpp · multimeter.cpp            │
│                 measurements.cpp · safety.cpp            │
│                 database.cpp · calibration.cpp           │
├──────────────────────────────────────────────────────────┤
│  SERVIÇOS       logger.cpp · diagnostics.cpp             │
│                 thermal.cpp · buzzer.cpp · leds.cpp      │
├──────────────────────────────────────────────────────────┤
│  HAL            hal.cpp · pins.h · display_globals.cpp   │
└──────────────────────────────────────────────────────────┘
```

Nenhum arquivo acima da HAL deve chamar `analogRead()`, `ledcWrite()` ou `digitalWrite()` diretamente em pino compartilhado. Se você precisar disso, a função pertence à HAL.

---

## Por que a HAL existe

Três motivos concretos, todos vindos de bugs reais:

**1. A API do LEDC mudou entre os cores 2.x e 3.x do Arduino-ESP32.** No 2.x é `ledcSetup(canal, freq, bits)` + `ledcAttachPin(pino, canal)`. No 3.x virou `ledcAttach(pino, freq, bits)`. A v3.2 usava as duas APIs em arquivos diferentes — uma delas ia falhar em qualquer core que você escolhesse. `hal_pwm_*()` resolve com `#if ESP_ARDUINO_VERSION_MAJOR`.

**2. Três pinos têm dois donos na Rev A.** Sem arbitragem, acender um LED corrompe a leitura de um sensor. `hal_bus_acquire()` / `hal_bus_release()` garantem exclusão mútua e restauram o estado anterior do pino.

**3. Havia três conversões de coordenada de toque diferentes** — em `main.cpp`, `buttons.cpp` e `safety.cpp`, com constantes e orientações divergentes. Hoje só existe `hal_touch_read()`.

### API da arbitragem

```c
if (hal_bus_acquire(HAL_BUS_ONEWIRE, 300)) {   // timeout em ms
    // o LED rival já foi apagado; o pino é seu
    sensors.requestTemperatures();
    hal_bus_release(HAL_BUS_ONEWIRE);          // LED restaurado ao estado anterior
}
```

Sempre trate o `false`. Um timeout significa que outro subsistema está usando o pino — devolva o último valor conhecido em vez de bloquear a tarefa.

---

## As duas tarefas

```
TaskUI          prio 2 · período 20 ms · pilha 6 KB
                toque → ui_handle_touch() → ui_update() → toast → buzzer/LEDs

TaskMeasurement prio 1 · período 100 ms · pilha 4 KB
                safety_update() → medição conforme o estado → diag → NVS
```

**Regra:** a tarefa de interface nunca faz medição lenta, e a tarefa de medição nunca desenha uma tela inteira.

A v3.2 violava as duas: `safety_update()` redesenhava a tela de bloqueio a cada 100 ms a partir da tarefa de medição, e a descarga de capacitor bloqueava por 1,1 segundo com `delay()`.

### Acesso ao display

Um mutex **recursivo** protege o barramento SPI:

```c
LOCK_TFT();
tft.fillRect(...);
UNLOCK_TFT();
```

É recursivo de propósito — funções de desenho chamam outras funções de desenho. Mas cuidado ao chamar um widget de dentro de uma seção travada: os widgets de `uiwidgets.cpp` fazem seu próprio lock. O padrão é liberar antes:

```c
UNLOCK_TFT();
widget_progress_bar(...);
LOCK_TFT();
```

---

## Máquina de estados

`currentAppState` (enum `AppState`, em `types.h`) determina o que a tarefa de medição mede e o que a de interface desenha. É `volatile` porque as duas tarefas leem.

Para adicionar uma tela:

1. Novo valor no enum `AppState`
2. Entrada no array de menu apropriado em `menu.cpp`
3. `case` no `switch` de `ui_update()` chamando sua função de desenho
4. `case` no `switch` de `TaskMeasurement` se a tela precisar de medição contínua
5. Tratamento de toque em `ui_handle_touch()`

---

## Adicionando uma medição

Toda medição nova vai em `analysis.cpp`. O contrato é:

```c
float analysis_measure_alguma_coisa() {
    if (!hal_probe_available()) return 0.0f;                    // 1
    if (!hal_bus_acquire(HAL_BUS_PROBE_DRIVE, 200)) return 0.0f; // 2

    drive_high(PROBE_RANGE_LOW);                                 // 3
    delayMicroseconds(500);
    float v = hal_adc_read_volts(PIN_ADC_PROBE1, 24);
    drive_idle();

    hal_bus_release(HAL_BUS_PROBE_DRIVE);                        // 4

    if (v < LIMIAR_MINIMO) return 0.0f;                          // 5
    return converte(v);
}
```

1. Confirme que o hardware existe. Nunca devolva um número quando não há como medir.
2. Tome o barramento e trate o timeout.
3. Excite, espere estabilizar, leia, volte a alta impedância.
4. **Sempre** libere, inclusive nos caminhos de erro.
5. Rejeite leituras fora da faixa física em vez de propagar lixo.

Depois exponha em `measurements.h` se a interface precisar, e adicione o caso em `analysis_identify()` se entrar na identificação automática.

---

## Banco de dados

Duas fontes, propósitos diferentes:

**Catálogo interno** (`kCatalog` em `database.cpp`) — 50 componentes reais em flash, sempre disponíveis. É o que alimenta `db_judge()`. Para adicionar:

```c
{ "BC547", COMP_TRANSISTOR_NPN, 300, 110, 800, 0.70f, "hFE", "E-B-C",
  "NPN 45V 100mA uso geral" },
//  nominal ─┘   mínimo ─┘  máximo ─┘  param2 ─┘
```

**Cartão SD** (`COMPBD.CSV`) — 5.726 registros consultados por varredura sob demanda. Nunca é carregado inteiro na RAM. Use `db_sd_find()`, `db_sd_find_by_value()` ou `db_sd_find_equivalents()`.

Formato do CSV:

```
nome,tipo,nominal,minimo,maximo,param2,p1,p2,p3,descricao,categoria,flag
```

Os códigos de tipo estão no enum `DbCsvType`.

---

## Persistência

| Namespace NVS | Conteúdo | Módulo |
| :--- | :--- | :--- |
| `sondvolt` | `DeviceSettings` + `UsageStats` | `diagnostics.cpp` |
| `calib` | offsets das pontas | `calibration.cpp` |
| `mmcal` | ganho do ZMPT, escala do INA219, divisor DC | `multimeter.cpp` |

As configurações usam número mágico (`0x53564C54`) e versão de formato (`kSettingsVersion`). **Ao alterar o layout de `DeviceSettings`, incremente a versão** — assim uma gravação antiga é descartada em vez de lida errado.

A gravação é adiada 8 segundos por `settings_mark_dirty()` / `settings_flush_if_needed()`, para não escrever na flash a cada toque na tela.

---

## Compilando

```bash
pio run                    # Rev A
pio run -e cyd-revb        # Rev B
pio run -t upload
pio device monitor
```

O `-w` foi removido do `platformio.ini`. O projeto compila **sem nenhum aviso** com `-Wall -Wextra` nas duas revisões. Se a sua alteração gerar um aviso, ele é real — a flag `-w` da v3.2 escondia macros redefinidas com valores conflitantes.

### Verificação sem hardware

Não é preciso ter a placa para validar sintaxe, tipos e símbolos. Um harness de compilação no host com stubs das bibliotecas Arduino compila e **linka** todas as unidades de tradução, o que pega a grande maioria dos erros antes do upload.

---

## Convenções

**Nomes.** `modulo_acao()` para funções públicas (`analysis_measure_esr`), `snake_case` estático para internas, `gNome` para estado global de arquivo, `kNome` para constantes.

**Comentários.** Explique *por quê*, não *o quê*. `// incrementa i` não ajuda ninguém; `// A ordem importa: o primeiro ramo capturava tudo acima de 50 V` evita que alguém reintroduza o bug.

**Acentuação.** O código-fonte usa ASCII puro nos comentários. A documentação em Markdown usa português com acentos normalmente.

**Buffers.** `snprintf()` sempre, `sprintf()` nunca. Cheque o tamanho do destino.

**Ponto flutuante.** `float` no ESP32 tem unidade em hardware; `double` é emulado e lento. Use `double` só onde a precisão exige — a soma de quadrados do RMS, por exemplo, onde 256 amostras de 2048² passam de 1e9.

---

## Onde estão as coisas

| Preciso mexer em... | Arquivo |
| :--- | :--- |
| pinagem | `pins.h` |
| constante de medição, cor, tempo | `config.h` |
| algoritmo de medição | `analysis.cpp` |
| multímetro, ZMPT, INA219 | `multimeter.cpp` |
| proteção elétrica | `safety.cpp` |
| catálogo de componentes | `database.cpp` |
| tela | `ui.cpp` |
| menu | `menu.cpp` |
| widget reutilizável | `uiwidgets.cpp` |
| autoteste, saúde, NVS | `diagnostics.cpp` |
| acesso a pino, PWM, ADC, toque | `hal.cpp` |
