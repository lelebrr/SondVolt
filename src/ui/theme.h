// ============================================================================
// Sondvolt v5.0 - Sistema de Design
// ============================================================================
// Arquivo : theme.h
//
// Este e o unico dono da aparencia do aparelho. Cor, espacamento, raio de
// canto e escala de fonte saem daqui - nao de numeros soltos espalhados
// pelas telas.
//
// POR QUE ISSO IMPORTA NUMA TELA DE 320x240
// -----------------------------------------
// Um display pequeno perdoa muito pouco. Se cada tela escolhe sua propria
// margem, o resultado parece desalinhado mesmo quando cada peca isolada esta
// certa. Uma escala fixa de espacamento resolve isso de graca.
//
// AS DECISOES DE COR
// ------------------
// A paleta e escura por padrao porque o aparelho vive em bancada, muitas
// vezes sob luminaria forte apontada para a placa - fundo escuro cansa menos
// e faz o valor medido saltar.
//
// Os tons vem de uma rampa de cinza-azulado (nao cinza puro): o azul leve
// afasta a interface do preto morto e faz as cores de acento parecerem mais
// vivas por contraste. E o mesmo truque de interface de editor de codigo.
//
// Todas as combinacoes de texto sobre fundo foram escolhidas com contraste
// suficiente para leitura a meio metro, que e a distancia real de quem esta
// com o ferro de solda na mao.
// ============================================================================

#ifndef THEME_H
#define THEME_H

#include <Arduino.h>

// ============================================================================
// 1. RAMPA DE SUPERFICIES
// ============================================================================
// Quatro niveis de profundidade. Cada um e um passo perceptivel do anterior:
// menos que isso e invisivel na TFT, mais que isso vira degrau feio.

#define TH_DARK_BG_0        0x0841   // fundo da tela        (#08111f)
#define TH_DARK_BG_1        0x10A3   // cartao               (#101a2b)
#define TH_DARK_BG_2        0x1926   // cartao elevado       (#182636)
#define TH_DARK_BG_3        0x2989   // borda e divisoria    (#283245)

#define TH_DARK_TX_HI       0xFFFF   // valor principal
#define TH_DARK_TX_MD       0xC618   // texto de corpo
#define TH_DARK_TX_LO       0x8410   // rotulo e legenda
#define TH_DARK_TX_DIM      0x5AEB   // texto desabilitado

// Tema claro para bancada com muita luz natural. Mantem a MESMA estrutura de
// quatro niveis, so invertida - por isso qualquer tela funciona nos dois.
#define TH_LIGHT_BG_0       0xF79E   // #f0f2f5
#define TH_LIGHT_BG_1       0xFFFF
#define TH_LIGHT_BG_2       0xEF7D
#define TH_LIGHT_BG_3       0xCE79

#define TH_LIGHT_TX_HI      0x0841
#define TH_LIGHT_TX_MD      0x31A6
#define TH_LIGHT_TX_LO      0x6B4D
#define TH_LIGHT_TX_DIM     0x9CD3

// ============================================================================
// 2. CORES DE ACENTO
// ============================================================================
// Cada uma tem UM significado, sempre o mesmo, em qualquer tela. E o que
// permite o usuario aprender a interface sem ler nada.

#define TH_ACCENT           0x2F7D   // ciano-turquesa: o instrumento, o ativo
#define TH_ACCENT_DIM       0x1BCF   // o mesmo, recuado

#define TH_SUCCESS          0x3766   // verde: aprovado, dentro da faixa
#define TH_WARNING          0xFD40   // ambar: suspeito, exige atencao
#define TH_DANGER           0xF9A6   // vermelho: reprovado, perigo eletrico
#define TH_INFO             0x5D5F   // azul: informacao neutra
#define TH_SPECIAL          0xA51F   // roxo: modo especial, comparacao

// Verde de traco para o osciloscopio. Mais saturado que TH_SUCCESS porque
// precisa se destacar sobre preto puro, e e o unico lugar onde isso acontece.
#define TH_TRACE            0x07E8

// ============================================================================
// 3. ESCALA DE ESPACAMENTO
// ============================================================================
// Multiplos de 4 pixels. Toda margem, todo respiro entre elementos sai desta
// lista. Nao existe "um pouquinho mais": ou e SP_2 ou e SP_3.

#define TH_SP_1             4
#define TH_SP_2             8
#define TH_SP_3             12
#define TH_SP_4             16
#define TH_SP_5             20
#define TH_SP_6             24

