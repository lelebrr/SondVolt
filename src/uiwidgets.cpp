// ============================================================================
// Sondvolt v4.0 - Componentes Visuais Reutilizaveis (implementacao)
// ============================================================================

#include "uiwidgets.h"
#include "display_globals.h"
#include "display_mutex.h"
#include "visual.h"
#include "fonts.h"
#include "hal.h"
#include "buzzer.h"
#include "analysis.h"
#include <math.h>

// Largura de um caractere da fonte 5x7 no tamanho 1.
static const int16_t kCharW = 6;

static int16_t text_width(const char* s, uint8_t size) {
    return (int16_t)(strlen(s ? s : "") * kCharW * size);
}

static void text_centered(int16_t cx, int16_t y, const char* s,
                          uint16_t color, uint8_t size) {
    draw_text_5x7(tft, cx - text_width(s, size) / 2, y, s, color, size);
}

// ============================================================================
// 1. NOTIFICACOES
// ============================================================================

static char       gToastMsg[48] = { 0 };
static ToastLevel gToastLevel   = TOAST_INFO;
static uint32_t   gToastUntil   = 0;
static bool       gToastDrawn   = false;

static uint16_t toast_color(ToastLevel level) {
    switch (level) {
        case TOAST_SUCCESS: return V_SUCCESS;
        case TOAST_WARNING: return V_WARNING;
        case TOAST_ERROR:   return V_ALERT;
        default:            return V_CYAN_ELECTRIC;
    }
}

void widget_toast(const char* message, ToastLevel level, uint16_t durationMs) {
    if (!message) return;
    strncpy(gToastMsg, message, sizeof(gToastMsg) - 1);
    gToastMsg[sizeof(gToastMsg) - 1] = '\0';
    gToastLevel = level;
    gToastUntil = millis() + durationMs;
    gToastDrawn = false;

    // Um som discreto ajuda quando o usuario esta olhando para a placa e nao
    // para a tela.
    if (level == TOAST_ERROR)        buzzer_error();
    else if (level == TOAST_SUCCESS) buzzer_ok();
}

bool widget_toast_active() {
    return (gToastMsg[0] != '\0') && (millis() < gToastUntil);
}

void widget_toast_clear() {
    gToastMsg[0] = '\0';
    gToastUntil  = 0;
    gToastDrawn  = false;
}

void widget_toast_render() {
    if (gToastMsg[0] == '\0') return;

    if (millis() >= gToastUntil) {
        // Apaga a faixa deixando o fundo limpo para a tela redesenhar.
        LOCK_TFT();
        tft.fillRect(0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30, V_BG_DARK);
        UNLOCK_TFT();
        widget_toast_clear();
        return;
    }

    if (gToastDrawn) return;   // desenha uma vez so, nao a cada ciclo

    const uint16_t color = toast_color(gToastLevel);
    const int16_t  h = 26;
    const int16_t  y = SCREEN_HEIGHT - h - 3;

    LOCK_TFT();
    tft.fillRoundRect(8, y, SCREEN_WIDTH - 16, h, V_RADIUS_MD,
                      color_mix(color, V_BG_DARK, 205));
    tft.drawRoundRect(8, y, SCREEN_WIDTH - 16, h, V_RADIUS_MD, color);
    // Barrinha de acento na lateral esquerda
    tft.fillRoundRect(11, y + 4, 3, h - 8, 1, color);
    draw_text_5x7(tft, 22, y + h / 2 - 3, gToastMsg, V_TEXT_MAIN, 1);
    UNLOCK_TFT();

    gToastDrawn = true;
}

// ============================================================================
// 2. BARRA DE PROGRESSO
// ============================================================================

void widget_progress_bar(int16_t x, int16_t y, int16_t w, int16_t h,
                         uint8_t percent, uint16_t color) {
    if (percent > 100) percent = 100;
    const int16_t radius = (h >= 8) ? 4 : 2;
    const int16_t inner  = w - 4;
    const int16_t fill   = (int16_t)((int32_t)inner * percent / 100);

    LOCK_TFT();
    tft.fillRoundRect(x, y, w, h, radius, V_BG_SURFACE);
    tft.drawRoundRect(x, y, w, h, radius, color_mix(color, V_BG_DARK, 120));
    if (fill > 0) {
        tft.fillRoundRect(x + 2, y + 2, fill, h - 4,
                          (radius > 2) ? radius - 2 : 1, color);
    }
    UNLOCK_TFT();
}

