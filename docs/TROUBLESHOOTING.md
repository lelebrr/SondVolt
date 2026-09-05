# Solução de problemas

Comece sempre por **`Mais > Diagnóstico`**. Essa tela roda um autoteste de 10 subsistemas e diz qual está com problema, o que economiza a maior parte da investigação.

---

## Leitura do Diagnóstico

| Resultado | Significa |
| :--- | :--- |
| **OK** | subsistema presente e respondendo dentro do esperado |
| **AVISO** | presente, mas com algo fora do normal (precisa ajuste) |
| **FALHA** | ausente ou com defeito — a função depende dele |
| **AUSENTE** | opcional e não instalado, o firmware segue sem ele |

---

## Medição de componentes

### "Circuito de pontas ausente" / Pontas de prova em FALHA

**Causa.** O circuito de excitação não está montado, ou o resistor de referência está aberto.

Este é o problema mais comum ao migrar da v3.2, porque **esse circuito não existia**. As pontas estão em GPIO34 e GPIO35, que são entrada apenas no ESP32 e não conseguem aplicar tensão em nada. Sem um pino de saída alimentando o divisor, não há como medir componente passivo.

**Solução.** Monte o circuito descrito em [WIRING.md](WIRING.md): GPIO27 através de 10 kΩ 1% e GPIO22 através de 470 Ω 1%, ambos chegando na ponta 1.

**Como o firmware detecta.** Ele alterna `PIN_PROBE_DRIVE` entre alto e baixo e verifica se a leitura da ponta 1 acompanha. Diferença menor que 500 contas de ADC significa divisor ausente.

### Resistência sempre mostra "OL"

- Pontas realmente abertas — é o comportamento correto
- Cabo de prova rompido: teste encostando as pontas, deve mostrar quase 0 Ω
- Resistor de referência de 10 kΩ aberto

### Resistência com valor errado por um fator constante

O resistor de referência não é o valor nominal. Meça-o com um multímetro confiável e ajuste `PROBE_REF_RESISTOR` em `pins.h`. A precisão do aparelho não pode ser melhor que a do resistor de referência — por isso a recomendação de 1%.

### Resistência com alguns ohms a mais

Falta calibrar. `Mais > Calibrar` mede a resistência dos próprios cabos e desconta de todas as leituras seguintes. Fica gravado na NVS.

### Capacitância sempre zero

1. Descarregue o capacitor primeiro (botão DESCARREGAR na tela do capacímetro)
2. Abaixo de 1 nF está fora da faixa mensurável
3. Capacitor em curto lê como resistor, não como capacitor
4. MOSFET de descarga em curto mantém a ponta aterrada — meça o GPIO17 com o aparelho desligado

### ESR sempre zero

Normal em capacitores cerâmicos e de filme, onde a ESR é baixa demais para o método. A medição de ESR faz sentido em eletrolíticos, que é onde ela denuncia defeito.

### hFE não aparece / transistor não é identificado

- Confira a pinagem. O firmware assume E-B-C e testa NPN primeiro
- Darlingtons (TIP120) têm Vbe de ~1,4 V e podem cair fora da janela de detecção
- Transistor de potência com hFE abaixo de 20 é reportado como SUSPEITO, que muitas vezes é o diagnóstico correto

---

## Multímetro

### Tensão DC absurdamente alta (uma pilha lendo centenas de volts)

**Se você está na v3.2, é o bug conhecido:** a faixa `RANGE_AUTO` caía no ramo do fator de 600 V e multiplicava toda leitura por 181. Corrigido na v4.0.

**Na v4.0**, verifique o divisor externo. `multimeter_set_dc_divider()` precisa refletir a razão real: 1,0 para medida direta, 11,0 para um divisor 10:1.

### Tensão AC aparece com as pontas desconectadas

O trimpot do ZMPT101B está fora do centro. Com a entrada AC desligada, a saída deve repousar em 1,65 V.

