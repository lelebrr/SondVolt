# Guia do Desenvolvedor - Component Tester PRO v3.0 (CYD Edition)

## 1. Arquitetura do Sistema

O firmware v3.0 foi totalmente reescrito para o **ESP32**, aproveitando sua arquitetura dual-core e maior capacidade de memória.

### Componentes Principais
- **Core:** Baseado no framework Arduino para ESP32.
- **Gráficos:** Utiliza a biblioteca `TFT_eSPI` com suporte a DMA para máxima performance no display ILI9341.
- **Entrada:** Sistema de zonas de toque (Touch Zones) no controlador XPT2046.
- **Dados:** Persistência via NVS (Preferences) e Database via SD Card (HSPI).
- **Multímetro:** Integração I2C (INA219) e processamento analógico AC (ZMPT101B).

---

## 2. Estrutura de Arquivos

```
src/
├── main.cpp          # Ponto de entrada, loop principal e splash
├── config.h          # Definições de pinagem e constantes de hardware
├── globals.h         # Estados da máquina e variáveis compartilhadas
├── menu.cpp/h        # Sistema de menu 2x4 e telas secundárias
├── measurements.cpp  # Rotinas de medição de componentes (12-bit)
├── multimeter.cpp    # Lógica de multímetro AC True RMS e DC
├── database.cpp      # Gerenciamento do SD e índice em RAM
├── drawings.cpp      # Primitivas gráficas e ícones vetoriais
├── buttons.cpp       # Handler de toque e debouncing
├── buzzer.cpp        # Geração de tons via LEDC PWM
├── leds.cpp          # Controle do LED RGB integrado
├── thermal.cpp       # Driver OneWire para DS18B20
└── utils.cpp         # Funções matemáticas e calibração
```

---

## 3. Máquina de Estados

O sistema opera em uma máquina de estados centralizada em `globals.h`:

- `STATE_MENU`: Menu principal em grade.
- `STATE_MEASURING`: Seleção de medição de componentes.
- `STATE_MULTIMETER`: Tela de multímetro AC/DC.
- `STATE_THERMAL_PROBE`: Monitor térmico.
- `STATE_SCANNER`: Auto-detecção contínua.
- `STATE_HISTORY`: Visualização de logs recentes.
- `STATE_STATS`: Estatísticas de hardware.
- `STATE_SETTINGS`: Ajustes de brilho e calibração.
- `STATE_ABOUT`: Informações do sistema.

---

## 4. Otimizações ESP32

### Gerenciamento de Memória
- **Database Index:** No boot, o arquivo `COMPBD.CSV` é escaneado uma vez. O offset de início de cada categoria é salvo no array `categoryIndex` em RAM. Isso permite buscas quase instantâneas.
- **Buffers:** Leituras de arquivo usam buffers de 512 bytes para minimizar acessos ao SD.

### Performance Gráfica
- O driver `TFT_eSPI` está configurado para o barramento **VSPI** a 40MHz.
- Ícones são desenhados usando primitivas GFX para evitar o uso excessivo de memória com bitmaps.

### Barramentos SPI Separados
- **TFT (VSPI):** Pinos 15, 2, 14, 13, 12.
- **SD (HSPI):** Pinos 5, 18, 23, 19.
Isso elimina a latência de troca de Chip Select e permite maior velocidade no cartão SD.

---

## 5. Como Contribuir

1. **Adicionar Medição:** Implemente a função em `measurements.cpp` e adicione ao menu em `measurements_handle`.
2. **Novos Sensores:** Conecte ao barramento I2C (GPIO 27/22) e adicione a lógica em `multimeter.cpp`.
3. **UI/UX:** Modifique `drawings.cpp` para criar novos ícones ou componentes visuais.