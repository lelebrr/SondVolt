// ============================================================================
// Sondvolt v3.2 — Gerenciador de UI
// ============================================================================

#include "ui.h"
#include "graphics.h"
#include "menu.h"
#include "fonts.h"
#include "display_globals.h" 
#include "display_mutex.h"   
#include "globals.h"         
#include "buzzer.h"
#include "logger.h"
#include "help.h"
#include "visual.h"
#include "multimeter.h"
#include "leds.h"
#include "hal.h"
#include "analysis.h"
#include "measurements.h"
#include "database.h"
#include "thermal.h"
#include "calibration.h"
#include "diagnostics.h"
#include "uiwidgets.h"
#include "safety.h"
#include "screens.h"
#include "netsvc.h"
#include "jobs.h"
#include <Arduino.h>

static AppState lastKnownState = STATE_SPLASH;
static bool isMenuMode = true;
static bool isTouching = false;
static bool hasMoved = false;
static bool needsScreenRedraw = true;
static int16_t uiScrollY = 0; // Nomeado genericamente para reuso
static uint32_t touchStartTime = 0;
static uint16_t touchStartX = 0;
static uint16_t touchStartY = 0;
static uint16_t lastTouchX = 0;
static uint16_t lastTouchY = 0;
static const int8_t SETTINGS_ITEM_COUNT = 18;
static const int16_t SETTINGS_ITEM_HEIGHT = 45;
static const int16_t SETTINGS_VISIBLE_HEIGHT = 165;
static const int16_t SETTINGS_SCROLL_MAX = (SETTINGS_ITEM_COUNT * SETTINGS_ITEM_HEIGHT) - SETTINGS_VISIBLE_HEIGHT;

static bool is_help_available_state(AppState state) {
    switch (state) {
        case STATE_MEASURE_RESISTOR:
        case STATE_MEASURE_CAPACITOR:
        case STATE_MEASURE_DIODE:
        case STATE_MEASURE_TRANSISTOR:
        case STATE_MEASURE_INDUCTOR:
        case STATE_MEASURE_IC:
        case STATE_MEASURE_LED:
        case STATE_MEASURE_GENERIC:
        case STATE_MULTIMETER:
        case STATE_THERMAL_PROBE:
        case STATE_THERMAL_CAMERA:
        case STATE_SCANNER:
            return true;
        default:
            return false;
    }
}

// Prototipos Internos (Ordem Organizacional)
static void draw_instrument_content(); 
static void draw_about_screen();
static void draw_settings_screen();
static void draw_history_screen();
static void draw_status_bar();
static void backlight_update();
static void backlight_on();
static void draw_loading_step(const char* msg, int progress);

static bool isDialogActive = false;
static int8_t dialogTargetIdx = -1;
static void draw_confirmation_dialog(const char* msg);
static void draw_cpu_info_screen();
static void apply_theme(uint8_t idx);
static void draw_thermal_camera_screen();
static void draw_safety_alert_screen();
static void draw_help_screen();
void draw_calibration_screen(); // Referenciado em menu.h
static void draw_thermal_probe_screen();
static void draw_comparator_screen();

// ============================================================================
// SEQUENCIA DE BOOT
// ============================================================================
// A versao anterior fingia inicializar: mostrava "Montando SD Card..." com um
// delay(400) e seguia em frente, sem montar coisa alguma. As mensagens nao
// tinham relacao com o que estava acontecendo.
//
// Agora a barra de boot e alimentada pelo autoteste de verdade
// (diag_run_selftest), e cada linha exibida corresponde a um subsistema que
// esta realmente sendo verificado naquele instante.
// ============================================================================

void ui_boot_begin() {
    colors_update();

    // O backlight so acende depois do logo estar desenhado, para o usuario
    // nao ver o lixo da memoria de video no instante da energizacao.
    hal_pwm_attach(PIN_TFT_BL, LEDC_CH_BACKLIGHT,
                   BACKLIGHT_PWM_FREQ, BACKLIGHT_PWM_BITS);
    hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, 0);

    LOCK_TFT();
    draw_logo_full();
    UNLOCK_TFT();

    // Acende suave, em vez de estourar o brilho de uma vez.
    for (uint16_t b = 0; b <= deviceSettings.backlight; b += 8) {
        hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, b);
        delay(6);
    }
    hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, deviceSettings.backlight);
    backlightOn = true;
}

void ui_boot_progress(uint8_t percent, const char* message) {
    draw_loading_step(message ? message : "", percent);
}

void ui_init() {
    colors_update();
    menu_init();

    if (!backlightOn) {
        hal_pwm_attach(PIN_TFT_BL, LEDC_CH_BACKLIGHT,
                       BACKLIGHT_PWM_FREQ, BACKLIGHT_PWM_BITS);
        hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, deviceSettings.backlight);
        backlightOn = true;
    }

    draw_loading_step("Sistema pronto", 100);
    delay(500);

    currentAppState  = STATE_MENU;
    previousAppState = STATE_MENU;
    lastActivityMs   = millis();
}

void ui_update() {
    backlight_update();
    if (!backlightOn) return; // Nao processa UI se tela estiver desligada

    // ------------------------------------------------------------------
    // Sobreposicao de seguranca
    // ------------------------------------------------------------------
    // draw_safety_alert_screen() existia no codigo desde a v3.1 e NUNCA era
    // chamada: o aparelho detectava (ou deveria detectar) tensao perigosa e
    // nao avisava nada na tela. Agora ela cobre qualquer outra tela enquanto
    // o estado de perigo estiver ativo, e so sai quando o usuario confirma.
    if (safetyStatus.state == SAFETY_STATE_DANGER) {
        static bool alertDrawn = false;
        if (!alertDrawn) {
            draw_safety_alert_screen();
            alertDrawn = true;
        }
        // Toque em qualquer lugar da faixa do botao reconhece o aviso.
        if (isTouching && lastTouchY > 210) {
            safety_acknowledge_warning();
            alertDrawn = false;
            lastKnownState = STATE_SPLASH;   // forca redesenho completo
        }
        return;
    }

    // Detecta mudança de estado para redesenho total
    if (currentAppState != lastKnownState) {
        // Libera o hardware da tela anterior ANTES de montar a proxima.
        // O osciloscopio segura o ADC e o gerador segura o pino de
        // excitacao: sair sem devolver deixa o resto do aparelho cego.
        if (screens_handles((AppState)lastKnownState)) {
            screens_exit((AppState)lastKnownState);
        }
        if (screens_handles((AppState)currentAppState)) {
            screens_enter((AppState)currentAppState);
        }

        LOCK_TFT();
        tft.fillScreen(V_BG_DARK);
        UNLOCK_TFT();

        needsScreenRedraw = true;
        uiScrollY = 0;
        
        switch (currentAppState) {
            case STATE_MENU:         
            case STATE_SUBMENU_TEMP: 
            case STATE_SUBMENU_MAIS: 
                isMenuMode = true;  
                menu_refresh();
                menu_draw(); 
                break;
            case STATE_ABOUT:    isMenuMode = false; graphics_draw_header("SOBRE O SISTEMA"); break;
            case STATE_SETTINGS: isMenuMode = false; graphics_draw_header("CONFIGURACOES"); break;
            case STATE_HISTORY:  isMenuMode = false; graphics_draw_header("HISTORICO"); break;
            case STATE_STATS:    isMenuMode = false; graphics_draw_header("INFO TECNICA CPU"); break;
            case STATE_HELP:     isMenuMode = false; graphics_draw_header("AJUDA CONTEXTUAL"); break;
            default:             isMenuMode = false; graphics_draw_header("INSTRUMENTO"); break;
        }
        
        // Desenho inicial da moldura e botões comuns
        if (!isMenuMode || currentAppState == STATE_SUBMENU_TEMP || currentAppState == STATE_SUBMENU_MAIS) {
            // graphics_draw_back_button(); // REMOVIDO: Já desenhado por graphics_draw_header
            // Botão ajuda apenas em telas de medição
            if (is_help_available_state((AppState)currentAppState)) {
                graphics_draw_help_button();
            }
        }
        
        lastKnownState = currentAppState;
    }

    // As telas da v5.0 desenham por conta propria, inclusive o cabecalho.
    if (screens_handles((AppState)currentAppState)) {
        screens_draw((AppState)currentAppState);
        lastKnownState = currentAppState;
        return;
    }

    // Atualização contínua da tela ativa
    if (isMenuMode) {
        menu_draw();
    } 
    else {
        if (needsScreenRedraw) {
            switch (currentAppState) {
                case STATE_ABOUT:    draw_about_screen();    break;
                case STATE_SETTINGS: draw_settings_screen(); break;
                case STATE_HISTORY:  draw_history_screen();  break;
                case STATE_STATS:    draw_cpu_info_screen(); break;
                case STATE_THERMAL_CAMERA: draw_thermal_camera_screen(); break;
                case STATE_COMPARATOR: draw_comparator_screen(); break;
                case STATE_CALIBRATION: draw_calibration_screen(); break;
                case STATE_THERMAL_PROBE: draw_thermal_probe_screen(); break;
                case STATE_HELP:     draw_help_screen();     break;
                default:
                    if (currentAppState >= 10 && currentAppState < 40) {
                        draw_instrument_content();
                    }
                    break;
            }
            needsScreenRedraw = false;
        }
        
        // A barra de status e instrumentos podem precisar de atualização periódica
        static uint32_t lastFastUpdate = 0;
        if (millis() - lastFastUpdate > 500) {
            if (currentAppState >= 10 && currentAppState < 40) draw_instrument_content();
            draw_status_bar(); 
            lastFastUpdate = millis();
        }
    }
}

