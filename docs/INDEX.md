# 📚 Documentação Técnica — Component Tester PRO v3.0

Bem-vindo ao centro de documentação do **Component Tester PRO**. Este diretório contém todos os manuais e especificações necessários para montar, operar e expandir o seu dispositivo.

---

## 🏗️ Mapa da Documentação

| Guia | Descrição | Público |
|:---|:---|:---|
| 🏠 **[Página Principal](../README.md)** | Visão geral, recursos de elite e início rápido. | Todos |
| 📖 **[Manual do Usuário](MANUAL.md)** | Instruções de operação, navegação touch e modos de teste. | Usuários |
| 🏗️ **[Guia de Hardware](HARDWARE.md)** | Esquemas, lista de materiais e diagramas de ligação. | Makers |
| 📍 **[Referência de Pinagem](PINOUT.md)** | Mapeamento detalhado dos GPIOs do ESP32 CYD. | Eletrônicos |
| 📱 **[Estrutura de Menus](MENUS.md)** | Detalhamento da interface Grid e zonas de toque. | Todos |
| ⚙️ **[Configurações](CONFIG.md)** | Ajustes de firmware, calibração e persistência NVS. | Avançado |
| 👨‍💻 **[Desenvolvimento](DEVELOP.md)** | Arquitetura do código, banco de dados e contribuição. | Devs |
| 🚑 **[Resolução de Problemas](FAQ.md)** | Perguntas frequentes e guia de troubleshooting. | Todos |

---

## 🚀 Caminhos de Aprendizado

### Sou novo no projeto
1. Leia o [README.md](../README.md) para entender o que o dispositivo faz.
2. Siga o guia de [Hardware](HARDWARE.md) para montar seu tester.
3. Consulte o [Manual](MANUAL.md) para sua primeira medição.

### Quero customizar o firmware
1. Entenda a pinagem no [Pinout](PINOUT.md).
2. Estude a arquitetura em [Desenvolvimento](DEVELOP.md).
3. Modifique o arquivo `src/config.h` conforme necessário.

---

## 🛠️ Especificações Técnicas (Resumo)

- **Processador:** ESP32 Dual Core @ 240MHz.
- **Memória:** 520KB RAM / 4MB Flash.
- **Display:** TFT 2.8" (320x240) com Touch Resistivo.
- **Sensores:** AC Isolate (ZMPT), DC Precision (INA219), Thermal (DS18B20).
- **Banco de Dados:** Busca em RAM otimizada com log no SD Card.

---

<p align="center">
  <i>Última atualização: Abril de 2026 — Component Tester PRO Team</i>
</p>