// ============================================================================
// Sondvolt v5.0 - Telas dos Instrumentos de Bancada (implementacao)
// ============================================================================

#include "screens.h"
#include "scope.h"
#include "jobs.h"
#include "sorting.h"
#include "netsvc.h"
#include "thermalcam.h"
#include "expander.h"
#include "analysis.h"
#include "measurements.h"
#include "database.h"
#include "graphics.h"
#include "uiwidgets.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "visual.h"
#include "fonts.h"
#include "buzzer.h"
#include "hal.h"
#include "config.h"
#include "diagnostics.h"
#include <math.h>

// ----------------------------------------------------------------------------
// Auxiliares locais
// ----------------------------------------------------------------------------
static int16_t tw(const char* s, uint8_t size) {
    return (int16_t)(strlen(s ? s : "") * 6 * size);
}

static void centered(int16_t cx, int16_t y, const char* s,
                     uint16_t color, uint8_t size) {
    draw_text_5x7(tft, cx - tw(s, size) / 2, y, s, color, size);
}

// Botao padrao das telas de instrumento.
static void button(int16_t x, int16_t y, int16_t w, int16_t h,
                   const char* label, uint16_t bg, uint16_t fg) {
    LOCK_TFT();
    tft.fillRoundRect(x, y, w, h, V_RADIUS_SM, bg);
    tft.drawRoundRect(x, y, w, h, V_RADIUS_SM, color_mix(bg, V_PURE_WHITE, 60));
    draw_text_5x7(tft, x + (w - tw(label, 1)) / 2, y + (h - 8) / 2,
                  label, fg, 1);
    UNLOCK_TFT();
}

static bool hit(uint16_t tx, uint16_t ty,
                int16_t x, int16_t y, int16_t w, int16_t h) {
    return (tx >= (uint16_t)x && tx <= (uint16_t)(x + w) &&
            ty >= (uint16_t)y && ty <= (uint16_t)(y + h));
}

