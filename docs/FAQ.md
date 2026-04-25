# ❓ Perguntas Frequentes — Sondvolt v3.2

Aqui você encontra as respostas para as dúvidas mais comuns sobre montagem, operação e manutenção do Sondvolt.

---

## Hardware e Montagem

### 1. O que é necessário para começar?

Você precisa dos seguintes componentes:

| Item | Necessário | Notas |
|:---|:---:|:---|
| ESP32-2432S028R (CYD) | ✅ Obrigatório | Placa base |
| Cartão MicroSD (8GB+) | ✅ Recomendado | FAT32, COMPBD.CSV |
| ZMPT101B | ❌ Opcional | Multímetro AC |
| INA219 | ❌ Opcional | Multímetro DC |
| DS18B20 | ❌ Opcional | Sonda de temperatura |
| Cabos e jumpers | ❌ Opcional | Conexões |

Consulte o [Guia de Hardware](HARDWARE.md) para detalhes.

### 2. A tela não responde ao toque. O que fazer?

O touchscreen da CYD é **resistivo**, não capacitivo. Isso significa:

- **Toque:** Requer pressão física (use a unha ou stylus)
- **Calibração:** Acesse **Ajustes → Calibrar Touch** se estiver desalinhado
- **Hardware:** Verifique o cabo flat do touchscreen (atrás do LCD)

### 3. O cartão SD não é reconhecido

Verifique os seguintes pontos:

| Problema | Solução |
|:---|:---|
| Formatação errada | Deve ser **FAT32**, não exFAT |
| Arquivo faltando | `COMPBD.CSV` deve estar na raiz |
| Slot solto | Insira com a placa desligada |
| Cartão corrompido | Reformatar e recarregar arquivo |

### 4. Quais pinos posso usar para expansão?

Os pinos livres para expansão são:

- **GPIO 4:** OneWire (DS18B20)
- **GPIO 22:** I2C SCL
- **GPIO 27:** I2C SDA
- **GPIO 34:** Entrada ADC (ZMPT101B)
- **GPIO 35:** Entrada ADC (Probe)

---

## Medições e Sensores

### 5. As leituras AC estão instáveis

O sensor ZMPT101B requer calibração:

1. Ajuste o potenciômetro azul no módulo para uma tensão conhecida
2. Ajuste o `ZMPT Scale` no menu **Ajustes**
3. Use a fórmula: `EscalaNova = leituraMultímetro / leituraCYD`

### 6. O Probe não detecta componentes

Verifique:

1. **Conexão:** Componente entre Probe (+) e GND
2. **Resistor 10kΩ:** Instalado no divisor de tensão?
3. **Componente:** Teste com um resistor conhecido (10kΩ)

### 7. Como funciona a identificação automática?

O sistema:
1. Mede os parâmetros básicos (Vf, hFE, capacitância)
2. Cruza os dados com `COMPBD.CSV`
3. Exibe o Part Number se houver correspondência

### 8. Posso usar em outro display ESP32?

Sim, mas será necessário:
1.リwirPinagem manual do display ILI9341
2. Alterar definições em `src/config.h`
3. O código foi otimizado para a CYD

---

## Banco de Dados e Software

### 9. Como atualizar o banco de dados?

1. Remova o cartão SD
2. Copie `COMPBD.CSV` para o computador
3. Abra em Excel ou editor de texto
4. Adicione seguindo o formato:

```
Tipo,hFE_Min,hFE_Max,Vf_Min,Vf_Max,PartNumber
NPN,100,200,0.6,0.7,BC547
```

5. Salve em CSV (UTF-8) e copie de volta

### 10. Como adicionar novos modos de medição?

1. Adicione o modo em `menu.cpp`
2. Implemente a função em `measurements.cpp`
3. Adicione ao handler em `measurements_handle()`

---

## Segurança

### 11. Posso medir diretamente a rede elétrica?

> **🔴 PERIGO:** Não conecte fios nu diretamente ao ESP32.
> Use sempre o módulo **ZMPT101B** que possui isolação galvânica.

### 12. Quais são os limites de medição?

| Sistema | Limite Máximo |
|:---|:---:|
| AC (ZMPT) | 250V RMS |
| DC (INA219) | 26V / 3.2A |
| Probe (GPIO) | 3.3V máximo |

---

## Troubleshooting Rápido

| Sintoma | Causa | Solução |
|:---|:---|:---|
| Tela branca | Driver não carregou | Verifique PlatformIO |
| LED vermelho piscando | SD não detectado | Insira cartão com COMPBD.CSV |
| Toque invertido | Não calibrado | Acesse Ajustes |
| Reset cíclico | Alimentação fraca | Use fonte 5V/2A |
| Bipes constantes | Temperatura alta | Afaste DS18B20 |

---

<p align="center">
<i>Ainda tem dúvidas? Consulte o <b>Manual do Usuário</b> ou abra uma <b>Issue</b> no GitHub.</i>
</p>