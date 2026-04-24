# Manual do Usuário - Component Tester PRO v3.0 (CYD Edition)

## 1. Introdução

Bem-vindo ao **Component Tester PRO v3.0**, a solução definitiva para testes de componentes eletrônicos baseada na plataforma ESP32. Esta versão utiliza a placa **Cheap Yellow Display (CYD)**, oferecendo uma interface touchscreen colorida e processamento de alta velocidade.

---

## 2. Primeiro Uso

### 2.1 Ligando o Dispositivo
Conecte o dispositivo a uma fonte de alimentação 5V via porta Micro USB. Você será saudado por uma tela de boot (Splash Screen) com progresso de inicialização do sistema, SD Card e sensores.

### 2.2 Navegação por Toque
Não há botões físicos. Utilize as seguintes zonas de toque na tela:

- **TOPO:** Navega para Cima.
- **BASE:** Navega para Baixo.
- **ESQUERDA:** Navega para a Esquerda.
- **DIREITA:** Navega para a Direita.
- **CENTRO:** Confirma a seleção (OK).
- **CANTO INF. DIREITO:** Volta ao menu anterior (BACK).

---

## 3. Menu Principal (Grid)

O menu principal utiliza ícones modernos em uma grade 2x4:

1. **Medir:** Menu com 13 modos clássicos de teste de componentes.
2. **Multimetro:** Medição de Tensão AC (RMS), DC, Corrente e Potência.
3. **Termica:** Monitoramento de temperatura via sonda DS18B20.
4. **Scanner:** Modo de identificação automática contínua.
5. **Historico:** Visualização dos últimos 50 registros de medição.
6. **Estatist:** Dados de uso e saúde do hardware.
7. **Config:** Ajustes de brilho e calibração.
8. **Sobre:** Versão do firmware e especificações técnicas.

---

## 4. Como Medir Componentes

1. Acesse o menu **Medir**.
2. Selecione o tipo de componente (ex: Capacitor).
3. Conecte o componente ao **Probe Principal (GPIO 35)** e ao GND.
4. Pressione o centro da tela (OK) para iniciar a leitura.
5. O resultado aparecerá com o valor medido e um ícone visual do componente.
6. Se o componente for identificado no banco de dados do SD Card, o nome (Part Number) será exibido.

---

## 5. Uso do Multímetro Profissional

O multímetro utiliza sensores externos de alta precisão:
- **Tensão AC:** Use o módulo ZMPT101B para medir tensões de rede elétrica com isolação galvânica.
- **Tensão/Corrente DC:** Use o módulo INA219 para monitorar fontes de bancada e circuitos DC (até 26V).

> [!CAUTION]
> **SEGURANÇA:** Ao medir tensões AC, certifique-se de que não há partes expostas. Use sempre uma caixa isolada para o projeto.

---

## 6. Configurações e Calibração

O sistema salva as configurações automaticamente na memória **NVS**.

- **Brilho:** Ajuste para o conforto visual e economia de energia.
- **Calibração:** Se as leituras estiverem imprecisas, use a função de calibração no menu Config para ajustar os offsets do ADC.

---

## 7. Dicas de Manutenção

- **SD Card:** Mantenha o arquivo `COMPBD.CSV` na raiz do cartão. Sem ele, a identificação de componentes não funcionará.
- **Limpeza:** Use um pano seco para limpar a tela touchscreen. Evite objetos pontiagudos.

---

## Suporte
Consulte o arquivo [TROUBLESHOOTING.md](TROUBLESHOOTING.md) caso encontre problemas na inicialização ou medições.