// Tela padrao para recurso que exige hardware ausente. Ser explicito sobre o
// que falta poupa o usuario de achar que o firmware esta com defeito.
static void draw_missing_hardware(const char* title, const char* whatIsMissing,
                                  const char* howToFix) {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header(title);

    LOCK_TFT();
    tft.fillRoundRect(16, 60, SCREEN_WIDTH - 32, 130, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(16, 60, SCREEN_WIDTH - 32, 130, V_RADIUS_LG, V_WARNING);

    tft.fillTriangle(160, 74, 148, 96, 172, 96, V_WARNING);
    draw_text_5x7(tft, 158, 86, "!", V_BG_DARK, 1);

    centered(160, 108, "RECURSO INDISPONIVEL", V_WARNING, 1);
    centered(160, 128, whatIsMissing, V_TEXT_MAIN, 1);
    centered(160, 148, howToFix, V_TEXT_SUB, 1);
    centered(160, 168, "Veja docs/WIRING.md", V_TEXT_SUB, 1);
    UNLOCK_TFT();
}

// ============================================================================
// OSCILOSCOPIO
// ============================================================================

static ScopeCapture gCapture;
static bool         gScopeRunning = false;
static uint32_t     gScopeLastDraw = 0;

// Taxas oferecidas ao usuario, da mais lenta para a mais rapida.
static const uint32_t kScopeRates[] = { 5000, 20000, 50000, 100000, 200000 };
static const uint8_t  kScopeRateCount = 5;
static uint8_t        gScopeRateIdx = 3;

void screen_scope_enter() {
    gScopeRateIdx = (deviceSettings.scopeRateIdx < kScopeRateCount)
                  ? deviceSettings.scopeRateIdx : 3;

    if (!scope_begin(kScopeRates[gScopeRateIdx])) {
        widget_toast("Falha ao iniciar o osciloscopio", TOAST_ERROR);
        return;
    }
    gScopeRunning = true;
    memset(&gCapture, 0, sizeof(gCapture));
}

void screen_scope_exit() {
    scope_end();
    gScopeRunning = false;
    deviceSettings.scopeRateIdx = gScopeRateIdx;
    settings_mark_dirty();
}

void screen_scope_draw() {
    if (!scope_active()) {
        draw_missing_hardware("OSCILOSCOPIO",
                              "O ADC nao pode ser reservado",
                              "Saia de outra medicao e tente de novo");
        return;
    }

    // Limita o refresh: capturar e desenhar mais rapido que isso so
    // consome CPU sem o olho perceber diferenca.
    if ((millis() - gScopeLastDraw) < 120) return;
    gScopeLastDraw = millis();

    if (gScopeRunning) scope_capture(&gCapture, 200);

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("OSCILOSCOPIO");

    const int16_t gx = 8, gy = 44, gw = 236, gh = 130;

    LOCK_TFT();
    tft.fillRect(gx, gy, gw, gh, 0x0000);
    tft.drawRect(gx, gy, gw, gh, V_DIVIDER);

    // Retícula: 10 divisoes na horizontal, 8 na vertical, como num
    // osciloscopio de bancada.
    for (uint8_t i = 1; i < 10; i++) {
        int16_t x = gx + (int16_t)((int32_t)gw * i / 10);
        for (int16_t y = gy + 3; y < gy + gh - 3; y += 5) {
            tft.drawPixel(x, y, 0x2104);
        }
    }
    for (uint8_t i = 1; i < 8; i++) {
        int16_t y = gy + (int16_t)((int32_t)gh * i / 8);
        for (int16_t x = gx + 3; x < gx + gw - 3; x += 5) {
            tft.drawPixel(x, y, 0x2104);
        }
    }

    // --- Traco -------------------------------------------------------------
    if (gCapture.valid && gCapture.count > 1) {
        const float vMax = scope_get_attenuator() ? 33.0f : 3.3f;
        int16_t prevX = gx + 1;
        int16_t prevY = gy + gh / 2;
        bool first = true;

        // Comeca no ponto de gatilho, para a onda ficar parada na tela em vez
        // de correr lateralmente a cada captura.
        uint16_t start = gCapture.triggered ? gCapture.triggerIndex : 0;

        for (uint16_t i = 0; i + start < gCapture.count; i++) {
            uint16_t idx = start + i;
            int16_t px = gx + 1 + (int16_t)(((int32_t)i * (gw - 2)) /
                                            (gCapture.count - start));
            if (px >= gx + gw - 1) break;

            float v = scope_sample_to_volts(gCapture.samples[idx]);
            int16_t py = gy + gh - 2 -
                         (int16_t)((v / vMax) * (float)(gh - 4));
            if (py < gy + 1)          py = gy + 1;
            if (py > gy + gh - 2)     py = gy + gh - 2;

            if (!first) tft.drawLine(prevX, prevY, px, py, V_NEON_GREEN);
            prevX = px; prevY = py; first = false;
        }

        // Linha do nivel de gatilho
        if (scope_get_trigger() != SCOPE_TRIG_AUTO) {
            float lv = scope_get_trigger_level();
            int16_t ly = gy + gh - 2 - (int16_t)((lv / vMax) * (float)(gh - 4));
            if (ly > gy && ly < gy + gh) {
                for (int16_t x = gx + 2; x < gx + gw - 2; x += 8) {
                    tft.drawPixel(x, ly, V_WARNING);
                }
            }
        }
    } else {
        centered(gx + gw / 2, gy + gh / 2 - 4, "sem sinal", V_TEXT_SUB, 1);
    }

    // --- Painel lateral com as medidas --------------------------------------
    const int16_t px = 250;
    tft.fillRoundRect(px, gy, 62, gh, V_RADIUS_SM, V_BG_SURFACE);
    tft.drawRoundRect(px, gy, 62, gh, V_RADIUS_SM, V_DIVIDER);

    char buf[28];
    int16_t ty = gy + 6;

    draw_text_5x7(tft, px + 5, ty, "Vpp", V_TEXT_SUB, 1); ty += 10;
    analysis_format_eng(gCapture.vPeakToPeak, buf, sizeof(buf), 2);
    draw_text_5x7(tft, px + 5, ty, buf, V_NEON_GREEN, 1); ty += 16;

    draw_text_5x7(tft, px + 5, ty, "Freq", V_TEXT_SUB, 1); ty += 10;
    if (gCapture.frequencyHz > 0.5f) {
        analysis_format_eng(gCapture.frequencyHz, buf, sizeof(buf), 2);
        strncat(buf, "Hz", sizeof(buf) - strlen(buf) - 1);
    } else {
        strncpy(buf, "--", sizeof(buf));
    }
    draw_text_5x7(tft, px + 5, ty, buf, V_CYAN_ELECTRIC, 1); ty += 16;

    draw_text_5x7(tft, px + 5, ty, "Duty", V_TEXT_SUB, 1); ty += 10;
    snprintf(buf, sizeof(buf), "%.0f%%", gCapture.dutyPercent);
    draw_text_5x7(tft, px + 5, ty, buf, V_TEXT_MAIN, 1); ty += 16;

    draw_text_5x7(tft, px + 5, ty, "Media", V_TEXT_SUB, 1); ty += 10;
    analysis_format_eng(gCapture.vAverage, buf, sizeof(buf), 2);
    draw_text_5x7(tft, px + 5, ty, buf, V_TEXT_MAIN, 1);

    // --- Rodape de estado ---------------------------------------------------
    draw_text_5x7(tft, gx, gy + gh + 4, scope_timebase_text(), V_TEXT_SUB, 1);

    const char* trigName = "AUTO";
    switch (scope_get_trigger()) {
        case SCOPE_TRIG_RISING:  trigName = "SUBIDA";  break;
        case SCOPE_TRIG_FALLING: trigName = "DESCIDA"; break;
        case SCOPE_TRIG_SINGLE:  trigName = "UNICO";   break;
        default: break;
    }
    snprintf(buf, sizeof(buf), "%s %s", trigName,
             gCapture.triggered ? "OK" : "--");
    draw_text_5x7(tft, gx + 96, gy + gh + 4, buf,
                  gCapture.triggered ? V_SUCCESS : V_TEXT_SUB, 1);

    draw_text_5x7(tft, gx + 176, gy + gh + 4,
                  scope_get_attenuator() ? "10x" : "1x", V_WARNING, 1);
    UNLOCK_TFT();

    // --- Botoes -------------------------------------------------------------
    button(8,   192, 70, 26, gScopeRunning ? "PAUSAR" : "RODAR",
           gScopeRunning ? V_ALERT : V_SUCCESS, V_BG_DARK);
    button(86,  192, 70, 26, "TEMPO",   V_BG_SURFACE,    V_TEXT_MAIN);
    button(164, 192, 70, 26, "GATILHO", V_BG_SURFACE,    V_TEXT_MAIN);
    button(242, 192, 70, 26, "ATENUAR", V_BG_SURFACE,    V_TEXT_MAIN);
}

bool screen_scope_touch(uint16_t x, uint16_t y) {
    if (y < 190) return false;

    if (hit(x, y, 8, 192, 70, 26)) {
        gScopeRunning = !gScopeRunning;
        buzzer_click();
        return true;
    }
    if (hit(x, y, 86, 192, 70, 26)) {
        gScopeRateIdx = (uint8_t)((gScopeRateIdx + 1) % kScopeRateCount);
        scope_set_rate(kScopeRates[gScopeRateIdx]);
        buzzer_click();
        return true;
    }
    if (hit(x, y, 164, 192, 70, 26)) {
        ScopeTrigger next = (ScopeTrigger)((scope_get_trigger() + 1) % 4);
        scope_set_trigger(next, scope_get_trigger_level());
        buzzer_click();
        return true;
    }
    if (hit(x, y, 242, 192, 70, 26)) {
        if (!scope_set_attenuator(!scope_get_attenuator())) {
            widget_toast("Atenuador exige a placa de expansao", TOAST_WARNING);
        }
        buzzer_click();
        return true;
    }
    return false;
}

// ============================================================================
// TRACADOR DE CURVA I-V
// ============================================================================

static CurveTrace gCurve;
static bool       gCurveDone = false;

void screen_curve_enter() { memset(&gCurve, 0, sizeof(gCurve)); gCurveDone = false; }
void screen_curve_exit()  { }

void screen_curve_draw() {
    if (!curve_available()) {
        draw_missing_hardware("TRACADOR DE CURVA",
                              "Falta o shunt do tracador",
                              "Instale a placa de expansao Bancada");
        return;
    }

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("CURVA I-V");

    const int16_t gx = 40, gy = 46, gw = 200, gh = 132;

    LOCK_TFT();
    tft.fillRect(gx, gy, gw, gh, 0x0000);
    tft.drawRect(gx, gy, gw, gh, V_DIVIDER);

    // Eixos
    draw_text_5x7(tft, 6, gy - 2, "I", V_TEXT_SUB, 1);
    draw_text_5x7(tft, gx + gw - 6, gy + gh + 6, "V", V_TEXT_SUB, 1);

    if (gCurveDone && gCurve.valid && gCurve.count > 1) {
        float maxV = (gCurve.maxVolts > 0.1f) ? gCurve.maxVolts : 3.3f;
        float maxI = (gCurve.maxAmps  > 1e-6f) ? gCurve.maxAmps  : 1e-3f;

        int16_t prevX = gx, prevY = gy + gh - 1;
        for (uint8_t i = 0; i < gCurve.count; i++) {
            int16_t px = gx + (int16_t)((gCurve.points[i].volts / maxV)
                                        * (float)(gw - 2));
            int16_t py = gy + gh - 1 -
                         (int16_t)((gCurve.points[i].amps / maxI)
                                   * (float)(gh - 2));
            if (px > gx + gw - 1) px = gx + gw - 1;
            if (py < gy)          py = gy;

            if (i > 0) tft.drawLine(prevX, prevY, px, py, V_NEON_GREEN);
            prevX = px; prevY = py;
        }

        char buf[32];
        analysis_format_eng(maxI, buf, sizeof(buf), 1);
        strncat(buf, "A", sizeof(buf) - strlen(buf) - 1);
        draw_text_5x7(tft, 4, gy + 8, buf, V_TEXT_SUB, 1);

        snprintf(buf, sizeof(buf), "%.2fV", maxV);
        draw_text_5x7(tft, gx + gw - 34, gy + gh - 10, buf, V_TEXT_SUB, 1);

        tft.fillRoundRect(8, gy + gh + 12, SCREEN_WIDTH - 16, 20,
                          V_RADIUS_SM, V_BG_SURFACE);
        centered(160, gy + gh + 18, gCurve.interpretation, V_CYAN_ELECTRIC, 1);
    } else {
        centered(gx + gw / 2, gy + gh / 2 - 4,
                 gCurveDone ? "varredura falhou" : "toque em TRACAR",
                 V_TEXT_SUB, 1);
    }
    UNLOCK_TFT();

    button(90, 210, 140, 26, "TRACAR", V_VIBRANT_PURPLE, V_PURE_WHITE);
}

bool screen_curve_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 90, 210, 140, 26)) {
        buzzer_measure_start();
        widget_toast("Varrendo...", TOAST_INFO, 1200);
        gCurveDone = curve_trace(&gCurve);
        if (gCurveDone) buzzer_ok();
        else            widget_toast("Nada conectado", TOAST_WARNING);
        return true;
    }
    return false;
}

