# 📱 Estrutura de Menus — Component Tester PRO

A interface do v3.0 é baseada em um sistema de **Grid Adaptativo**, otimizado para o display de 2.8" e navegação touchscreen.

---

## 1. Menu Principal (Grid 2x4)

O menu principal organiza as ferramentas em blocos visuais. Atualmente, temos **7 ferramentas** principais:

| Coluna A | Coluna B |
|:---:|:---:|
| **⚡ Medir** | **🌡️ Térmica** |
| **🔍 Scanner** | **🔌 Multímetro** |
| **📜 Histórico** | **📊 Estatísticas** |
| **⚙️ Ajustes** | *(Vazio)* |

### Descrição dos Itens:
1. **Medir:** Acesso ao menu secundário com os 13 modos clássicos de teste.
2. **Térmica:** Monitoramento em tempo real via sonda DS18B20 com gráfico de tendência.
3. **Scanner:** Modo "Auto-detect" contínuo para identificação rápida.
4. **Multímetro:** Medições profissionais de AC (RMS), DC (V/I/P) via sensores externos.
5. **Histórico:** Visualização das últimas 50 medições salvas na memória.
6. **Estatísticas:** Dados de uso, taxa de falha de componentes e saúde do ESP32.
7. **Ajustes:** Configurações de brilho, som, tempo de timeout e calibração.

---

## 2. Menu de Medições (13 Modos)

Ao entrar em **Medir**, você terá acesso a uma lista vertical com as seguintes ferramentas:

| # | Modo | Uso Recomendado |
|:---:|:---|:---|
| 1 | **Capacitor** | Capacitores de poliéster, cerâmicos e eletrolíticos. |
| 2 | **Resistor** | Resistores de precisão e potência. |
| 3 | **Diodo/LED** | Teste de semicondutores e verificação de LEDs. |
| 4 | **Transistor** | Identificação automática de pinagem e ganho (hFE). |
| 5 | **Indutor** | Medição de bobinas e transformadores. |
| 6 | **Volt DC** | Voltímetro básico via ADC (0-3.3V). |
| 7 | **Frequência** | Medição de sinais digitais e osciladores. |
| 8 | **PWM Out** | Geração de sinais para teste de drivers. |
| 9 | **Optoacopl.** | Teste de isoladores ópticos de 4 pinos. |
| 10 | **Cabo/Cont.** | Teste de continuidade sonoro (Buzzer). |
| 11 | **Ponte Ret.** | Teste de pontes de diodo completas. |
| 12 | **Auto Detect** | Identificação inteligente (Modo Principal). |
| 13 | **ESR Meter** | Resistência interna de capacitores (in-circuit). |

---

## 3. Navegação por Gestos (Zonas de Toque)

Embora os ícones sejam clicáveis, você pode navegar usando gestos nas bordas da tela:

- **TOPO:** Desce o cursor/menu.
- **BASE:** Sobe o cursor/menu.
- **CENTRO:** "OK" / Selecionar.
- **CANTO INF. DIREITO:** "Back" / Voltar / Cancelar.

---

## 4. Barra de Status (Topo)

Exibida em todas as telas, fornece informações críticas:
- **FW Version:** Versão atual do sistema.
- **Temp:** Temperatura da sonda (se presente).
- **Ícone SD:** Indica se o cartão foi montado com sucesso.
- **Ícone Power:** Status da alimentação.