`Mais > Diagnóstico`, linha **Sensor AC**, mostra a leitura de repouso em contas. O ideal fica entre 1500 e 2600; fora disso ela reporta "ajustar trimpot".

> A v3.2 assumia o zero fixo em 2048, então qualquer desvio do trimpot virava tensão fantasma. A v4.0 calcula o zero a partir da média das próprias amostras, o que tolera um trimpot razoavelmente desajustado — mas o ajuste ainda melhora a faixa dinâmica.

### Tensão AC lê consistentemente alto ou baixo

Calibração de ganho. Meça a rede com um multímetro de referência e chame `multimeter_calibrate_zmpt(tensaoReal)`. O valor fica gravado na NVS (namespace `mmcal`).

### Corrente e potência indisponíveis

O INA219 não foi detectado no barramento I²C.

- Confira o endereço: 0x40 com todos os jumpers A0/A1 abertos
- Confira SDA no GPIO27 e SCL no GPIO22
- O I²C compartilha pinos com a excitação das pontas; se a arbitragem estiver travada, o autoteste reporta AUSENTE
- Verifique a alimentação de 3,3 V do módulo

### Corrente com valor aleatório

Se você está na v3.2: o código lia o INA219 sem escrever o ponteiro de registrador e sem aplicar as escalas do datasheet. Os números eram lixo. Corrigido na v4.0.

---

## Temperatura

### Sensor térmico AUSENTE

- Resistor de pull-up de 4,7 kΩ obrigatório no barramento OneWire
- Verifique a alimentação do DS18B20
- Na Rev A o OneWire está no GPIO4; na Rev B, no GPIO32. Confira se o firmware foi compilado para a revisão certa

### Temperatura some quando um LED acende

Sintoma clássico da Rev A: GPIO4 é LED vermelho **e** OneWire. Um LED aceso mantém o pino alto e o sensor não responde.

Na v4.0 a HAL apaga o LED antes de cada leitura e o restaura depois, então isso não deveria mais acontecer. Se acontecer, verifique se `hal_init()` está sendo chamado no início do `setup()`.

A solução definitiva é montar a Rev B (`pio run -e cyd-revb`), onde o OneWire tem pino próprio.

---

## Cartão SD

### Cartão SD em AVISO

O aparelho funciona sem cartão — o catálogo interno de 50 componentes fica em flash. O que se perde é o histórico de medições e o banco de 5.726 registros.

- Formate em FAT32 (não exFAT, não NTFS)
- Copie `sd_files/sdcard/COMPBD.CSV` para a **raiz** do cartão
- Cartões acima de 32 GB costumam dar problema

### Cartão detectado mas nada é gravado

- `Ajustes > Salvar histórico` precisa estar ligado
- Cartão protegido contra gravação
- Cartão cheio: o log rotaciona em 256 KB, mas o resto do cartão pode estar ocupado

> Na v3.2 **nada** era gravado, porque `logger_write()` só era chamado por `task_manager.cpp`, que nunca era criado. Corrigido na v4.0.

### Cartão falha intermitentemente

O firmware tenta 10 MHz e cai para 4 MHz automaticamente. Se ainda assim falhar, encurte os fios do cartão ou reduza `SD_SPI_SPEED_MHZ` em `pins.h`.

---

## Display e toque

### Tela preta ao ligar

- Confira o GPIO21 (backlight)
- Cabo flat do display mal encaixado
- Se o LED de alimentação acende mas a tela não, verifique no monitor serial (115200 baud) se o boot chega até `[SYS] Autoteste concluido`

### Imagem deslocada ou com cores erradas

O driver correto para a CYD é `ILI9341_2_DRIVER`, já definido no `platformio.ini`. Se você mudou, volte. Cores invertidas indicam que alguém alterou `tft.invertDisplay()` ou `tft.setSwapBytes()`.

### Toque não responde ou responde no lugar errado

