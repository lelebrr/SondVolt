# Esquema de ligação

Este documento descreve o circuito externo que o Sondvolt v4.0 precisa. A parte mais importante é o **circuito de excitação das pontas**, que não existia nas versões anteriores e sem o qual medir resistência e capacitância é impossível.

---

## Por que existe um circuito de excitação

As pontas de prova estão ligadas a GPIO35 e GPIO34. Esses pinos do ESP32 são **entrada apenas** — não conseguem aplicar tensão em nada. Para medir um componente passivo é preciso injetar uma corrente conhecida e observar a queda de tensão, e essa injeção precisa vir de um pino com driver de saída.

A v3.2 tentava fazer isso direto pelo GPIO35 e a chamada simplesmente não surtia efeito. O resultado eram números que variavam com o ruído ambiente.

---

## Circuito de medição de componentes

```
                 GPIO27  (drive faixa alta)
                    │
                  ┌─┴─┐
                  │10k│  R1  1%  ← referência para 1 kΩ a 2 MΩ
                  └─┬─┘
                    │
                 GPIO22  (drive faixa baixa)
                    │
                  ┌─┴─┐
                  │470│  R2  1%  ← referência para 0,5 Ω a 2 kΩ
                  └─┬─┘
                    │
     GPIO35 ────────┼──────────────► PONTA 1  (jaque banana vermelho)
    (leitura)       │
                    │
                 COMPONENTE
                    │
     GPIO34 ────────┼──────────────► PONTA 2  (jaque banana preto)
    (leitura)       │
                    │
                  ┌─┴─┐
       GPIO17 ───►│ Q1│  MOSFET canal N (2N7000 ou BS170)
    (descarga)    └─┬─┘  dreno na ponta 1, fonte no GND
                    │
                   GND
```

### Como o firmware usa isso

**Resistência.** Aplica nível alto no drive escolhido e lê a tensão na ponta 1. O componente forma um divisor com o resistor de referência:

```
Rx = Rref × V / (Vcc − V)
```

Com auto-range: começa no resistor de 10 kΩ, e se o resultado ficar abaixo de 2 kΩ repete no de 470 Ω, onde a resolução é melhor.

**Capacitância.** Descarrega pelo MOSFET, aplica o drive e cronometra até a tensão chegar a 63,2% da alimentação. Nesse ponto `t = R × C`, então `C = t / R`.

**ESR.** Um capacitor descarregado se comporta como curto no primeiro instante. A tensão que sobra imediatamente após um pulso de 25 µs é a queda na resistência série. Usa o resistor de 470 Ω e uma leitura única, sem média — a média destruiria o transitório.

**Tensão direta.** Com o resistor de 470 Ω circulam cerca de 5 mA, corrente de teste típica para uma junção de silício. A tensão lida na ponta 1 é o Vf.

**hFE.** Base pelo resistor de 10 kΩ (Ib ≈ 0,26 mA), coletor pelo de 470 Ω. Mede a tensão do coletor com a base ativa e sem ela; a diferença entre os dois estados é a assinatura de um dispositivo com ganho.

### Lista de peças deste bloco

| Qtd | Componente | Valor | Observação |
| :-- | :--- | :--- | :--- |
| 1 | Resistor | 10 kΩ 1% | referência da faixa alta — a precisão dele é a precisão do aparelho |
| 1 | Resistor | 470 Ω 1% | referência da faixa baixa |
| 1 | MOSFET canal N | 2N7000 ou BS170 | descarga de capacitor |
| 1 | Resistor | 100 kΩ | pull-down do gate do MOSFET |
| 2 | Jaque banana 4 mm | vermelho e preto | pontas de prova |

> [!TIP]
> Use resistores de 1% ou melhor. Um resistor de 5% no lugar do de referência limita a precisão de todas as medições a 5%, por melhor que seja o firmware.

### Verificação

O firmware testa esse circuito sozinho no boot: alterna `PIN_PROBE_DRIVE` entre alto e baixo e verifica se a leitura da ponta 1 acompanha. Se a diferença for menor que 500 contas de ADC, conclui que o divisor não está montado e desabilita as medições de componente — mostrando "Circuito de pontas ausente" em vez de números inventados.

Confira em `Mais > Diagnóstico`, linha **Pontas de prova**.

---

## Sensor de tensão AC (ZMPT101B)

