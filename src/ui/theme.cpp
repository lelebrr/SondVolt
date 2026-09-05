// ============================================================================
// Sondvolt v5.0 - Sistema de Design (implementacao)
// ============================================================================

#include "theme.h"

// ----------------------------------------------------------------------------
// Paleta ativa
// ----------------------------------------------------------------------------
// Comeca no tema escuro com acento turquesa. theme_apply() troca tudo isso de
// uma vez, e nenhuma tela precisa saber que o tema mudou - ela le os mesmos
// nomes de sempre.

uint16_t th_bg0       = TH_DARK_BG_0;
uint16_t th_bg1       = TH_DARK_BG_1;
uint16_t th_bg2       = TH_DARK_BG_2;
uint16_t th_bg3       = TH_DARK_BG_3;

uint16_t th_txHi      = TH_DARK_TX_HI;
uint16_t th_txMd      = TH_DARK_TX_MD;
uint16_t th_txLo      = TH_DARK_TX_LO;
uint16_t th_txDim     = TH_DARK_TX_DIM;

uint16_t th_accent    = TH_ACCENT;
uint16_t th_accentDim = TH_ACCENT_DIM;

// ----------------------------------------------------------------------------
// Cores de acento
// ----------------------------------------------------------------------------
// Cada entrada tem a cor cheia e uma versao recuada. A recuada nao e a cheia
// escurecida na hora: e um tom escolhido para continuar legivel sobre o fundo
// escuro, coisa que escurecer mecanicamente nem sempre garante.

struct AccentPair {
    uint16_t full;
    uint16_t dim;
    const char* name;
};

static const AccentPair kAccents[TH_ACCENT_COUNT] = {
    { 0x2F7D, 0x1BCF, "Turquesa" },
    { 0x3766, 0x2444, "Verde"    },
    { 0xFD40, 0xA260, "Ambar"    },
    { 0xA51F, 0x6014, "Violeta"  },
    { 0x5D5F, 0x2A9B, "Azul"     },
};

// ----------------------------------------------------------------------------
// Aplicacao do tema
// ----------------------------------------------------------------------------

void theme_apply(bool darkMode, ThemeAccent accent) {
    if (accent >= TH_ACCENT_COUNT) accent = TH_ACCENT_TEAL;

    if (darkMode) {
        th_bg0  = TH_DARK_BG_0;
        th_bg1  = TH_DARK_BG_1;
        th_bg2  = TH_DARK_BG_2;
        th_bg3  = TH_DARK_BG_3;

        th_txHi = TH_DARK_TX_HI;
        th_txMd = TH_DARK_TX_MD;
        th_txLo = TH_DARK_TX_LO;
        th_txDim = TH_DARK_TX_DIM;
    } else {
        th_bg0  = TH_LIGHT_BG_0;
        th_bg1  = TH_LIGHT_BG_1;
        th_bg2  = TH_LIGHT_BG_2;
        th_bg3  = TH_LIGHT_BG_3;

        th_txHi = TH_LIGHT_TX_HI;
        th_txMd = TH_LIGHT_TX_MD;
        th_txLo = TH_LIGHT_TX_LO;
        th_txDim = TH_LIGHT_TX_DIM;
    }

    th_accent    = kAccents[accent].full;
    th_accentDim = kAccents[accent].dim;

    // No tema claro o acento precisa escurecer um pouco para manter contraste
    // sobre fundo branco. O mesmo turquesa que brilha no escuro fica lavado
    // no claro.
    if (!darkMode) {
        th_accent    = th_shade(th_accent, 70);
        th_accentDim = th_shade(th_accentDim, 40);
    }
}

const char* theme_accent_name(ThemeAccent accent) {
    if (accent >= TH_ACCENT_COUNT) return "?";
    return kAccents[accent].name;
}

uint16_t theme_accent_swatch(ThemeAccent accent) {
    if (accent >= TH_ACCENT_COUNT) return kAccents[0].full;
    return kAccents[accent].full;
}