void widget_progress_labeled(int16_t x, int16_t y, int16_t w,
                             const char* label, uint8_t percent,
                             uint16_t color) {
    char pct[8];
    snprintf(pct, sizeof(pct), "%u%%", (unsigned)(percent > 100 ? 100 : percent));

    LOCK_TFT();
    tft.fillRect(x, y, w, 24, V_BG_DARK);
    draw_text_5x7(tft, x, y, label ? label : "", V_TEXT_SUB, 1);
    draw_text_5x7(tft, x + w - text_width(pct, 1), y, pct, color, 1);
    UNLOCK_TFT();

    widget_progress_bar(x, y + 11, w, 8, percent, color);
}

// ============================================================================
// 3. GRAFICO
// ============================================================================

void widget_graph_init(GraphBuffer* g, bool autoScale) {
    if (!g) return;
    memset(g, 0, sizeof(GraphBuffer));
    g->autoScale = autoScale;
    g->minValue  = 0.0f;
    g->maxValue  = 1.0f;
}

void widget_graph_push(GraphBuffer* g, float value) {
    if (!g) return;
    if (isnan(value) || isinf(value)) return;

    g->points[g->head] = value;
    g->head = (uint8_t)((g->head + 1) % GRAPH_MAX_POINTS);
    if (g->count < GRAPH_MAX_POINTS) g->count++;
}

void widget_graph_set_range(GraphBuffer* g, float minV, float maxV) {
    if (!g || maxV <= minV) return;
    g->autoScale = false;
    g->minValue  = minV;
    g->maxValue  = maxV;
}

// Percorre o buffer circular na ordem cronologica.
static float graph_at(const GraphBuffer* g, uint8_t i) {
    uint8_t start = (g->count < GRAPH_MAX_POINTS)
                  ? 0
                  : g->head;
    return g->points[(start + i) % GRAPH_MAX_POINTS];
}

float widget_graph_min(const GraphBuffer* g) {
    if (!g || g->count == 0) return 0.0f;
    float m = graph_at(g, 0);
    for (uint8_t i = 1; i < g->count; i++) {
        float v = graph_at(g, i);
        if (v < m) m = v;
    }
    return m;
}

float widget_graph_max(const GraphBuffer* g) {
    if (!g || g->count == 0) return 0.0f;
    float m = graph_at(g, 0);
    for (uint8_t i = 1; i < g->count; i++) {
        float v = graph_at(g, i);
        if (v > m) m = v;
    }
    return m;
}

float widget_graph_average(const GraphBuffer* g) {
    if (!g || g->count == 0) return 0.0f;
    double sum = 0.0;
    for (uint8_t i = 0; i < g->count; i++) sum += graph_at(g, i);
    return (float)(sum / g->count);
}

void widget_graph_draw(GraphBuffer* g, int16_t x, int16_t y,
                       int16_t w, int16_t h, uint16_t color,
                       const char* unit) {
    if (!g || w < 20 || h < 20) return;

    float lo = g->minValue, hi = g->maxValue;
    if (g->autoScale && g->count > 1) {
        lo = widget_graph_min(g);
        hi = widget_graph_max(g);
        float span = hi - lo;
        if (span < 1e-6f) span = (fabsf(hi) > 1e-6f) ? fabsf(hi) * 0.1f : 1.0f;
        // Margem de 10 % em cima e embaixo para a linha nao encostar na borda.
        lo -= span * 0.1f;
        hi += span * 0.1f;
    }
    if (hi <= lo) hi = lo + 1.0f;

    LOCK_TFT();

    // Moldura e fundo
    tft.fillRoundRect(x, y, w, h, V_RADIUS_SM, V_BG_SURFACE);
    tft.drawRoundRect(x, y, w, h, V_RADIUS_SM, V_DIVIDER);

    // Grade horizontal em quartos
    for (uint8_t i = 1; i < 4; i++) {
        int16_t gy = y + (int16_t)((int32_t)h * i / 4);
        for (int16_t gx = x + 3; gx < x + w - 3; gx += 6) {
            tft.drawPixel(gx, gy, color_mix(V_DIVIDER, V_BG_SURFACE, 90));
        }
    }

    if (g->count >= 2) {
        const int16_t plotX = x + 2;
        const int16_t plotY = y + 2;
        const int16_t plotW = w - 4;
        const int16_t plotH = h - 4;
        const float   scale = (float)plotH / (hi - lo);

        int16_t prevX = plotX;
        int16_t prevY = plotY + plotH -
                        (int16_t)((graph_at(g, 0) - lo) * scale);

        for (uint8_t i = 1; i < g->count; i++) {
            int16_t px = plotX + (int16_t)((int32_t)plotW * i / (g->count - 1));
            int16_t py = plotY + plotH -
                         (int16_t)((graph_at(g, i) - lo) * scale);

            if (py < plotY)              py = plotY;
            if (py > plotY + plotH)      py = plotY + plotH;

            tft.drawLine(prevX, prevY, px, py, color);
            // Segunda linha logo abaixo deixa o traco mais legivel na TFT.
            tft.drawLine(prevX, prevY + 1, px, py + 1,
                         color_mix(color, V_BG_SURFACE, 110));
            prevX = px;
            prevY = py;
        }
    }

    // Rotulos de escala
    char buf[16];
    analysis_format_eng(hi, buf, sizeof(buf), 1);
    if (unit) strncat(buf, unit, sizeof(buf) - strlen(buf) - 1);
    draw_text_5x7(tft, x + 4, y + 3, buf, V_TEXT_SUB, 1);

    analysis_format_eng(lo, buf, sizeof(buf), 1);
    if (unit) strncat(buf, unit, sizeof(buf) - strlen(buf) - 1);
    draw_text_5x7(tft, x + 4, y + h - 10, buf, V_TEXT_SUB, 1);

    UNLOCK_TFT();
}

