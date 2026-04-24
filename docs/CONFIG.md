# Configurações - Component Tester PRO v3.0 (CYD Edition)

## 1. Sistema de Persistência (NVS)

Diferente do Arduino Uno, o ESP32 utiliza a **NVS (Non-Volatile Storage)** através da API `Preferences`. Os dados são salvos na Flash do microcontrolador de forma robusta e organizada por chaves.

### Vantagens do NVS:
- **Auto-save:** As configurações são salvas instantaneamente ao serem alteradas.
- **Durabilidade:** Gerenciamento de desgaste da Flash integrado.
- **Espaço:** Suporte a muito mais dados que a EEPROM tradicional.

---

## 2. Configurações Disponíveis

### Brilho do LCD
- **Range:** 0% a 100%.
- **Funcionamento:** Controlado via hardware por PWM (LEDC) no GPIO 21.
- **Dica:** Brilhos menores (30-50%) economizam bateria e reduzem o calor.

### Modo Silencioso
- **Opções:** ON / OFF.
- **Funcionamento:** Quando ativado, desativa os tons do buzzer/speaker (GPIO 26).
- **Feedback:** Alertas visuais via LEDs RGB e tela continuam ativos.

### Calibração ADC (Offsets)
- **Função:** Ajusta o zero das medições analógicas.
- **Offsets:** `offset1` (Probe Principal) e `offset2` (Reservado).
- **Ajuste:** Feito via menu para garantir que leituras em aberto sejam 0.0V.

### ZMPT Scale (Calibração AC)
- **Função:** Fator multiplicador para a leitura do sensor de tensão AC.
- **Importância:** Cada módulo ZMPT101B possui uma sensibilidade levemente diferente devido ao potenciômetro físico.
- **Ajuste:** Compare a leitura com um multímetro de bancada e ajuste o fator de escala no menu.

---

## 3. Dados Armazenados

| Chave | Tipo | Descrição |
|:---|:---|:---|
| `offset1` | Float | Calibração do ADC 1 |
| `offset2` | Float | Calibração do ADC 2 |
| `darkMode` | Bool | (Reservado para temas futuros) |
| `silentMode` | Bool | Silenciar buzzer |
| `backlight` | Int | Nível de brilho (0-255) |
| `zmptScale` | Float | Escala do sensor AC |
| `total` | ULong | Total de medições realizadas |
| `faulty` | ULong | Total de componentes defeituosos |

---

## 4. Reset de Fábrica

Para restaurar os valores padrão (Limpando a NVS):
1. Acesse o menu **Config**.
2. Selecione a opção **"Limpar Tudo"** (se disponível) ou reflashe o firmware com a opção `Erase Flash` no PlatformIO.

---

## 5. Notas de Segurança

> [!WARNING]
> A calibração do **ZMPT101B** envolve medir tensões AC (110V/220V). **EXTREMO CUIDADO** ao manipular a rede elétrica. Nunca abra o aparelho ou toque nos terminais enquanto estiver conectado à tomada.