```
  REDE 127V/220V
       │
       ├──► [ FUSÍVEL RÁPIDO 5A ] ──► [ CHAVE LIGA/DESLIGA ]
       │                                        │
       │        ┌───────────────────────────────┴────┐
       │        │      BLOCO DE PROTEÇÃO             │
       │        │   [VARISTOR 14D431]                │
       │        │   [DIODO TVS P6KE400A]             │
       │        └───────────────┬────────────────────┘
       │                        │
       └────────────────────────┴──► ENTRADA DO ZMPT101B
                                            │
                                     saída analógica
                                            │
                                         GPIO36
```

> [!WARNING]
> O fusível, o varistor e o diodo TVS **não são opcionais**. Solde o varistor e o TVS diretamente nos terminais de entrada do módulo ZMPT101B, com as pernas o mais curtas possível — a indutância parasita de um fio longo anula a proteção contra transientes rápidos.

### Ajuste do trimpot

Com a entrada AC desconectada, ajuste o trimpot do ZMPT101B até que a saída DC fique em **1,65 V** (meia escala do ADC de 3,3 V). O autoteste verifica isso: em `Mais > Diagnóstico`, a linha **Sensor AC** mostra "ajustar trimpot" se a leitura de repouso estiver fora da faixa de 1500 a 2600 contas.

### Calibração de ganho

O ganho padrão (`ZMPT_DEFAULT_GAIN`, 0,3707 V por conta) serve como ponto de partida. Para calibrar de verdade, meça a rede com um multímetro de referência e use `multimeter_calibrate_zmpt(tensaoReal)`. O valor fica gravado na NVS.

---

## Sensor de corrente (INA219)

```
   FONTE ──► [ Vin+ ]  INA219  [ Vin− ] ──► CARGA
                          │
                   SDA ── GPIO27
                   SCL ── GPIO22
                   VCC ── 3V3
                   GND ── GND
```

O shunt padrão dos módulos comerciais é de 0,1 Ω, o que dá alcance de ±3,2 A. O firmware configura o registrador de calibração para um LSB de corrente de 100 µA.

Endereço I²C: **0x40** (todos os jumpers A0/A1 abertos).

> [!NOTE]
> O barramento I²C compartilha pinos com a excitação das pontas. Isso é intencional: os dois saem pelo conector P3. O firmware faz arbitragem temporal e nunca aciona os dois ao mesmo tempo.

---

## Sonda térmica (DS18B20)

```
   3V3 ──┬──[ 4k7 ]──┬── DQ ── GPIO4  (Rev A)  ou  GPIO32 (Rev B)
         │           │
       VDD          DS18B20
         │           │
        GND ────────GND
```

O resistor de pull-up de 4,7 kΩ é obrigatório no barramento OneWire.

Na Rev A o GPIO4 é compartilhado com o LED vermelho. O firmware apaga o LED antes de cada leitura e o restaura depois — mas se você estiver montando do zero, use a Rev B e evite o problema.

---

## Alimentação e cores de fio

| Barramento | Cor | Bitola | Uso |
| :--- | :--- | :--- | :--- |
| Fase AC | Marrom | 18 AWG | entrada de energia |
| Neutro AC | Azul claro | 18 AWG | retorno |
| +5 V | Vermelho | 22 AWG | alimentação de sensores |
| +3,3 V | Laranja | 24 AWG | OneWire, lógica |
| GND | Preto | 22 AWG | referência comum |
| I²C SDA | Amarelo | 26 AWG | dados |
| I²C SCL | Verde | 26 AWG | clock |
| Analógico | Roxo | 26 AWG | sinais de medição |

Use par trançado nas linhas SDA/SCL para reduzir interferência, e mantenha os fios analógicos longe dos de rede elétrica.

---

## Checklist de montagem

- [ ] Resistores de referência de 10 kΩ e 470 Ω são de 1% ou melhor
- [ ] MOSFET de descarga com pull-down de 100 kΩ no gate
- [ ] Fusível de 5 A instalado e testado
- [ ] Varistor e TVS soldados direto no ZMPT, pernas curtas
- [ ] Pull-up de 4,7 kΩ no barramento OneWire
- [ ] Capacitor de 100 nF de desacoplamento junto a cada módulo
- [ ] Isolação mínima de 10 mm entre bornes AC e bornes DC
- [ ] Trimpot do ZMPT ajustado para 1,65 V em repouso
- [ ] Cartão MicroSD formatado em FAT32 com `COMPBD.CSV` na raiz
- [ ] `Mais > Diagnóstico` sem nenhuma linha em FALHA