// ============================================================================
// 4. HOLD / MIN / MAX
// ============================================================================

void tracker_reset(MeasureTracker* t) {
    if (!t) return;
    memset(t, 0, sizeof(MeasureTracker));
    t->minimum = 0.0f;
    t->maximum = 0.0f;
}

void tracker_push(MeasureTracker* t, float value) {
    if (!t) return;
    if (isnan(value) || isinf(value)) return;
    if (t->hold) return;                 // congelado: ignora novas amostras

    t->current = value;

    if (t->samples == 0) {
        t->minimum = value;
        t->maximum = value;
    } else {
        if (value < t->minimum) t->minimum = value;
        if (value > t->maximum) t->maximum = value;
    }

    t->sum += (double)value;
    t->samples++;
    t->average = (float)(t->sum / t->samples);
}

void tracker_toggle_hold(MeasureTracker* t) {
    if (!t) return;
    t->hold = !t->hold;
    if (t->hold) t->heldValue = t->current;
    buzzer_click();
}

float tracker_display_value(const MeasureTracker* t) {
    if (!t) return 0.0f;
    return t->hold ? t->heldValue : t->current;
}

void tracker_draw_stats(const MeasureTracker* t, int16_t x, int16_t y,
                        const char* unit) {
    if (!t) return;
    char line[40], val[20];

    LOCK_TFT();
    tft.fillRect(x, y, 150, 34, V_BG_DARK);

    analysis_format_value(t->minimum, unit, val, sizeof(val));
    snprintf(line, sizeof(line), "MIN %s", val);
    draw_text_5x7(tft, x, y, line, V_TEXT_SUB, 1);

    analysis_format_value(t->maximum, unit, val, sizeof(val));
    snprintf(line, sizeof(line), "MAX %s", val);
    draw_text_5x7(tft, x, y + 11, line, V_TEXT_SUB, 1);

    analysis_format_value(t->average, unit, val, sizeof(val));
    snprintf(line, sizeof(line), "MED %s", val);
    draw_text_5x7(tft, x, y + 22, line, V_TEXT_SUB, 1);

    if (t->hold) {
        tft.fillRoundRect(x + 108, y, 40, 14, 3, V_WARNING);
        draw_text_5x7(tft, x + 114, y + 4, "HOLD", V_BG_DARK, 1);
    }
    UNLOCK_TFT();
}

// ============================================================================
// 5. MEDIDORES
// ============================================================================