// ============================================================================
// MEDIDOR DE RIPPLE
// ============================================================================

static RippleResult gRipple;
static uint32_t     gRippleLast = 0;

void screen_ripple_enter() { memset(&gRipple, 0, sizeof(gRipple)); }
void screen_ripple_exit()  { expander_ripple_coupling(false); }

void screen_ripple_draw() {
    if (!ripple_available()) {
        draw_missing_hardware("MEDIDOR DE RIPPLE",
                              "Falta o acoplamento AC",
                              "Instale a placa de expansao Bancada");
        return;
    }

    // Uma medicao por segundo: o processo insere e remove o capacitor de
    // acoplamento, o que leva tempo e nao vale repetir mais rapido.
    if ((millis() - gRippleLast) > 1000) {
        gRipple = ripple_measure();
        gRippleLast = millis();
    }

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("ONDULACAO (RIPPLE)");

    char buf[64], val[20];

    // Cartao principal: a porcentagem e o numero que decide o diagnostico.
    uint16_t accent = V_SUCCESS;
    if (gRipple.ripplePercent > 10.0f)     accent = V_ALERT;
    else if (gRipple.ripplePercent > 3.0f) accent = V_WARNING;

    snprintf(val, sizeof(val), "%.1f", gRipple.ripplePercent);
    widget_value_card(10, 46, 145, 78, "ONDULACAO", val, "% do DC", accent);

    analysis_format_eng(gRipple.dcVolts, buf, sizeof(buf), 2);
    strncat(buf, "V", sizeof(buf) - strlen(buf) - 1);
    widget_value_card(165, 46, 145, 78, "TRILHO DC", buf, "medio",
                      V_CYAN_ELECTRIC);

    LOCK_TFT();
    tft.fillRoundRect(10, 132, 300, 56, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 132, 300, 56, V_RADIUS_MD, V_DIVIDER);

    analysis_format_eng(gRipple.rippleVpp, val, sizeof(val), 2);
    snprintf(buf, sizeof(buf), "Vpp: %sV", val);
    draw_text_5x7(tft, 20, 140, buf, V_TEXT_MAIN, 1);

    analysis_format_eng(gRipple.rippleRms, val, sizeof(val), 2);
    snprintf(buf, sizeof(buf), "RMS: %sV", val);
    draw_text_5x7(tft, 170, 140, buf, V_TEXT_MAIN, 1);

    if (gRipple.frequencyHz > 1.0f) {
        analysis_format_eng(gRipple.frequencyHz, val, sizeof(val), 1);
        // 100 ou 120 Hz aponta retificador da rede; kHz aponta fonte chaveada.
        const char* origem = (gRipple.frequencyHz < 200.0f)
                           ? "rede retificada" : "fonte chaveada";
        snprintf(buf, sizeof(buf), "Freq: %sHz (%s)", val, origem);
    } else {
        snprintf(buf, sizeof(buf), "Freq: nao detectada");
    }
    draw_text_5x7(tft, 20, 156, buf, V_TEXT_SUB, 1);

    draw_text_5x7(tft, 20, 172, gRipple.verdict, accent, 1);
    UNLOCK_TFT();

    button(90, 196, 140, 26, "MEDIR AGORA", V_BG_SURFACE, V_TEXT_MAIN);
}

