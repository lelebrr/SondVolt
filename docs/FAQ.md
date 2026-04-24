# Frequently Asked Questions (FAQ) - Component Tester PRO v2.0

## Índice

1. [Geral](#1-general)
2. [Hardware](#2-hardware)
3. [Medições](#3-measurements)
4. [Problemas](#4-problems)
5. [Software](#5-software)
6. [Avançado](#6-advanced)

---

## 1. Geral

### Q: O que é o Component Tester?

**R:** É um dispositivo para testar e identificar componentes eletrônicos. Você conecta um componente e ele mostra o valor ou se está funcionando.

---

### Q: Quais componentes ele pode testar?

**R:** Ele testa 13 tipos diferentes:
- Resistores
- Capacitores
- Diodos e LEDs
- Transistores NPN
- Indutores
- Tensão DC
- Frequência
- PWM
- Optocouplers
- Cabos (continuidade)
- Pontes retificadoras
- Auto-detecção
- Continuidade com beep

---

### Q: É difícil de usar?

**R:** Não! É super simples:
1. Conecte o componente aos probes
2. Selecione o tipo de medição
3. Veja o resultado na tela

Temos um [MANUAL.md](MANUAL.md) completo para iniciantes.

---

### Q: Posso usar sem conhecimento prévio?

**R:** Sim! O manual foi escrito especialmente para quem nunca usou um testador antes. Cada passo está explicado.

---

## 2. Hardware

### Q: O que preciso comprar?

**R:** Lista básica:
- Arduino Uno R3
- TFT 2.8" ILI9341
- Módulo SD Card
- Sonda DS18B20
- 6 botões push
- LED verde e vermelho
- Buzzer 5V
- Resistores, fios, probes

Veja [HARDWARE.md](HARDWARE.md) para lista completa.

---

### Q: Posso usar outro display?

**R:** O código foi desenvolvido para TFT 2.8" ILI9341. Outros displays requeririam mudanças no código.

---

### Q: Os probes são inclusos?

**R:** Não, você precisa comprar ou fazer seus probes. Recomendamos probes com garra jacaré ou pinças SMD.

---

### Q: Posso usar pilhas AA?

**R:** Não diretamente. O Arduino Uno precisa de 7-12V DC. Use uma fonte regulated ou adapte uma bateria com um conversor boost.

---

### Q: Qual a precisão das medições?

**R:** Aproximadamente ±5% para компоненты passivos, dependendo do componente e、温度. Para maior precisão, use multímetro profesional.

---

## 3. Medições

### Q: Por que meu capacitor mostra 0?

**R:** Possíveis causas:
1. Capacitor muito pequeno (< 1nF)
2. Capacitor vazado/queimado
3. Conexão ruim

Tente limpar as pontas e reconnectar.

---

### Q: Resistor mostra OPEN mas funciona?

**R:**
1. Verifique a conexão
2. Tente inverter os probes
3. O resistor pode estar fora do range

---

### Q: Transistor não é detectado?

**R:** Este testador só detecta transistores BJT NPN. Para PNP ou MOSFET, precisa de outro equipamento.

---

### Q: Posso testar LEDs RGB?

**R:** Sim! Conecte cada terminal (R, G, B, comum) separadamente. Cada LED interno será detectado.

---

### Q: E capacitores SMD?

**R:** Sim, use pinças crocodilo ou probes finos. O valor pode não aparecer se muito pequeno.

---

### Q: Testa bateria?

**R:** Não diretamente. Use a função Volt DC para verificar tensão de baterias (0-5V apenas).

---

## 4. Problemas

### Q: Tela fica preta?

**R:**
1. Verifique conexões do TFT
2. Verifique backlight (3.3V)
3. Recarregue o firmware

Veja [TROUBLESHOOTING.md](TROUBLESHOOTING.md).

---

### Q: Botões não funcionam?

**R:**
1. Verifique conexões
2. Use multímetro para testar
3. O resistor 10kΩ pode estar queimado

---

### Q: Medições estão erradas?

**R:**
1. Recalibre (Menu > Config > Calibrar)
2. Verifique probes
3. A temperatura afeta

---

### Q: Sonda térmica não funciona?

**R:**
1. Verifique pino A3
2. Use resistor 4.7kΩ pull-up
3. Biblioteca pode faltando

---

### Q: Não guarda configurações?

**R:** Sempre selecione "5. Salvar Cfg" após mudar configurações!

---

### Q: SD Card não lê/grava?

**R:**
1. Formate em FAT16/32
2. Verifique pino CS (D4)
3. Biblioteca correta?

---

## 5. Software

### Q: Como compilo?

**R:**
```bash
pio run
```

---

### Q: Como carrego o firmware?

**R:**
```bash
pio run --target upload
```

---

### Q: Preciso de bibliotecas?

**R:**
- Adafruit ILI9341
- Adafruit GFX
- OneWire
- DallasTemperature
- SD (built-in)

---

### Q: Posso modificar o código?

**R:** Sim! O código está disponível. See [DEVELOP.md](DEVELOP.md) para detalhes.

---

### Q: Como atualizo o firmware?

**R:**
```bash
pio run --target upload
```
Isso sobrescreve o anterior. Configurações na EEPROM são preservadas.

---

### Q: Posso adicionar mais funções?

**R:** A memória está quase cheia (96%), mas pequenas alterações são possíveis.

---

## 6. Avançado

### Q: Como adiciono novos testes?

**R:** Edite measurements.cpp e adicione ao menu em measurements.cpp.

---

### Q: Como mudo os pinos?

**R:** Edite config.h com novos pinos.

---

### Q: Posso adicionar display maior?

**R:** Precisa alterar código. Displays maiores requerem mais memória.

---

### Q: Como funciona a auto-detecção?

**R:** Testa propriedades em ordem (forward voltage, resistência, etc.) e retorna o primeiro match.

---

## Quick Reference

| Pergunta | Resposta Rápida |
|----------|-----------------|
| Não liga | Verifique energia e conexões |
| Tela vazia | Verifique TFT e backlight |
| Botões travados | Recarregue firmware |
| Medições erradas | Recalibre |
| Não salva | Selecione Salvar Cfg |
| Mais help | See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |

---

## Perguntas Não Listas Aqui?

1. Leia [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. Verifique [MANUAL.md](MANUAL.md)
3. Check [COMPONENTS.md](COMPONENTS.md)

---

*More questions? Open an issue on GitHub or contact the developer.*