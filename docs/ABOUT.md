# Sobre o Component Tester PRO v3.2 — História e Objetivo

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

---

## História do Projeto

O **Component Tester PRO** nasceu da necessidade de criar uma ferramenta de teste de componentes acessível, porém profissional, para makers e técnicos em eletrônica. O projeto começou como um protótipo simples utilizando o popular Cheap Yellow Display (ESP32-2432S028R), uma placa de desenvolvimento de baixo custo que combina um display TFT touchscreen com um microcontrolador ESP32 poderoso.

O nome "Component Tester PRO" reflete a missão fundamental do equipamento: fornecer uma ferramenta profissional e completa para teste e análise de componentes eletrônicos.

### Evolução

| Versão | Marco |
|:-------|:------|
| v1.0 | Protótipo inicial com teste básico de resistores |
| v2.0 | Adição de capacitores, diodos e transistores |
| v2.5 | Integração do multímetro AC/DC |
| v3.0 | Sistema completo com segurança, MicroSD e interface profissional |
| **v3.2** | **Versão atual** — True RMS, segurança avançada e auditoria de materiais |

O projeto evoluiu de um testador simples para um verdadeiro laboratório portátil de eletrônica, capaz de testar mais de 12 tipos de componentes diferentes e funcionar como multímetro True RMS completo.

---

## Objetivo do Equipamento

### Missão

> **"Proporcionar aos makers, técnicos e entusiastas uma ferramenta profissional de teste de componentes e medições elétricas, acessível e de código aberto."**

### Objetivos Específicos

1. **Identificação Automática**: Detectar e caracterizar componentes eletrônicos sem necessidade de conhecimento prévio das especificações

2. **Medições Precisas**: Fornecer leituras confiáveis de tensão, corrente, resistência e capacitância com precisão de ±2%

3. **Segurança em Primeiro Lugar**: Proteger o usuário e o equipamento através de bloqueios automáticos e alertas em medições de alta tensão

4. **Acessibilidade**: Manter o projetoopen source, com custos de hardware mínimos, para democratizar o acesso à ferramenta

5. **Portabilidade**: Oferecer um equipamento compacto e autossuficiente que funcione como laboratório de eletrônica portátil

---

## Diferenciais em Relação a Outros Testadores

### Comparativo com Opções do Mercado

| Característica | Component Tester PRO v3.2 | LCR-T4/T7 | Multímetro Tradicional |
|:---------------|:--------------------------|:----------|:----------------------|
| **Teste de componentes** | ✓ Sim (>12 tipos) | ✓ Sim | ✗ Não |
| **Multímetro True RMS** | ✓ Sim | ✗ Não | ✓ Sim (profissional) |
| **Display touchscreen** | ✓ Sim | ✓ Sim | ✗ Não |
| **Banco de dados personalizável** | ✓ CSV no MicroSD | ✗ Limitado | ✗ Não |
| **Histórico de medições** | ✓ Com data/hora | ✗ Não | ✗ Não |
| **Código aberto** | ✓ Sim | ✗ Não | ✗ Não |
| **Custo** | ~R$150-200 | ~R$200-300 | ~R$100-500 |
| **Medição 220V** | ✓ Com proteção | ✗ Não | ✓ Sim |
| **Auditoria de materiais** | ✓ BOM detalhado | ✗ Não | ✗ Não |

### Vantagens Competitivas

1. **Versatilidade Incomparável**
   - Único equipamento que combina testador de componentes E multímetro True RMS
   - Não há necessidade de dois equipamentos distintos

2. **Personalização Total**
   - Banco de dados em CSV permite adicionar componentes customizados
   - Código aberto para modificações e melhorias

3. **Interface Profissional**
   - Display touchscreen intuitivo
   - Visual moderno com ícones vetoriais e animações

4. **Registro de Dados**
   - Histórico automático com timestamp
   - Ideal para documentação e análise de medições

5. **Segurança Avançada**
   - Bloqueio automático para tensões perigosas
   - Confirmação obrigatória para medições de 220V
   - Alertas visuais e sonoros

---

## Especificações Diferenciadas

| Recurso | Detalhe |
|:--------|:--------|
| **True RMS Real** | Usa transformador ZMPT101B para medições AC precisas |
| **Corrente Bidirecional** | Sensor INA219 mede corrente em ambas as direções |
| **Calibração Automática** | Sistema compensa tolerâncias das pontas de prova |
| **Atualização de Banco de Dados** | COMPBD.CSV editável diretamente no MicroSD |
| **Log de Operações** | Cada medição é registrada com data, hora e status |

---

## Versão

**Component Tester PRO v3.2**

- Firmware: v3.2.0
- Hardware: ESP32-2432S028R (Cheap Yellow Display)
- Última atualização: Abril/2026

---

<p align="center">
<i>"Teste. Meça. Confirme."</i>
</p>