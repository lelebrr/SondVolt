# Documentação - Component Tester PRO v2.0

## Índice Geral

| Arquivo | Descrição | Público-Alvo |
|---------|-----------|--------------|
| [README.md](README.md) | Visão geral do projeto | Todos |
| [MANUAL.md](MANUAL.md) | Manual completo iniciante | Usuários |
| [GUIDES.md](GUIDES.md) | Guias passo a passo | Usuários |
| [COMPONENTS.md](COMPONENTS.md) | Como testar cada componente | Usuários |
| [MENUS.md](MENUS.md) | Descrição dos menus | Usuários |
| [HARDWARE.md](HARDWARE.md) | Especificações de hardware | Usuários/Eletrônicos |
| [PINOUT.md](PINOUT.md) | Pinagem detalhada | Eletrônicos |
| [CONFIG.md](CONFIG.md) | Configurações do dispositivo | Usuários |
| [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Solução de problemas | Todos |
| [FAQ.md](FAQ.md) | Perguntas frequentes | Todos |
| [DEVELOP.md](DEVELOP.md) | Guia do desenvolvedor | Desenvolvedores |

---

## Guia de Leitura

### Para Iniciantes

```
1. Leia [MANUAL.md](MANUAL.md)
   └─ Entenda como usar o dispositivo

2. Leia [GUIDES.md](GUIDES.md)
   └─ Aprenda cada função

3. Consulte [COMPONENTS.md](COMPONENTS.md)
   └─ Veja como testar componentes específicos
```

### Para Montagem

```
1. Leia [HARDWARE.md](HARDWARE.md)
   └─ Veja os componentes necessários

2. Leia [PINOUT.md](PINOUT.md)
   └─ Entenda as conexões

3. Consulte [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
   └─ Resolva problemas
```

### Para Uso Diário

```
1. Quick reference: [FAQ.md](FAQ.md)
2. Menu overview: [MENUS.md](MENUS.md)
3. Configurações: [CONFIG.md](CONFIG.md)
```

### Para Desenvolvimento

```
1. Leia [DEVELOP.md](DEVELOP.md)
   └─ Arquitetura do código

2. Consulte [HARDWARE.md](HARDWARE.md)
   └─ Pinos e configurações
```

---

## Quick Reference Cards

### Botões

| Botão | Função |
|------|--------|
| UP/DW | Navegar menu |
| OK | Selecionar/Confirmar |
| BCK | Voltar |

### Menu Principal

```
1. Medir      → 13 tipos de medição
2. Termica    → Temperatura DS18B20
3. Scanner   → Auto-detecção
4. Historico → Últimas 10 medições
5. Estatist  → Estatísticas
6. Config    → Configurações
7. Sobre     → Informações
```

---

## Tabela de Conteúdo Detalhada

### README.md

- O que é o Component Tester
- Lista de funcionalidades (badges)
- Especificações técnicas
- Como compilar
- Autor

### MANUAL.md

- Introdução para iniciantes
- Primeiro uso
- Navegação nos menus
- Funções de medição
- Sonda térmica
- Configurações
- Interpretação de resultados
- Dicas de segurança

### GUIDES.md

- 13 guias passo a passo
- Como medir cada componente
- Conexões corretas
- Interpretação de resultados

### COMPONENTS.md

- Resistores (código de cores)
- Capacitores (tipos, valores)
- Diodos e LEDs
- Transistores (BJT NPN)
- Indutores
- Optocouplers
- Pontes retificadoras
- Cabos
- Tensões
- Frequência/PWM
- Sonda térmica

### MENUS.md

- Menu Principal
- Menu Medições
- Menu Configurações
- Tela Térmica
- Tela Scanner
- Tela Histórico
- Tela Estatísticas
- Tela Sobre
- Diagrama de estados

### HARDWARE.md

- Componentes de hardware
- Arduino Uno R3
- TFT 2.8" ILI9341
- Sistema de botões
- LEDs
- Buzzer
- DS18B20
- SD Card
- EEPROM

### PINOUT.md

- Tabela de pinos completa
- A0 - Probe 1
- A1 - Probe 2
- D2-D8 - Botões
- D9 - Buzzer
- D10/D11 - LEDs
- D12/D13/A2 - TFT
- A3 - OneWire

### CONFIG.md

- Calibração
- Modo Escuro
- Modo Silencioso
- Timeout
- EEPROM

### TROUBLESHOOTING.md

- Problemas gerais
- Display
- Medições
- Botões
- LEDs/Buzzer
- Sonda térmica
- SD Card
- Software

### FAQ.md

- 25+ perguntas
- Geral
- Hardware
- Medições
- Problemas
- Software
- Avançado

### DEVELOP.md

- Arquitetura
- Estrutura de arquivos
- Variáveis globais
- Pinos
- Menu system
- Medições
- EEPROM
- API reference

---

## Links Úteis

- [Arduino](https://www.arduino.cc/)
- [PlatformIO](https://platformio.org/)
- [Adafruit ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
- [OneWire](https://www.pjrc.com/teensy/td_libs_OneWire.html)
- [DallasTemperature](https://github.com/milesburton/Dallas-Temperature-Archive)

---

## Cronologia de Versões

| Versão | Data | Alterações |
|--------|------|------------|
| v2.0 | 04/03/2026 | Primeira versão completa |

---

*Última atualização: 24/04/2026*