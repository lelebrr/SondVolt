# Solução de Problemas - Component Tester PRO v3.0 (CYD Edition)

## 1. Problemas Gerais de Hardware

### 🔴 O Dispositivo não liga (Tela Preta)
- **Verifique a Alimentação:** A CYD requer 5V via Micro USB. Verifique se o cabo está em bom estado.
- **Interruptor:** Se houver um interruptor físico no seu case, verifique se está na posição ON.
- **Porta USB:** Tente outra porta USB no PC ou um carregador de celular de 1A+.

### 🔴 Tela Branca ou com Listras
- **Firmware:** Verifique se o upload do firmware via PlatformIO foi concluído com sucesso.
- **Drivers:** Certifique-se de que a biblioteca `TFT_eSPI` está configurada corretamente em `platformio.ini` para o driver ILI9341.
- **Pinos:** Verifique se não há curtos nos pinos expostos da placa.

---

## 2. Problemas de Toque (Touchscreen)

### 🟡 O toque não responde ou está invertido
- **Zonas de Toque:** Lembre-se que a navegação é baseada em zonas (Topo, Base, Esquerda, Direita, Centro).
- **Pressão:** O touchscreen é resistivo, requer uma leve pressão (não é capacitivo como um celular).
- **Calibração:** Se as zonas parecerem deslocadas, verifique as constantes de calibração em `buttons.cpp`.

---

## 3. Problemas de Medição

### 🟡 Medição de Componentes sempre em "OPEN"
- **Pino de Medição:** Verifique a conexão física no **GPIO 35**. Ele é o pino de entrada principal.
- **Referência:** Certifique-se de que o componente está conectado entre o GPIO 35 e o GND comum da placa.
- **Componente Danificado:** Teste com um resistor conhecido (ex: 10k).

### 🟡 Multímetro AC (ZMPT101B) não marca nada ou oscila muito
- **Potenciômetro Físico:** O módulo ZMPT101B tem um potenciômetro azul. Ajuste-o até que a leitura no multímetro bata com a realidade.
- **Ruído:** Certifique-se de que os fios AC estão longe dos fios de sinal do ESP32.
- **Calibração Digital:** Ajuste o `ZMPT Scale` no menu **Config**.

### 🟡 INA219 (DC) não detectado
- **Endereço I2C:** O código espera o endereço padrão `0x40`.
- **Barramento:** Verifique se o SDA está no **GPIO 27** e SCL no **GPIO 22**.
- **Alimentação do Sensor:** O INA219 deve ser alimentado com 3.3V da CYD.

---

## 4. Problemas de Cartão SD

### 🔴 "SD Card: Erro na inicialização"
- **Formatação:** O cartão deve estar formatado em **FAT32**.
- **Capacidade:** Recomenda-se cartões de até 32GB.
- **Pinos HSPI:** A CYD usa pinos específicos para o SD (18, 23, 19, 5). Verifique se não há nada obstruindo o slot.
- **Arquivo Faltando:** Verifique se o arquivo `COMPBD.CSV` está na raiz do cartão.

---

## 5. Problemas de Áudio e Luz

### 🟡 Buzzer/Speaker não faz som
- **Modo Silencioso:** Verifique se o Modo Silencioso está **OFF** no menu **Config**.
- **Conexão:** O speaker deve estar conectado ao conector JST de 2 pinos (GPIO 26) da CYD.

### 🟡 Brilho não altera
- **GPIO 21:** Verifique se não há nada forçando o GPIO 21 para HIGH ou LOW externamente. O controle é via PWM (LEDC).

---

## Códigos de Log Serial
Ao conectar o dispositivo ao PC, abra o Monitor Serial (115200 baud) para ver mensagens detalhadas:
- `SD Card: OK` -> SD detectado.
- `DB: Indice construido` -> Banco de dados pronto.
- `INA219: OK` -> Sensor DC detectado.
- `INA219: ERRO` -> Sensor DC não encontrado (o sistema usará fallback ADC).