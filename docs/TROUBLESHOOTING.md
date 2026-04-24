# Resolução de Problemas — Sondvolt v3.1

Este guia ajuda a diagnosticar e resolver problemas comuns do Sondvolt.

---

## Diagnóstico Inicial

Antes de prosseguir, verifique:

| Verificação | Como Fazer |
|:---|:---|
| **Alimentação** | Cabo USBfirmware e fuente 5V/2A |
| **Conexões** | Sensores bem encaixados |
| **Serial** | Monitor 115200 baud para logs |

---

## Problemas de Inicialização

### 🔴 Dispositivo não liga (Tela Preta)

| Causa Possível | Solução |
|:---|:---|
| Cabo USB ruim | Use cabo de boa qualidade |
| Fonte insuficiente | Use fonte 5V/2A |
| Porta USB fraca | Tente outra porta USB |
| Firmware corrompido | Recarregue via PlatformIO |

### 🔴 Tela Branca ou Listras

| Causa Possível | Solução |
|:---|:---|
| Driver TFT não configurado | Verifique `platformio.ini` |
| Pinagem incorreta | Confirme definições em `config.h` |
| Curto nos pinos | Inspecione visualmente |

### 🔴 LED Vermelho Piscando

| Causa Possível | Solução |
|:---|:---|
| SD Card ausente | Insira cartão SD |
| Arquivo faltando | Copie COMPBD.CSV para raiz |
| Formatação errada | Reformatar em FAT32 |

---

## Problemas de Toque (Touchscreen)

### 🟡 Toque não responde

| Causa Possível | Solução |
|:---|:---|
| Força insuficiente | Use unha ou stylus (resistivo) |
| Cabo solto | Verifique conexão flat traseiro |
| Calibração errada | Recalibre em Ajustes |

### 🟡 Toque invertido ou deslocado

1. Acesse **Ajustes → Calibrar Touch**
2. Toque nos pontos indicados séquencialmente
3. Reinicie o dispositivo

---

## Problemas de Medição

### 🟡 Probe não detecta componentes (sempre OPEN)

| Causa Possível | Solução |
|:---|:---|
| Pino errado | Use GPIO 35 (CN1-1) |
| Resistor 10kΩ faltando | Instale no divisor |
| Componente danificado | Teste com resistor conhecido |

### 🟡 Leituras imprecisas

| Causa Possível | Solução |
|:---|:---|
| Offset ADC | Calibre em Ajustes → Calibração |
| Ruído eletromagnético | Afaste fios de sinal |
| Referência ruim | GND compartilhado |

### 🟡 Multímetro AC (ZMPT101B) instável

| Causa Possível | Solução |
|:---|:---|
| Potenciômetro desalinhado | Ajuste o azul no módulo |
| Escala digital | Ajuste ZMPT Scale em Ajustes |
| Ruído | Afaste fios AC dos fios de dados |
| Alimentação instável | Use 5V estável |

### 🟡 INA219 não detectado

| Causa Possível | Solução |
|:---|:---|
| Endereço I2C | Verifique endereço 0x40 |
| Conexão I2C | SDA→GPIO 27, SCL→GPIO 22 |
| Pull-ups | Adicione 4.7kΩ se necessário |
| Alimentação | Use 3.3V (não 5V!) |

### 🟡 DS18B20 não responde

| Causa Possível | Solução |
|:---|:---|
| Pull-up faltando |Instale resistor 4.7kΩ |
| Pino errado | Use GPIO 4 |
| Cabo longo | Máximo 100m (reduza) |

---

## Problemas de Cartão SD

### 🔴 "SD Card: Erro na inicialização"

| Causa Possível | Solução |
|:---|:---|
| Formatação errada | Formate em FAT32 |
| Capacidade | Use até 32GB |
| Arquivo faltando | COMPBD.CSV na raiz |
| Slot sujo | Limpe com ar comprimido |

### 🔴 Banco de dados não carrega

1. Verifique se `COMPBD.CSV` está na raiz
2. Confirme formato UTF-8
3. Verifique separadores (vírgula)

---

## Problemas de Áudio e Vídeo

### 🟡 Buzzer não funciona

| Causa Possível | Solução |
|:---|:---|
| Modo silencioso | Desative em Ajustes |
| Conexão speaker | Verifique GPIO 26 |
| Tomador incorreto | Ajuste frequência |

### 🟡 Brilho não altera

| Causa Possível | Solução |
|:---|:---|
| PWM conflitando | Use apenas brilho em Ajustes |
| Hardware | Verifique GPIO 21 |

### 🟡 LEDs RGB não funcionam

- LED Verde: GPIO 16
- LED Vermelho: GPIO 17
- LED Azul: GPIO 4 (compartilha com OneWire)

---

## Códigos de Erro do Serial

Conecte ao PC e abra o Monitor Serial (115200 baud):

| Mensagem | Significado | Ação |
|:---|:---|:---|
| `SD Card: OK` | Cartão detectado | ✅ Normal |
| `SD Card: ERRO` | Falha na inicialização | Verifique SD |
| `DB: OK` | Banco de dados carregado | ✅ Normal |
| `DB: ERRO` | Arquivo não encontrado | Copie COMPBD.CSV |
| `INA219: OK` | Sensor DC detectado | ✅ Normal |
| `INA219: ERRO` | Sensor não responde | Verifique conexões |
| `ZMPT: OK` | Sensor AC detectado | ✅ Normal |
| `ZMPT: ERRO` | Sem sinal AC | Verifique sensor |
| `DS18B20: OK` | Sonda detectada | ✅ Normal |
| `DS18B20: ERRO` | Sem resposta | Verifique OneWire |

---

## Tabela de Problemas e Soluções

| Problema | Causa | Solução |
|:---|:---|:---|
| Não liga |Fonte/USB | Use fonte 5V/2A |
| Tela branca | Driver | Verifique config TFT |
| Sem toque | Resistivo | Use unha/stylus |
| OPEN constante | Pino | Use GPIO 35 |
| AC instável | Calibração | Ajuste ZMPT101B |
| INA219 erro | I2C | SDA→27, SCL→22 |
| SD erro | Formato | FAT32, COMPBD.CSV |
| Bipes | Modo | Desative silencioso |

---

## Reset de Fábrica

### Via Software

1. Acesse **Ajustes → Limpar Tudo**
2. Confirme a ação
3. O dispositivo reiniciará

### Via Hardware

```bash
pio run -e cyd --target erase
pio run -e cyd --target upload
```

> ⚠️ Isso apaga todas as configurações e dados.

---

##Avisos de Segurança

> **🔴 PERIGO:** Não opere com as mãos molhadas ao medir AC.
> **⚠️ ATENÇÃO:** Sempre desconecte antes de manipular conexões.

---

<p align="center">
<i>Component Tester PRO v3.0 — Resolução de Problemas</i>
</p>