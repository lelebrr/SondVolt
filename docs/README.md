# 📚 Central de Documentação

Bem-vindo à central de conhecimento do **Component Tester PRO v3.2**. Aqui você encontrará tudo o que precisa para montar, configurar e operar seu dispositivo com maestria.

---

## 🗺️ Mapa da Documentação

Para facilitar sua navegação, dividimos o conteúdo em trilhas de conhecimento:

### 🚀 Para Começar (Iniciantes)
- 📖 **[Manual do Usuário](MANUAL.md):** O primeiro passo para entender como operar o dispositivo.
- 🎓 **[Guias Passo a Passo](GUIDES.md):** Tutoriais detalhados para cada funcionalidade de medição.
- ❓ **[FAQ](FAQ.md):** Respostas rápidas para as dúvidas mais comuns.

### 🔌 Hardware & Montagem (Eletrônica)
- 🛠️ **[Especificações de Hardware](HARDWARE.md):** Lista de componentes e detalhes técnicos.
- 📍 **[Diagrama de Pinagem](PINOUT.md):** Referência detalhada de todas as conexões do ESP32.
- 🚧 **[Solução de Problemas](TROUBLESHOOTING.md):** Guia para resolver erros de montagem ou operação.

### 🧪 Referência Técnica
- 🧩 **[Testando Componentes](COMPONENTS.md):** Como interpretar os dados de capacitores, resistores, etc.
- 🗺️ **[Arquitetura de Menus](MENUS.md):** Mapa visual de todas as telas e submenus.
- ⚙️ **[Guia de Configuração](CONFIG.md):** Como calibrar probes e personalizar o sistema.

### 💻 Para Desenvolvedores
- 🖥️ **[Guia do Desenvolvedor](DEVELOP.md):** Explicação da estrutura do código e como contribuir.

---

## 🏗️ Arquitetura do Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                 COMPONENT TESTER PRO v3.2 - ARQUITETURA         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Interface     │    │   Sistema de    │    │   Banco de  │ │
│  │   Touchscreen   │◄──►│   Medição       │◄──►│   Dados     │ │
│  │   (TFT 2.8")    │    │   Principal     │    │   (SD Card) │ │
│  └─────────────────┘    └─────────────────┘    └─────────────┘ │
│         │                       │                       │     │
│         ▼                       ▼                       ▼     │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Controle de   │    │   Sensoriamento │    │   Armazen.  │ │
│  │   Navegação     │    │   (ADC, I2C)    │    │   Histórico │ │
│  └─────────────────┘    └─────────────────┘    └─────────────┘ │
│                                                                 │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────┐ │
│  │   Sistema de    │    │   Interface     │    │   Sistema   │ │
│  │   Segurança     │    │   Gráfica       │    │   de Alertas │ │
│  │   (True RMS)    │    │   (UI/UX)       │    │   (LED/Buzz)│ │
│  └─────────────────┘    └─────────────────┘    └─────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 🛠️ Resumo de Hardware

### 📊 Especificações Técnicas

| Componente | Especificação | Função |
|:---|:---|:---|
| **Microcontrolador** | ESP32-2432S028R | Processamento principal |
| **Display** | TFT 2.8" SPI 320x240 | Interface gráfica touchscreen |
| **Armazenamento** | SD Card | Dados de medição e configurações |
| **Interface** | Touchscreen + Botões + Buzzer | Controle e feedback sonoro |
| **Sensores** | ZMPT101B + INA219 + DS18B20 | medição AC/DC/Temperatura |
| **Proteção** | Fusível 5A + Varistor + TVS | Segurança em redes 220V |

### 🔄 Fluxo de Operação

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Início    │───►│ Auto-Detec. │───►│  Medição    │───►│  Armazen.   │
│   Sistema   │    │  Componente │    │   Dados     │    │   Resultados│
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │                   │
       ▼                   ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Configuração │    │  Teste       │    │  Análise    │    │  Visualização│
│ Inicial     │    │  Específico  │    │  Estatística│    │  Gráfica     │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
```

> [!TIP]
> Sempre verifique a seção de [Calibração](CONFIG.md) antes de realizar medições críticas para garantir a máxima precisão.

> [!WARNING]
> Nunca exponha o dispositivo a tensões acima de 30V DC ou 250V AC sem proteção adequada. Versão 3.2 requer Fusível 5A, Varistor 14D431 e TVS P6KE400A para operação em redes 220V.

---

## 📈 Desempenho e Precisão

### 🎯 Precisão das Medições

```
Resistores:    ±0.5% (0-1MΩ)
Capacitores:   ±2% (1pF-100µF)
Tensão DC:     ±0.3% (0-30V)
Tensão AC:     ±1.5% (0-250V) - True RMS
Corrente:      ±1.0% (0-3A)
Temperatura:   ±0.5°C (-10°C~85°C)
```

### ⚡ Performance

| Operação | Tempo | Sucesso |
|:---|:---:|:---:|
| Auto-Detecção | < 2s | 98% |
| Medição DC | < 1s | 99.5% |
| Medição AC | < 3s | 97% |
| Banco de Dados | < 0.5s | 100% |

---

## 🔗 Links Rápidos

- [Início Rápido](../README.md)
- [Diagrama de Conexões](PINOUT.md)
- [Como Testar Transistores](COMPONENTS.md#transistores)

---

## 🎨 Elementos Visuais da Documentação

Este projeto utiliza elementos visuais padronizados para melhorar a experiência do usuário:

### 📋 Tipos de Alertas

> [!NOTE]
> Informações importantes que devem ser observadas durante o uso.

> [!TIP]
> Sugestões e melhores práticas para otimizar o uso do dispositivo.

> [!WARNING]
> Alertas sobre condições que podem causar danos ao dispositivo.

> [!ERROR]
> Mensagens críticas que indicam problemas sérios.

---

<p align="center">
  <b>📱 Component Tester PRO v3.2</b> - Documentação Oficial
</p>