void widget_bargraph(int16_t x, int16_t y, int16_t w, int16_t h,
                     float value, float minV, float maxV,
                     float warnAt, float dangerAt) {
    if (maxV <= minV) return;

    float clamped = value;
    if (clamped < minV) clamped = minV;
    if (clamped > maxV) clamped = maxV;

    const int16_t inner = w - 4;
    const int16_t fill  = (int16_t)((clamped - minV) / (maxV - minV) * inner);

    uint16_t color = V_SUCCESS;
    if (value >= dangerAt)     color = V_ALERT;
    else if (value >= warnAt)  color = V_WARNING;

    LOCK_TFT();
    tft.fillRoundRect(x, y, w, h, 3, V_BG_SURFACE);
    tft.drawRoundRect(x, y, w, h, 3, V_DIVIDER);
    if (fill > 0) tft.fillRoundRect(x + 2, y + 2, fill, h - 4, 2, color);

    // Marcas das zonas de aviso e perigo
    int16_t warnX   = x + 2 + (int16_t)((warnAt   - minV) / (maxV - minV) * inner);
    int16_t dangerX = x + 2 + (int16_t)((dangerAt - minV) / (maxV - minV) * inner);
    if (warnX   > x + 2 && warnX   < x + w - 2)
        tft.drawFastVLine(warnX,   y + 1, h - 2, V_WARNING);
    if (dangerX > x + 2 && dangerX < x + w - 2)
        tft.drawFastVLine(dangerX, y + 1, h - 2, V_ALERT);
    UNLOCK_TFT();
}

void widget_gauge(int16_t cx, int16_t cy, int16_t radius,
                  float value, float minV, float maxV, uint16_t color) {
    if (maxV <= minV || radius < 10) return;

    float ratio = (value - minV) / (maxV - minV);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    // Arco de 220 graus, comecando em 160 e terminando em 380 (= 20).
    const float startDeg = 160.0f;
    const float sweepDeg = 220.0f;
    const float endDeg   = startDeg + sweepDeg * ratio;

    LOCK_TFT();
    // Trilha de fundo
    for (float a = startDeg; a <= startDeg + sweepDeg; a += 2.0f) {
        float rad = a * PI / 180.0f;
        int16_t px = cx + (int16_t)(cosf(rad) * radius);
        int16_t py = cy + (int16_t)(sinf(rad) * radius);
        tft.fillCircle(px, py, 2, V_BG_SURFACE);
    }
    // Arco preenchido
    for (float a = startDeg; a <= endDeg; a += 2.0f) {
        float rad = a * PI / 180.0f;
        int16_t px = cx + (int16_t)(cosf(rad) * radius);
        int16_t py = cy + (int16_t)(sinf(rad) * radius);
        tft.fillCircle(px, py, 2, color);
    }
    UNLOCK_TFT();
}

// ============================================================================
// 6. DIALOGOS
// ============================================================================

// Quebra uma frase em ate maxLines linhas que caibam em maxChars colunas,
// cortando nos espacos. Devolve quantas linhas foram usadas.
static uint8_t wrap_text(const char* text, char lines[][40],
                         uint8_t maxLines, uint8_t maxChars) {
    if (!text) return 0;
    if (maxChars > 39) maxChars = 39;

    uint8_t line = 0;
    size_t pos = 0, len = strlen(text);

    while (pos < len && line < maxLines) {
        size_t take = len - pos;
        if (take > maxChars) {
            take = maxChars;
            // Recua ate o ultimo espaco para nao cortar palavra ao meio.
            size_t back = take;
            while (back > 0 && text[pos + back] != ' ') back--;
            if (back > 0) take = back;
        }
        memcpy(lines[line], text + pos, take);
        lines[line][take] = '\0';
        pos += take;
        while (pos < len && text[pos] == ' ') pos++;
        line++;
    }
    return line;
}

