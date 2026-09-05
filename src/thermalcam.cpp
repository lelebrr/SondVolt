// ============================================================================
// Sondvolt v5.0 - Camera Termica MLX90640 (implementacao)
// ============================================================================

#include "thermalcam.h"
#include "hal.h"
#include "config.h"
#include "globals.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "visual.h"
#include "fonts.h"
#include <Wire.h>
#include <SdFat.h>
#include <math.h>

#include <MLX90640_API.h>
#include <MLX90640_I2C_Driver.h>

extern SdFat sd;

// ----------------------------------------------------------------------------
// Estado
// ----------------------------------------------------------------------------
static paramsMLX90640  gParams;          // ~1,7 KB de coeficientes
static ThermalFrame    gFrame;
static bool            gPresent    = false;
static bool            gParamsOk   = false;
static uint8_t         gRefreshHz  = 2;
static ThermalPalette  gPalette    = TCAM_PALETTE_IRON;

// ============================================================================
// CICLO DE VIDA
// ============================================================================

bool tcam_init() {
    gPresent  = false;
    gParamsOk = false;
    memset(&gFrame, 0, sizeof(gFrame));

    if (!hal_bus_acquire(HAL_BUS_I2C, 500)) return false;

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FREQ_HZ);

    Wire.beginTransmission(MLX90640_ADDR);
    bool responds = (Wire.endTransmission() == 0);

    if (!responds) {
        hal_bus_release(HAL_BUS_I2C);
        LOG_SERIAL_F("[CAM] MLX90640 nao encontrado em 0x33");
        return false;
    }

    // A EEPROM do sensor tem 832 palavras de coeficientes de calibracao.
    // A leitura demora, mas so acontece uma vez por boot.
    static uint16_t eeprom[832];
    if (MLX90640_DumpEE(MLX90640_ADDR, eeprom) != 0) {
        hal_bus_release(HAL_BUS_I2C);
        LOG_SERIAL_F("[CAM] Falha ao ler a EEPROM do sensor");
        return false;
    }

    if (MLX90640_ExtractParameters(eeprom, &gParams) != 0) {
        hal_bus_release(HAL_BUS_I2C);
        LOG_SERIAL_F("[CAM] Coeficientes de calibracao invalidos");
        return false;
    }

    // Modo xadrez e o recomendado pelo fabricante; o modo TV entrelacado
    // produz artefatos visiveis em cenas com movimento.
    MLX90640_SetChessMode(MLX90640_ADDR);
    MLX90640_SetResolution(MLX90640_ADDR, 0x03);   // 19 bits, o maximo
    tcam_set_refresh_hz(gRefreshHz);

    hal_bus_release(HAL_BUS_I2C);

    gPresent  = true;
    gParamsOk = true;
    LOG_SERIAL_F("[CAM] MLX90640 pronto (32x24)");
    return true;
}

bool tcam_present()  { return gPresent; }
bool tcam_redetect() { return tcam_init(); }

void tcam_release() {
    gPresent  = false;
    gParamsOk = false;
}

void tcam_set_refresh_hz(uint8_t hz) {
    // O registrador aceita potencias de dois codificadas de 0 a 7.
    uint8_t code;
    switch (hz) {
        case 1:  code = 0x01; break;
        case 2:  code = 0x02; break;
        case 4:  code = 0x03; break;
        case 8:  code = 0x04; break;
        default: code = 0x02; hz = 2; break;
    }
    gRefreshHz = hz;
    if (gPresent) MLX90640_SetRefreshRate(MLX90640_ADDR, code);
}

uint8_t tcam_get_refresh_hz() { return gRefreshHz; }

// ============================================================================
// CAPTURA
// ============================================================================

