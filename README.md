# Sondvolt v4.0

![Logo](assets/logo.png)

<p align="center">
  <img src="https://img.shields.io/badge/Vers%C3%A3o-v4.0.0-blue.svg" alt="Versão">
  <img src="https://img.shields.io/badge/Licen%C3%A7a-MIT-green.svg" alt="Licença">
  <img src="https://img.shields.io/badge/Plataforma-ESP32-orange.svg" alt="Plataforma">
  <img src="https://img.shields.io/badge/Build-limpo%20com%20--Wall%20--Wextra-brightgreen.svg" alt="Build">
</p>

<p align="center">
  <strong>Testador de componentes e multímetro de bancada para ESP32-2432S028R (Cheap Yellow Display)</strong>
</p>

---

## O que mudou na v4.0

A v4.0 não é uma versão de features — é a versão em que o aparelho passou a **funcionar de verdade**. A v3.2 tinha o código de som, LEDs, banco de dados, calibração e histórico todo escrito e correto, mas **nenhum deles era inicializado**. Além disso, várias medições eram fisicamente impossíveis com a pinagem publicada, e alguns valores exibidos na tela eram texto fixo, não medição.

O [CHANGELOG](docs/CHANGELOG.md) tem a lista completa. Os quatro mais graves:

| # | Problema na v3.2 | Efeito para quem usava |
| :-- | :--- | :--- |
| 1 | `pinMode(GPIO35, OUTPUT)` para excitar as pontas | GPIO34–39 são **entrada apenas** no ESP32. Resistência e capacitância nunca foram medidas de verdade |
| 2 | Proteção elétrica comparava um número de 0 a 1 com limiares em volts | O bloqueio automático **jamais disparava**. A proteção existia no papel |
| 3 | Faixa DC padrão multiplicava a leitura por 181 | Uma pilha de 1,5 V aparecia como 272 V |
| 4 | `buzzer_init()`, `leds_init()`, `db_init()`, `calibration_init()` nunca chamados | Sem som, sem LEDs, banco vazio, calibração perdida a cada boot |

Também foram removidos os **valores falsos** que a interface exibia como se fossem medidos: `ESR: 0.12 Ohms`, `hFE: 245`, `Vbe: 642mV`, `Q: 4.2 @ 1kHz`. Eram strings constantes. Hoje, o que não pode ser medido aparece como `---` com o motivo.

---

## Recursos

### Identificação automática de componentes

Encoste o componente nas pontas e o aparelho decide o que ele é: resistor, capacitor cerâmico ou eletrolítico, diodo de silício ou Schottky, LED (com estimativa de cor pela tensão direta), transistor NPN, MOSFET canal N, indutor ou fio em curto.

### Medições

- **Resistência** — 0,5 Ω a 2 MΩ, auto-range entre dois resistores de referência
- **Capacitância** — 1 nF a 4700 µF pelo método da constante de tempo RC
- **ESR** — resistência série do capacitor, o sintoma que denuncia eletrolítico ressecado
- **Tensão direta (Vf)** — de diodos e LEDs, a 5 mA
- **hFE** — ganho de corrente de transistores bipolares
- **Indutância** — 100 µH a 100 mH pela constante de tempo L/R
- **Frequência e ciclo de trabalho** — de sinais lógicos e PWM
- **Resistência interna de bateria**

### Multímetro

- **Tensão AC True RMS** — 256 amostras, com remoção automática do offset do ZMPT101B e detecção de surto
- **Tensão e corrente DC** — via INA219, com o protocolo I²C correto e as escalas do datasheet
- **Continuidade** com apito, resistência e potência

### Engenharia aplicada

- Código de cores de resistor desenhado na tela a partir do valor medido
- Valor comercial mais próximo nas séries E6, E12 e E24, com o desvio percentual
- Tolerância sugerida (1%, 2%, 5%, 10% ou 20%)
- Notação de engenharia com prefixo SI em todas as telas

### Banco de dados

- **50 componentes reais** em flash, sempre disponíveis, com parâmetros de datasheet: BC547, 2N2222, TIP120, IRFZ44N, 1N4148, 1N4007, LM7805, NE555 e outros
- Consulta ao `COMPBD.CSV` do cartão SD por varredura sob demanda — **5.726 registros sem gastar RAM**
- Busca por valor e sugestão de equivalentes

### Segurança elétrica

- Vigilância contínua da tensão nas pontas fora do modo multímetro
- Bloqueio automático de 10 s após três detecções perigosas seguidas
- Tela de confirmação obrigatória de fusível, varistor e TVS antes do modo multímetro
- Tela de alerta em tela cheia (que na v3.2 existia mas nunca era exibida)

### Diagnóstico

- Autoteste de 10 subsistemas no boot, alimentando a barra de progresso real
- Monitoramento contínuo de heap, pilha das tarefas e temperatura do chip
- Estatísticas de uso persistidas na NVS

---

## Hardware

### Pinagem (CYD Rev A)