bool screen_ripple_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 90, 196, 140, 26)) {
        gRippleLast = 0;   // forca nova medicao no proximo desenho
        buzzer_click();
        return true;
    }
    return false;
}

// ============================================================================
// GERADOR DE SINAL
// ============================================================================

// Frequencias de uso comum em bancada, para nao obrigar a digitar.
static const uint32_t kSigPresets[] = { 100, 440, 1000, 10000, 32768, 100000 };
static const uint8_t  kSigPresetCount = 6;
static uint8_t        gSigPresetIdx = 2;

void screen_siggen_enter() { }
void screen_siggen_exit()  { siggen_stop(); }

void screen_siggen_draw() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("GERADOR DE SINAL");

    char buf[40], val[24];
    bool on = siggen_active();

    analysis_format_eng((float)siggen_frequency(), val, sizeof(val), 2);
    strncat(val, "Hz", sizeof(val) - strlen(val) - 1);
    widget_value_card(10, 46, 190, 74, "FREQUENCIA", val, "onda quadrada",
                      on ? V_NEON_GREEN : V_TEXT_SUB);

    snprintf(val, sizeof(val), "%u", (unsigned)siggen_duty());
    widget_value_card(210, 46, 100, 74, "DUTY", val, "%",
                      on ? V_CYAN_ELECTRIC : V_TEXT_SUB);

    LOCK_TFT();
    tft.fillRoundRect(10, 128, 300, 46, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 128, 300, 46, V_RADIUS_MD, V_DIVIDER);

    // A frequencia real raramente e exatamente a pedida: o LEDC divide um
    // clock fixo. Mostrar as duas evita confusao na hora de medir.
    snprintf(buf, sizeof(buf), "Real: %.1f Hz", siggen_actual_frequency());
    draw_text_5x7(tft, 20, 136, buf, V_TEXT_MAIN, 1);

    draw_text_5x7(tft, 20, 152,
                  expander_present() ? "Saida ligada na ponta 1"
                                     : "Sem expansor: saida so no GPIO22",
                  expander_present() ? V_SUCCESS : V_WARNING, 1);

    draw_text_5x7(tft, 180, 136, "Amplitude: 0 a 3,3 V", V_TEXT_SUB, 1);
    draw_text_5x7(tft, 180, 152, "Use divisor p/ nivel menor", V_TEXT_SUB, 1);
    UNLOCK_TFT();

    button(10,  182, 70, 26, "FREQ -", V_BG_SURFACE, V_TEXT_MAIN);
    button(86,  182, 70, 26, "FREQ +", V_BG_SURFACE, V_TEXT_MAIN);
    button(164, 182, 70, 26, "DUTY -", V_BG_SURFACE, V_TEXT_MAIN);
    button(240, 182, 70, 26, "DUTY +", V_BG_SURFACE, V_TEXT_MAIN);

    button(90, 212, 140, 26, on ? "PARAR" : "GERAR",
           on ? V_ALERT : V_SUCCESS, V_BG_DARK);
}

bool screen_siggen_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 90, 212, 140, 26)) {
        if (siggen_active()) {
            siggen_stop();
            widget_toast("Gerador desligado", TOAST_INFO);
        } else {
            if (siggen_start(kSigPresets[gSigPresetIdx], 50))
                widget_toast("Gerador ligado", TOAST_SUCCESS);
            else
                widget_toast("Circuito de pontas ocupado", TOAST_WARNING);
        }
        return true;
    }

    if (y < 180 || y > 210) return false;

    if (hit(x, y, 10, 182, 70, 26)) {
        if (gSigPresetIdx > 0) gSigPresetIdx--;
        if (siggen_active()) siggen_set_frequency(kSigPresets[gSigPresetIdx]);
        buzzer_click();
        return true;
    }
    if (hit(x, y, 86, 182, 70, 26)) {
        if (gSigPresetIdx + 1 < kSigPresetCount) gSigPresetIdx++;
        if (siggen_active()) siggen_set_frequency(kSigPresets[gSigPresetIdx]);
        buzzer_click();
        return true;
    }
    if (hit(x, y, 164, 182, 70, 26)) {
        uint8_t d = siggen_duty();
        if (d > 10) siggen_set_duty((uint8_t)(d - 10));
        buzzer_click();
        return true;
    }
    if (hit(x, y, 240, 182, 70, 26)) {
        uint8_t d = siggen_duty();
        if (d < 90) siggen_set_duty((uint8_t)(d + 10));
        buzzer_click();
        return true;
    }
    return false;
}

// ============================================================================
// TESTE DE ZENER
// ============================================================================

static ZenerResult gZener;
static bool        gZenerDone = false;

void screen_zener_enter() { memset(&gZener, 0, sizeof(gZener)); gZenerDone = false; }
void screen_zener_exit()  { expander_boost_enable(false); }

void screen_zener_draw() {
    if (!zener_available()) {
        draw_missing_hardware("TESTE DE ZENER",
                              "Falta a fonte auxiliar de 12 V",
                              "Instale o boost MT3608 na placa Bancada");
        return;
    }

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("TESTE DE ZENER");

    char val[20];
    if (gZenerDone && gZener.valid) {
        snprintf(val, sizeof(val), "%.2f", gZener.zenerVolts);
        widget_value_card(60, 52, 200, 84, "TENSAO DE JOELHO", val, "V",
                          V_NEON_GREEN);
    } else {
        widget_value_card(60, 52, 200, 84, "TENSAO DE JOELHO", "---", "V",
                          V_TEXT_SUB);
    }

    LOCK_TFT();
    tft.fillRoundRect(10, 146, 300, 44, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 146, 300, 44, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 20, 154,
                  gZenerDone ? gZener.detail : "Conecte o Zener nas pontas",
                  gZenerDone && gZener.valid ? V_TEXT_MAIN : V_TEXT_SUB, 1);
    // A fonte chega a 12 V, entao o joelho so e visivel ate cerca de 11 V.
    draw_text_5x7(tft, 20, 170, "Faixa util: 2 a 11 V (fonte de 12 V)",
                  V_TEXT_SUB, 1);
    UNLOCK_TFT();

    button(90, 200, 140, 28, "MEDIR", V_ALERT, V_PURE_WHITE);
}

