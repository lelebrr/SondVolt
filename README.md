# Sondvolt Component Tester PRO v3.2

![Logo](assets/logo.png)
![Hero Image](assets/hero.png)

<p align="center">
  <a href="https://github.com/lelebrr/SondVolt/releases">
    <img src="https://img.shields.io/badge/Version-v3.2.1-blue.svg" alt="Versão">
  </a>
  <a href="https://github.com/lelebrr/SondVolt/blob/main/docs/LICENSE.md">
    <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="Licença">
  </a>
  <a href="https://github.com/lelebrr/SondVolt">
    <img src="https://img.shields.io/badge/Platform-ESP32-orange.svg" alt="Plataforma">
  </a>
  <a href="https://github.com/lelebrr/SondVolt/actions">
    <img src="https://img.shields.io/badge/Status-Active-brightgreen.svg" alt="Status">
  </a>
</p>

<p align="center">
  <strong>Um testador profissional de componentes eletrônicos construído para ESP32-2432S028R (Cheap Yellow Display)</strong>
</p>

---

## 📋 Tabela de Conteúdo

- [🚀 Features](#-features)
- [🛠 Hardware Configuration](#-hardware-configuration)
- [📖 Como Usar](#-como-usar)
- [⚠️ Informações de Segurança](#-informações-de-segurança)
- [🔧 Instalação](#-instalação)
- [📚 Documentação Completa](#-documentação-completa)
- [🤝 Contribuição](#-contribuição)
- [📄 Licença](#-licença)

---

## 🚀 Features

### 🧩 Teste de Componentes
- **Identificação Automática**: Detecção automática de resistores, capacitores, diodos, transistores (NPN/PNP), indutores e mais.
- **Precisão Avançada**: Calibração automática para medições de baixa resistência e capacitância.
- **Interface Intuitiva**: Touch screen com navegação intuitiva e feedback visual.

### 📊 Multímetro Digital Profissional
- **True RMS Certificado**: Medição de tensão AC (0-250V) com algoritmo de 128 amostras e detecção de **Tensão de Pico**.
- **Detecção de Surto**: Monitoramento em tempo real de transientes e picos de tensão da rede elétrica.
- **Medição DC Precisa**: Tensão (0-26V) e Corrente (0-3A) via sensor INA219.

### 🎯 Funcionalidades Avançadas
- **Histórico de Medições**: Salva automaticamente as últimas 100 medições em formato CSV no MicroSD.
- **Interface Rica**:
  - Tela de Splash animada
  - Navegação por toque intuitiva
  - Diminuição automática do backlight (45s para economia de energia)
  - Sistema de erros amigável
- **Feedback Sonoro**: Tons profissionais para sucesso, erro e status de medição.

### 🎨 Interface Visual
![Interface Showcase](assets/ui_collage.png)
![Component Showcase](assets/components_showcase.png)

---

## 🛠 Hardware Configuration (CYD Pinout)

| Periférico | Pino | Descrição |
| :--- | :--- | :--- |
| **TFT CS/DC/RST** | 15, 2, 0 | Fixed Hardware |
| **TFT BL** | 21 | Backlight (PWM) |
| **Touch CS/IRQ** | 33, 36 | XPT2046 Interface |
| **SD CS** | 5 | MicroSD Interface |
| **Audio** | 26 | AUDIO: IO26 (Amplified) |
| **Probe 1** | 35 | Analog (IO35 / Port IO1) |
| **Probe 2** | 34 | Analog (Internal / Port IO2) |
| **ZMPT AC** | 36 | Analog (Shared with IRQ) |
| **I2C SDA/SCL** | 27, 22 | Expansion Ports (INA219) |
| **OneWire** | 4 | Thermal (Shared with LED Red) |

<p align="center">
  <img src="assets/hardware_layout.png" alt="Layout do Hardware" width="400">
</p>

---

## 📖 Como Usar

1. **Inicialização**: Ligue o dispositivo. Aguarde a animação de splash terminar.
2. **Calibração**: Vá para `Menu > Calibração`. Curto-circuite as pontas quando solicitado e siga as instruções na tela.
3. **Medição**: Conecte qualquer componente na Probe 1 e Probe 2. O sistema detectará automaticamente e exibirá o tipo e valor.
4. **Histórico**: Veja medições passadas em `Menu > Histórico`. Elas são sincronizadas com o `measurements.csv` no seu cartão SD.
5. **Configurações**: Ajuste brilho e preferências de som no menu `Configurações`.

---

## ⚠️ Informações de Segurança e Proteção Robustas

> [!IMPORTANT]
> **SISTEMA DE PROTEÇÃO ATIVA (Obrigatório para 220V)**:
> Para medir tensões AC de forma segura, o Sondvolt v3.2 **exige** a instalação de:
> - Fusível Rápido de 5A
> - Varistor 14D431 + TVS Diode P6KE400A
> - Filtros RC de amostragem
>
> **O software bloqueia automaticamente o equipamento se detectar tensões perigosas fora do modo multímetro.**

> [!WARNING]
> - **NUNCA** meça componentes em circuito energizado. Descarregue capacitadores antes de testar.
> - Tensão máxima nas pontas Probe 1/2 é **3.3V**. Para tensões maiores, use a entrada específica do Multímetro.

> [!NOTE]
> Sempre verifique a seção de [Calibração](docs/CONFIG.md) antes de realizar medições críticas para garantir a máxima precisão.

---

## 🔧 Instalação

1. Abra este projeto no **VS Code com PlatformIO**.
2. Conecte seu board CYD via USB.
3. Clique em **Upload and Monitor**.
4. Formate seu cartão MicroSD para FAT32 e certifique-se de que está inserido antes de bootar.

### Pré-requisitos
- PlatformIO IDE para VS Code
- ESP32-2432S028R (Cheap Yellow Display)
- Cartão MicroSD FAT32
- Cabos de conexão

---

## 📚 Documentação Completa

Para documentação detalhada, consulte nossa [Central de Documentação](docs/README.md):

### 🚀 Para Começar
- [Manual do Usuário](docs/MANUAL.md) - Primeiros passos para operação
- [Guias Passo a Passo](docs/GUIDES.md) - Tutoriais detalhados
- [FAQ](docs/FAQ.md) - Respostas rápidas para dúvidas comuns

### 🔌 Hardware & Montagem
- [Especificações de Hardware](docs/HARDWARE.md) - Lista de componentes e detalhes técnicos
- [Diagrama de Pinagem](docs/PINOUT.md) - Referência detalhada de conexões
- [Solução de Problemas](docs/TROUBLESHOOTING.md) - Guia para resolver erros

### 🧪 Referência Técnica
- [Testando Componentes](docs/COMPONENTS.md) - Como interpretar dados
- [Arquitetura de Menus](docs/MENUS.md) - Mapa visual de telas
- [Guia de Configuração](docs/CONFIG.md) - Calibração e personalização

### 💻 Para Desenvolvedores
- [Guia do Desenvolvedor](docs/DEVELOP.md) - Estrutura do código e contribuição

---

## 🤝 Contribuição

Contribuições são bem-vindas! Por favor:

1. Faça um fork do projeto
2. Crie sua feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

---

## 📄 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](docs/LICENSE.md) para detalhes.

> Copyright © 2026 — Sondvolt Team
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

<p align="center">
  <strong>Built with ❤️ by Eletrônica DIY (2026)</strong><br>
  <em>Transformando projetos DIY em ferramentas profissionais</em>
</p>