Ajuste as constantes de calibração em `pins.h`:

```c
#define TOUCH_RAW_X_MIN       200
#define TOUCH_RAW_X_MAX       3700
#define TOUCH_RAW_Y_MIN       240
#define TOUCH_RAW_Y_MAX       3800
```

> Na v3.2 existiam **três** conversões de coordenada diferentes — em `main.cpp`, `buttons.cpp` e `safety.cpp` — com constantes e orientações divergentes. Os botões das telas de segurança ficavam espelhados. A v4.0 tem uma única conversão, em `hal_touch_read()`.

### Touchscreen em AVISO: "toque preso ativo"

O XPT2046 está reportando toque contínuo. Quase sempre é cabo flat mal encaixado.

---

## Som e LEDs

### Nenhum som

Na v3.2, `buzzer_init()` nunca era chamado — o canal LEDC nunca era configurado. Corrigido na v4.0.

Na v4.0, verifique `Ajustes > Sons` e `Ajustes > Modo silencioso`.

### O buzzer trava num tom contínuo

`buzzer_update()` não está sendo chamado. Ele vive no laço da tarefa de interface. Na v3.2 não era chamado em lugar nenhum, então o primeiro bipe tocava para sempre.

### LEDs invertidos (acendem quando deveriam apagar)

O LED RGB da CYD é de **ânodo comum**: nível baixo acende. Use `hal_led_write()`, que já trata a polaridade, em vez de `digitalWrite()` direto.

---

## Compilação

### Erro em `static_assert` dentro de `pins.h`

Você alterou a pinagem para uma configuração impossível. As mensagens são explícitas:

```
"PIN_PROBE_DRIVE precisa ter driver de saida"
```

Significa que o pino escolhido está entre GPIO34 e GPIO39, que são entrada apenas. O compilador está impedindo a repetição do bug que travou a v3.2.

### `ledcSetup` ou `ledcAttach` não declarado

Incompatibilidade entre o core 2.x e o 3.x do Arduino-ESP32. Não chame o LEDC direto — use `hal_pwm_attach()`, `hal_pwm_write()` e `hal_pwm_tone()`, que tratam as duas APIs.

O `platformio.ini` fixa `espressif32 @ 6.5.0` justamente para o build ser reproduzível.

### Avisos de macro redefinida

Alguma constante foi definida em dois lugares com valores diferentes. Cada uma tem um único dono: pinagem em `pins.h`, medição e cores em `config.h`, paleta visual em `visual.h`.

### Quero verificar se compila sem ter a placa

```bash
bash tools/hostcheck/check.sh
```

Compila e **linka** todo o firmware no PC com stubs das bibliotecas, nas duas revisões de hardware. Pega erros de sintaxe, tipo e símbolos faltando.

---

## Segurança

### O aparelho travou em "EQUIPAMENTO BLOQUEADO"

Foram detectadas três leituras de tensão perigosa seguidas. O bloqueio dura 10 segundos e libera sozinho.

Se está bloqueando sem motivo, o ZMPT está mal ajustado e reportando tensão fantasma. Veja a seção do trimpot.

### Nunca bloqueia, mesmo com a rede conectada

Na v3.2 o bloqueio **nunca** disparava: `safety_detect_danger()` calculava um número normalizado entre 0 e 1 e comparava com limiares de 50 V, 180 V e 250 V. Corrigido na v4.0.

Confirme que está rodando a v4.0 — a versão aparece em `Mais > Sobre`.

---

## Reiniciando do zero

**Restaurar padrões de fábrica.** `Ajustes > Restaurar padrões` limpa a NVS e recarrega as configurações originais. Não apaga a calibração das pontas.

**Apagar tudo, inclusive calibração.** Apague a flash completa:

```bash
pio run -t erase
pio run -t upload
```

**Reindexar o banco de dados** após trocar o cartão: `Mais > Diagnóstico` mostra a contagem de registros; a reindexação acontece no boot.
