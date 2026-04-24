# 🚀 Component Tester PRO v3.0 — CYD Edition

<p align="center">
  <img src="component_tester_hero_1777037867137.png" alt="Component Tester PRO v3.0 Hero" width="800">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32--2432S028R-yellow?style=for-the-badge&logo=espressif" alt="Platform">
  <img src="https://img.shields.io/badge/Framework-Arduino-00979D?style=for-the-badge&logo=arduino" alt="Framework">
  <img src="https://img.shields.io/badge/Version-3.0.0-blue?style=for-the-badge" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License">
</p>

---

## 🌟 Visão Geral

O **Component Tester PRO v3.0** é a evolução definitiva em diagnóstico de componentes eletrônicos. Projetado para a plataforma **Cheap Yellow Display (CYD)**, este dispositivo transforma um ESP32 em uma ferramenta de bancada profissional, combinando medições precisas com uma interface visual "Cyber Dark" premium e controle total via touchscreen.

> [!TIP]
> **Por que v3.0?** Saímos da limitação do ATmega328P para o poder do Dual Core 240MHz. Agora temos processamento paralelo para medições True RMS, busca instantânea em banco de dados de milhares de componentes e uma interface gráfica fluida.

---

## ✨ Recursos de Elite

| Categoria | Funcionalidade | Vantagem |
|:---|:---|:---|
| **⚡ Medição** | 13 modos incluindo Auto-detect | Identifica Transistores, MOSFETs, Diodos e mais. |
| **📊 Inteligência** | Database em tempo real | Identifica o Part Number exato (ex: BC547) via SD Card. |
| **🔌 Multímetro** | AC True RMS & DC Precision | Monitora rede elétrica e consumo de dispositivos DC. |
| **🌡️ Térmico** | Sonda DS18B20 | Monitoramento de temperatura com alertas visuais RGB. |
| **📱 Interface** | Touch UI 2.8" | Navegação intuitiva sem botões físicos necessários. |
| **💾 Registro** | Datalogger SD | Salva histórico de medições para relatórios técnicos. |

---

## 🛠️ Ecossistema de Hardware

O projeto é otimizado para a placa **ESP32-2432S028R**. Veja o guia completo em [Hardware](docs/HARDWARE.md).

- **Display:** TFT 2.8" 320x240 (ILI9341)
- **Touch:** XPT2046 Resistivo
- **Sensores Externos:** ZMPT101B (AC), INA219 (DC), DS18B20 (Temp)
- **Feedback:** Buzzer Magnético + LED RGB Integrado

---

## 🚀 Início Rápido

### 1. Preparação
- Instale o [VS Code](https://code.visualstudio.com/) + [PlatformIO](https://platformio.org/).
- Formate um MicroSD em **FAT32**.
- Copie `sd_files/COMPBD.CSV` para a raiz do cartão.

### 2. Firmware
```bash
# Clone
git clone https://github.com/lelebrr/Component_Tester.git

# Build & Upload
# O ambiente 'cyd' já está configurado no platformio.ini
pio run -e cyd --target upload
```

### 3. Conexão
Siga o [Diagrama de Ligação](docs/HARDWARE.md#diagrama-de-ligação) para conectar os sensores externos aos pinos expostos da CYD.

---

## 📂 Documentação Detalhada

Explore os manuais completos para dominar o seu Component Tester:

- 🏗️ **[Hardware & Montagem](docs/HARDWARE.md)**: Esquemas, PINOUT e lista de componentes.
- 📖 **[Manual do Usuário](docs/MANUAL.md)**: Como operar cada modo de teste.
- ⚙️ **[Configuração](docs/CONFIG.md)**: Personalize o firmware para suas necessidades.
- ❓ **[FAQ & Suporte](docs/FAQ.md)**: Resolução de problemas comuns.

---

## 📜 Licença

Este projeto é open-source sob a licença **MIT**. Sinta-se livre para clonar, modificar e melhorar!

---

<p align="center">
  Criado com ❤️ por <b>Leandro</b> | Component Tester PRO Team
</p>