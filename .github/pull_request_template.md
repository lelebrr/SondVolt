## O que muda

<!-- Uma frase. Se precisar de mais, provavelmente são dois PRs. -->

## Por quê

<!-- O problema que isso resolve. Se corrige um bug, descreva o sintoma. -->

## Verificação

- [ ] `bash tools/hostcheck/check.sh` passa limpo nas **três** revisões
- [ ] Compila sem avisos com `-Wall -Wextra`
- [ ] Testado em hardware real — revisão: <!-- A / B / C / não testado -->

## Hardware

- [ ] Não mexe em pinagem
- [ ] Mexe em pinagem — `pins.h` atualizado e `static_assert` passa
- [ ] Precisa de peça nova — BOM atualizada com as alternativas

## Documentação

- [ ] Não precisa
- [ ] CHANGELOG atualizado
- [ ] Documento afetado atualizado: <!-- WIRING / PINOUT / DEVELOP / ... -->

<!--
Convenções do projeto (docs/DEVELOP.md):
  - Comentários explicam POR QUÊ, não O QUÊ
  - snprintf sempre, sprintf nunca
  - Nada acima da HAL toca pino compartilhado diretamente
  - Medição que não pode ser feita devolve "indisponível", nunca um número inventado
-->
