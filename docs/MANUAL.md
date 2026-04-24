# 📖 Manual do Usuário — Component Tester PRO v3.0

Este manual descreve como operar o seu Component Tester PRO v3.0, desde a navegação básica até medições avançadas.

---

## 📱 Navegação e Interface

A interface foi projetada para ser operada inteiramente via **touchscreen**.

- **Grid de Menu:** A tela principal exibe ícones em um grid de 2x3. Toque em qualquer ícone para abrir a respectiva ferramenta.
- **Barra de Status:** No topo da tela, você verá a versão do firmware e a temperatura interna (se a sonda estiver conectada).
- **Botão Voltar:** Na maioria das telas de medição, toque no ícone de "Seta" ou "X" no canto superior para retornar ao menu.

---

## 🧪 Modos de Medição

### 1. Auto-Detect (Automático)
O modo mais potente. Insira o componente nas pontas de prova e o sistema tentará identificar automaticamente:
- **Resistores:** Valor em Ohms (Ω).
- **Capacitores:** Capacitância (nF/µF) e ESR.
- **Diodos:** Tensão de condução (Vf).
- **Transistores:** Tipo (NPN/PNP), Ganho (hFE) e pinagem.

### 2. Transistor Tester
Específico para análise detalhada de semicondutores. Exibe o símbolo do componente e identifica os terminais (Emissor, Base, Coletor).

### 3. Capacitor ESR
Mede a Resistência em Série Equivalente de capacitores eletrolíticos. Ideal para identificar capacitores "secos" em fontes de alimentação sem precisar retirá-los da placa (em alguns casos).

### 4. Multímetro AC (ZMPT101B)
Mede a tensão da rede elétrica em tempo real.
- **Aviso:** Certifique-se de que o sensor ZMPT está calibrado corretamente.
- **Display:** Exibe Vrms, Vpeak e a frequência estimada.

### 5. Multímetro DC (INA219)
Exibe simultaneamente:
- **Tensão (V):** 0 a 26V.
- **Corrente (A):** Até 3.2A.
- **Potência (W):** Cálculo instantâneo.

---

## 📊 Banco de Dados de Componentes

Ao finalizar uma medição, o sistema consulta o arquivo `COMPBD.CSV` no cartão SD.
- Se houver uma correspondência, o **Part Number** do componente aparecerá em destaque.
- Exemplo: Ao medir um transistor NPN com hFE de ~200, o sistema pode identificar como "BC547".

---

## ⚙️ Configurações (Settings)

Acesse o ícone de engrenagem para ajustar:
- **Brilho:** Ajuste o PWM do backlight (0-100%).
- **Calibração Touch:** Siga os pontos na tela para recalibrar o touchscreen.
- **Buzzer:** Ative ou desative os bipes de confirmação.
- **Limpar Log:** Apaga o histórico salvo no cartão SD.

---

## ⚠️ Avisos de Segurança

1. **Capacitores:** Sempre descarregue capacitores de alta tensão antes de conectá-los ao tester.
2. **Alta Tensão AC:** O módulo ZMPT101B lida com tensões letais. Nunca toque no módulo enquanto ele estiver conectado à rede elétrica.
3. **Polaridade:** O sensor INA219 possui polaridade (+ e -). Inverter a conexão pode danificar o módulo.

---

<p align="center">
  <i>Component Tester PRO v3.0 — Precisão e Tecnologia na sua bancada.</i>
</p>