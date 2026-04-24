# ❓ Perguntas Frequentes (FAQ) — Component Tester PRO

Aqui você encontra as respostas para as dúvidas mais comuns sobre montagem, operação e manutenção do seu Component Tester PRO v3.0.

---

## 🛠️ Hardware e Montagem

### 1. O que é necessário para começar?
Você precisa da placa **ESP32 CYD**, um cartão MicroSD e os sensores opcionais (ZMPT101B para AC, INA219 para DC e DS18B20 para temperatura). Veja a lista completa no [Guia de Hardware](HARDWARE.md).

### 2. A tela não responde ao toque. O que fazer?
- **Tipo de Tela:** Lembre-se que a tela é **resistiva**. Use a unha ou uma stylus (caneta de toque).
- **Calibração:** Se o toque estiver desalinhado, acesse `Settings > Calibrate` no menu do dispositivo.
- **Hardware:** Verifique se o cabo flat marrom do touchscreen (atrás do LCD) está bem encaixado.

### 3. O cartão SD não é reconhecido.
- **Formatação:** Deve ser obrigatoriamente **FAT32**.
- **Arquivo:** O arquivo `COMPBD.CSV` deve estar na **raiz** do cartão, não dentro de pastas.
- **Hardware:** O slot SD da CYD é sensível; tente inserir e remover o cartão com a placa desligada.

---

## ⚡ Medições e Sensores

### 4. As leituras do Multímetro AC estão instáveis.
- **Calibração:** Cada sensor ZMPT101B tem um potenciômetro azul. Você deve ajustá-lo enquanto mede uma tensão conhecida (ex: tomada) até que o valor no display coincida.
- **Filtro:** O firmware usa RMS por amostragem. Certifique-se de que o sensor está bem alimentado com 5V estáveis.

### 5. O Probe Principal não detecta nada.
- **Resistor de Referência:** Verifique se você instalou o resistor de **10kΩ** entre o pino de medição (GPIO 35) e o pino de teste.
- **Conexão:** Certifique-se de que o componente está fazendo bom contato com as garras.

### 6. Como funciona a identificação automática?
O sistema mede os parâmetros básicos (Vf, Hfe, Capacitância, etc.) e cruza esses dados com o `COMPBD.CSV`. Se os valores forem próximos aos de um componente conhecido, o nome dele (ex: 2N2222) aparecerá no topo da tela.

---

## ⚙️ Software e Customização

### 7. Como atualizo o banco de dados de componentes?
O banco de dados é um arquivo de texto simples (`CSV`). Você pode abrir o `sd_files/COMPBD.CSV` no seu computador, adicionar novas linhas seguindo o padrão e salvar de volta no cartão SD.

### 8. Posso usar este projeto em um ESP32 comum?
**Sim**, mas você terá que fazer todas as ligações do display ILI9341 e do touch manualmente. O código é otimizado para a pinagem da CYD, então você precisará alterar as definições no arquivo `src/config.h`.

---

## 🚑 Solução de Problemas (Troubleshooting)

| Sintoma | Causa Provável | Solução |
|:---|:---|:---|
| **Tela Branca** | Erro de driver ou pinagem | Verifique o `platformio.ini` (User_Setup.h). |
| **LED Vermelho piscando** | Cartão SD faltando ou corrompido | Insira um cartão com o arquivo `COMPBD.CSV`. |
| **Bipes constantes** | Alerta de temperatura alta | Afaste a sonda DS18B20 de fontes de calor. |
| **Reset cíclico** | Alimentação insuficiente | Use um cabo USB de boa qualidade e porta 2.0/3.0. |

---

<p align="center">
  <i>Ainda tem dúvidas? Abra uma <b>Issue</b> no GitHub ou consulte o <b>Manual do Usuário</b>.</i>
</p>