bool screen_zener_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 90, 200, 140, 28)) {
        buzzer_measure_start();
        widget_toast("Ligando fonte de 12 V...", TOAST_INFO, 1000);
        gZener = zener_measure();
        gZenerDone = true;
        if (gZener.valid) buzzer_ok();
        else              widget_toast(gZener.detail, TOAST_WARNING);
        return true;
    }
    return false;
}

// ============================================================================
// TRABALHOS
// ============================================================================

static ScrollList gJobList;
static bool       gJobsScanned = false;

static const char* job_item_text(uint16_t i) { return jobs_name_at((uint8_t)i); }

void screen_jobs_enter() {
    uint8_t n = jobs_scan();
    gJobsScanned = true;
    widget_list_init(&gJobList, 10, 68, 300, 4, 26, n);
}

void screen_jobs_exit() { gJobsScanned = false; }

void screen_jobs_draw() {
    if (!jobs_available()) {
        draw_missing_hardware("TRABALHOS",
                              "Cartao SD ausente",
                              "Insira um cartao FAT32 para usar trabalhos");
        return;
    }

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("TRABALHOS");

    // Faixa do trabalho ativo, sempre visivel: e o contexto de tudo que o
    // usuario medir a seguir.
    const JobInfo* active = jobs_active();
    LOCK_TFT();
    if (active) {
        tft.fillRoundRect(10, 42, 300, 22, V_RADIUS_SM,
                          color_mix(V_SUCCESS, V_BG_DARK, 200));
        tft.drawRoundRect(10, 42, 300, 22, V_RADIUS_SM, V_SUCCESS);
        char buf[72];
        snprintf(buf, sizeof(buf), "ATIVO: %.18s  (%lu medicoes)",
                 active->name, (unsigned long)active->measurementCount);
        draw_text_5x7(tft, 18, 49, buf, V_TEXT_MAIN, 1);
    } else {
        tft.fillRoundRect(10, 42, 300, 22, V_RADIUS_SM, V_BG_SURFACE);
        draw_text_5x7(tft, 18, 49, "Nenhum trabalho ativo", V_TEXT_SUB, 1);
    }
    UNLOCK_TFT();

    if (jobs_count() > 0) {
        widget_list_draw(&gJobList, job_item_text, nullptr);
    } else {
        LOCK_TFT();
        centered(160, 110, "Nenhum trabalho gravado", V_TEXT_SUB, 1);
        centered(160, 126, "Toque em NOVO para comecar", V_TEXT_SUB, 1);
        UNLOCK_TFT();
    }

    button(10,  186, 92, 26, "NOVO",     V_SUCCESS,    V_BG_DARK);
    button(110, 186, 92, 26, "ABRIR",    V_BG_SURFACE, V_TEXT_MAIN);
    button(210, 186, 100, 26, "FINALIZAR", active ? V_WARNING : V_BG_SURFACE,
           active ? V_BG_DARK : V_TEXT_SUB);
    button(10,  216, 300, 22, "RELATORIO DO TRABALHO ATIVO",
           V_BG_SURFACE, V_TEXT_MAIN);
}

bool screen_jobs_touch(uint16_t x, uint16_t y) {
    // --- NOVO ---------------------------------------------------------------
    if (hit(x, y, 10, 186, 92, 26)) {
        char name[JOB_MAX_NAME + 1] = { 0 };
        if (!widget_keyboard("NOME DO TRABALHO", name, sizeof(name))) return true;
        if (name[0] == '\0') return true;

        char customer[JOB_MAX_NAME + 1] = { 0 };
        widget_keyboard("CLIENTE (opcional)", customer, sizeof(customer));

        char device[JOB_MAX_NAME + 1] = { 0 };
        widget_keyboard("APARELHO (opcional)", device, sizeof(device));

        if (jobs_create(name, customer, device)) {
            widget_toast("Trabalho criado e ativo", TOAST_SUCCESS);
            screen_jobs_enter();
        } else {
            widget_toast("Falha ao criar o trabalho", TOAST_ERROR);
        }
        return true;
    }

    // --- ABRIR --------------------------------------------------------------
    if (hit(x, y, 110, 186, 92, 26)) {
        const JobInfo* sel = jobs_at((uint8_t)gJobList.selected);
        if (sel && jobs_open(sel->dirName)) {
            widget_toast("Trabalho aberto", TOAST_SUCCESS);
        } else {
            widget_toast("Selecione um trabalho na lista", TOAST_WARNING);
        }
        return true;
    }

    // --- FINALIZAR ----------------------------------------------------------
    if (hit(x, y, 210, 186, 100, 26)) {
        if (!jobs_has_active()) {
            widget_toast("Nenhum trabalho ativo", TOAST_WARNING);
            return true;
        }
        if (widget_confirm("FINALIZAR TRABALHO",
                           "O relatorio sera gerado e o trabalho fechado.",
                           "FINALIZAR", "VOLTAR")) {
            if (jobs_finish()) {
                widget_toast("Relatorio gravado no cartao", TOAST_SUCCESS);
                screen_jobs_enter();
            } else {
                widget_toast("Falha ao gerar o relatorio", TOAST_ERROR);
            }
        }
        return true;
    }

    // --- RELATORIO ----------------------------------------------------------
    if (hit(x, y, 10, 216, 300, 22)) {
        if (jobs_generate_report(nullptr)) {
            widget_toast("Relatorio atualizado", TOAST_SUCCESS);
        } else {
            widget_toast("Nenhum trabalho ativo", TOAST_WARNING);
        }
        return true;
    }

    // --- Lista --------------------------------------------------------------
    uint16_t activated = 0;
    if (widget_list_handle_touch(&gJobList, (int16_t)x, (int16_t)y, &activated)) {
        const JobInfo* sel = jobs_at((uint8_t)activated);
        if (sel && jobs_open(sel->dirName)) {
            widget_toast("Trabalho aberto", TOAST_SUCCESS);
        }
        return true;
    }
    return false;
}

// ============================================================================
// PAREAMENTO
// ============================================================================

