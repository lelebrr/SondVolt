# ✅ Guia de Teste — Sondvolt v3.2

Este guia ensina como testar e validar o Component Tester PRO após a montage a fisik. Siguan langkah-langkah exatamente untuk memverifikasi that semua komponen berfungsi dengan benar.

---

## Índice

1. [Testes Iniciais](#testes-iniciais)
2. [Teste do Display](#teste-do-display)
3. [Teste do Touchscreen](#teste-do-touchscreen)
4. [Teste do SD Card](#teste-do-sd-card)
5. [Teste dos Sensores](#teste-dos-sensores)
6. [Teste dos Modos de Medição](#teste-dos-modos-de-medição)
7. [Teste de Precisão](#teste-de-precisão)
8. [ checklist Final](#checklist-final)

---

## Testes Iniciais

### Material Necessário

- Multímetro digital (para referência)
- Fontede alimentação 5V/2A (USB-C)
- Cabos para probes
- Resistores de valores conhecidos (100Ω, 1kΩ, 10kΩ)
- Capacitores de valores conhecidos (100nF, 1µF, 100µF)
- Diodos 1N4004 ou similares
- Transistor BC547 ou similar

### Sequência de Testes

> ⚠️ **Aviso:** Sempre desligue a alimentação antes de fazer conexões.

```
SEQÜÊNCIA DE TESTES:
┌─────────────────────────────────────────────┐
│ 1. Verificação Visual                      │
│ 2. Teste do Display                         │
│ 3. Teste do Touchscreen                     │
│ 4. Teste do SD Card                         │
│ 5. Teste dos Sensores                       │
│ 6. Teste de Modos de Medição               │
│ 7. Teste de Precisão                        │
│ 8. Checklist Final                         │
└─────────────────────────────────────────────┘
```

---

## Teste do Display

### Objetivo

Verificar se o display TFT está funcionando corretamente.

### Procedimento

1. **Alimente o CYD** — Conecte via USB-C
2. **Observe a tela** — O display deve mostrar algo na inicialização
3. **Verifique backlight** — A tela deve estar iluminada

### Resultados Esperados

| Verificação |OK | Não OK |
|:---|:---:|:---:|
| Telaliga | ✅ | ❌ Verificar conexões GPIO 21 |
|Imagem visível | ✅ | ❌ Verificar conexões SPI |
| Backlight PWM | ✅ | ❌ Problema no GPIO 21 |

### Solução de Problemas

| Problema | Solução |
|:---|:---|
| Tela preta | Verificar conexões do barramento SPI (GPIO 12-15) |
| Imagem corrupta | Verificar conexões do SD Card (pode afetar SPI) |
| Backlight desligado | Verificar GPIO 21 e resistor limitador |

---

## Teste do Touchscreen

### Objetivo

Verificar se o touchscreen resistivo responde ao toque.

### Procedimento

1. **Inicialize o dispositivo** — Aguardeo menu principal
2. **Toque nos botões** — Toque nas áreas do menu
3. **Verifique resposta** — Os botões devem responder ao toque

### Resultados Esperados

| Verificação |OK | Não OK |
|:---|:---:|--:|
| Toque responde | ✅ | ❌ Recalibrar touchscreen |
| Posição precisa | ✅ | ❌ Recalibrar touchscreen |
|Toque múltiplo funciona | ✅ | ❌ Normal (touch resistivo) |

### Solução de Problemas

| Problema | Solução |
|:---|:---|
| Toque não responde | Verificar conexões GPIO 25, 32, 33, 39 |
| Posição incorreta | Acessar Ajustes → Calibrar Touch |
| Toque muito pequeno | Ajustar sensibilidade em Ajustes |

---

## Teste do SD Card

### Objetivo

Verificar se o SD Card é reconhecido e os dados são lidos corretamente.

### Procedimento

1. **Insira o SD Card** — Formato FAT32 com COMPBD.CSV
2. **Ligue o dispositivo** — Observe a tela de boot
3. **Verifique status** — Deve mostrar "SD Card OK"

### Resultados Esperados

| Verificação |OK | Não OK |
|:---|:---:|:---:|
| SD Card detectado | ✅ | ❌ Verificar formato FAT32 |
| COMPBD.CSV carregado | ✅ | ❌ Verificar arquivo na raiz |
| Banco de dados ativo | ✅ | ❌ Verificar formato do CSV |

### Solução de Problemas

| Problema | Solução |
|:---|:---|
| "SD Card N/C" | Verificar formato (deve ser FAT32, não exFAT) |
| "COMPBD.CSV N/E" | Copiar arquivo para a raiz do cartão |
| Erro de leitura | Verificar conexões GPIO 5 (CS) e SPI |

---

## Teste dos Sensores

### Teste do ZMPT101B (Tensão AC c/ Segurança)

1. **Teste de Bloqueio**: Antes de conectar a rede, verifique se ao tentar entrar no Multímetro AC a tela de confirmação de hardware aparece.
2. **Teste de Proteção**: Conecte os probes à rede. Saia do menu multímetro e tente usar o teste de componentes. O sistema deve emitir bip e exibir aviso de alta tensão se a rede estiver presente.
3. **Validação True RMS**: Meça uma rede 220V. O valor exibido deve coincidir com um multímetro CAT III confiável dentro de ±1.5%.

**Valores esperados:** 110V-127V ou 220V-240V AC.

### Teste do INA219 (Corrente/Tensão DC)

1. **Conecte o sensor** — Selon схем в [HARDWARE.md](HARDWARE.md)
2. **Acesse modo Multímetro** — Selecione "Tensão DC" ou "Corrente DC"
3. **Meça uma fonte** — Conecte a carga

**Valores esperados:** Conforme fonte conectada

### Teste do DS18B20 (Temperatura)

1. **Conecte o sensor** — Selon схем в [HARDWARE.md](HARDWARE.md)
2. **Acesse modo Temperatura**
3. **Observe a leitura**

**Valores esperados:** Temperatura ambiente (20-30°C típico)

---

## Teste dos Modos de Medição

### Modo Resistor

1. **Selecione Modo Resistor** — No menu Medir
2. **Conecte um resistor** — Entre os probes
3. **Compare com multímetro**

| Resistor | Leitura Esperada | Tolerância |
|:---|:---:|:---:|
| 100Ω | 95-105Ω | ±5% |
| 1kΩ | 0.95-1.05kΩ | ±5% |
| 10kΩ | 9.5-10.5kΩ | ±5% |
| 100kΩ | 95-105kΩ | ±5% |

### Modo Capacitor

1. **Selecione Modo Capacitor**
2. **Conecte um capacitor** — Observe polaridade( eletrolítico)
3. **Compare com multímetro**

| Capacitor | Leitura Esperada | Tolerância |
|:---|:---:|:---:|
| 100nF | 90-110nF | ±10% |
| 1µF | 0.9-1.1µF | ±10% |
| 100µF | 80-120µF | ±20% |
| 1000µF | 800-1200µF | ±20% |

### Modo Diodo

1. **Selecione Modo Diodo**
2. **Conecte um diodo** — Terminalcatodo (+) ao probe positivo
3. **Observe leitura**

| Diodo | Leitura Esperada |
|:---|:---|
| 1N4004 | 0.6-0.7V |
| 1N5819 | 0.3-0.4V |
| LED Vermelho | 1.8-2.2V |
| LED Verde | 2.0-2.5V |
| LED Azul | 2.5-3.0V |

### Modo Transistor

1. **Selecione Modo NPN ou PNP**
2. **Conecte o transistor** — Conforme pinagem (C, B, E)
3. **Observe identificação**

| Transistor | Resultado Esperado |
|:---|:---|
| BC547 | Identificado como NPN |
| BC557 | Identificado como PNP |
| 2N2222 |Identificado como NPN |
| BD139 | Identificado como NPN (PNP) |

---

## Teste de Precisão

### Tabela de Tolerâncias

| Modo | Precisão Esperada | Condição |
|:---|:---:|:---|
| Resistor | ±2% | >100Ω |
| Capacitor | ±10% | >100nF |
| Tensão DC | ±0.5% | Calibrado |
| Tensão AC (True RMS) | ±1.5% | 128 Amostras |
| Corrente DC | ±1.0% | Calibrado |
| Temperatura | ±0.5°C | DS18B20 |

### Calibração

Se os valores estiverem fora da tolerância:

1. **Acesse Ajustes** → Calibração
2. **Selecione o modo** que precisa calibração
3. **Ajuste Offset e Ganho**
4. **Repita o teste** até estar dentro da tolerância

---

## Checklist Final

Realize todos os testes abaixo antes de considerar o projeto completo:

```
CHECKLIST DE VALIDAÇÃO:
┌─────────────────────────────────────────┐
│ display                                    │
│ ☑ Display TFT liga corretamente             │
│ ☑ Imagem sem distorção                   │
│ ☑ Cores corretas (tema Cyber Dark)        │
│                                            │
│ Touchscreen                                │
│ ☑ Toque responde                         │
│ ☑ Posição precisa                        │
│                                            │
│ Armazenamento                              │
│ ☑ SD Card detectado                      │
│ ☑ COMPBD.CSV carregado                   │
│                                            │
│ Modos de Medição                          │
│ ☑ Resistor (testou com 100Ω, 1kΩ, 10kΩ)  │
│ ☑ Capacitor (testou com 100nF, 1µF)        │
│ ☑ Diodo (testou com 1N4004)              │
│ ☑ Transistor (testou com BC547)         │
│                                            │
│ Sensores                                   │
│ ☑ ZMPT101B mede tensão AC (se instalado) │
│ ☑ INA219 mede tensão DC (se instalado)   │
│ ☑ DS18B20 mede temperatura (se instaldo)│
│                                            │
│ Interface                                  │
│ ☑ Navegação pelo touchscreen             │
│ ☑ Histórico salvo no SD                │
│                                            │
│ Segurança                                  │
│ ☑ Sistema de Bloqueio 50V AC Ativo       │
│ ☑ Tela de Confirmação Hardware OK        │
│ ☑ Proteção contra Surtos Validada        │
│ ☑ Sem aquecimento excessivo              │
│ ☌ Sem cheiro de queimado                 │
│ ☑ Tensões dentro dos limites             │
└──────────────────────────────────────────┘
```

---

## Resultados

Após completar todos os testes, preencha:

| Teste | Status | Observações |
|:---|:---:|:---|
| Display | ✅/❌ | |
| Touchscreen | ✅/❌ | |
| SD Card | ✅/❌ | |
| Resistor | ✅/❌ | |
| Capacitor | ✅/❌ | |
| Diodo | ✅/❌ | |
| Transistor | ✅/❌ | |
| ZMPT101B | ✅/❌ | |
| INA219 | ✅/❌ | |
| DS18B20 | ✅/❌ | |
| Precisão | ✅/❌ | |

---

## Suporte

Se algum teste falhar:

1. Consulte [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. Verifique as conexões em [PINOUT.md](PINOUT.md)
3. Abra uma Issue no GitHub se o problema persist

---

<p align="center">
<i>✅ Última atualização: Abril de 2026 — Sondvolt Team v3.2</i>
</p>