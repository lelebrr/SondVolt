# Guia de Componentes - Component Tester PRO v3.0 (CYD Edition)

## 1. Introdução

Este guia descreve como testar cada tipo de componente eletrônico usando a plataforma ESP32 (CYD). Diferente da v2.0, esta versão utiliza um **ADC de 12 bits (4095 níveis)** e opera em lógica de **3.3V**.

### Configuração de Probes
- **Probe Principal (Signal):** Conectado ao **GPIO 35**.
- **Probe Comum (GND):** Conectado ao **GND** da placa.

---

## 2. Resistores

### Como Testar
1. Conecte o resistor entre o **Probe Principal** e o **GND**.
2. No menu **Medir**, selecione **"Resistor"**.
3. O valor será exibido em Ohms (Ω), Kilo-Ohms (kΩ) ou Mega-Ohms (MΩ).

### Resultados
- **Valor numérico:** Resistor OK.
- **"OPEN":** Resistor aberto ou valor acima de 1MΩ.
- **"SHORT":** Resistor em curto ou valor próximo de 0Ω.

---

## 3. Capacitores

> [!IMPORTANT]
> **DESCARGA:** Sempre descarregue capacitores eletrolíticos antes do teste para evitar danos ao GPIO 35.

### Como Testar
1. Conecte o capacitor entre o **Probe Principal** e o **GND**.
2. Respeite a polaridade para eletrolíticos (Positivo no Probe Principal).
3. Selecione **"Capacitor"** no menu.

### Limites
- **Mínimo:** ~1nF (devido à capacitância parasita do ESP32).
- **Máximo:** ~4700µF.

---

## 4. Diodos e LEDs

### Como Testar
1. Conecte o Ânodo no **Probe Principal** e o Cátodo no **GND**.
2. Selecione **"Diodo/LED"**.
3. O sistema medirá a **Tensão Direta (Vf)**.

### Tabela de Vf Típica (3.3V)
| Componente | Vf Típico |
|:---|:---|
| Diodo Silício (1N4148) | 0.6V - 0.7V |
| Diodo Schottky | 0.2V - 0.4V |
| LED Vermelho | 1.8V - 2.0V |
| LED Verde/Azul | 2.5V - 3.2V |

---

## 5. Transistores (BJT NPN)

### Como Testar
1. Conecte o **Coletor** ao Probe Principal, a **Base** ao pino de sinal secundário (se configurado) e o **Emissor** ao GND.
2. O sistema v3.0 identifica o ganho **hFE (Beta)**.

---

## 6. Sonda Térmica (DS18B20)

### Como Testar
1. Conecte a sonda ao conector JST de 4 pinos (utilizando o **GPIO 4**).
2. Acesse o menu **Termica**.
3. O gráfico de barras mostrará a temperatura em tempo real.

---

## 7. Multímetro Profissional (Novo)

### Tensão AC (RMS)
- **Sensor:** ZMPT101B conectado ao **GPIO 34**.
- **Função:** Mede tensões de 0-250V AC com isolação total.

### Tensão e Corrente DC
- **Sensor:** INA219 (I2C - GPIO 27/22).
- **Função:** Mede tensão (0-26V), corrente (até 3.2A) e potência (W).

---

## 8. Gerador de Sinal (PWM)

- **Saída:** O sinal é gerado no **GPIO 26** (mesmo pino do speaker, desconecte o speaker se for usar como gerador puro).
- **Frequência:** Ajustável via código ou fixo em 1kHz no menu.