void screen_sorting_enter() {
    sorting_set_tolerance(deviceSettings.pairTolerance);
}

void screen_sorting_exit() {
    deviceSettings.pairTolerance = sorting_get_tolerance();
    settings_mark_dirty();
}

void screen_sorting_draw() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("PAREAMENTO DE COMPONENTES");

    SortStats st = sorting_stats();
    char buf[72], val[20];

    // --- Resumo do lote ------------------------------------------------------
    LOCK_TFT();
    tft.fillRoundRect(10, 42, 300, 50, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 42, 300, 50, V_RADIUS_MD, V_DIVIDER);

    snprintf(buf, sizeof(buf), "%u pecas   %u pares   %u sem par",
             (unsigned)st.count, (unsigned)sorting_pair_count(),
             (unsigned)sorting_unpaired_count());
    draw_text_5x7(tft, 20, 50, buf, V_TEXT_MAIN, 1);

    if (st.count > 0) {
        analysis_format_eng(st.average, val, sizeof(val), 2);
        snprintf(buf, sizeof(buf), "Media %s   Dispersao %.1f%%",
                 val, st.spreadPct);
        draw_text_5x7(tft, 20, 64, buf, V_TEXT_SUB, 1);
    }
    draw_text_5x7(tft, 20, 78, sorting_quality_text(), V_CYAN_ELECTRIC, 1);

    snprintf(buf, sizeof(buf), "Tol %.1f%%", sorting_get_tolerance());
    draw_text_5x7(tft, 250, 50, buf, V_WARNING, 1);
    UNLOCK_TFT();

    // --- Pares formados ------------------------------------------------------
    LOCK_TFT();
    tft.fillRoundRect(10, 98, 300, 84, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 98, 300, 84, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 20, 104, "PARES CASADOS", V_NEON_GREEN, 1);

    uint8_t shown = sorting_pair_count();
    if (shown > 5) shown = 5;

    if (shown == 0) {
        draw_text_5x7(tft, 20, 126,
                      st.count < 2 ? "Meça pelo menos duas peças"
                                   : "Nenhum par dentro da tolerância",
                      V_TEXT_SUB, 1);
    } else {
        for (uint8_t i = 0; i < shown; i++) {
            const SortPair* p = sorting_pair_at(i);
            if (!p) continue;

            char a[16], b[16];
            analysis_format_eng(p->valueA, a, sizeof(a), 2);
            analysis_format_eng(p->valueB, b, sizeof(b), 2);
            snprintf(buf, sizeof(buf), "#%u + #%u   %s / %s   %.1f%%",
                     (unsigned)p->idA, (unsigned)p->idB, a, b,
                     p->deviationPct);
            draw_text_5x7(tft, 20, 118 + i * 12, buf, V_TEXT_MAIN, 1);
        }
    }
    UNLOCK_TFT();

    button(10,  188, 72, 26, "MEDIR",  V_SUCCESS,    V_BG_DARK);
    button(88,  188, 72, 26, "PAREAR", V_BG_SURFACE, V_TEXT_MAIN);
    button(166, 188, 72, 26, "TOL",    V_BG_SURFACE, V_TEXT_MAIN);
    button(244, 188, 66, 26, "LIMPAR", V_ALERT,      V_PURE_WHITE);
    button(10,  218, 300, 20, "EXPORTAR LOTE PARA O CARTAO",
           V_BG_SURFACE, V_TEXT_MAIN);
}

bool screen_sorting_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 10, 188, 72, 26)) {
        uint8_t idx = sorting_add_current();
        if (idx == 0xFF) widget_toast("Leitura nao utilizavel", TOAST_WARNING);
        else {
            char buf[32];
            snprintf(buf, sizeof(buf), "Peça #%u medida", (unsigned)(idx + 1));
            widget_toast(buf, TOAST_SUCCESS, 1200);
            sorting_compute_pairs();
        }
        return true;
    }
    if (hit(x, y, 88, 188, 72, 26)) {
        uint8_t n = sorting_compute_pairs();
        char buf[32];
        snprintf(buf, sizeof(buf), "%u par(es) formado(s)", (unsigned)n);
        widget_toast(buf, n > 0 ? TOAST_SUCCESS : TOAST_INFO);
        return true;
    }
    if (hit(x, y, 166, 188, 72, 26)) {
        // Cicla entre as tolerancias que fazem sentido na pratica.
        float t = sorting_get_tolerance();
        if      (t < 1.5f)  t = 2.0f;
        else if (t < 2.5f)  t = 5.0f;
        else if (t < 5.5f)  t = 10.0f;
        else if (t < 10.5f) t = 20.0f;
        else                t = 1.0f;
        sorting_set_tolerance(t);
        sorting_compute_pairs();
        buzzer_click();
        return true;
    }
    if (hit(x, y, 244, 188, 66, 26)) {
        if (widget_confirm("LIMPAR LOTE",
                           "Todas as peças medidas serão descartadas.",
                           "LIMPAR", "VOLTAR")) {
            sorting_clear();
            widget_toast("Lote limpo", TOAST_INFO);
        }
        return true;
    }
    if (hit(x, y, 10, 218, 300, 20)) {
        if (sorting_export("LOTE")) widget_toast("Lote exportado", TOAST_SUCCESS);
        else                        widget_toast("Falha ao exportar", TOAST_ERROR);
        return true;
    }
    return false;
}

// ============================================================================
// REDE
// ============================================================================

void screen_network_enter() { }
void screen_network_exit()  { }