bool tcam_read_frame(ThermalFrame* out) {
    if (!gPresent || !gParamsOk) return false;

    // Segurar o I2C durante toda a captura evita que uma medicao de
    // componente interrompa a leitura no meio e corrompa o quadro.
    if (!hal_bus_acquire(HAL_BUS_I2C, 800)) return false;

    // Um quadro completo do MLX90640 vem em duas subpaginas alternadas.
    static uint16_t raw[834];
    bool ok = true;

    for (uint8_t page = 0; page < 2 && ok; page++) {
        if (MLX90640_GetFrameData(MLX90640_ADDR, raw) < 0) { ok = false; break; }

        float ta = MLX90640_GetTa(raw, &gParams);
        // Emissividade de 0,95 e o valor tipico de plastico, placa de
        // circuito e componente encapsulado. Metal polido exigiria outro
        // valor - e por isso que dissipador de aluminio le mais frio do que
        // realmente esta.
        MLX90640_CalculateTo(raw, &gParams, 0.95f, ta - 8.0f, gFrame.pixels);
    }

    hal_bus_release(HAL_BUS_I2C);

    if (!ok) {
        LOG_SERIAL_F("[CAM] Falha ao ler quadro");
        return false;
    }

    // --- Estatisticas do quadro --------------------------------------------
    gFrame.minC = gFrame.pixels[0];
    gFrame.maxC = gFrame.pixels[0];
    double sum  = 0.0;
    gFrame.hotIndex  = 0;
    gFrame.coldIndex = 0;

    for (uint16_t i = 0; i < TCAM_PIXELS; i++) {
        float t = gFrame.pixels[i];

        // Pixel defeituoso ou leitura absurda: ignora em vez de estragar a
        // escala inteira da imagem.
        if (isnan(t) || t < -40.0f || t > 300.0f) {
            gFrame.pixels[i] = gFrame.minC;
            continue;
        }
        if (t < gFrame.minC) { gFrame.minC = t; gFrame.coldIndex = i; }
        if (t > gFrame.maxC) { gFrame.maxC = t; gFrame.hotIndex  = i; }
        sum += t;
    }

    gFrame.avgC    = (float)(sum / TCAM_PIXELS);
    gFrame.centerC = gFrame.pixels[(TCAM_ROWS / 2) * TCAM_COLS + (TCAM_COLS / 2)];
    gFrame.valid   = true;
    gFrame.timestampMs = millis();

    if (out) *out = gFrame;
    return true;
}

const ThermalFrame* tcam_last_frame() {
    return gFrame.valid ? &gFrame : nullptr;
}

// ============================================================================
// PALETAS
// ============================================================================

static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

uint16_t tcam_temp_to_color(float celsius, float minC, float maxC,
                            ThermalPalette palette) {
    if (maxC <= minC) return 0x0000;

    float t = (celsius - minC) / (maxC - minC);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    uint8_t r = 0, g = 0, b = 0;

    switch (palette) {
        case TCAM_PALETTE_IRON:
            // Preto -> roxo -> vermelho -> laranja -> amarelo -> branco.
            // E a paleta que o olho humano interpreta melhor como calor.
            if (t < 0.25f) {
                float k = t / 0.25f;
                r = (uint8_t)(80 * k); g = 0; b = (uint8_t)(120 * k);
            } else if (t < 0.50f) {
                float k = (t - 0.25f) / 0.25f;
                r = (uint8_t)(80 + 175 * k); g = 0;
                b = (uint8_t)(120 - 120 * k);
            } else if (t < 0.75f) {
                float k = (t - 0.50f) / 0.25f;
                r = 255; g = (uint8_t)(180 * k); b = 0;
            } else {
                float k = (t - 0.75f) / 0.25f;
                r = 255; g = (uint8_t)(180 + 75 * k); b = (uint8_t)(255 * k);
            }
            break;

        case TCAM_PALETTE_RAINBOW:
            if (t < 0.33f) {
                float k = t / 0.33f;
                r = 0; g = (uint8_t)(255 * k); b = (uint8_t)(255 - 155 * k);
            } else if (t < 0.66f) {
                float k = (t - 0.33f) / 0.33f;
                r = (uint8_t)(255 * k); g = 255; b = (uint8_t)(100 - 100 * k);
            } else {
                float k = (t - 0.66f) / 0.34f;
                r = 255; g = (uint8_t)(255 - 255 * k); b = 0;
            }
            break;

        case TCAM_PALETTE_GRAY:
        default:
            r = g = b = (uint8_t)(255 * t);
            break;
    }
    return rgb565(r, g, b);
}

void           tcam_set_palette(ThermalPalette p) { if (p < TCAM_PALETTE_COUNT) gPalette = p; }
ThermalPalette tcam_get_palette()                 { return gPalette; }

const char* tcam_palette_name(ThermalPalette p) {
    switch (p) {
        case TCAM_PALETTE_IRON:    return "Ferro";
        case TCAM_PALETTE_RAINBOW: return "Arco-iris";
        case TCAM_PALETTE_GRAY:    return "Cinza";
        default:                   return "?";
    }
}

// ============================================================================
// DESENHO
// ============================================================================

// Le um pixel da matriz com verificacao de limites.
static float pixel_at(const ThermalFrame* f, int col, int row) {
    if (col < 0) col = 0;
    if (row < 0) row = 0;
    if (col >= TCAM_COLS) col = TCAM_COLS - 1;
    if (row >= TCAM_ROWS) row = TCAM_ROWS - 1;
    return f->pixels[row * TCAM_COLS + col];
}

