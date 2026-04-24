# Descrição dos Menus - Component Tester PRO v3.0 (CYD Edition)

## 1. Menu Principal (Grid 2x4)

A interface agora utiliza um sistema de grade (Grid) otimizado para o display de 2.8" e navegação por toque.

### Layout do Menu
O menu principal é composto por 8 ícones organizados em uma grade de 2 colunas por 4 linhas.

| Coluna 1 | Coluna 2 |
|:---:|:---:|
| **Medir** | **Multimetro** |
| **Termica** | **Scanner** |
| **Historico** | **Estatist** |
| **Config** | **Sobre** |

### Navegação por Toque
A tela é dividida em zonas invisíveis para simular botões físicos:
- **TOPO:** Navega para Cima (UP)
- **BASE:** Navega para Baixo (DOWN)
- **CENTRO ESQUERDA:** Navega para Esquerda (LEFT)
- **CENTRO DIREITA:** Navega para Direita (RIGHT)
- **CENTRO:** Confirma Seleção (OK)
- **BORDA INFERIOR DIREITA:** Voltar (BACK)

---

## 2. Menu Medições (13 Modos)

Lista vertical de medições clássicas herdadas da v2.0, agora com processamento em 12-bit.

| # | Modo | Função |
|:---|:---|:---|
| 1 | **Capacitor** | Medição de capacitância |
| 2 | **Resistor** | Medição de resistência (Ohmímetro) |
| 3 | **Diodo/LED** | Teste de junção e tensão direta (Vf) |
| 4 | **Transistor** | Identificação de BJT (NPN/PNP) |
| 5 | **Indutor** | Medição de indutância |
| 6 | **Volt DC** | Voltímetro de precisão 0-3.3V |
| 7 | **Frequencia** | Frequencímetro |
| 8 | **PWM Out** | Gerador de sinal quadrado |
| 9 | **Optoacopl.** | Teste de isolamento óptico |
| 10 | **Cabo/Cont.** | Teste de continuidade de cabos |
| 11 | **Ponte Ret.** | Teste de pontes de diodo |
| 12 | **Auto Detect** | Identificação automática do componente |
| 13 | **Continuity** | Modo buzzer rápido |

---

## 3. Multímetro Profissional (Novo)

Esta tela exibe dados em tempo real dos sensores dedicados:

- **Tensão AC:** Valor True RMS (via ZMPT101B). Inclui barra visual e indicador de pico.
- **Tensão DC:** Tensão exata (via INA219).
- **Corrente DC:** Consumo em mA/A (via INA219).
- **Potência:** Cálculo em Watts (W).
- **Consumo Acumulado:** Energia em mAh/Wh.

---

## 4. Menu Configurações (NVS)

As configurações são salvas instantaneamente na memória NVS (Flash) do ESP32.

1. **Brilho do LCD:** Ajuste de 0 a 100% (Controle PWM/LEDC).
2. **Modo Silencioso:** Ativa/Desativa o buzzer.
3. **Calibração ADC:** Ajusta os offsets para medições de precisão.
4. **ZMPT Scale:** Fator de calibração para o sensor de tensão AC.
5. **Limpar Historico:** Apaga os registros da memória.

---

## 5. Tela Sobre (Stats do Sistema)

Exibe informações em tempo real do hardware ESP32:
- **Modelo:** ESP32-WROOM-32
- **Cores:** Dual Core @ 240MHz
- **Heap Livre:** Memória RAM disponível.
- **Uptime:** Tempo desde que o aparelho foi ligado.
- **Versão:** v3.0 (CYD Edition).