// Margem lateral padrao da tela. Tudo se alinha a ela.
#define TH_MARGIN           TH_SP_3

// ============================================================================
// 4. RAIOS DE CANTO
// ============================================================================
// Tres, nao cinco. Em 320x240 a diferenca entre raio 5 e raio 6 nao existe.

#define TH_RADIUS_SM        4        // botao, chip, barra
#define TH_RADIUS_MD        8        // cartao
#define TH_RADIUS_LG        12       // dialogo, painel principal

// ============================================================================
// 5. ALTURAS DE COMPONENTE
// ============================================================================
// Fixas, para os elementos alinharem entre telas diferentes sem esforco.

#define TH_HEADER_H         38       // cabecalho com titulo e voltar
#define TH_STATUSBAR_H      14       // rodape de estado
#define TH_BUTTON_H         28       // altura de toque confortavel
#define TH_ROW_H            26       // linha de lista
#define TH_CHIP_H           16       // etiqueta de status

// Area util entre o cabecalho e a barra de status.
#define TH_CONTENT_Y        (TH_HEADER_H + TH_SP_1)
#define TH_CONTENT_H        (240 - TH_HEADER_H - TH_STATUSBAR_H - TH_SP_2)

// Alvo minimo de toque. Abaixo disso o dedo erra - e um dedo com luva de
// bancada erra ainda mais.
#define TH_TOUCH_MIN        30

// ============================================================================
// 6. ESCALA TIPOGRAFICA
// ============================================================================
// A fonte e bitmap 5x7 escalada por inteiro. Nao existe meio tamanho, entao a
// escala e a propria lista de multiplicadores.

#define TH_FONT_CAPTION     1        //  5x7  - legenda, unidade
#define TH_FONT_BODY        1        //  5x7  - texto corrido
#define TH_FONT_SUBTITLE    2        // 10x14 - titulo de secao
#define TH_FONT_TITLE       3        // 15x21 - destaque
#define TH_FONT_DISPLAY     4        // 20x28 - valor medido
#define TH_FONT_HERO        5        // 25x35 - valor principal

// Largura de um caractere em cada escala (5 pixels + 1 de espaco).
#define TH_CHAR_W(size)     (6 * (size))
#define TH_CHAR_H(size)     (8 * (size))

// Largura em pixels de uma string, sem precisar da fonte carregada.
#define TH_TEXT_W(str, size)  ((int16_t)(strlen(str) * TH_CHAR_W(size)))

// ============================================================================
// 7. TEMPOS DE ANIMACAO
// ============================================================================
// Curtos de proposito. Animacao em instrumento de medicao nao pode atrasar a
// leitura: ela existe para dar continuidade visual, nao para impressionar.

#define TH_ANIM_INSTANT     0
#define TH_ANIM_FAST        120      // realce de toque
#define TH_ANIM_NORMAL      200      // troca de tela
#define TH_ANIM_SLOW        350      // entrada do dialogo

// ============================================================================
// 8. PALETA ATIVA (resolvida em runtime por theme_apply)
// ============================================================================
// As telas usam SEMPRE estes nomes. Trocar entre claro e escuro, ou mudar a
// cor de acento, muda apenas o conteudo destas variaveis.

extern uint16_t th_bg0;      // fundo da tela
extern uint16_t th_bg1;      // superficie de cartao
extern uint16_t th_bg2;      // superficie elevada
extern uint16_t th_bg3;      // borda e divisoria

extern uint16_t th_txHi;     // texto de maximo contraste
extern uint16_t th_txMd;     // texto de corpo
extern uint16_t th_txLo;     // rotulo
extern uint16_t th_txDim;    // desabilitado

extern uint16_t th_accent;   // cor de acento escolhida pelo usuario
extern uint16_t th_accentDim;

// ----------------------------------------------------------------------------
// Cores de acento disponiveis nos ajustes
// ----------------------------------------------------------------------------
enum ThemeAccent {
    TH_ACCENT_TEAL = 0,     // turquesa (padrao)
    TH_ACCENT_GREEN,
    TH_ACCENT_AMBER,
    TH_ACCENT_VIOLET,
    TH_ACCENT_BLUE,
    TH_ACCENT_COUNT
};

// Aplica o tema. Chamar no boot e sempre que o usuario mudar um ajuste.
void theme_apply(bool darkMode, ThemeAccent accent);

