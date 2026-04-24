# Guias Passo a Passo - Component Tester PRO v3.0 (CYD Edition)

## 1. Guia de Medição de Componentes (Modo Clássico)

Este guia cobre os 13 modos disponíveis no menu **Medir**.

### 🛠️ Resistores e Capacitores
1. Conecte o componente entre o **Probe Principal (GPIO 35)** e o **GND**.
2. No menu, use as zonas de toque (Topo/Base) para selecionar `Res` ou `Cap`.
3. Toque no **Centro** da tela para ler.
4. **Capacitores:** Sempre descarregue antes de conectar!

### 🚥 Diodos e LEDs
1. Conecte o **Ânodo** ao Probe Principal e o **Cátodo** ao GND.
2. Selecione `Diod` no menu.
3. Se for um LED, ele deve acender levemente durante o teste. A tela exibirá a tensão direta (Vf).

### 📐 Transistores (BJT NPN)
1. Conecte o **Coletor** ao Probe Principal e o **Emissor** ao GND.
2. O sistema identificará automaticamente o ganho (hFE).

---

## 2. Guia do Multímetro Profissional (Novo)

### ⚡ Medindo Tensão AC (Rede Elétrica)
1. Conecte o módulo **ZMPT101B** à rede elétrica (L/N).
2. Conecte a saída do módulo ao **GPIO 34**.
3. Acesse o menu **Multimetro**.
4. O valor **True RMS** será exibido com uma barra de intensidade.
5. > [!CAUTION]
   > Nunca toque nos terminais AC enquanto o circuito estiver energizado.

### 🔋 Medindo Corrente e Potência DC
1. Conecte o sensor **INA219** via barramento I2C (GPIO 27/22).
2. Intercale o sensor na linha positiva da carga que deseja medir.
3. A tela exibirá Tensão (V), Corrente (mA) e Potência (W) em tempo real.

---

## 3. Guia de Monitoramento Térmico

1. Conecte a sonda **DS18B20** ao **GPIO 4**.
2. Acesse o menu **Termica**.
3. O sistema exibirá a temperatura atual e um histórico visual rápido.
4. Alertas de cor (Verde -> Amarelo -> Vermelho) indicam se o componente está superaquecendo.

---

## 4. Guia de Calibração do Sistema

1. Acesse o menu **Config**.
2. Para calibar o ADC: Selecione `Calibrar` e garanta que os probes estão desconectados.
3. Para calibrar o AC: Compare a leitura de `ZMPT` com um multímetro externo e ajuste o `Scale` no menu até os valores baterem.
4. As configurações são salvas instantaneamente na memória **NVS**.

---

## 5. Guia de Identificação por Banco de Dados

1. Certifique-se de que o **MicroSD** está inserido com o arquivo `COMPBD.CSV`.
2. Use o modo **Scanner** ou **Auto Detect**.
3. Quando um componente é medido, o ESP32 busca no banco de dados em RAM e exibe o Part Number (ex: "BC547") se houver um match de parâmetros.