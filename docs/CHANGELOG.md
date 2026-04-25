# Changelog — Component Tester PRO v3.2

Este documento apresenta o histórico de versões do Component Tester PRO, incluindo novas funcionalidades, correções de bugs e outras mudanças importantes.

---

## Formato das Entradas

Cada entrada segue o formato:

```markdown
## [Versão] — Data

### Adicionado
### Alterado
### Corrigido
### Removido
### BREAKING CHANGES
```

---

## [3.2.0] — Abril 2026

### Adicionado
- **Motor True RMS Profissional** — Medição de tensão AC com 128 amostras e detecção de **Tensão de Pico**.
- **Sistema de Segurança Multinível** — Bloqueio automático por software se detectar >50V fora do modo multímetro.
- **Detecção de Surtos e Transientes** — Alerta visual `[SURGE!]` para picos de tensão anormais.
- **Tela de Confirmação de Hardware** — Bloqueio mandatório que exige confirmação de componentes de proteção (Fusível/Varistor/TVS).
- **Auditoria de Materiais (BOM)** — Novo guia detalhado para montagem segura em 220V.
- **Exportação de dados** — CSV formatado para análise externa com novos campos de pico.

### Alterado
- **Amostragem Otimizada** — Buffer de 128 amostras (potência de 2) para maior precisão e menor jitter.
- **Interface de Multímetro** — Layout renovado exibindo RMS, Pico e Status de Segurança simultaneamente.
- **Proteção de Hardware** — Novos requisitos obrigatórios (Fusível 5A, Varistor 14D431, TVS P6KE400A).
- **Refatoração do Watchdog de Segurança** — Agora rodando em background no TaskMeasurement.
- **Melhoria na Precisão AC** — Redução do erro para ±1.5% em toda a faixa de 0-250V.

### Corrigido
- Problemas de estabilidade com medições de alta frequência
- Erros de calibração em temperaturas extremas
- Lentidão na inicialização do sistema
- Bugs na interface touchscreen
- Inconsistências nos dados salvos no SD Card
- Problemas de memória em operações contínuas

### Removido
- Modos de teste obsoletos e não documentados
- Suporte a hardware legado (ATmega328P)

### BREAKING CHANGES
- Requer ESP32 com firmware atualizado
- Formato do banco de dados alterado (compatibilidade mantida com conversão automática)
- Nova pinagem para sensores externos

---

## [3.0.0] — Abril 2026

### Adicionado
- **Interface CYD completa** — Primeira versão estável para ESP32-2432S028R (Cheap Yellow Display)
- **13 modos de medição** — Suporte para teste de componentes:
  - Resistor
  - Capacitor
  - Diodo
  - LED
  - Zener
  - Transistor NPN
  - Transistor PNP
  - MOSFET N-Channel
  - MOSFET P-Channel
  - Tiristor (SCR)
  - Triac
  - DIAC
  - Indutor
- **Modo Auto-detecção** — Scanner automático de componentes
- **Multímetro AC/DC** — Medição de tensão AC (via ZMPT101B) e DC (via INA219)
- **Sensor de temperatura** — Suporte para sonda DS18B20 via OneWire
- **Interface touchscreen** — Display TFT 2.8" ILI9341 com touch resistivo XPT2046
- **Banco de dados SQLite-like** — Arquivo COMPBD.CSV no cartão SD
- **Datalogger** — Registro de medições com timestamp no SD Card
- **Visualização de histórico** — Últimas medições salvas
- **Sistema de estatísticas** — Uso do dispositivo
- **Calibração** — Offset e ganho ajusteis para cada modo
- **Interface Cyber Dark** — Tema visual premium
- **Suporte a múltiplos probes** — Probe positivo e negativo

### Alterado
- **Migração de ATmega328P para ESP32** — Grande refatoração da base de código
- **Novo sistema de menus** — Navegação grid 2x3 via touchscreen
- **Display maior** — TFT 2.8" (320x240) substituindo display 16x2 ou 20x4
- **Armazenamento** — SD Card substituindo EEPROM interna
- **Processamento** — Dual Core 240MHz vs 16MHz single core

### Corrigido
- Limitações de memória do ATmega328P
- Precisão limitada de medição
- Interface de usuário limitada
- Banco de dados pequeno

### BREAKING CHANGES
- Pinagem completamente alterada para ESP32
- Banco de dados agora em CSV no SD Card (não mais hardcoded)
- Require formulário SD Card para operação completa
- Sensores externos (ZMPT101B, INA219, DS18B20) são opcionais

---

## Versões Legado

*As versões 2.x e anteriores baseadas em ATmega328P foram descontinuadas. O histórico completo está disponível no branch `legacy`.*

---

## Roadmap de Versões Futuras

Consulte o arquivo [ROADMAP.md](ROADMAP.md) para informações sobre versões futuras.

---

## Como Ler o Changelog

### Convenções de Semântica

| Símbolo | Significado |
|:---|:---|
| `[+]` | Nova funcionalidade |
| `[~]` | Alteração existente |
| `[-]` | Funcionalidade removida |
| `[!]` | Mudança incompatível (Breaking Change) |
| `[x]` | Correção de bug |

### Tipos de Mudanças

- **Adicionado**: Novas funcionalidades
- **Alterado**: Funcionalidades existentes modificadas
- **Corrigido**: Bugs e problemas resolvidos
- **Removido**: Funcionalidades removidas
- **BREAKING CHANGES**: Mudanças que quebram compatibilidade versões anteriores

---

## Contribuindo para o Changelog

Para adicionar uma entrada ao changelog:

1. Abra uma Issue com a tag `changelog`
2. Descreva a mudança realizada
3. Maintainers adicionarão à próxima versão

Ou envie um Pull Request diretamente editando este arquivo.

---

## Histórico

| Versão | Data | Status |
|:---|:---|:---:|
| 3.2.0 | Abril 2026 | ✅ Estável |
| 3.0.0 | Abril 2026 | ✅ Estável |
| 2.x e anteriores | Antes 2026 | ⚠️ Descontinuado |

---

<p align="center">
<i>Última atualização: Abril de 2026 — Component Tester PRO Team</i>
</p>