void tcam_draw(const ThermalFrame* frame, int16_t x, int16_t y,
               int16_t w, int16_t h) {
    if (!frame || !frame->valid || w < 8 || h < 8) return;

    const float minC = frame->minC;
    const float maxC = frame->maxC;

    // Interpolacao bilinear: para cada pixel da tela, encontra a posicao
    // correspondente na matriz de 32x24 e mistura os quatro vizinhos.
    // Sem isso a imagem fica em blocos grandes e ilegivel.
    LOCK_TFT();
    for (int16_t py = 0; py < h; py++) {
        float srcY = ((float)py / (float)h) * (TCAM_ROWS - 1);
        int   row  = (int)srcY;
        float fy   = srcY - row;

        for (int16_t px = 0; px < w; px++) {
            float srcX = ((float)px / (float)w) * (TCAM_COLS - 1);
            int   col  = (int)srcX;
            float fx   = srcX - col;

            float p00 = pixel_at(frame, col,     row);
            float p10 = pixel_at(frame, col + 1, row);
            float p01 = pixel_at(frame, col,     row + 1);
            float p11 = pixel_at(frame, col + 1, row + 1);

            float top    = p00 + (p10 - p00) * fx;
            float bottom = p01 + (p11 - p01) * fx;
            float value  = top + (bottom - top) * fy;

            tft.drawPixel(x + px, y + py,
                          tcam_temp_to_color(value, minC, maxC, gPalette));
        }
    }

    // --- Mira central -------------------------------------------------------
    int16_t cx = x + w / 2;
    int16_t cy = y + h / 2;
    tft.drawFastHLine(cx - 6, cy, 13, 0xFFFF);
    tft.drawFastVLine(cx, cy - 6, 13, 0xFFFF);

    // --- Marcador do ponto mais quente --------------------------------------
    int16_t hotCol = frame->hotIndex % TCAM_COLS;
    int16_t hotRow = frame->hotIndex / TCAM_COLS;
    int16_t hx = x + (int16_t)((hotCol * w) / TCAM_COLS);
    int16_t hy = y + (int16_t)((hotRow * h) / TCAM_ROWS);
    tft.drawCircle(hx, hy, 5, 0xFFFF);
    tft.drawCircle(hx, hy, 6, 0x0000);

    UNLOCK_TFT();
}

void tcam_draw_scale(const ThermalFrame* frame, int16_t x, int16_t y,
                     int16_t w, int16_t h) {
    if (!frame || !frame->valid) return;

    LOCK_TFT();
    // Gradiente vertical: quente em cima, frio embaixo.
    for (int16_t i = 0; i < h; i++) {
        float t = 1.0f - ((float)i / (float)h);
        float celsius = frame->minC + t * (frame->maxC - frame->minC);
        tft.drawFastHLine(x, y + i, w,
                          tcam_temp_to_color(celsius, frame->minC,
                                             frame->maxC, gPalette));
    }
    tft.drawRect(x - 1, y - 1, w + 2, h + 2, V_DIVIDER);

    char buf[12];
    snprintf(buf, sizeof(buf), "%.0fC", frame->maxC);
    draw_text_5x7(tft, x - 2, y - 10, buf, V_TEXT_MAIN, 1);
    snprintf(buf, sizeof(buf), "%.0fC", frame->minC);
    draw_text_5x7(tft, x - 2, y + h + 3, buf, V_TEXT_SUB, 1);
    UNLOCK_TFT();
}

// ============================================================================
// SNAPSHOT
// ============================================================================

bool tcam_save_snapshot(const char* name) {
    if (!gFrame.valid || !sdCardPresent) return false;

    char path[64];
    snprintf(path, sizeof(path), "/TERMICO/%.8s.CSV", name ? name : "SNAP");

    LOCK_TFT();
    sd.mkdir("/TERMICO");
    FsFile f = sd.open(path, FILE_WRITE);
    if (!f) { UNLOCK_TFT(); return false; }

    // Uma linha por fileira da matriz: abre direto em planilha e da para
    // gerar um mapa de calor no computador.
    char line[TCAM_COLS * 7 + 8];
    for (uint8_t row = 0; row < TCAM_ROWS; row++) {
        size_t pos = 0;
        for (uint8_t col = 0; col < TCAM_COLS; col++) {
            float t = gFrame.pixels[row * TCAM_COLS + col];
            int n = snprintf(line + pos, sizeof(line) - pos, "%.2f%s",
                             t, (col == TCAM_COLS - 1) ? "" : ";");
            if (n < 0 || (size_t)n >= sizeof(line) - pos) break;
            pos += (size_t)n;
        }
        f.println(line);
    }
    f.close();
    UNLOCK_TFT();

    LOG_SERIAL_FMT("[CAM] Snapshot gravado em %s\n", path);
    return true;
}