void screen_network_draw() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("REDE E ATUALIZACAO");

    NetStatus ns = net_get_status();
    char buf[80];

    const char* stateText = "desligada";
    uint16_t stateColor = V_TEXT_SUB;
    switch (ns.state) {
        case NET_CONNECTING: stateText = "conectando";      stateColor = V_WARNING; break;
        case NET_CONNECTED:  stateText = "conectada";       stateColor = V_SUCCESS; break;
        case NET_AP_MODE:    stateText = "ponto de acesso"; stateColor = V_CYAN_ELECTRIC; break;
        case NET_FAILED:     stateText = "falhou";          stateColor = V_ALERT; break;
        default: break;
    }

    LOCK_TFT();
    tft.fillRoundRect(10, 42, 300, 118, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 42, 300, 118, V_RADIUS_MD, V_DIVIDER);

    int16_t ty = 50;
    snprintf(buf, sizeof(buf), "Situacao: %s", stateText);
    draw_text_5x7(tft, 20, ty, buf, stateColor, 1); ty += 15;

    snprintf(buf, sizeof(buf), "Rede: %s", ns.ssid[0] ? ns.ssid : "nao configurada");
    draw_text_5x7(tft, 20, ty, buf, V_TEXT_MAIN, 1); ty += 15;

    if (net_is_connected()) {
        snprintf(buf, sizeof(buf), "Endereco: http://%s/", ns.ip);
        draw_text_5x7(tft, 20, ty, buf, V_CYAN_ELECTRIC, 1); ty += 15;

        if (ns.state == NET_CONNECTED) {
            snprintf(buf, sizeof(buf), "Sinal: %d dBm", (int)ns.rssi);
            draw_text_5x7(tft, 20, ty, buf, V_TEXT_SUB, 1); ty += 15;
        } else {
            snprintf(buf, sizeof(buf), "Senha do AP: %s", NET_AP_PASSWORD);
            draw_text_5x7(tft, 20, ty, buf, V_TEXT_SUB, 1); ty += 15;
        }
    } else {
        ty += 30;
    }

    char when[24];
    net_format_datetime(when, sizeof(when));
    snprintf(buf, sizeof(buf), "Relogio: %s", when);
    draw_text_5x7(tft, 20, ty, buf,
                  net_time_valid() ? V_SUCCESS : V_WARNING, 1); ty += 15;

    snprintf(buf, sizeof(buf), "OTA: %s   Acessos: %lu",
             ns.otaReady ? "pronta" : "inativa", (unsigned long)ns.clients);
    draw_text_5x7(tft, 20, ty, buf, V_TEXT_SUB, 1);
    UNLOCK_TFT();

    button(10,  168, 145, 26, net_is_connected() ? "DESLIGAR" : "LIGAR",
           net_is_connected() ? V_ALERT : V_SUCCESS, V_BG_DARK);
    button(165, 168, 145, 26, "CONFIGURAR REDE", V_BG_SURFACE, V_TEXT_MAIN);
    button(10,  200, 145, 26, "SINCRONIZAR HORA", V_BG_SURFACE, V_TEXT_MAIN);
    button(165, 200, 145, 26, "AJUSTAR HORA", V_BG_SURFACE, V_TEXT_MAIN);
}

bool screen_network_touch(uint16_t x, uint16_t y) {
    // --- LIGAR / DESLIGAR ---------------------------------------------------
    if (hit(x, y, 10, 168, 145, 26)) {
        if (net_is_connected()) {
            net_stop();
            deviceSettings.wifiEnabled = false;
            widget_toast("WiFi desligado", TOAST_INFO);
        } else {
            widget_toast("Conectando...", TOAST_INFO, 3000);
            if (net_start()) {
                deviceSettings.wifiEnabled = true;
                NetStatus ns = net_get_status();
                char buf[48];
                snprintf(buf, sizeof(buf), "Pronto: http://%s/", ns.ip);
                widget_toast(buf, TOAST_SUCCESS, 4000);
            } else {
                widget_toast("Nao foi possivel conectar", TOAST_ERROR);
            }
        }
        settings_mark_dirty();
        return true;
    }

    // --- CONFIGURAR ---------------------------------------------------------
    if (hit(x, y, 165, 168, 145, 26)) {
        char ssid[33] = { 0 };
        strncpy(ssid, net_get_ssid(), sizeof(ssid) - 1);
        if (!widget_keyboard("NOME DA REDE (SSID)", ssid, sizeof(ssid))) return true;

        char pass[65] = { 0 };
        if (!widget_keyboard("SENHA DA REDE", pass, sizeof(pass))) return true;

        if (net_set_credentials(ssid, pass)) {
            widget_toast("Credenciais gravadas", TOAST_SUCCESS);
        } else {
            widget_toast("Falha ao gravar", TOAST_ERROR);
        }
        return true;
    }

    // --- SINCRONIZAR --------------------------------------------------------
    if (hit(x, y, 10, 200, 145, 26)) {
        if (!net_is_connected()) {
            widget_toast("Ligue o WiFi primeiro", TOAST_WARNING);
        } else if (net_sync_time()) {
            widget_toast("Hora sincronizada", TOAST_SUCCESS);
        } else {
            widget_toast("Servidor de hora nao respondeu", TOAST_ERROR);
        }
        return true;
    }

    // --- AJUSTAR HORA A MAO -------------------------------------------------
    // Existe para quem nunca vai ligar o WiFi mas ainda quer data no laudo.
    if (hit(x, y, 165, 200, 145, 26)) {
        char date[16] = { 0 };
        if (!widget_keyboard("DATA DDMMAAAA", date, sizeof(date), true)) return true;
        char time[10] = { 0 };
        if (!widget_keyboard("HORA HHMM", time, sizeof(time), true)) return true;

        if (strlen(date) >= 8 && strlen(time) >= 4) {
            char part[6];
            memcpy(part, date, 2);     part[2] = '\0'; uint8_t  d  = atoi(part);
            memcpy(part, date + 2, 2); part[2] = '\0'; uint8_t  mo = atoi(part);
            memcpy(part, date + 4, 4); part[4] = '\0'; uint16_t yr = atoi(part);
            memcpy(part, time, 2);     part[2] = '\0'; uint8_t  hh = atoi(part);
            memcpy(part, time + 2, 2); part[2] = '\0'; uint8_t  mi = atoi(part);

            net_set_time_manual(yr, mo, d, hh, mi, 0);
            widget_toast("Hora definida", TOAST_SUCCESS);
        } else {
            widget_toast("Formato invalido", TOAST_WARNING);
        }
        return true;
    }
    return false;
}

// ============================================================================
// CAMERA TERMICA
// ============================================================================

static ThermalFrame gTFrame;
static uint32_t     gTLast = 0;

void screen_thermalcam_enter() { memset(&gTFrame, 0, sizeof(gTFrame)); }
void screen_thermalcam_exit()  { }

