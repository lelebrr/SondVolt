# 🤝 Guia de Contribuição — Sondvolt v3.2

<p align="center">
  <img src="../assets/logo.png" alt="Sondvolt Logo" width="150">
</p>

Obrigado pelo seu interesse em contribuir com o Component Tester PRO! Este documento fornece todas as informações necessárias para começar a colaborar com o projeto.

---

## Índice

1. [Código de Conduta](#código-de-conduta)
2. [Formas de Contribuir](#formas-de-contribuir)
3. [Configuração do Ambiente](#configuração-do-ambiente)
4. [Processo de Desenvolvimento](#processo-de-desenvolvimento)
5. [Padrões de Código](#padrões-de-código)
6. [Envio de Pull Request](#envio-de-pull-request)
7. [Reconhecimento](#reconhecimento)

---

## Código de Conduta

Este projeto adota o [Contributor Covenant](https://www.contributor-covenant.org/) versão 2.1. Ao contribuir, você concorda em seguir este código.

**Resumo das regras:**
- Ser respeitoso e acolhedor
- Aceitar críticas construtivas de forma profissional
- Focar no que é melhor para a comunidade
- Evitar linguagem e comportamento discriminatory

---

## Formas de Contribuir

Há várias formas de contribuir com o projeto:

| Tipo | Descrição | Como Contribuir |
|:---|:---|:---|
| **Código** | Implementar novas funcionalidades ou correções | Pull Requests |
| **Documentação** | Melhorar manuais e guias | Editar arquivos em `docs/` |
| **Testes** | Criar e manter testes unitários | Adicionar em `test/` |
| **Feedback** | Reportar bugs e sugerir funcionalidades | Issues no GitHub |
| **Tradução** | Traduzir documentação | Criar arquivos localizeados |
| **，社区** | Responder dúvidas | Discussões |
| **Doação** | Apoiar financeiramente | Via PayPal/PIX |

---

## Configuração do Ambiente

### Pré-requisitos

- **Hardware:**
  - Computador com Windows, macOS ou Linux
  - Cabo USB-C (para UART)

- **Software:**
  - [VS Code](https://code.visualstudio.com/) (recomendado)
  - [PlatformIO](https://platformio.org/) (extensão para VS Code)
  - [Git](https://git-scm.com/)
  - [Python 3.8+](https://python.org/) (para scripts)

### Instalação

1. **Clone o repositório:**

```bash
git clone https://github.com/lelebrr/Component_Tester.git
cd Component_Tester
```

2. **Abra no VS Code:**

```bash
code .
```

3. **Instale as dependências:**

O PlatformIO instalará as dependências automaticamente ao compilar.

4. **Configure o SD Card:**

- Formate um cartão microSD em **FAT32**
- Copie `sd_files/COMPBD.CSV` para a raiz do cartão
- Insira o cartão no slot do CYD

### Compilação

```bash
# Compile o firmware
pio run -e cyd

# Compile e faça upload
pio run -e cyd --target upload

# Monitor serial
pio device monitor --monitor-speed 115200
```

---

## Processo de Desenvolvimento

### 1. Escolha uma Tarefa

- Browse as [Issues](https://github.com/lelebrr/Component_Tester/issues)
- Escolha uma issue marcada como `good first issue` para iniciantes
- Ou propose uma nova funcionalidade

### 2. Crie uma Branch

```bash
# Atualize seu repositório local
git fetch origin
git checkout main
git pull origin main

# Crie uma nova branch
git checkout -b feature/minha-nova-funcionalidade
# ou
git checkout -b fix/descricao-do-bug
```

### 3. Faça as Alterações

- Implemente sua funcionalidade
- Ou corrija o bug
- Mantenha o código limpo e organizado

### 4. Teste Localmente

```bash
# Compile para verificar erros
pio run -e cyd

# Se houver testes, execute-os
pio test -e cyd
```

### 5. Commit as Alterações

```bash
# Adicione os arquivos modificados
git add .

# Faça o commit com mensagem descritiva
git commit -m "feat: adicionar modo de teste para MOSFETs

- Adicionado novo modo de teste no menu
- Curva Vgs vs Id implementada
- Banco de dados atualizado com MOSFETs comuns"
```

### 6. Push e Pull Request

```bash
# Faça push da sua branch
git push -u origin feature/minha-nova-funcionalidade
```

Agora abra um Pull Request no GitHub.

---

## Padrões de Código

Este projeto segue padrões específicos para manter a consistência.

### style Guide

**Nomenclatura:**
```cpp
// Funções: camelCase comverbo no início
void initDisplay();
void readVoltage();
void calibrateSensor();

// Variáveis: camelCase
int currentValue;
float temperatureCelsius;

// Constantes: SCREAMING_SNAKE_CASE
const int MAX_ADC_VALUE = 4095;
const float VREF = 3.3;

// Classes/Structs: PascalCase
class DisplayManager {};
struct ComponentData {};
```

**Indentação:**
- Use 2 espaços (não tabs)
- Não use tabs nos arquivos

**Comentários:**
- Comente funções públicas com Doxygen
- Comente código complexo quando necessário
- Evite comentários óbvios

### Estrura de Arquivos

```
src/
├── main.cpp                 # Loop principal
├── config/
│   └── config.h             # Configurações do projeto
├── display/
│   ├── display.cpp
│   └── display.h
├── sensors/
│   ├── zMPT101B.cpp
│   └── zMPT101B.h
├── components/
│   ├── component.cpp
│   └── component.h
├── storage/
│   ├── sdcard.cpp
│   └── sdcard.h
└── utils/
    └── utils.cpp
```

### Convenções de Commits

Siga o padrão [Conventional Commits](https://www.conventionalcommits.org/):

```
<tipo>(<escopo>): <descrição>

[corpo opcional]

[tipo opcional: #123]
```

**Tipos:**
- `feat`: Nova funcionalidade
- `fix`: Correção de bug
- `docs`: Documentação
- `style`: Formatação
- `refactor`: Refatoração
- `test`: Testes
- `chore`: Tarefas administrativas

**Exemplos:**
```
feat(multimeter): adicionar modo de tensão AC True RMS

Corrige #45

Adicionado modo de tensão AC com medição True RMS usando ZMPT101B.
Configurado para mostrar valor RMS real, não média.
```

```
fix(sensors): corrigir leitura negativa no INA219

O sensor теперь retorna valores negativos para corrente.
Corrigido problema de overflow em medisões de descarga.
```

### Convenções de Pull Requests

- Use o modelo de PR do GitHub
- Descreva claramente as mudanças
- Inclua screenshots para mudanças visuais
- Liste testes realizados
- Marque issues relacionadas

---

## Envio de Pull Request

### Checklist Antes do PR

- [ ] Código compila sem erros ou warnings
- [ ] Testes passam (se aplicável)
- [ ] Código segue os padrões de estilo
- [ ] Documentação atualizada (se aplicável)
- [ ] Commit messages claros e descritivos
- [ ] Branch está atualizada com main

### Processo de Revisão

1. **Aguarde a revisão** — Maintainers revisarão seu PR
2. **Responda ao feedback** — Faça alterações solicitadas
3. **Aguarde aprovação** — Após aprovado, será mergeado
4. **Celebre!** — Sua contribuição foi aceita!

### O que Esperar

- **Tempo de resposta:** 1-7 dias
- **Frequência de merges:** Semanal
- **Feedback:** Respeitoso e construtivo

---

## Reconhecimento

Todos os contribuidores serão reconhecidos. Formas de reconhecimento:

| Tipo | Reconhecimento |
|:---|:---|
| **Primeiro PR** | Mention no README |
| **Contributor Regular** | Adição ao Hall of Fame |
| **Maintainer** | Convite para equipe |
| **Doador** | Mention no Hall of Fame + Badge |

### Hall of Fame

Consulte o arquivo [AUTHORS.md](AUTHORS.md) para ver todos os contribuidores.

---

## Perguntas Frequentes

**Posso contribuir sem saber programar?**
Sim! Você pode contribuir com documentação, tradução, feedback, ou doar.

**Preciso de hardware para testar?**
Idealmente sim, mas aceitamos contribuições de documentação mesmo sem hardware.

**Como reportar um bug?**
Abra uma Issue com a tag `bug` e siga o modelo fornecido.

**Posso propor uma nova funcionalidade?**
Sim! Abra uma Issue com a tag `feature-request` e descreva sua ideia.

---

## Contato

- **GitHub Issues:** Para bugs e funcionalidades
- **GitHub Discussions:** Para perguntas gerais
- **Email:** Para assuntos privados

Obrigado por contribuir com o Component Tester PRO!

---

<p align="center">
<i>🤝 Última atualização: Abril de 2026 — Sondvolt Team v3.2</i>
</p>