bool widget_confirm(const char* title, const char* message,
                    const char* okLabel, const char* cancelLabel,
                    uint32_t timeoutMs) {
    const int16_t boxX = 18, boxY = 38;
    const int16_t boxW = SCREEN_WIDTH - 36, boxH = SCREEN_HEIGHT - 70;

    char lines[4][40];
    uint8_t lineCount = wrap_text(message, lines, 4, 44);

    LOCK_TFT();
    tft.fillRoundRect(boxX + 3, boxY + 3, boxW, boxH, V_RADIUS_LG, 0x0000);
    tft.fillRoundRect(boxX, boxY, boxW, boxH, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(boxX, boxY, boxW, boxH, V_RADIUS_LG, V_WARNING);

    text_centered(SCREEN_WIDTH / 2, boxY + 12, title ? title : "CONFIRMAR",
                  V_WARNING, 1);
    tft.drawLine(boxX + 12, boxY + 26, boxX + boxW - 12, boxY + 26, V_DIVIDER);

    for (uint8_t i = 0; i < lineCount; i++) {
        text_centered(SCREEN_WIDTH / 2, boxY + 38 + i * 12, lines[i],
                      V_TEXT_MAIN, 1);
    }

    // Botoes
    const int16_t btnY = boxY + boxH - 34;
    const int16_t btnW = (boxW - 36) / 2;
    tft.fillRoundRect(boxX + 12, btnY, btnW, 26, V_RADIUS_MD, V_ALERT);
    text_centered(boxX + 12 + btnW / 2, btnY + 10, cancelLabel, V_TEXT_MAIN, 1);

    tft.fillRoundRect(boxX + boxW - 12 - btnW, btnY, btnW, 26, V_RADIUS_MD,
                      V_SUCCESS);
    text_centered(boxX + boxW - 12 - btnW / 2, btnY + 10, okLabel,
                  V_BG_DARK, 1);
    UNLOCK_TFT();

    buzzer_alert();
    hal_touch_wait_release();

    const uint32_t start = millis();
    uint16_t tx = 0, ty = 0;

    while ((millis() - start) < timeoutMs) {
        if (hal_touch_read(&tx, &ty)) {
            if (ty >= btnY && ty <= btnY + 26) {
                if (tx >= (uint16_t)(boxX + 12) &&
                    tx <= (uint16_t)(boxX + 12 + btnW)) {
                    buzzer_click();
                    hal_touch_wait_release();
                    return false;
                }
                if (tx >= (uint16_t)(boxX + boxW - 12 - btnW) &&
                    tx <= (uint16_t)(boxX + boxW - 12)) {
                    buzzer_ok();
                    hal_touch_wait_release();
                    return true;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // Tempo esgotado: por seguranca, a resposta padrao e "nao".
    return false;
}

void widget_error_screen(const char* title, const char* cause,
                         const char* whatToDo, uint16_t errorCode) {
    char lines[3][40];

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);

    // Faixa superior vermelha
    tft.fillRect(0, 0, SCREEN_WIDTH, 38, color_mix(V_ALERT, V_BG_DARK, 150));
    tft.drawFastHLine(0, 38, SCREEN_WIDTH, V_ALERT);

    // Triangulo de atencao
    tft.fillTriangle(24, 10, 14, 30, 34, 30, V_WARNING);
    draw_text_5x7(tft, 22, 19, "!", V_BG_DARK, 1);

    draw_text_5x7(tft, 46, 15, title ? title : "ERRO", V_TEXT_MAIN, 2);

    if (errorCode > 0) {
        char code[16];
        snprintf(code, sizeof(code), "COD %u", (unsigned)errorCode);
        draw_text_5x7(tft, SCREEN_WIDTH - text_width(code, 1) - 8, 4,
                      code, V_WARNING, 1);
    }

    int16_t y = 54;
    draw_text_5x7(tft, 14, y, "O QUE ACONTECEU", V_ALERT, 1);
    y += 14;
    uint8_t n = wrap_text(cause, lines, 3, 48);
    for (uint8_t i = 0; i < n; i++) {
        draw_text_5x7(tft, 14, y, lines[i], V_TEXT_MAIN, 1);
        y += 12;
    }

    y += 10;
    draw_text_5x7(tft, 14, y, "COMO RESOLVER", V_SUCCESS, 1);
    y += 14;
    n = wrap_text(whatToDo, lines, 3, 48);
    for (uint8_t i = 0; i < n; i++) {
        draw_text_5x7(tft, 14, y, lines[i], V_TEXT_MAIN, 1);
        y += 12;
    }

    tft.fillRoundRect(SCREEN_WIDTH / 2 - 45, SCREEN_HEIGHT - 30, 90, 24,
                      V_RADIUS_MD, V_CYAN_ELECTRIC);
    text_centered(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 22, "ENTENDI", V_BG_DARK, 1);
    UNLOCK_TFT();

    buzzer_error();
    hal_touch_wait_release();

    uint16_t tx, ty;
    hal_touch_wait(&tx, &ty, 30000);
    hal_touch_wait_release();
}

void widget_alert(const char* title, const char* message) {
    widget_confirm(title, message, "OK", "OK", 15000);
}

// ============================================================================
// 7. TECLADO NA TELA
// ============================================================================

// Layout compacto que cabe em 320x240 com alvos de toque de 30x26 pixels.
static const char* kKbRows[4] = {
    "1234567890",
    "QWERTYUIOP",
    "ASDFGHJKL_",
    "ZXCVBNM.-/"
};

bool widget_keyboard(const char* title, char* out, size_t outLen,
                     bool numericOnly) {
    if (!out || outLen < 2) return false;

    char text[32] = { 0 };
    strncpy(text, out, sizeof(text) - 1);
    size_t len = strlen(text);

    const int16_t keyW = 30, keyH = 26, gapX = 2, gapY = 2;
    const int16_t kbY = 96;
    const uint8_t rowCount = numericOnly ? 1 : 4;

    bool needsRedraw = true;
    hal_touch_wait_release();

    while (true) {
        if (needsRedraw) {
            LOCK_TFT();
            tft.fillScreen(V_BG_DARK);

            // Cabecalho e campo de texto
            draw_text_5x7(tft, 12, 10, title ? title : "DIGITE", V_CYAN_ELECTRIC, 1);
            tft.fillRoundRect(12, 26, SCREEN_WIDTH - 24, 30, V_RADIUS_MD,
                              V_BG_SURFACE);
            tft.drawRoundRect(12, 26, SCREEN_WIDTH - 24, 30, V_RADIUS_MD,
                              V_CYAN_ELECTRIC);
            draw_text_5x7(tft, 20, 36, text, V_TEXT_MAIN, 2);
            // Cursor
            tft.fillRect(20 + text_width(text, 2), 34, 2, 16, V_CYAN_ELECTRIC);

            // Botoes de acao
            tft.fillRoundRect(12, 64, 70, 24, V_RADIUS_SM, V_ALERT);
            draw_text_5x7(tft, 26, 72, "APAGA", V_TEXT_MAIN, 1);

            tft.fillRoundRect(90, 64, 70, 24, V_RADIUS_SM, V_BG_SURFACE);
            draw_text_5x7(tft, 112, 72, "ESC", V_TEXT_SUB, 1);

            tft.fillRoundRect(SCREEN_WIDTH - 82, 64, 70, 24, V_RADIUS_SM,
                              V_SUCCESS);
            draw_text_5x7(tft, SCREEN_WIDTH - 62, 72, "OK", V_BG_DARK, 1);

            // Teclas
            for (uint8_t r = 0; r < rowCount; r++) {
                const char* row = kKbRows[r];
                uint8_t cols = strlen(row);
                int16_t rowW = cols * (keyW + gapX) - gapX;
                int16_t x0 = (SCREEN_WIDTH - rowW) / 2;
                int16_t y  = kbY + r * (keyH + gapY);

                for (uint8_t c = 0; c < cols; c++) {
                    int16_t kx = x0 + c * (keyW + gapX);
                    char label[2] = { row[c], '\0' };
                    tft.fillRoundRect(kx, y, keyW, keyH, V_RADIUS_SM,
                                      V_BG_SURFACE);
                    tft.drawRoundRect(kx, y, keyW, keyH, V_RADIUS_SM, V_DIVIDER);
                    text_centered(kx + keyW / 2, y + keyH / 2 - 3, label,
                                  V_TEXT_MAIN, 1);
                }
            }
            UNLOCK_TFT();
            needsRedraw = false;
        }

        uint16_t tx, ty;
        if (!hal_touch_read(&tx, &ty)) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }

        // Barra de acoes
        if (ty >= 64 && ty <= 88) {
            if (tx >= 12 && tx <= 82) {                       // APAGA
                if (len > 0) { text[--len] = '\0'; }
                buzzer_click();
                needsRedraw = true;
            } else if (tx >= 90 && tx <= 160) {               // ESC
                buzzer_click();
                hal_touch_wait_release();
                return false;
            } else if (tx >= (uint16_t)(SCREEN_WIDTH - 82)) { // OK
                buzzer_ok();
                strncpy(out, text, outLen - 1);
                out[outLen - 1] = '\0';
                hal_touch_wait_release();
                return true;
            }
            hal_touch_wait_release();
            continue;
        }

        // Teclas
        for (uint8_t r = 0; r < rowCount; r++) {
            const char* row = kKbRows[r];
            uint8_t cols = strlen(row);
            int16_t rowW = cols * (keyW + gapX) - gapX;
            int16_t x0 = (SCREEN_WIDTH - rowW) / 2;
            int16_t y  = kbY + r * (keyH + gapY);

            if (ty < (uint16_t)y || ty > (uint16_t)(y + keyH)) continue;

            for (uint8_t c = 0; c < cols; c++) {
                int16_t kx = x0 + c * (keyW + gapX);
                if (tx < (uint16_t)kx || tx > (uint16_t)(kx + keyW)) continue;

                if (len < sizeof(text) - 1) {
                    text[len++] = (row[c] == '_') ? ' ' : row[c];
                    text[len] = '\0';
                    buzzer_click();
                    needsRedraw = true;
                }
                break;
            }
        }
        hal_touch_wait_release(400);
    }
}

// ============================================================================
// 8. LISTAS COM ROLAGEM
// ============================================================================

void widget_list_init(ScrollList* list, int16_t x, int16_t y, int16_t w,
                      uint8_t visibleRows, int16_t rowHeight,
                      uint16_t itemCount) {
    if (!list) return;
    list->x           = x;
    list->y           = y;
    list->w           = w;
    list->visibleRows = visibleRows;
    list->rowHeight   = rowHeight;
    list->itemCount   = itemCount;
    list->selected    = 0;
    list->topIndex    = 0;
}

void widget_list_scroll(ScrollList* list, int8_t delta) {
    if (!list || list->itemCount == 0) return;

    int32_t top = (int32_t)list->topIndex + delta * list->visibleRows;
    int32_t maxTop = (int32_t)list->itemCount - list->visibleRows;
    if (maxTop < 0) maxTop = 0;
    if (top < 0) top = 0;
    if (top > maxTop) top = maxTop;
    list->topIndex = (uint16_t)top;
}

void widget_list_draw(ScrollList* list,
                      const char* (*itemText)(uint16_t),
                      uint16_t (*itemColor)(uint16_t)) {
    if (!list || !itemText) return;

    LOCK_TFT();
    tft.fillRect(list->x, list->y, list->w,
                 list->visibleRows * list->rowHeight, V_BG_DARK);

    for (uint8_t r = 0; r < list->visibleRows; r++) {
        uint16_t index = list->topIndex + r;
        if (index >= list->itemCount) break;

        int16_t ry = list->y + r * list->rowHeight;
        bool selected = (index == list->selected);
        uint16_t fg = itemColor ? itemColor(index) : V_TEXT_MAIN;

        if (selected) {
            tft.fillRoundRect(list->x, ry, list->w, list->rowHeight - 2,
                              V_RADIUS_SM, color_mix(V_CYAN_ELECTRIC,
                                                     V_BG_DARK, 210));
            tft.drawRoundRect(list->x, ry, list->w, list->rowHeight - 2,
                              V_RADIUS_SM, V_CYAN_ELECTRIC);
        } else if ((r % 2) == 1) {
            // Faixas alternadas ajudam o olho a seguir a linha.
            tft.fillRect(list->x, ry, list->w, list->rowHeight - 2,
                         color_mix(V_BG_SURFACE, V_BG_DARK, 160));
        }

        draw_text_5x7(tft, list->x + 8, ry + (list->rowHeight - 8) / 2,
                      itemText(index), fg, 1);
    }

    // Barra de rolagem
    if (list->itemCount > list->visibleRows) {
        int16_t trackH = list->visibleRows * list->rowHeight;
        int16_t barH = (int16_t)((int32_t)trackH * list->visibleRows /
                                 list->itemCount);
        if (barH < 12) barH = 12;
        int16_t barY = list->y + (int16_t)((int32_t)(trackH - barH) *
                       list->topIndex /
                       (list->itemCount - list->visibleRows));

        int16_t barX = list->x + list->w - 4;
        tft.fillRoundRect(barX, list->y, 3, trackH, 1, V_BG_SURFACE);
        tft.fillRoundRect(barX, barY, 3, barH, 1, V_CYAN_ELECTRIC);
    }
    UNLOCK_TFT();
}

bool widget_list_handle_touch(ScrollList* list, int16_t tx, int16_t ty,
                              uint16_t* activatedIndex) {
    if (!list) return false;

    int16_t trackH = list->visibleRows * list->rowHeight;
    if (ty < list->y || ty > list->y + trackH)   return false;
    if (tx < list->x || tx > list->x + list->w)  return false;

    uint8_t row = (uint8_t)((ty - list->y) / list->rowHeight);
    uint16_t index = list->topIndex + row;
    if (index >= list->itemCount) return false;

    // Primeiro toque seleciona, segundo toque no mesmo item confirma.
    if (list->selected == index) {
        if (activatedIndex) *activatedIndex = index;
        buzzer_ok();
        return true;
    }
    list->selected = index;
    buzzer_click();
    return false;
}

// ============================================================================
// 9. CODIGO DE CORES DE RESISTOR
// ============================================================================

void widget_resistor_bands(int16_t x, int16_t y, int16_t w, int16_t h,
                           float ohms) {
    const char* bands[4] = { nullptr, nullptr, nullptr, nullptr };
    bool ok = analysis_resistor_color_bands(ohms, bands);

    const int16_t bodyX = x + 18;
    const int16_t bodyW = w - 36;

    LOCK_TFT();
    tft.fillRect(x, y, w, h, V_BG_DARK);

    // Terminais
    tft.fillRect(x, y + h / 2 - 1, 18, 3, 0xC618);
    tft.fillRect(x + w - 18, y + h / 2 - 1, 18, 3, 0xC618);

    // Corpo bege com cantos arredondados
    tft.fillRoundRect(bodyX, y, bodyW, h, h / 3, 0xE71C);
    tft.drawRoundRect(bodyX, y, bodyW, h, h / 3, 0xB596);

    if (!ok) {
        text_centered(x + w / 2, y + h / 2 - 3, "---", 0x0000, 1);
        UNLOCK_TFT();
        return;
    }

    // Tres faixas de valor agrupadas e a de tolerancia isolada a direita.
    const int16_t bandW = 8;
    for (uint8_t i = 0; i < 3; i++) {
        int16_t bx = bodyX + 8 + i * (bandW + 6);
        tft.fillRect(bx, y + 2, bandW, h - 4,
                     analysis_color_band_rgb(bands[i]));
    }
    int16_t tolX = bodyX + bodyW - 14;
    tft.fillRect(tolX, y + 2, bandW, h - 4, analysis_color_band_rgb(bands[3]));
    UNLOCK_TFT();
}

// ============================================================================
// 10. CABECALHOS E CARTOES
// ============================================================================

void widget_value_card(int16_t x, int16_t y, int16_t w, int16_t h,
                       const char* label, const char* value,
                       const char* unit, uint16_t accentColor) {
    LOCK_TFT();
    tft.fillRoundRect(x + 2, y + 2, w, h, V_RADIUS_MD, 0x0000);
    tft.fillRoundRect(x, y, w, h, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(x, y, w, h, V_RADIUS_MD,
                      color_mix(accentColor, V_BG_SURFACE, 150));

    // Faixa de acento no topo
    tft.fillRoundRect(x, y, w, 3, 1, accentColor);

    draw_text_5x7(tft, x + 8, y + 9, label ? label : "", V_TEXT_SUB, 1);

    // Valor grande, ajustando o tamanho se for comprido demais.
    const char* v = value ? value : "---";
    uint8_t size = 3;
    while (size > 1 && text_width(v, size) > (w - 16)) size--;
    draw_text_5x7(tft, x + 8, y + h / 2 - 4, v, V_TEXT_MAIN, size);

    if (unit && unit[0]) {
        draw_text_5x7(tft, x + w - text_width(unit, 1) - 8, y + h - 14,
                      unit, accentColor, 1);
    }
    UNLOCK_TFT();
}

void widget_status_chip(int16_t x, int16_t y, const char* text,
                        uint16_t color) {
    if (!text) return;
    int16_t w = text_width(text, 1) + 16;

    LOCK_TFT();
    tft.fillRoundRect(x, y, w, 16, 8, color_mix(color, V_BG_DARK, 190));
    tft.drawRoundRect(x, y, w, 16, 8, color);
    tft.fillCircle(x + 7, y + 8, 2, color);
    draw_text_5x7(tft, x + 13, y + 5, text, V_TEXT_MAIN, 1);
    UNLOCK_TFT();
}

void widget_section_divider(int16_t x, int16_t y, int16_t w,
                            const char* title) {
    LOCK_TFT();
    if (title && title[0]) {
        draw_text_5x7(tft, x, y, title, V_NEON_GREEN, 1);
        int16_t tw = text_width(title, 1) + 8;
        if (tw < w) tft.drawLine(x + tw, y + 3, x + w, y + 3, V_DIVIDER);
    } else {
        tft.drawLine(x, y, x + w, y, V_DIVIDER);
    }
    UNLOCK_TFT();
}

void widget_spinner(int16_t cx, int16_t cy, int16_t radius, uint8_t step,
                    uint16_t color) {
    const uint8_t dots = 8;

    LOCK_TFT();
    for (uint8_t i = 0; i < dots; i++) {
        float rad = (i * 360.0f / dots) * PI / 180.0f;
        int16_t px = cx + (int16_t)(cosf(rad) * radius);
        int16_t py = cy + (int16_t)(sinf(rad) * radius);

        // O ponto ativo fica cheio e os anteriores vao desbotando.
        uint8_t distance = (uint8_t)((i + dots - (step % dots)) % dots);
        uint8_t alpha = (uint8_t)(60 + distance * 24);
        if (alpha > 250) alpha = 250;
        tft.fillCircle(px, py, 2, color_mix(color, V_BG_DARK, alpha));
    }
    UNLOCK_TFT();
}
