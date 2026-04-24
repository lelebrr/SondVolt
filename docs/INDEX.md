# Documentação Técnica — Sondvolt v3.1

Bem-vindo ao centro de documentação do **Sondvolt**. Este diretório contém todos os manuais, especificações e guias necessários para montar, operar e expandir o seu dispositivo de teste de componentes eletrônicos.

---

## Mapa da Documentação

| # | Guia | Descrição | Público |
|:-|:---|:---|:---|
| 1 | [](../README.md) | Visão geral do projeto, recursos e especificações principais | Todos |
| 2 | [README](README.md) | Visão geral da central de documentación | Todos |
| 3 | [Manual do Usuário](MANUAL.md) | Instruções completas de operação, navegação touch e modos de teste | Usuários |
| 4 | [Guia de Hardware](HARDWARE.md) | Esquemas, lista de materiais completa e diagramas de ligação | Makers |
| 5 | [Referência de Pinagem](PINOUT.md) | Mapeamento detalhado completo dos GPIOs do ESP32 CYD | Eletrônicos |
| 6 | [Componentes Suportados](COMPONENTS.md) | Lista completa de componentes com ícones e instruções de teste | Usuários |
| 7 | [Estrutura de Menus](MENUS.md) | Detalhamento da interface Grid, zonas de toque e navegação | Todos |
| 8 | [Configurações](CONFIG.md) | Ajustes de firmware, calibração, NVS e atualização do banco de dados | Avançado |
| 9 | [Desenvolvimento](DEVELOP.md) | Arquitetura do código, estrutura do banco de dados e contribuição | Desenvolvedores |
| 10 | [Changelog](CHANGELOG.md) | Histórico de versões e mudanças importantes do projeto | Todos |
| 11 | [Roadmap](ROADMAP.md) | Planejamento de funcionalidades futuras do projeto | Todos |
| 12 | [Contribuição](CONTRIBUTING.md) | Guia de como contribuir com o projeto | Desenvolvedores |
| 13 | [Teste](TESTING.md) | Guia de como testar o equipamento após montagem | Usuários |
| 14 | [FAQ](FAQ.md) | Perguntas frecuentes sobre uso e operação | Todos |
| 15 | [Resolução de Problemas](TROUBLESHOOTING.md) | Guia de troubleshooting e diagnóstico de problemas | Todos |

---

## Especificações Técnicas Principais

| Especificação | Valor |
|:---|:---|
| **Microcontrolador** | ESP32-WROOM-32 Dual Core @ 240MHz |
| **Memória** | 520KB SRAM / 4MB Flash SPI |
| **Display** | TFT 2.8" (320x240) com Touch Resistivo |
| **Interface de Armazenamento** | Slot MicroSD (até 32GB FAT32) |
| **Sensor AC** | ZMPT101B (Transformador de Tensão Isolado) |
| **Sensor DC** | INA219 (I2C Bi-Direcional) |
| **Sensor de Temperatura** | DS18B20 (OneWire) |
| **Tensão AC Máxima** | 250V RMS (modo multímetro) |
| **Tensão DC Máxima** | 26V (modo DC) |
| **Corrente DC Máxima** | ±3.2A (modo INA219) |
| **Precisão de Medição** | ±2% (calibrado) |
| **Banco de Dados** | COMPBD.CSV no MicroSD |

---

## Avisos de Segurança

> **PERIGO:** Este projeto envolve medições em tensão de rede elétrica (110V/220V). Sempre tome precauções adequadas e **NÃO** manipule o circuito enquanto estiver energizado.

> **ATENÇÃO:** Use luvas isolantes e ferramentas adequadas ao trabalhar com tensões superiores a 50V. O projeto inclui sistema de alertas, mas a segurança pessoal é responsabilidade do operador.

> **IMPORTANTE:** Sempre verifique as conexões antes de energizar o circuito. Erros de fiação podem danificar componentes e representar risco de choque elétrico.

---

## Novos Arquivos Adicionados

Esta seção foi criada para documentar as novas funcionalidades e melhorias introduzidas no projeto:

| Arquivo | Descrição |
|:---|:---|
| [CHANGELOG.md](CHANGELOG.md) | Histórico completo de versões e mudanças |
| [ROADMAP.md](ROADMAP.md) | Planejamento de funcionalidades futuras |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Guia para contribuidores |
| [TESTING.md](TESTING.md) | Guia de validação pós-montagem |
| [LICENSE.md](LICENSE.md) | Licença MIT e termos de uso |

---

## Caminhos de Aprendizado

### Sou novo no projeto
1. Leia o [README](../README.md) para entender o que o dispositivo faz.
2. Siga o guia de [Hardware](HARDWARE.md) para montar seu tester.
3. Consulte o [Manual](MANUAL.md) para sua primeira medição.
4. Revise as [Configurações](CONFIG.md) para calibrate o sistema.

### Quero usar como multímetro
1. Leia a seção de [Multímetro AC/DC](MANUAL.md#modo-multímetro-acdc) no Manual.
2. Configure o banco de dados em [COMPONENTS](COMPONENTS.md).
3. Entenda os alertas de segurança em [Hardware](HARDWARE.md).

### Quero customizar o firmware
1. Entenda a pinagem no [Pinout](PINOUT.md).
2. Estude a arquitetura em [Desenvolvimento](DEVELOP.md).
3. Modifique as configurações em [Config](CONFIG.md).

---

## Recursos Adicionais

- **Banco de Dados:** O arquivo `COMPBD.CSV` no MicroSD contém todos os componentes suportados. Atualize regularmente para novos componentes.
- **Logging:** Os resultados de medições são salvos no cartão SD com timestamp.
- **Calibração:** O sistema permite calibração de offset e ganho para cada modo de medição.

---

<p align="center">
<i>Última atualização: Abril de 2026 — Sondvolt Team</i>
</p>