// Nome legivel de uma cor de acento, para a tela de ajustes.
const char* theme_accent_name(ThemeAccent accent);

// Amostra da cor, para desenhar o seletor.
uint16_t theme_accent_swatch(ThemeAccent accent);

// ============================================================================
// 9. UTILIDADES DE COR
// ============================================================================

// Mistura duas cores RGB565. alpha 0 devolve `a`, 255 devolve `b`.
// Faz a interpolacao por canal, respeitando os 5-6-5 bits de cada um.
inline uint16_t th_mix(uint16_t a, uint16_t b, uint8_t alpha) {
    const uint8_t ar = (a >> 11) & 0x1F, ag = (a >> 5) & 0x3F, ab = a & 0x1F;
    const uint8_t br = (b >> 11) & 0x1F, bg = (b >> 5) & 0x3F, bb = b & 0x1F;

    const uint8_t r = (uint8_t)((ar * (255 - alpha) + br * alpha) >> 8);
    const uint8_t g = (uint8_t)((ag * (255 - alpha) + bg * alpha) >> 8);
    const uint8_t bl = (uint8_t)((ab * (255 - alpha) + bb * alpha) >> 8);

    return (uint16_t)((r << 11) | (g << 5) | bl);
}

// Versao suave de uma cor, para fundo de destaque: mistura com a superficie
// ate sobrar so uma insinuacao do tom original.
inline uint16_t th_tint(uint16_t color, uint16_t surface) {
    return th_mix(surface, color, 46);
}

// Versao ainda mais suave, para faixa de fundo atras de texto.
inline uint16_t th_wash(uint16_t color, uint16_t surface) {
    return th_mix(surface, color, 24);
}

// Escurece uma cor - usado em sombra e em estado pressionado.
inline uint16_t th_shade(uint16_t color, uint8_t amount) {
    return th_mix(color, 0x0000, amount);
}

// Clareia uma cor - usado em realce e borda superior de cartao.
inline uint16_t th_light(uint16_t color, uint8_t amount) {
    return th_mix(color, 0xFFFF, amount);
}

// Cor de texto que garante leitura sobre um fundo qualquer.
// Calcula a luminancia percebida (o olho e muito mais sensivel ao verde) e
// escolhe preto ou branco - o que nao se faz e chutar.
inline uint16_t th_on(uint16_t background) {
    const uint16_t r = ((background >> 11) & 0x1F) * 255 / 31;
    const uint16_t g = ((background >> 5)  & 0x3F) * 255 / 63;
    const uint16_t b = (background & 0x1F) * 255 / 31;

    // Coeficientes de luminancia da ITU-R BT.601, em inteiros.
    const uint32_t luma = (r * 299 + g * 587 + b * 114) / 1000;
    return (luma > 140) ? 0x0000 : 0xFFFF;
}

// Cor de status a partir de um julgamento. Centraliza a regra para nenhuma
// tela inventar a propria.
inline uint16_t th_status_color(uint8_t status) {
    switch (status) {
        case 0:  return TH_SUCCESS;   // STATUS_GOOD
        case 1:                       // STATUS_SUSPECT
        case 2:  return TH_WARNING;   // STATUS_WARNING
        case 3:                       // STATUS_BAD
        case 7:  return TH_DANGER;    // STATUS_SHORT
        case 6:  return TH_WARNING;   // STATUS_LEAKY
        default: return th_txDim;
    }
}

// ============================================================================
// 10. COMPATIBILIDADE
// ============================================================================
// Nomes antigos apontando para os tokens novos, para as telas existentes
// continuarem compilando enquanto sao migradas.

#define THEME_RADIUS        TH_RADIUS_MD
#define THEME_MARGIN        TH_MARGIN
#define THEME_HEADER_H      TH_HEADER_H
#define THEME_FOOTER_H      TH_STATUSBAR_H
#define FONT_SIZE_SMALL     TH_FONT_BODY
#define FONT_SIZE_MEDIUM    TH_FONT_SUBTITLE
#define FONT_SIZE_LARGE     TH_FONT_TITLE
#define FONT_SIZE_HUGE      TH_FONT_HERO
#define THEME_SHADOW_COLOR  0x0000
#define THEME_HIGHLIGHT     0xFFFF
#define THEME_MEASURING     TH_INFO
#define THEME_THERMAL       TH_SPECIAL

#endif // THEME_H