| Periférico | Pinos | Observação |
| :--- | :--- | :--- |
| TFT ILI9341 | MOSI 13, MISO 12, SCK 14, CS 15, DC 2, RST 0, BL 21 | fixo na placa |
| Touch XPT2046 | MOSI 32, MISO 39, SCK 25, CS 33 | barramento HSPI dedicado |
| MicroSD | MOSI 23, MISO 19, SCK 18, CS 5 | barramento próprio, **não** compartilhado com a TFT |
| Ponta 1 / Ponta 2 | 35 / 34 | entrada apenas |
| ZMPT101B (AC) | 36 | compartilhado com a IRQ do touch (não usada) |
| **Excitação das pontas** | **27 (10 kΩ) e 22 (470 Ω)** | **novo na v4.0 — obrigatório** |
| Descarga de capacitor | 17 | compartilhado com o LED azul |
| I²C (INA219) | SDA 27, SCL 22 | compartilhado com a excitação |
| OneWire (DS18B20) | 4 | compartilhado com o LED vermelho |
| Buzzer | 26 | |
| LED RGB | R 4, G 16, B 17 | **ânodo comum: nível baixo acende** |

> [!IMPORTANT]
> **O circuito de excitação das pontas é novo e obrigatório.** Sem ele, medir resistência e capacitância é fisicamente impossível — as pontas estão em GPIOs de entrada apenas. O firmware detecta a ausência no boot e desabilita essas funções em vez de mostrar números inventados. O esquema está em [docs/WIRING.md](docs/WIRING.md).

Os três pinos compartilhados são arbitrados em software pela HAL (`hal_bus_acquire` / `hal_bus_release`): o LED é apagado, o pino emprestado, e o LED restaurado ao estado anterior. Quem for montar do zero deve seguir a **Rev B**, sem compartilhamento — veja [docs/PINOUT.md](docs/PINOUT.md).

---

## Instalação

```bash
git clone https://github.com/lelebrr/SondVolt.git
cd SondVolt
pio run -t upload -e cyd          # fiação Rev A (padrão)
pio run -t upload -e cyd-revb     # fiação Rev B, sem pinos compartilhados
```

Formate o MicroSD em FAT32 e copie `sd_files/sdcard/COMPBD.CSV` para a raiz do cartão.

### Pré-requisitos

- PlatformIO no VS Code
- Placa ESP32-2432S028R (CYD)
- Cartão MicroSD FAT32 (opcional: sem ele o catálogo interno de 50 componentes continua funcionando)

---

## Primeiro uso

1. **Ligue.** O autoteste roda sozinho e a barra de boot mostra o que está sendo verificado.
2. **Confira o Diagnóstico.** `Mais > Diagnóstico` lista os 10 subsistemas. Se "Pontas de prova" estiver como FALHA, o circuito de excitação não está montado.
3. **Calibre.** `Mais > Calibrar` mede a resistência dos cabos (pontas encostadas) e a capacitância parasita (pontas afastadas). Leva uns 15 segundos e fica gravado na NVS.
4. **Meça.** `Teste Auto` identifica sozinho. As telas específicas dão mais detalhe.

---

## Segurança

> [!WARNING]
> **Nunca meça componentes em circuito energizado.** Descarregue capacitores antes de testar — o aparelho tem função de descarga própria na tela do capacímetro.
>
> A tensão máxima nas pontas 1 e 2 é **3,3 V**. Para tensões maiores use a entrada específica do multímetro.

> [!IMPORTANT]
> Para medir a rede elétrica (127 V ou 220 V) é **obrigatório** instalar:
> - Fusível rápido de 5 A
> - Varistor 14D431
> - Diodo TVS P6KE400A
> - Filtros RC de amostragem
>
> O firmware exige confirmação dessas peças antes de liberar o modo multímetro e bloqueia o aparelho por 10 segundos se detectar tensão perigosa fora dele. Detalhes em [docs/SAFETY.md](docs/SAFETY.md).

---

## Documentação

**Para começar**
[Manual do Usuário](docs/MANUAL.md) · [Guias](docs/GUIDES.md) · [FAQ](docs/FAQ.md)

**Hardware**
[Pinagem e conflitos](docs/PINOUT.md) · [Esquema de ligação](docs/WIRING.md) · [Especificações](docs/HARDWARE.md) · [Montagem](docs/ASSEMBLY.md) · [Lista de materiais](BOM-Sondvolt.md)

**Referência técnica**
[Componentes](docs/COMPONENTS.md) · [Menus](docs/MENUS.md) · [Configuração](docs/CONFIG.md) · [Segurança](docs/SAFETY.md)

**Desenvolvimento**
[Arquitetura do código](docs/DEVELOP.md) · [Histórico de versões](docs/CHANGELOG.md) · [Solução de problemas](docs/TROUBLESHOOTING.md) · [Contribuindo](docs/CONTRIBUTING.md)

---

## Arquitetura em uma tela

```
                      setup()
                         |
                    hal_init()          <- ADC, LEDC, arbitragem de pinos
                         |
              display / touch / SPI
                         |
        settings_load()  ->  buzzer  ->  leds
                         |
              logger_init()  ->  db_init()
                         |
      measurements / calibration / thermal / multimeter / safety
                         |
                 diag_run_selftest()
                         |
        +----------------+----------------+
        |                                 |
   TaskUI (prio 2)                 TaskMeasurement (prio 1)
   20 ms, 6 KB pilha               100 ms, 4 KB pilha
        |                                 |
   toque, desenho,                  vigilância elétrica,
   som, LEDs                        medição, saúde do sistema
```

Os dois acessos ao display são serializados por um mutex recursivo (`LOCK_TFT`). Todo o hardware específico do ESP32 está isolado em `hal.cpp`.

---

## Licença

MIT — veja [docs/LICENSE.md](docs/LICENSE.md).

<p align="center">
  <strong>Feito para bancada de conserto, não para vitrine.</strong>
</p>