void screen_thermalcam_draw() {
    if (!tcam_present()) {
        draw_missing_hardware("CAMERA TERMICA",
                              "MLX90640 nao detectado em 0x33",
                              "Confira a ligacao no conector P3");
        return;
    }

    // O sensor entrega 2 quadros por segundo a 400 kHz de I2C.
    if ((millis() - gTLast) > 500) {
        tcam_read_frame(&gTFrame);
        gTLast = millis();
    }

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();
    graphics_draw_header("CAMERA TERMICA");

    if (gTFrame.valid) {
        // Area de 240x180 mantem a proporcao 4:3 da matriz 32x24.
        tcam_draw(&gTFrame, 8, 42, 240, 180);
        tcam_draw_scale(&gTFrame, 262, 52, 16, 150);

        LOCK_TFT();
        char buf[32];
        snprintf(buf, sizeof(buf), "Centro %.1fC", gTFrame.centerC);
        draw_text_5x7(tft, 8, 226, buf, V_TEXT_MAIN, 1);

        snprintf(buf, sizeof(buf), "Max %.1fC", gTFrame.maxC);
        draw_text_5x7(tft, 118, 226, buf, V_ALERT, 1);

        snprintf(buf, sizeof(buf), "Min %.1fC", gTFrame.minC);
        draw_text_5x7(tft, 218, 226, buf, V_CYAN_ELECTRIC, 1);
        UNLOCK_TFT();
    } else {
        LOCK_TFT();
        centered(160, 130, "aguardando quadro...", V_TEXT_SUB, 1);
        UNLOCK_TFT();
    }

    button(262, 208, 50, 22, "PALETA", V_BG_SURFACE, V_TEXT_MAIN);
}

bool screen_thermalcam_touch(uint16_t x, uint16_t y) {
    if (hit(x, y, 262, 208, 50, 22)) {
        ThermalPalette next =
            (ThermalPalette)((tcam_get_palette() + 1) % TCAM_PALETTE_COUNT);
        tcam_set_palette(next);
        deviceSettings.thermalPalette = (uint8_t)next;
        settings_mark_dirty();
        widget_toast(tcam_palette_name(next), TOAST_INFO, 1200);
        return true;
    }
    // Um toque na imagem grava o quadro no cartao.
    if (hit(x, y, 8, 42, 240, 180)) {
        if (tcam_save_snapshot("TERMO")) {
            widget_toast("Snapshot gravado no cartao", TOAST_SUCCESS);
        } else {
            widget_toast("Falha ao gravar", TOAST_ERROR);
        }
        return true;
    }
    return false;
}

// ============================================================================
// DESPACHO
// ============================================================================

bool screens_handles(AppState state) {
    switch (state) {
        case STATE_SCOPE:
        case STATE_CURVE_TRACER:
        case STATE_RIPPLE:
        case STATE_SIGGEN:
        case STATE_ZENER:
        case STATE_JOBS_LIST:
        case STATE_SORTING:
        case STATE_NETWORK:
        case STATE_THERMAL_CAMERA:
            return true;
        default:
            return false;
    }
}

void screens_enter(AppState state) {
    switch (state) {
        case STATE_SCOPE:          screen_scope_enter();      break;
        case STATE_CURVE_TRACER:   screen_curve_enter();      break;
        case STATE_RIPPLE:         screen_ripple_enter();     break;
        case STATE_SIGGEN:         screen_siggen_enter();     break;
        case STATE_ZENER:          screen_zener_enter();      break;
        case STATE_JOBS_LIST:      screen_jobs_enter();       break;
        case STATE_SORTING:        screen_sorting_enter();    break;
        case STATE_NETWORK:        screen_network_enter();    break;
        case STATE_THERMAL_CAMERA: screen_thermalcam_enter(); break;
        default: break;
    }
}

void screens_draw(AppState state) {
    switch (state) {
        case STATE_SCOPE:          screen_scope_draw();      break;
        case STATE_CURVE_TRACER:   screen_curve_draw();      break;
        case STATE_RIPPLE:         screen_ripple_draw();     break;
        case STATE_SIGGEN:         screen_siggen_draw();     break;
        case STATE_ZENER:          screen_zener_draw();      break;
        case STATE_JOBS_LIST:      screen_jobs_draw();       break;
        case STATE_SORTING:        screen_sorting_draw();    break;
        case STATE_NETWORK:        screen_network_draw();    break;
        case STATE_THERMAL_CAMERA: screen_thermalcam_draw(); break;
        default: break;
    }
}

bool screens_touch(AppState state, uint16_t x, uint16_t y) {
    switch (state) {
        case STATE_SCOPE:          return screen_scope_touch(x, y);
        case STATE_CURVE_TRACER:   return screen_curve_touch(x, y);
        case STATE_RIPPLE:         return screen_ripple_touch(x, y);
        case STATE_SIGGEN:         return screen_siggen_touch(x, y);
        case STATE_ZENER:          return screen_zener_touch(x, y);
        case STATE_JOBS_LIST:      return screen_jobs_touch(x, y);
        case STATE_SORTING:        return screen_sorting_touch(x, y);
        case STATE_NETWORK:        return screen_network_touch(x, y);
        case STATE_THERMAL_CAMERA: return screen_thermalcam_touch(x, y);
        default: return false;
    }
}

void screens_exit(AppState state) {
    // Liberar o hardware ao sair nao e opcional: o osciloscopio monopoliza o
    // ADC e o gerador segura o pino de excitacao. Sem isso, a proxima tela
    // que tentar medir nao consegue.
    switch (state) {
        case STATE_SCOPE:          screen_scope_exit();      break;
        case STATE_CURVE_TRACER:   screen_curve_exit();      break;
        case STATE_RIPPLE:         screen_ripple_exit();     break;
        case STATE_SIGGEN:         screen_siggen_exit();     break;
        case STATE_ZENER:          screen_zener_exit();      break;
        case STATE_JOBS_LIST:      screen_jobs_exit();       break;
        case STATE_SORTING:        screen_sorting_exit();    break;
        case STATE_NETWORK:        screen_network_exit();    break;
        case STATE_THERMAL_CAMERA: screen_thermalcam_exit(); break;
        default: break;
    }
}
