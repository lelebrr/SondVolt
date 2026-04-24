# Documentação - Component Tester PRO v3.0 (CYD Edition)

## 🏗️ Índice Geral

| Arquivo | Descrição | Público-Alvo |
|:---|:---|:---|
| [README.md](README.md) | Visão geral, especificações e como compilar | Todos |
| [MANUAL.md](MANUAL.md) | Guia de uso, navegação e medições | Usuários |
| [HARDWARE.md](HARDWARE.md) | Detalhes da placa CYD e sensores extras | Eletrônicos |
| [PINOUT.md](PINOUT.md) | Mapeamento completo de pinos ESP32 | Eletrônicos |
| [MENUS.md](MENUS.md) | Estrutura da interface 2x4 e zonas de toque | Todos |
| [CONFIG.md](CONFIG.md) | Ajustes, NVS e calibração de sensores | Usuários |
| [DEVELOP.md](DEVELOP.md) | Arquitetura do código e guia de contribuição | Desenvolvedores |
| [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Guia de resolução de problemas | Todos |

---

## 🚀 Guia Rápido

### Para Novos Usuários
1. Comece pelo [README.md](README.md) para entender as capacidades do dispositivo.
2. Leia o [MANUAL.md](MANUAL.md) para aprender a navegar usando o touchscreen.
3. Veja o [MENUS.md](MENUS.md) para entender o layout da interface.

### Para Desenvolvedores e Makers
1. Confira o [PINOUT.md](PINOUT.md) antes de conectar novos sensores.
2. Leia o [DEVELOP.md](DEVELOP.md) para entender como o sistema de banco de dados em RAM funciona.
3. Use o [CONFIG.md](CONFIG.md) para entender como calibrar os sensores ZMPT101B e INA219.

---

## 📊 Status da Migração (Arduino → ESP32)

- [x] **Core:** Migrado para ESP32 (Dual Core).
- [x] **Display:** TFT_eSPI (High Performance) - OK.
- [x] **Touch:** Interface XPT2046 - OK.
- [x] **Database:** RAM Caching (Busca instantânea) - OK.
- [x] **Multímetro:** Integração True RMS e I2C - OK.
- [x] **Config:** Persistência via NVS (Preferences) - OK.

---

## 🛠️ Tecnologias Utilizadas

- **Microcontrolador:** ESP32-WROOM-32.
- **Gráficos:** TFT_eSPI (ILI9341).
- **Touch:** XPT2046.
- **Sensores:** ZMPT101B (AC), INA219 (DC), DS18B20 (Temp).
- **Armazenamento:** MicroSD via HSPI bus.

---

*Última atualização: 24 de Abril de 2026*