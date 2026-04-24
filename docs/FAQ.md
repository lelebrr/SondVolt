# Frequently Asked Questions (FAQ) - Component Tester PRO v3.0 (CYD Edition)

## 1. Geral

### Q: Quais os principais recursos desta versão?
**R:** O Component Tester PRO v3.0 oferece:
- Interface Touchscreen intuitiva.
- Processador Dual Core de 240MHz de alta performance.
- ADC de 12 bits para máxima precisão.
- Multímetro Profissional (AC True RMS e DC).
- Banco de dados de componentes carregado via MicroSD.


---

## 2. Hardware

### Q: O que é a placa CYD?
**R:** É a "Cheap Yellow Display", uma placa de desenvolvimento baseada no ESP32 que já vem com tela TFT de 2.8", touchscreen e slot para cartão SD integrados.

### Q: Preciso soldar algo?
**R:** A placa CYD vem pronta. Você precisará apenas conectar os sensores externos (ZMPT101B, INA219, DS18B20) e as pontas de prova (probes) nos conectores laterais ou pinos de expansão.

### Q: Posso usar bateria?
**R:** Sim! Como é um ESP32, você pode usar uma bateria LiPo com um módulo carregador TP4056 e um regulador de tensão para alimentar a placa via 5V ou 3.3V.

---

## 3. Medições e Sensores

### Q: O multímetro AC é seguro?
**R:** O sensor **ZMPT101B** possui um transformador de isolação, o que isola o ESP32 da rede elétrica. No entanto, a manipulação de fios de 110V/220V deve ser feita com **EXTREMO CUIDADO** e proteção adequada.

### Q: Por que a leitura do INA219 não aparece?
**R:** O sistema tenta detectar o sensor via I2C no boot. Se não encontrar, ele entra em modo "Fallback", usando o ADC interno (GPIO 35) para medir tensão DC básica (0-3.3V).

### Q: Qual a precisão do Ohmímetro?
**R:** Com o ADC de 12 bits do ESP32, a precisão é excelente. Recomendamos realizar a calibração no menu **Config** para melhores resultados em diferentes faixas de resistência.

---

## 4. Software e Customização

### Q: Como adiciono novos componentes ao banco de dados?
**R:** Basta editar o arquivo `COMPBD.CSV` no cartão SD usando o Excel ou bloco de notas, seguindo o formato de colunas existente. O ESP32 reconstruirá o índice automaticamente no próximo boot.

### Q: Posso mudar as cores da interface?
**R:** Sim, as cores são definidas em `config.h` através de constantes como `UI_COLOR_BG`, `UI_COLOR_TEXT`, etc.

### Q: Como atualizo o firmware?
**R:** Utilize o PlatformIO e conecte a placa via USB. O comando `pio run --target upload` fará todo o trabalho.

---

## 5. Problemas Comuns

### Q: A tela não responde ao toque.
**R:** O touchscreen da CYD é **resistivo**. Pressione levemente com a ponta da unha ou uma caneta stylus. Verifique se o cabo flat do touch não está solto.

### Q: O cartão SD não é reconhecido.
**R:** Formate em **FAT32**. Cartões muito antigos ou de altíssima velocidade (UHS-II) podem ter problemas de compatibilidade. Recomenda-se cartões classe 10 de até 32GB.

---

*Ainda tem dúvidas? Consulte o [MANUAL.md](MANUAL.md) ou o [TROUBLESHOOTING.md](TROUBLESHOOTING.md).*