// Função para desenhar o conteúdo dinâmico (Valores, etc)
static void draw_instrument_content() {
    // ------------------------------------------------------------------
    // NOTA IMPORTANTE SOBRE ESTA TELA
    // ------------------------------------------------------------------
    // A versao anterior exibia valores INVENTADOS em texto fixo:
    // "ESR: 0.12 Ohms", "hFE: 245", "Vbe: 642mV", "Q: 4.2 @ 1kHz",
    // "Ir: < 10nA", "Rdc: 0.8 Ohms". Nada disso era medido - eram strings
    // constantes. Um instrumento que mostra numero falso e pior que um
    // instrumento que nao mostra nada, porque o tecnico confia nele.
    //
    // Agora todo valor vem do motor de analise, e o que nao pode ser medido
    // aparece como "---" ou com o motivo da indisponibilidade.
    // ------------------------------------------------------------------
    LOCK_TFT();

    char valBuf[24]  = "---";
    char subBuf1[42] = "";
    char subBuf2[42] = "";
    const char* title = "INSTRUMENTO";
    const char* unit  = "";
    uint16_t color = V_CYAN_ELECTRIC;
    IconType icon  = ICON_UNKNOWN;

    const bool probesReady = hal_probe_available();
    AnalysisResult res = measurements_get_last_result();

    switch (currentAppState) {

        case STATE_MEASURE_RESISTOR: {
            title = "OHMIMETRO";
            color = V_WARNING;
            icon  = ICON_RESISTOR;
            unit  = "Ohm";

            if (!probesReady) {
                strcpy(subBuf1, "Circuito de pontas ausente");
                strcpy(subBuf2, "Veja Diagnostico");
            } else if (lastResistance >= ANALYSIS_OPEN * 0.9f) {
                strcpy(valBuf, "OL");
                strcpy(subBuf1, "Pontas em aberto");
                strcpy(subBuf2, "Conecte o resistor");
            } else {
                analysis_format_eng(lastResistance, valBuf, sizeof(valBuf), 2);
                float nearest = analysis_nearest_e_series(lastResistance, 24);
                float dev = analysis_e_series_deviation(lastResistance, 24);
                char n[16];
                analysis_format_eng(nearest, n, sizeof(n), 2);
                snprintf(subBuf1, sizeof(subBuf1), "E24: %sOhm (%+.1f%%)", n, dev);
                snprintf(subBuf2, sizeof(subBuf2), "Tolerancia sugerida: %u%%",
                         (unsigned)analysis_suggest_tolerance(lastResistance, 24));
            }
            break;
        }

        case STATE_MEASURE_CAPACITOR: {
            title = "CAPACIMETRO";
            color = V_CYAN_ELECTRIC;
            icon  = ICON_CAPACITOR;
            unit  = "F";

            if (!probesReady) {
                strcpy(subBuf1, "Circuito de pontas ausente");
            } else if (lastCapacitance <= 0.0f) {
                strcpy(subBuf1, "Nenhum capacitor detectado");
                strcpy(subBuf2, "Descarregue antes de medir");
            } else {
                analysis_format_eng(lastCapacitance, valBuf, sizeof(valBuf), 2);
                float esr = measurements_get_esr();
                if (esr > 0.0f) {
                    snprintf(subBuf1, sizeof(subBuf1), "ESR medida: %.2f Ohm", esr);
                    // ESR alta e o sintoma classico de eletrolitico ressecado.
                    if (esr > 8.0f && lastCapacitance > 1e-6f) {
                        strcpy(subBuf2, "ESR ALTA: suspeito de defeito");
                        color = V_ALERT;
                    } else {
                        strcpy(subBuf2, "ESR dentro do esperado");
                    }
                } else {
                    strcpy(subBuf1, "ESR nao mensuravel");
                }
            }
            break;
        }

        case STATE_MEASURE_DIODE:
        case STATE_MEASURE_LED: {
            const bool isLed = (currentAppState == STATE_MEASURE_LED);
            title = isLed ? "TESTE DE LED" : "TESTE DE DIODO";
            color = isLed ? V_NEON_GREEN : V_ALERT;
            icon  = isLed ? ICON_LED : ICON_DIODE;
            unit  = "V";

            float vf = res.forwardVoltage;
            if (vf <= 0.0f) {
                strcpy(subBuf1, "Sem juncao detectada");
                strcpy(subBuf2, "Inverta as pontas e repita");
            } else {
                snprintf(valBuf, sizeof(valBuf), "%.3f", vf);
                snprintf(subBuf1, sizeof(subBuf1), "Vf a ~5 mA");
                snprintf(subBuf2, sizeof(subBuf2), "%s", res.detail);
            }
            break;
        }

        case STATE_MEASURE_TRANSISTOR: {
            title = "ANALISADOR DE TRANSISTOR";
            color = V_VIBRANT_PURPLE;
            icon  = ICON_TRANSISTOR_NPN;
            unit  = "hFE";

            if (res.gain > 0.0f) {
                snprintf(valBuf, sizeof(valBuf), "%.0f", res.gain);
                snprintf(subBuf1, sizeof(subBuf1), "Vbe: %.3f V", res.forwardVoltage);
                snprintf(subBuf2, sizeof(subBuf2), "Tipo: %s",
                         analysis_type_name(res.type));
            } else {
                strcpy(subBuf1, "Nenhum ganho detectado");
                strcpy(subBuf2, "Confira a pinagem E-B-C");
            }
            break;
        }

        case STATE_MEASURE_INDUCTOR: {
            title = "MEDIDOR DE INDUTANCIA";
            color = V_WARNING;
            icon  = ICON_INDUCTOR;
            unit  = "H";

            if (lastInductance > 0.0f) {
                analysis_format_eng(lastInductance, valBuf, sizeof(valBuf), 2);
                snprintf(subBuf1, sizeof(subBuf1), "DCR: %.2f Ohm", lastResistance);
                strcpy(subBuf2, "Faixa util: 100uH a 100mH");
            } else {
                strcpy(subBuf1, "Fora da faixa mensuravel");
                strcpy(subBuf2, "Valido de 100uH a 100mH");
            }
            break;
        }

        case STATE_MEASURE_IC:
        case STATE_SCANNER: {
            title = (currentAppState == STATE_SCANNER) ? "SCANNER AUTOMATICO"
                                                       : "ANALISE DE CI";
            color = V_VIBRANT_PURPLE;
            icon  = ICON_AUTO;

            if (res.valid && res.type != COMP_NONE) {
                strncpy(valBuf, res.valueText, sizeof(valBuf) - 1);
                snprintf(subBuf1, sizeof(subBuf1), "%s", res.label);
                snprintf(subBuf2, sizeof(subBuf2), "%s", res.detail);
            } else {
                strcpy(subBuf1, "Conecte o componente");
                strcpy(subBuf2, "A deteccao roda continuamente");
            }
            break;
        }

        case STATE_MULTIMETER: {
            MultimeterReading mr = multimeter_get_last_reading();
            title = "MULTIMETRO";
            color = mr.valid ? V_CYAN_ELECTRIC : V_ALERT;
            icon  = (mr.mode == MMODE_CONTINUITY) ? ICON_CONTINUITY
                                                  : ICON_MULTIMETER;

            if (mr.valid) {
                multimeter_format_value(mr.value, valBuf, sizeof(valBuf));
                unit = mr.unitAbbrev ? mr.unitAbbrev : "";
                snprintf(subBuf1, sizeof(subBuf1), "%s  |  %s",
                         multimeter_mode_name(mr.mode),
                         multimeter_state_name(mr.state));

                if (mr.mode == MMODE_AC_VOLTAGE) {
                    snprintf(subBuf2, sizeof(subBuf2), "Pico: %.1f V%s",
                             mr.peakValue, mr.surgeDetected ? "  SURTO!" : "");
                    if (mr.surgeDetected) color = V_ALERT;
                } else {
                    snprintf(subBuf2, sizeof(subBuf2), "Faixa: %s",
                             (mr.range == RANGE_AUTO) ? "automatica" : "manual");
                }
            } else {
                strcpy(valBuf, "---");
                snprintf(subBuf1, sizeof(subBuf1), "%s indisponivel",
                         multimeter_mode_name(mr.mode));
                strcpy(subBuf2, "Sensor ausente ou nao calibrado");
            }
            break;
        }

        case STATE_THERMAL_PROBE: {
            title = "TERMOMETRO DE CONTATO";
            color = thermal_status_color();
            icon  = ICON_TEMP;
            unit  = "C";

            if (thermal_is_valid()) {
                snprintf(valBuf, sizeof(valBuf), "%.1f", lastTemperature);
                snprintf(subBuf1, sizeof(subBuf1), "Estado: %s",
                         thermal_status_text());
                snprintf(subBuf2, sizeof(subBuf2), "DS18B20 - %u sensor(es)",
                         (unsigned)thermal_device_count());
            } else {
                strcpy(subBuf1, "Sonda DS18B20 nao encontrada");
                strcpy(subBuf2, "Conecte no barramento OneWire");
            }
            break;
        }

        case STATE_CALIBRATION: {
            title = "CALIBRACAO DE PONTAS";
            color = V_WARNING;
            icon  = ICON_RESISTOR;

            if (calibration_is_valid()) {
                snprintf(valBuf, sizeof(valBuf), "%.2f",
                         analysis_get_resistance_offset());
                unit = "Ohm";
                snprintf(subBuf1, sizeof(subBuf1), "Offset dos cabos gravado");
                snprintf(subBuf2, sizeof(subBuf2), "C parasita: %.0f pF",
                         analysis_get_capacitance_offset() * 1e12f);
            } else {
                strcpy(valBuf, "---");
                strcpy(subBuf1, "Aparelho nao calibrado");
                strcpy(subBuf2, "Toque em CALIBRAR AGORA");
            }
            break;
        }

        case STATE_MEASURE_GENERIC: {
            title = "TESTE AUTOMATICO";
            color = V_NEON_GREEN;

            if (!probesReady) {
                icon = ICON_WARNING;
                strcpy(subBuf1, "Circuito de pontas ausente");
                strcpy(subBuf2, "Sem ele nao ha como medir");
                color = V_ALERT;
            } else if (res.valid && res.type != COMP_NONE) {
                strncpy(valBuf, res.valueText, sizeof(valBuf) - 1);
                snprintf(subBuf1, sizeof(subBuf1), "%s", res.label);
                snprintf(subBuf2, sizeof(subBuf2), "%s", res.detail);
                color = db_status_color(res.status);

                switch (res.type) {
                    case COMP_RESISTOR:             icon = ICON_RESISTOR; break;
                    case COMP_CAPACITOR_CERAMIC:
                    case COMP_CAPACITOR_ELECTRO:
                    case COMP_CAPACITOR:            icon = ICON_CAPACITOR; break;
                    case COMP_DIODE:                icon = ICON_DIODE; break;
                    case COMP_LED:                  icon = ICON_LED; break;
                    case COMP_TRANSISTOR_NPN:
                    case COMP_TRANSISTOR_PNP:       icon = ICON_TRANSISTOR_NPN; break;
                    case COMP_MOSFET_N:
                    case COMP_MOSFET_P:             icon = ICON_MOSFET_N; break;
                    case COMP_INDUCTOR:             icon = ICON_INDUCTOR; break;
                    default:                        icon = ICON_AUTO; break;
                }
            } else {
                icon = ICON_AUTO;
                strcpy(subBuf1, "Insira o componente");
                strcpy(subBuf2, "A identificacao e automatica");
            }
            break;
        }

        default:
            title = "ANALISANDO";
            break;
    }

    graphics_draw_header(title);

    // --- Coluna esquerda: icone do componente -------------------------------
    tft.fillRoundRect(12, 55, 65, 95, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(12, 55, 65, 95, V_RADIUS_MD, color);
    draw_bitmap_icon(icon, 28, 87);

    // --- Painel principal: o valor -------------------------------------------
    tft.fillRoundRect(85, 52, 225, 105, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(85, 52, 225, 105, V_RADIUS_LG, color);

    // Escolhe a maior fonte que ainda cabe no painel.
    const int16_t panelW = 205;
    uint8_t fontSize = 5;
    while (fontSize > 1 &&
           (int16_t)(strlen(valBuf) * 6 * fontSize) > panelW) {
        fontSize--;
    }

    int16_t valWidth = (int16_t)(strlen(valBuf) * 6 * fontSize);
    int16_t valX = 197 - valWidth / 2;
    int16_t valY = (fontSize >= 5) ? 90 : 96;

    // Sombra sutil melhora a leitura de longe, sob luz de bancada.
    draw_text_5x7(tft, valX + 1, valY + 1, valBuf,
                  color_mix(V_BG_DARK, color, 210), fontSize);
    draw_text_5x7(tft, valX, valY, valBuf, color, fontSize);

    if (unit[0]) {
        draw_text_5x7(tft, 300 - (int16_t)strlen(unit) * 6, 132,
                      unit, V_TEXT_SUB, 1);
    }

    // --- Faixas de informacao secundaria -------------------------------------
    tft.fillRoundRect(12, 162, 296, 42, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(12, 162, 296, 42, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 22, 172, subBuf1, V_TEXT_MAIN, 1);
    draw_text_5x7(tft, 22, 187, subBuf2, V_TEXT_SUB, 1);

    // --- Codigo de cores, exclusivo do ohmimetro ------------------------------
    if (currentAppState == STATE_MEASURE_RESISTOR &&
        lastResistance > 1.0f && lastResistance < 99e6f) {
        UNLOCK_TFT();
        widget_resistor_bands(180, 208, 128, 26, lastResistance);
        LOCK_TFT();
    }

    // --- Botoes de acao --------------------------------------------------------
    if (currentAppState == STATE_MEASURE_GENERIC && res.valid) {
        tft.fillRoundRect(10, 208, 145, 26, V_RADIUS_SM, V_BG_HIGHLIGHT);
        tft.drawRoundRect(10, 208, 145, 26, V_RADIUS_SM, V_CYAN_ELECTRIC);
        draw_text_5x7(tft, 32, 217, "TESTAR DE NOVO", V_TEXT_MAIN, 1);

        tft.fillRoundRect(165, 208, 145, 26, V_RADIUS_SM, V_SUCCESS);
        draw_text_5x7(tft, 205, 217, "SALVAR", V_BG_DARK, 1);
    }

    if (currentAppState == STATE_CALIBRATION) {
        tft.fillRoundRect(80, 208, 160, 26, V_RADIUS_SM, V_VIBRANT_PURPLE);
        draw_text_5x7(tft, 110, 217, "CALIBRAR AGORA", V_TEXT_MAIN, 1);
    }

    if (currentAppState == STATE_MEASURE_CAPACITOR) {
        tft.fillRoundRect(10, 208, 160, 26, V_RADIUS_SM, V_WARNING);
        draw_text_5x7(tft, 28, 217, "DESCARREGAR", V_BG_DARK, 1);
    }

    // --- Sobreposicao de descarga ----------------------------------------------
    if (isDischarging) {
        tft.fillRoundRect(40, 70, 240, 100, V_RADIUS_LG, V_BG_SURFACE);
        tft.drawRoundRect(40, 70, 240, 100, V_RADIUS_LG, V_CYAN_ELECTRIC);
        draw_text_5x7(tft, 62, 92, "DESCARREGANDO CAPACITOR",
                      V_CYAN_ELECTRIC, 1);

        UNLOCK_TFT();
        widget_progress_bar(60, 126, 200, 16,
                            (uint8_t)(dischargeProgress * 100.0f),
                            V_CYAN_ELECTRIC);
        LOCK_TFT();

        char perc[8];
        snprintf(perc, sizeof(perc), "%d%%", (int)(dischargeProgress * 100.0f));
        draw_text_5x7(tft, 150, 150, perc, V_TEXT_MAIN, 1);
    }

    UNLOCK_TFT();
}

static void draw_status_bar() {
    LOCK_TFT();
    // Barra de status translúcida (Slightly lower and thinner)
    tft.fillRect(0, 228, 320, 12, color_mix(V_BG_HIGHLIGHT, V_BG_DARK, 180)); 
    tft.drawFastHLine(0, 228, 320, V_DIVIDER);
    
    // Status SD
    tft.fillCircle(10, 234, 3, sdCardPresent ? V_NEON_GREEN : V_ALERT);
    draw_text_5x7(tft, 20, 231, sdCardPresent ? "SD" : "No-SD", sdCardPresent ? V_NEON_GREEN : V_ALERT, 1);
    
    // Firmware
    // Versao e relogio vem do firmware, nao de texto fixo: assim nao ficam
    // desatualizados numa proxima versao.
    {
        char status[40];
        char clock[16];
        net_format_time(clock, sizeof(clock));
        snprintf(status, sizeof(status), "v%s  %s", FW_VERSION,
                 net_time_valid() ? clock : "");
        draw_text_5x7(tft, 96, 231, status, V_TEXT_SUB, 1);

        // Trabalho ativo, quando houver: e o contexto de tudo que for medido.
        if (jobs_has_active()) {
            char job[24];
            snprintf(job, sizeof(job), "[%.12s]", jobs_active_name());
            draw_text_5x7(tft, 200, 231, job, V_NEON_GREEN, 1);
        }
    }
    
    // Bateria Ícone (Compacto)
    tft.drawRoundRect(290, 231, 18, 8, 1, V_PURE_WHITE);
    tft.fillRect(291, 232, 12, 6, V_NEON_GREEN);
    
    UNLOCK_TFT();
}

static void draw_about_screen() {
    LOCK_TFT();
    tft.fillRoundRect(15, 50, 290, 165, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(15, 50, 290, 155, V_RADIUS_LG, V_CYAN_ELECTRIC);
    
    draw_logo_small(210, 65);
    draw_text_5x7(tft, 35, 70, "SONDVOLT PRO", V_CYAN_ELECTRIC, 2);
    tft.drawLine(35, 90, 285, 90, V_DIVIDER);
    
    char buf[64];
    sprintf(buf, "CPU: ESP32 v%d @ %dMHz", ESP.getChipRevision(), getCpuFrequencyMhz());
    draw_text_5x7(tft, 35, 100, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "RAM: %d KB / Flash: %d MB", ESP.getFreeHeap()/1024, ESP.getFlashChipSize()/(1024*1024));
    draw_text_5x7(tft, 35, 115, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "SD: %s (%s)", sdCardPresent ? "OK" : "ERR", sdCardError ? "Hardware" : "Vazio");
    draw_text_5x7(tft, 35, 130, buf, sdCardPresent ? V_NEON_GREEN : V_ALERT, 1);
    
    uint32_t up = millis() / 1000;
    sprintf(buf, "Ligado ha %02lu:%02lu:%02lu",
            (unsigned long)(up / 3600UL),
            (unsigned long)((up % 3600UL) / 60UL),
            (unsigned long)(up % 60UL));
    draw_text_5x7(tft, 35, 145, buf, V_TEXT_SUB, 1);
    
    SystemHealth health = diag_get_health();
    sprintf(buf, "Chip: %.0f C | Heap min: %lu KB", health.chipTempC,
            (unsigned long)(health.minFreeHeap / 1024));
    draw_text_5x7(tft, 35, 160, buf, V_TEXT_SUB, 1);
    
    // Rodapé de status
    tft.fillRect(15, 185, 290, 20, V_BG_HIGHLIGHT);
    draw_text_5x7(tft, 50, 192, "SISTEMA INTEGRADO - LICENCA PRO", V_NEON_GREEN, 1);
    
    UNLOCK_TFT();
}

static void draw_loading_step(const char* msg, int progress) {
    LOCK_TFT();
    // Barra de progresso Neon
    int barW = 240;
    int barH = 10;
    int x = (320 - barW) / 2;
    int y = 205;
    
    // Fundo da barra
    tft.fillRoundRect(x - 2, y - 2, barW + 4, barH + 4, 6, 0x0000);
    tft.drawRoundRect(x - 1, y - 1, barW + 2, barH + 2, 5, V_BG_HIGHLIGHT);
    
    // Progresso
    if (progress > 0) {
        tft.fillRoundRect(x, y, (barW * progress) / 100, barH, 4, V_CYAN_ELECTRIC);
        // Efeito de brilho no progresso
        if (progress < 100) {
            tft.fillCircle(x + (barW * progress) / 100, y + barH/2, 4, V_PURE_WHITE);
        }
    }
    
    // Mensagem centralizada
    tft.fillRect(0, y - 30, 320, 25, V_BG_DARK); 
    int textX = 160 - (strlen(msg) * 3);
    draw_text_5x7(tft, textX, y - 18, msg, V_TEXT_MAIN, 1);
    
    UNLOCK_TFT();
}

static void draw_settings_screen() {
    LOCK_TFT();
    tft.fillRect(10, 45, 300, 165, C_BACKGROUND);
    
    const char* opts[] = { 
        "Brilho da Tela", "Sons do Sistema", "Auto-Desligamento",
        "Unidades / Units", "Tema do Sistema", "Modo Noturno",
        "Grade em Graficos", "Animacoes UI", "Auto-Salvar Historico",
        "Confirmar Acoes", "Modo Especialista", "Idioma",
        "Beep Forte", "Calibracao Probe", "SD: Recarregar",
        "Limpar Historico", "Reset de Fabrica", "Informacoes CPU"
    };
    
    const char* themeNames[] = { "Verde", "Azul", "Laranja", "Roxo" };
    const char* languageNames[] = { "PT-BR", "EN", "ES" };
    
    char bVal[16], sVal[16], aVal[16], lVal[16];
    sprintf(bVal, "%d%%", (deviceSettings.backlight * 100) / 255);
    strcpy(sVal, deviceSettings.soundEnabled ? "LIGADO" : "MUDO");
    if (!deviceSettings.autoSleep) strcpy(aVal, "DESL.");
    else if (deviceSettings.autoSleepMs <= 60000UL) strcpy(aVal, "1 min");
    else if (deviceSettings.autoSleepMs <= 300000UL) strcpy(aVal, "5 min");
    else strcpy(aVal, "15 min");
    strcpy(lVal, languageNames[deviceSettings.languageIdx % 3]);

    const char* vals[] = { 
        bVal, 
        sVal, 
        aVal, 
        deviceSettings.unitsMetric ? "Metrico" : "Imperial",
        themeNames[deviceSettings.themeIdx % 4],
        deviceSettings.darkMode ? "DARK" : "LIGHT",
        deviceSettings.showGrid ? "ON" : "OFF",
        deviceSettings.animations ? "ON" : "OFF",
        deviceSettings.autoSaveHistory ? "ON" : "OFF",
        deviceSettings.confirmActions ? "ON" : "OFF",
        deviceSettings.expertMode ? "ON" : "OFF",
        lVal,
        deviceSettings.strongBeep ? "FORTE" : "NORMAL",
        deviceSettings.calibrated ? "OK" : "PEND",
        sdCardPresent ? "OK" : "SCAN",
        "CLEAR", "WARN", "VER"
    };
    
    int16_t y_start = 55 + uiScrollY;
    
    for(int i=0; i<SETTINGS_ITEM_COUNT; i++) {
        int16_t y = y_start + i * SETTINGS_ITEM_HEIGHT;
        if (y < 10 || y > 210) continue; 
        
        uint16_t boxColor = 0x1082;
        uint16_t borderColor = C_DIVIDER;
        
        tft.fillRoundRect(20, y, 280, 40, 6, boxColor);
        tft.drawRoundRect(20, y, 280, 40, 6, borderColor);
        
        draw_text_5x7(tft, 35, y + 15, opts[i], TFT_WHITE, 1);
        draw_text_5x7(tft, 285 - (strlen(vals[i])*6), y + 15, vals[i], COLOR_PRIMARY, 1);
    }
    
    // Indicador de Scroll
    tft.fillRect(310, 50, 4, 150, C_DIVIDER);
    int totalContent = SETTINGS_ITEM_COUNT * SETTINGS_ITEM_HEIGHT;
    int barH = (SETTINGS_VISIBLE_HEIGHT * 150) / totalContent;
    if (barH < 20) barH = 20;
    if (barH > 150) barH = 150;
    int scrollAbs = -uiScrollY;
    if (scrollAbs < 0) scrollAbs = 0;
    if (scrollAbs > SETTINGS_SCROLL_MAX) scrollAbs = SETTINGS_SCROLL_MAX;
    int barY = 50 + ((SETTINGS_SCROLL_MAX > 0) ? (scrollAbs * (150 - barH) / SETTINGS_SCROLL_MAX) : 0);
    tft.fillRect(310, barY, 4, barH, COLOR_PRIMARY);
    
    UNLOCK_TFT();
}

static void draw_history_screen() {
    LOCK_TFT();
    tft.fillRoundRect(15, 50, 290, 155, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(15, 50, 290, 155, V_RADIUS_LG, V_CYAN_ELECTRIC);
    
    HistoryItem history[5];
    uint8_t count = logger_get_recent(history, 5);
    
    if (count == 0) {
        draw_text_5x7(tft, 80, 120, "NENHUM REGISTRO ENCONTRADO", V_TEXT_SUB, 1);
    } else {
        for(int i=0; i<count; i++) {
            int16_t y = 65 + i * 28;
            char line[64];
            snprintf(line, sizeof(line), "%s: %.2f %s", history[i].componentName, history[i].value, history[i].unit);
            draw_text_5x7(tft, 28, y, line, V_TEXT_MAIN, 1);
            tft.drawFastHLine(25, y + 15, 270, V_DIVIDER);
        }
    }
    
    // Botão Limpar (Subido para não conflitar com footer)
    tft.fillRoundRect(100, 208, 120, 18, 6, V_ALERT);
    draw_text_5x7(tft, 128, 214, "LIMPAR TUDO", V_PURE_WHITE, 1);
    
    UNLOCK_TFT();
}

bool ui_handle_touch(uint16_t x, uint16_t y) {
    lastActivityMs = millis();
    if (!backlightOn) {
        backlight_on();
        return true; 
    }

    // Telas da v5.0: elas tratam a propria area util. O canto superior
    // esquerdo continua sendo o botao de voltar em todas.
    if (screens_handles((AppState)currentAppState)) {
        if (x < 40 && y < 40) {
            buzzer_click();
            currentAppState = STATE_MENU;
            return true;
        }
        if (screens_touch((AppState)currentAppState, x, y)) return true;
        return false;
    }

    // Botão Voltar (Sempre disponível fora do menu principal)
    if (!isMenuMode || currentAppState == STATE_SUBMENU_TEMP || currentAppState == STATE_SUBMENU_MAIS) {
        if (x < 40 && y < 40) {
            buzzer_click();
            if (currentAppState == STATE_HELP) {
                currentAppState = (AppState)previousAppState; // Volta para o instrumento
            } else {
                currentAppState = STATE_MENU;
            }
            return true;
        }
        
        // Botão Ajuda "i" (Canto superior direito) apenas em telas de medição
        if (is_help_available_state((AppState)currentAppState)) {
            if (x > 280 && y < 40) {
                buzzer_click();
                previousAppState = (AppState)currentAppState; // Salva o estado atual para voltar
                currentAppState = STATE_HELP;
                return true;
            }
        }

        // Botões Testar Novamente / Salvar (Teste Auto)
        if (currentAppState == STATE_MEASURE_GENERIC && y > 200) {
            if (x < 160) {
                buzzer_click();
                measurements_run_full_test();
                needsScreenRedraw = true;
                return true;
            } else {
                // SALVAR: agora grava mesmo. Antes so mexia num array em RAM
                // que se perdia no proximo boot, apesar de o manual prometer
                // historico no cartao SD.
                AnalysisResult r = measurements_get_last_result();
                if (!r.valid) {
                    widget_toast("Nada para salvar", TOAST_WARNING);
                    return true;
                }

                float value = r.resistance;
                switch (r.type) {
                    case COMP_CAPACITOR:
                    case COMP_CAPACITOR_CERAMIC:
                    case COMP_CAPACITOR_ELECTRO: value = r.capacitance; break;
                    case COMP_INDUCTOR:          value = r.inductance; break;
                    case COMP_DIODE:
                    case COMP_LED:               value = r.forwardVoltage; break;
                    case COMP_TRANSISTOR_NPN:
                    case COMP_TRANSISTOR_PNP:    value = r.gain; break;
                    default: break;
                }

                bool saved = logger_write_result(r.label, value,
                                                 analysis_type_unit(r.type),
                                                 r.status);
                update_recent_tests(r.label, value, db_status_string(r.status));

                widget_toast(saved ? "Medicao salva no cartao"
                                   : "Sem cartao SD: salvo so na memoria",
                             saved ? TOAST_SUCCESS : TOAST_WARNING);
                needsScreenRedraw = true;
                return true;
            }
        }

        // Botão Calibrar Agora
        if (currentAppState == STATE_CALIBRATION &&
            x >= 70 && x <= 250 && y >= 200 && y <= 236) {
            buzzer_click();
            // Antes isto apenas copiava a ultima leitura para os offsets, o
            // que gravava como "erro das pontas" o valor do componente que
            // estivesse conectado. Agora roda a rotina guiada de verdade.
            calibration_run_auto();
            probeOffsetResistance  = analysis_get_resistance_offset();
            probeOffsetCapacitance = analysis_get_capacitance_offset();
            needsScreenRedraw = true;
            return true;
        }

        // Botao Descarregar (tela do capacimetro)
        if (currentAppState == STATE_MEASURE_CAPACITOR &&
            x >= 10 && x <= 170 && y >= 200 && y <= 238) {
            buzzer_click();
            widget_toast("Descarregando...", TOAST_INFO, 1500);
            measurements_discharge_capacitor();
            needsScreenRedraw = true;
            return true;
        }

        // Botão Salvar Referência (Comparador)
        if (currentAppState == STATE_COMPARATOR && x >= 70 && x <= 250 && y >= 200 && y <= 238) {
            buzzer_click();
            referenceComp.value = lastResistance + lastCapacitance + lastVoltage;
            referenceComp.type = COMP_GENERIC; // Simplificado
            strcpy(referenceComp.name, "Ref. Manual");
            referenceComp.isSet = true;
            needsScreenRedraw = true;
            return true;
        }

        // Alternar Modos do Multímetro (Toque nas caixas de modo ou valor)
        if (currentAppState == STATE_MULTIMETER) {
            if (y > 150 || (x > 85 && y > 52 && y < 157)) {
                buzzer_click();
                MultimeterMode m = multimeter_get_mode();
                int nextMode = (int)m + 1;
                if (nextMode > 5) nextMode = 0;
                multimeter_set_mode((MultimeterMode)nextMode);
                needsScreenRedraw = true;
                return true;
            }
        }

        // Limpar Histórico
        if (currentAppState == STATE_HISTORY && x >= 80 && x <= 240 && y >= 200 && y <= 236) {
            buzzer_click();
            logger_clear();
            memset(recentTests, 0, sizeof(recentTests));
            needsScreenRedraw = true;
            return true;
        }
    }

    // Captura inicial para Tap vs Swipe em qualquer tela
    if (!isTouching) {
        touchStartTime = millis();
        touchStartX = x;
        touchStartY = y;
        lastTouchX = x;
        lastTouchY = y;
        isTouching = true;
        hasMoved = false;
        return true;
    } else {
        // Threshold aumentado para 20px para evitar que tremores cancelem o clique
        if (abs((int16_t)x - (int16_t)touchStartX) > 20 || abs((int16_t)y - (int16_t)touchStartY) > 20) {
            hasMoved = true;
        }

        if (isDialogActive) {
            // Apenas bloqueia interação com o fundo, o processamento real ocorre no release
            return true;
        }
        
        // Rolagem Genérica (Ajustes, Ajuda, Calibração se necessário)
        if ((currentAppState == STATE_SETTINGS || currentAppState == STATE_HELP) && hasMoved) {
            int16_t dy = (int16_t)y - (int16_t)lastTouchY;
            uiScrollY += dy;
            
            // Limites dependentes do estado
            if (uiScrollY > 0) uiScrollY = 0;
            if (currentAppState == STATE_SETTINGS) {
                if (uiScrollY < -SETTINGS_SCROLL_MAX) uiScrollY = -SETTINGS_SCROLL_MAX;
            } else { // STATE_HELP
                if (uiScrollY < -100) uiScrollY = -100;
            }
            needsScreenRedraw = true;
        }
        
        lastTouchX = x;
        lastTouchY = y;
    }

    return true;
}

void ui_reset_touch_state() {
    if (isTouching) {
        uint32_t duration = millis() - touchStartTime;
        int16_t deltaX = (int16_t)lastTouchX - (int16_t)touchStartX;

        if (isDialogActive) {
            // Lógica de botões do diálogo (Sim/Não) no release
            if (lastTouchY > 140 && lastTouchY < 180) {
                if (lastTouchX > 40 && lastTouchX < 150) { // NÃO
                    isDialogActive = false;
                    needsScreenRedraw = true;
                } else if (lastTouchX > 170 && lastTouchX < 280) { // SIM
                    if (dialogTargetIdx == 15) {
                        logger_clear();
                        memset(recentTests, 0, sizeof(recentTests));
                        Serial.println("HISTORICO APAGADO!");
                    }
                    if (dialogTargetIdx == 16) {
                        // Reset de fábrica (limpa NVS se necessário)
                        ESP.restart();
                    }
                    isDialogActive = false;
                    needsScreenRedraw = true;
                }
            }
            isTouching = false;
            return;
        }
        if (isMenuMode) {
            if (abs(deltaX) > 50 && duration < 500) {
                if (deltaX < -50) menu_scroll(1);
                else if (deltaX > 50) menu_scroll(-1);
            } 
            else if (!hasMoved) {
                menu_handle_touch(lastTouchX, lastTouchY);
            }
        }
        else if (currentAppState == STATE_SETTINGS) {
            if (!hasMoved) {
                // Calcula qual item foi clicado baseado no Scroll
                // Header acaba em 40, itens começam em 55 + scroll
                int16_t relativeY = lastTouchY - (55 + uiScrollY);
                int8_t itemIdx = relativeY / SETTINGS_ITEM_HEIGHT;
                
                if (itemIdx >= 0 && itemIdx < SETTINGS_ITEM_COUNT) {
                    buzzer_click();
                    
                    // Feedback visual: Pisca o item
                    LOCK_TFT();
                    tft.drawRoundRect(20, 55 + uiScrollY + itemIdx * SETTINGS_ITEM_HEIGHT, 280, 40, 6, TFT_WHITE);
                    UNLOCK_TFT();
                    delay(80);
                    
                    // Ações específicas
                    switch(itemIdx) {
                        case 0: // Brilho em passos de 20%
                            if (deviceSettings.backlight >= 255) deviceSettings.backlight = 51;
                            else deviceSettings.backlight += 51; 
                            hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, deviceSettings.backlight);
                            break;
                        case 1: // Som
                            deviceSettings.soundEnabled = !deviceSettings.soundEnabled;
                            break;
                        case 2: // Auto-Desligamento: OFF -> 1min -> 5min -> 15min -> OFF
                            if (!deviceSettings.autoSleep) {
                                deviceSettings.autoSleep = true;
                                deviceSettings.autoSleepMs = 60000UL;
                            } else if (deviceSettings.autoSleepMs <= 60000UL) {
                                deviceSettings.autoSleepMs = 300000UL;
                            } else if (deviceSettings.autoSleepMs <= 300000UL) {
                                deviceSettings.autoSleepMs = 900000UL;
                            } else {
                                deviceSettings.autoSleep = false;
                                deviceSettings.autoSleepMs = 0;
                            }
                            break;
                        case 3: // Unidades
                            deviceSettings.unitsMetric = !deviceSettings.unitsMetric;
                            break;
                        case 4: // Tema
                            deviceSettings.themeIdx = (deviceSettings.themeIdx + 1) % 4;
                            apply_theme(deviceSettings.themeIdx);
                            break;
                        case 5: // Modo Noturno
                            deviceSettings.darkMode = !deviceSettings.darkMode;
                            colors_update();
                            break;
                        case 6: // Grade
                            deviceSettings.showGrid = !deviceSettings.showGrid;
                            break;
                        case 7: // Animacoes
                            deviceSettings.animations = !deviceSettings.animations;
                            break;
                        case 8: // Auto salvar historico
                            deviceSettings.autoSaveHistory = !deviceSettings.autoSaveHistory;
                            break;
                        case 9: // Confirmar acoes
                            deviceSettings.confirmActions = !deviceSettings.confirmActions;
                            break;
                        case 10: // Modo especialista
                            deviceSettings.expertMode = !deviceSettings.expertMode;
                            break;
                        case 11: // Idioma
                            deviceSettings.languageIdx = (deviceSettings.languageIdx + 1) % 3;
                            break;
                        case 12: // Beep forte
                            deviceSettings.strongBeep = !deviceSettings.strongBeep;
                            break;
                        case 13: // Calibração
                            currentAppState = STATE_CALIBRATION;
                            break;
                        case 14: // SD Scan
                            sdCardError = !logger_init();
                            break;
                        case 15: // Limpar Histórico
                            if (deviceSettings.confirmActions) {
                                isDialogActive = true;
                                dialogTargetIdx = 15;
                                draw_confirmation_dialog("Limpar todo o historico?");
                            } else {
                                logger_clear();
                                memset(recentTests, 0, sizeof(recentTests));
                            }
                            break;
                        case 16: // Reset
                            if (deviceSettings.confirmActions) {
                                isDialogActive = true;
                                dialogTargetIdx = 16;
                                draw_confirmation_dialog("RESTAURAR PADRAO DE FABRICA?");
                            } else {
                                ESP.restart();
                            }
                            break;
                        case 17: // CPU Info
                            currentAppState = STATE_STATS;
                            needsScreenRedraw = true;
                            return;
                        default: 
                            break;
                    }
                    
                    if (!isDialogActive) needsScreenRedraw = true;
                }
            }
        }
    }
    isTouching = false;
    hasMoved = false;
}

void ui_calibration_update_progress(uint8_t progress, const char* msg) {
    LOCK_TFT();
    tft.fillRoundRect(20, 160, 280, 45, V_RADIUS_MD, V_BG_SURFACE);
    char buf[32];
    sprintf(buf, "CALIBRANDO: %d%%", progress);
    draw_text_5x7(tft, 35, 168, buf, V_PURE_WHITE, 1);
    draw_text_5x7(tft, 35, 185, msg, V_CYAN_ELECTRIC, 1);
    UNLOCK_TFT();
}

void ui_calibration_show_result(bool success, const char* msg) {
    LOCK_TFT();
    uint16_t resCol = success ? V_NEON_GREEN : V_ALERT;
    tft.fillRoundRect(20, 100, 280, 85, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(20, 100, 280, 85, V_RADIUS_LG, resCol);
    
    draw_text_5x7(tft, 160 - (strlen(success ? "SUCESSO!" : "FALHA!") * 6), 120, success ? "SUCESSO!" : "FALHA!", resCol, 2);
    draw_text_5x7(tft, 40, 155, msg, V_TEXT_MAIN, 1);
    UNLOCK_TFT();
    delay(3000);
}

// ============================================================================
// CONTROLE DE BACKLIGHT E ENERGIA
// ============================================================================
static void backlight_update() {
    if (backlightOn && (millis() - lastActivityMs > 60000)) { // 60 segundos
        hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, BACKLIGHT_DIM_LEVEL); // Desliga via PWM
        backlightOn = false;
    }
}

static void backlight_on() {
    hal_pwm_write(PIN_TFT_BL, LEDC_CH_BACKLIGHT, deviceSettings.backlight);
    backlightOn = true;
    lastActivityMs = millis();
}

static void draw_confirmation_dialog(const char* msg) {
    LOCK_TFT();
    // Blur simulado (mais agressivo)
    for(int y=0; y<240; y+=3) {
        tft.drawFastHLine(0, y, 320, 0x0000);
    }
    
    tft.fillRoundRect(30, 70, 260, 125, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(30, 70, 260, 125, V_RADIUS_LG, V_CYAN_ELECTRIC);
    
    // Icone de Aviso
    draw_text_5x7(tft, 145, 85, "!", V_WARNING, 3);
    
    draw_text_5x7(tft, 160 - (strlen(msg)*3), 115, msg, V_TEXT_MAIN, 1);
    
    // Botão NÃO
    tft.fillRoundRect(50, 145, 100, 35, 6, V_ALERT);
    draw_text_5x7(tft, 85, 158, "NAO", V_PURE_WHITE, 1);
    
    // Botão SIM
    tft.fillRoundRect(170, 145, 100, 35, 6, V_NEON_GREEN);
    draw_text_5x7(tft, 205, 158, "SIM", V_BG_DARK, 1);
    
    UNLOCK_TFT();
}

static void draw_cpu_info_screen() {
    // Tela de diagnostico: resultado do autoteste + saude do sistema.
    // A versao anterior anunciava "PROCESSADOR: ESP32-S3 CORE" numa placa que
    // usa um ESP32 classico, e a temperatura do die vinha de uma funcao
    // interna sem calibracao, convertida com a formula errada.
    diag_update();
    SystemHealth h = diag_get_health();

    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    UNLOCK_TFT();

    graphics_draw_header("DIAGNOSTICO DO SISTEMA");

    // ---------------------------------------------------------------- coluna 1
    LOCK_TFT();
    tft.fillRoundRect(10, 46, 150, 150, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(10, 46, 150, 150, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 18, 52, "AUTOTESTE", V_NEON_GREEN, 1);
    tft.drawLine(18, 62, 152, 62, V_DIVIDER);

    for (uint8_t i = 0; i < TEST_COUNT && i < 9; i++) {
        const SelfTestEntry* e = diag_selftest_get((SelfTestItem)i);
        if (!e) continue;

        int16_t ty = 68 + i * 14;
        draw_text_5x7(tft, 18, ty, diag_item_name((SelfTestItem)i),
                      V_TEXT_MAIN, 1);

        const char* label = diag_result_name(e->result);
        int16_t lw = (int16_t)strlen(label) * 6;
        draw_text_5x7(tft, 152 - lw, ty, label,
                      diag_result_color(e->result), 1);
    }
    UNLOCK_TFT();

    // ---------------------------------------------------------------- coluna 2
    LOCK_TFT();
    tft.fillRoundRect(166, 46, 144, 150, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(166, 46, 144, 150, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 174, 52, "SISTEMA", V_CYAN_ELECTRIC, 1);
    tft.drawLine(174, 62, 302, 62, V_DIVIDER);

    char buf[48];
    int16_t y = 68;

    snprintf(buf, sizeof(buf), "%s rev %u", ESP.getChipModel(),
             (unsigned)ESP.getChipRevision());
    draw_text_5x7(tft, 174, y, buf, V_TEXT_MAIN, 1); y += 13;

    snprintf(buf, sizeof(buf), "CPU %u MHz", (unsigned)h.cpuFreqMhz);
    draw_text_5x7(tft, 174, y, buf, V_TEXT_MAIN, 1); y += 13;

    snprintf(buf, sizeof(buf), "Flash %lu MB",
             (unsigned long)(ESP.getFlashChipSize() / (1024UL * 1024UL)));
    draw_text_5x7(tft, 174, y, buf, V_TEXT_MAIN, 1); y += 13;

    snprintf(buf, sizeof(buf), "Heap %lu KB",
             (unsigned long)(h.freeHeap / 1024));
    draw_text_5x7(tft, 174, y, buf,
                  h.heapWarning ? V_ALERT : V_TEXT_MAIN, 1); y += 13;

    snprintf(buf, sizeof(buf), "Minimo %lu KB",
             (unsigned long)(h.minFreeHeap / 1024));
    draw_text_5x7(tft, 174, y, buf, V_TEXT_SUB, 1); y += 13;

    // Pilha em palavras de 4 bytes; abaixo de ~350 ha risco de estouro.
    snprintf(buf, sizeof(buf), "Pilha UI %lu",
             (unsigned long)h.uiStackFree);
    draw_text_5x7(tft, 174, y, buf,
                  h.stackWarning ? V_WARNING : V_TEXT_SUB, 1); y += 13;

    snprintf(buf, sizeof(buf), "Pilha med %lu",
             (unsigned long)h.measureStackFree);
    draw_text_5x7(tft, 174, y, buf,
                  h.stackWarning ? V_WARNING : V_TEXT_SUB, 1); y += 13;

    char up[20];
    analysis_format_duration(h.uptimeMs, up, sizeof(up));
    snprintf(buf, sizeof(buf), "Ligado ha %s", up);
    draw_text_5x7(tft, 174, y, buf, V_TEXT_SUB, 1); y += 13;

    UsageStats u = diag_get_usage();
    snprintf(buf, sizeof(buf), "%lu medicoes",
             (unsigned long)u.totalMeasurements);
    draw_text_5x7(tft, 174, y, buf, V_TEXT_SUB, 1);
    UNLOCK_TFT();

    // ------------------------------------------------------- barra de memoria
    UNLOCK_TFT();
    widget_progress_labeled(10, 200, 300, "Uso de memoria RAM",
                            h.heapUsedPct,
                            h.heapWarning ? V_ALERT : V_NEON_GREEN);
    LOCK_TFT();
    UNLOCK_TFT();
}

void draw_calibration_screen() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("CALIBRACAO PROBES");
    
    tft.fillRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_CYAN_ELECTRIC);
    
    draw_text_5x7(tft, 35, 75, "ESTADO: AGUARDANDO", V_WARNING, 1);
    
    char buf[64];
    sprintf(buf, "OFFSET R: %.2f Ohm", probeOffsetResistance);
    draw_text_5x7(tft, 35, 100, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "OFFSET C: %.2f uF", probeOffsetCapacitance);
    draw_text_5x7(tft, 35, 120, buf, V_TEXT_MAIN, 1);
    
    draw_text_5x7(tft, 35, 150, "Curte-circuite as pontas", V_TEXT_SUB, 1);
    draw_text_5x7(tft, 35, 162, "para calibrar Resistência.", V_TEXT_SUB, 1);
    
    // Botão Calibrar (Subido para não conflitar com footer)
    tft.fillRoundRect(80, 208, 160, 18, 6, V_CYAN_ELECTRIC);
    draw_text_5x7(tft, 115, 214, "CALIBRAR AGORA", V_BG_DARK, 1);
    
    UNLOCK_TFT();
}

static void draw_thermal_probe_screen() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("PONTEIRA TERMICA");
    
    tft.fillRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_CYAN_ELECTRIC);
    
    char buf[64];
    sprintf(buf, "%.1f", lastTemperature);
    int16_t w = strlen(buf) * 6 * 6;
    draw_text_5x7(tft, 160 - w/2, 100, buf, V_VIBRANT_PURPLE, 6);
    draw_text_5x7(tft, 160 + w/2 + 5, 110, "C", V_TEXT_SUB, 2);
    
    draw_text_5x7(tft, 80, 180, "SENSOR DS18B20 CONECTADO", V_NEON_GREEN, 1);
    
    UNLOCK_TFT();
}

static void apply_theme(uint8_t idx) {
    uint16_t colors[] = { 0x07E0, 0x001F, 0xFD20, 0x780F }; // Verde, Azul, Laranja, Roxo
    deviceSettings.themeColor = colors[idx];
    colors_update();
}

static void draw_comparator_screen() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("COMPARADOR PRO");

    // Box Referência (Esquerda)
    tft.fillRoundRect(12, 55, 142, 115, V_RADIUS_MD, V_BG_SURFACE);
    uint16_t refCol = referenceComp.isSet ? V_CYAN_ELECTRIC : V_TEXT_SUB;
    tft.drawRoundRect(12, 55, 142, 115, V_RADIUS_MD, refCol);
    
    draw_text_5x7(tft, 35, 65, "REFERENCIA", V_TEXT_SUB, 1);
    
    if (referenceComp.isSet) {
        char buf[16];
        sprintf(buf, "%.2f", referenceComp.value);
        // Valor com sombra
        draw_text_5x7(tft, 32, 102, buf, 0x0000, 3);
        draw_text_5x7(tft, 30, 100, buf, V_CYAN_ELECTRIC, 3);
        
        draw_text_5x7(tft, 35, 145, referenceComp.name, V_TEXT_SUB, 1);
    } else {
        draw_text_5x7(tft, 45, 100, "VAZIO", V_TEXT_SUB, 2);
    }

    // Box Medição Atual (Direita)
    tft.fillRoundRect(166, 55, 142, 115, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(166, 55, 142, 115, V_RADIUS_MD, V_NEON_GREEN);
    draw_text_5x7(tft, 185, 65, "TESTE ATUAL", V_TEXT_SUB, 1);
    
    float currentVal = lastResistance + lastCapacitance + lastVoltage;
    char valBuf[16];
    sprintf(valBuf, "%.2f", currentVal);
    // Valor com sombra
    draw_text_5x7(tft, 182, 102, valBuf, 0x0000, 3);
    draw_text_5x7(tft, 180, 100, valBuf, V_NEON_GREEN, 3);

    // Resultado da Comparação
    tft.fillRoundRect(12, 180, 296, 48, V_RADIUS_LG, V_BG_HIGHLIGHT);
    if (!referenceComp.isSet) {
        draw_text_5x7(tft, 60, 198, "SALVE UMA REFERENCIA PRIMEIRO", V_WARNING, 1);
    } else {
        float diff = ((currentVal - referenceComp.value) / referenceComp.value) * 100.0f;
        if (abs(diff) < 5.0f) {
            draw_text_5x7(tft, 95, 195, "EQUIVALENTE (OK)", V_SUCCESS, 1);
            tft.drawRoundRect(12, 180, 296, 48, V_RADIUS_LG, V_SUCCESS);
            led_status_good();
        } else {
            const char* msg = (diff > 5.0f) ? "VALOR MAIOR (+)" : "VALOR MENOR (-)";
            uint16_t col = (diff > 5.0f) ? V_CYAN_ELECTRIC : V_ALERT;
            draw_text_5x7(tft, 105, 195, msg, col, 1);
            tft.drawRoundRect(12, 180, 296, 48, V_RADIUS_LG, col);
            if (diff > 5.0f) led_status_working(); else led_status_bad();
        }
    }

    // Botão Salvar Referência
    tft.fillRoundRect(80, 208, 160, 18, 4, V_CYAN_ELECTRIC);
    draw_text_5x7(tft, 102, 213, "SALVAR COMO REFERENCIA", V_BG_DARK, 1);

    UNLOCK_TFT();
}
static void draw_thermal_camera_screen() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("CAMERA TERMICA PRO");
    
    // Área da Câmera (Layout Modernizado)
    tft.fillRoundRect(8, 48, 244, 184, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(8, 48, 244, 184, V_RADIUS_MD, V_BG_HIGHLIGHT);
    
    // Simulação de Mapa de Calor com interpolação visual sutil
    for(int y=0; y<18; y++) {
        for(int x=0; x<24; x++) {
            // Efeito degradê simulado
            uint8_t heat = (x * 10) + (y * 4);
            uint16_t simColor = color_mix(V_ALERT, V_DEEP_BLUE, 255 - heat);
            tft.fillRect(10 + x*10, 50 + y*10, 10, 10, simColor);
        }
    }
    
    // Barra Lateral de Escala Neon
    for(int i=0; i<180; i++) {
        uint16_t c = color_mix(V_ALERT, V_DEEP_BLUE, (i * 255) / 180);
        tft.drawFastHLine(262, 50 + i, 12, c);
    }
    draw_text_5x7(tft, 280, 50, "80C", V_ALERT, 1);
    draw_text_5x7(tft, 280, 220, "20C", V_CYAN_ELECTRIC, 1);
    
    // Mira Central com Glow
    tft.drawFastHLine(120, 140, 20, V_PURE_WHITE);
    tft.drawFastVLine(130, 130, 20, V_PURE_WHITE);
    // Valor Central com destaque
    draw_text_5x7(tft, 137, 157, "32.4 C", 0x0000, 1);
    draw_text_5x7(tft, 135, 155, "32.4 C", V_PURE_WHITE, 1);
    
    UNLOCK_TFT();
}

static void draw_safety_alert_screen() {
    LOCK_TFT();
    tft.fillScreen(0x8000); // Fundo bordô escuro
    
    // Glow Vermelho pulsante (estático aqui)
    for(int i=0; i<5; i++) {
        tft.drawRoundRect(10-i, 10-i, 300+i*2, 220+i*2, V_RADIUS_LG, V_ALERT);
    }
    
    // Triangulo de Alerta Neon
    tft.fillTriangle(160, 40, 90, 150, 230, 150, V_PURE_WHITE);
    tft.fillTriangle(160, 55, 105, 140, 215, 140, V_ALERT);
    draw_text_5x7(tft, 152, 92, "!", V_PURE_WHITE, 5);
    
    draw_text_5x7(tft, 42, 172, "ALTA TENSAO DETECTADA!", 0x0000, 2);
    draw_text_5x7(tft, 40, 170, "ALTA TENSAO DETECTADA!", V_PURE_WHITE, 2);
    
    draw_text_5x7(tft, 45, 200, "Desconecte as ponteiras imediatamente.", V_TEXT_MAIN, 1);
    
    tft.fillRoundRect(100, 220, 120, 30, 6, V_PURE_WHITE);
    draw_text_5x7(tft, 125, 230, "OK / CIENTE", V_ALERT, 1);
    
    UNLOCK_TFT();
}

static void draw_help_screen() {
    LOCK_TFT();
    const HelpInfo* info = help_get_for_state((AppState)previousAppState);
    
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("MANUAL DE CONEXAO");
    
    if (!info) {
        draw_text_5x7(tft, 60, 120, "SEM AJUDA PARA ESTE MODO", V_TEXT_SUB, 1);
        UNLOCK_TFT();
        return;
    }

    tft.fillRoundRect(10, 50, 300, 160, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(10, 50, 300, 160, V_RADIUS_LG, V_CYAN_ELECTRIC);

    // Área de Clippagem simulada para scroll
    int16_t contentY = 62 + uiScrollY;
    
    // Titulo da Ajuda Neon
    if (contentY > 40) {
        draw_text_5x7(tft, 25, contentY, info->title, V_CYAN_ELECTRIC, 2);
        tft.drawLine(20, contentY + 23, 290, contentY + 23, V_DIVIDER);
    }
    
    // Box dos Probes (Layout Visual)
    int16_t probesY = 95 + uiScrollY;
    if (probesY > 40 && probesY < 210) {
        tft.fillRoundRect(215, probesY, 85, 65, V_RADIUS_MD, V_BG_HIGHLIGHT);
        const char* probeLabel = "PROBES";
        IconType hIcon = ICON_AUTO;
        switch ((AppState)previousAppState) {
            case STATE_MEASURE_RESISTOR:   probeLabel = "RES"; hIcon = ICON_RESISTOR; break;
            case STATE_MEASURE_CAPACITOR:  probeLabel = "CAP"; hIcon = ICON_CAPACITOR; break;
            case STATE_MEASURE_DIODE:      probeLabel = "DIO"; hIcon = ICON_DIODE; break;
            case STATE_MEASURE_LED:        probeLabel = "LED"; hIcon = ICON_LED; break;
            case STATE_MEASURE_TRANSISTOR: probeLabel = "BJT"; hIcon = ICON_TRANSISTOR_NPN; break;
            case STATE_MEASURE_INDUCTOR:   probeLabel = "IND"; hIcon = ICON_INDUCTOR; break;
            case STATE_MULTIMETER:         probeLabel = "MULT"; hIcon = ICON_MULTIMETER; break;
            case STATE_THERMAL_PROBE:
            case STATE_THERMAL_CAMERA:     probeLabel = "TEMP"; hIcon = ICON_TEMP; break;
            default:                       probeLabel = "AUTO"; hIcon = ICON_AUTO; break;
        }
        draw_text_5x7(tft, 228, probesY + 7, probeLabel, V_TEXT_SUB, 1);
        draw_bitmap_icon(hIcon, 241, probesY + 20);
        
        // Desenho dos terminais 1, 2, 3
        tft.fillCircle(235, probesY + 40, 10, V_ALERT);         draw_text_5x7(tft, 232, probesY + 37, "1", V_PURE_WHITE, 1);
        tft.fillCircle(257, probesY + 40, 10, V_CYAN_ELECTRIC); draw_text_5x7(tft, 254, probesY + 37, "2", V_BG_DARK, 1);
        tft.fillCircle(279, probesY + 40, 10, V_VIBRANT_PURPLE); draw_text_5x7(tft, 276, probesY + 37, "3", V_PURE_WHITE, 1);
    }
    
    // Conteudo Organizaddo (com scroll)
    int16_t y_items = 100 + uiScrollY;
    const char* labels[] = { "CONEXAO:", "PROBES:", "POLAR.:", "DICA:" };
    const char* values[] = { info->connection, info->probes, info->polarity, info->tips };
    
    for(int i=0; i<4; i++) {
        if (y_items > 40 && y_items < 210) {
            draw_text_5x7(tft, 20, y_items, labels[i], V_CYAN_ELECTRIC, 1);
            draw_text_5x7(tft, 20, y_items + 12, values[i], V_TEXT_MAIN, 1);
        }
        y_items += 28;
    }
    
    // Footer instrucao
    tft.fillRect(10, 215, 300, 20, V_BG_HIGHLIGHT);
    draw_text_5x7(tft, 80, 222, "PRESSIONE VOLTAR PARA MEDIR", V_NEON_GREEN, 1);
    
    UNLOCK_TFT();
}
