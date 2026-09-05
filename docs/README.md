# Documentação do Sondvolt

Índice da documentação. Se você chegou agora, comece pelo [README principal](../README.md).

---

## Comece por aqui

| Documento | Para quê |
| :--- | :--- |
| [MANUAL.md](MANUAL.md) | operação completa do aparelho |
| [GUIDES.md](GUIDES.md) | tutoriais passo a passo |
| [FAQ.md](FAQ.md) | dúvidas frequentes |
| [MENUS.md](MENUS.md) | mapa das telas |

## Vou montar o hardware

| Documento | Para quê |
| :--- | :--- |
| [../BOM-Sondvolt.md](../BOM-Sondvolt.md) | lista de materiais |
| **[WIRING.md](WIRING.md)** | **esquema de ligação — leia antes de soldar** |
| [PINOUT.md](PINOUT.md) | pinagem, conflitos e as duas revisões de fiação |
| [ASSEMBLY.md](ASSEMBLY.md) | montagem mecânica |
| [HARDWARE.md](HARDWARE.md) | especificações técnicas |
| [SAFETY.md](SAFETY.md) | proteção elétrica |

> [!IMPORTANT]
> O circuito de excitação das pontas descrito em [WIRING.md](WIRING.md) é **novo na v4.0 e obrigatório**. Sem ele, resistência e capacitância não têm como ser medidas — as pontas ficam em GPIOs de entrada apenas do ESP32. Foi a causa raiz de as medições nunca terem funcionado até a v3.2.

## Estou usando o aparelho

| Documento | Para quê |
| :--- | :--- |
| [COMPONENTS.md](COMPONENTS.md) | como interpretar cada tipo de componente |
| [CONFIG.md](CONFIG.md) | ajustes e calibração |
| **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** | **quando algo não funciona** |
| [TESTING.md](TESTING.md) | procedimentos de verificação |

## Vou mexer no código

| Documento | Para quê |
| :--- | :--- |
| **[DEVELOP.md](DEVELOP.md)** | **arquitetura, camadas e convenções** |
| [CHANGELOG.md](CHANGELOG.md) | o que mudou e por quê |
| [CONTRIBUTING.md](CONTRIBUTING.md) | como enviar contribuições |
| [ROADMAP.md](ROADMAP.md) | o que vem pela frente |
| [../futuros.md](../futuros.md) | ideias de expansão |

## Referência

| Documento | Para quê |
| :--- | :--- |
| [ABOUT.md](ABOUT.md) | sobre o projeto |
| [BRANDING.md](BRANDING.md) | identidade visual |
| [BOM_AUDIT.md](BOM_AUDIT.md) | auditoria de materiais |
| [LICENSE.md](LICENSE.md) | licença MIT |

---

## Verificação sem hardware

Dá para validar o firmware inteiro sem ter a placa:

```bash
bash tools/hostcheck/check.sh
```

Compila e **linka** todas as unidades de tradução num PC, usando stubs das bibliotecas Arduino, nas duas revisões de hardware. Pega erros de sintaxe, de tipo e símbolos faltando antes de você gravar na placa.

---

## Atalho por sintoma

| Situação | Vá para |
| :--- | :--- |
| Medição de componente não funciona | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → "Circuito de pontas ausente" |
| Tensão AC aparece sem estar conectada | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → trimpot do ZMPT |
| Corrente indisponível | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → INA219 |
| Temperatura some com LED aceso | [PINOUT.md](PINOUT.md) → conflito do GPIO4 |
| Toque no lugar errado | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → calibração do touch |
| Nada é gravado no cartão | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → cartão SD |
| Erro de `static_assert` ao compilar | [PINOUT.md](PINOUT.md) → verificação em tempo de compilação |
| Quero entender o que mudou na v4.0 | [CHANGELOG.md](CHANGELOG.md) |
