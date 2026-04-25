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
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

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

static bool is_help_available_state(AppState state) {
    switch (state) {
        case STATE_MEASURE_RESISTOR:
        case STATE_MEASURE_CAPACITOR:
        case STATE_MEASURE_DIODE:
        case STATE_MEASURE_TRANSISTOR:
        case STATE_MEASURE_INDUCTOR:
        case STATE_MEASURE_LED:
        case STATE_MEASURE_GENERIC:
        case STATE_MULTIMETER:
        case STATE_THERMAL_PROBE:
        case STATE_THERMAL_CAMERA:
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
static bool isHelpActive = false;
static bool dialogHandled = false;
static int8_t dialogTargetIdx = -1;
static void draw_confirmation_dialog(const char* msg);
static void draw_help_overlay();
static void draw_cpu_info_screen();
static void apply_theme(uint8_t idx);
static void draw_thermal_camera_screen();
static void draw_safety_alert_screen();
static void draw_help_screen();
void draw_calibration_screen(); // Referenciado em menu.h
static void draw_thermal_probe_screen();
static void draw_comparator_screen();

void ui_init() {
    colors_update();
    // Configura PWM para Backlight - Nova API v3.0+
    ledcAttach(PIN_TFT_BL, 5000, 8); // Pino, Frequência, Resolução
    ledcWrite(PIN_TFT_BL, deviceSettings.backlight);
    backlightOn = true;
    
    // 1. Mostrar Logo Full Screen imediatamente
    draw_logo_full();
    
    // 2. Sequencia de Loading Funcional
    draw_loading_step("Iniciando Kernel...", 10);
    delay(500);
    
    draw_loading_step("Configurando Perifericos...", 25);
    menu_init();
    delay(400);
    
    draw_loading_step("Montando SD Card...", 40);
    delay(400);
    
    draw_loading_step("Verificando Ponteiras...", 55);
    delay(600);
    
    draw_loading_step("Testando Sensor Externo...", 70);
    delay(500);
    
    draw_loading_step("Calibrando ADCs...", 85);
    delay(400);
    
    draw_loading_step("Sistema Pronto!", 100);
    delay(800);

    currentAppState = STATE_MENU;
    lastActivityMs = millis();
}

void ui_update() {
    backlight_update();
    if (!backlightOn) return; // Nao processa UI se tela estiver desligada

    // Detecta mudança de estado para redesenho total
    if (currentAppState != lastKnownState) {
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
    LOCK_TFT();
    char valBuf[32];
    char subBuf1[32] = "";
    char subBuf2[32] = "";
    const char* title = "INSTRUMENTO";
    uint16_t color = V_CYAN_ELECTRIC;
    IconType icon = ICON_UNKNOWN;

    switch (currentAppState) {
        case STATE_MEASURE_RESISTOR:
            title = "OHMMETRO DE PRECISAO";
            color = V_WARNING;
            sprintf(valBuf, "%.2f", lastResistance);
            strcpy(subBuf1, "Tolerancia: +/- 1%");
            strcpy(subBuf2, "Max: 10M Ohms");
            break;
        case STATE_MEASURE_CAPACITOR:
            title = "MEDIDOR DE CAPACITANCIA";
            color = V_CYAN_ELECTRIC;
            sprintf(valBuf, "%.2f", lastCapacitance);
            strcpy(subBuf1, "ESR: 0.12 Ohms");
            strcpy(subBuf2, "Vloss: 0.8%");
            break;
        case STATE_MEASURE_DIODE:
            title = "TESTE DE DIODO / SEMIC.";
            color = V_ALERT;
            sprintf(valBuf, "%.3f", lastVoltage);
            strcpy(subBuf1, "Uf: mV");
            strcpy(subBuf2, "Ir: < 10nA");
            icon = ICON_DIODE;
            break;
        case STATE_MEASURE_LED:
            title = "TESTE DE LED / OPTO";
            color = V_NEON_GREEN;
            sprintf(valBuf, "%.3f", lastVoltage);
            strcpy(subBuf1, "Vf: Volts");
            strcpy(subBuf2, "Intensidade: OK");
            icon = ICON_LED;
            break;
        case STATE_MEASURE_TRANSISTOR:
            title = "ANALISADOR BJT/MOSFET";
            color = V_VIBRANT_PURPLE;
            sprintf(valBuf, "hFE: 245");
            strcpy(subBuf1, "Vbe: 642mV");
            strcpy(subBuf2, "Tipo: NPN (BCE)");
            icon = ICON_TRANSISTOR_NPN;
            break;
        case STATE_MEASURE_INDUCTOR:
            title = "MEDIDOR DE INDUTANCIA";
            color = V_WARNING;
            sprintf(valBuf, "%.2f", lastInductance);
            strcpy(subBuf1, "Q: 4.2 @ 1kHz");
            strcpy(subBuf2, "Rdc: 0.8 Ohms");
            break;
        case STATE_MULTIMETER: {
            MultimeterReading mr = multimeter_get_last_reading();
            title = "MULTIMETRO DIGITAL";
            color = V_CYAN_ELECTRIC;
            
            if (mr.valid) {
                multimeter_format_value(mr.value, valBuf, sizeof(valBuf));
            } else {
                strcpy(valBuf, "---");
            }
            
            const char* mName = "DC AUTO";
            switch(mr.mode) {
                case MMODE_DC_VOLTAGE: mName = "TENSAO DC"; break;
                case MMODE_AC_VOLTAGE: mName = "TENSAO AC"; break;
                case MMODE_DC_CURRENT: mName = "CORRENTE DC"; break;
                case MMODE_RESISTANCE: mName = "RESISTENCIA"; break;
                case MMODE_CONTINUITY: mName = "CONTINUIDADE"; break;
                case MMODE_POWER:      mName = "POTENCIA DC"; break;
            }
            sprintf(subBuf1, "Modo: %s", mName);
            sprintf(subBuf2, "Range: %s", (mr.range == RANGE_AUTO) ? "AUTO" : "MANUAL");
            break;
        }
        case STATE_THERMAL_PROBE:
            title = "TERMOMETRO DE CONTATO";
            color = V_WARNING;
            sprintf(valBuf, "%.1f", lastTemperature);
            strcpy(subBuf1, "Sensor: DS18B20");
            strcpy(subBuf2, "Estabilidade: OK");
            break;
        case STATE_CALIBRATION:
            title = "CALIBRACAO DE PONTAS";
            color = V_WARNING;
            sprintf(valBuf, "CURTO");
            strcpy(subBuf1, "Una as pontas (1 e 2)");
            strcpy(subBuf2, "Aguarde estabilizar...");
            break;
        case STATE_MEASURE_GENERIC:
            title = "TESTE AUTOMATICO";
            color = V_NEON_GREEN;
            if (lastCapacitance > 0.01) {
                sprintf(valBuf, "%.2f", lastCapacitance);
                strcpy(subBuf1, "Capacitor Detectado");
                strcpy(subBuf2, "ESR: 0.12 Ohms");
            } else if (lastResistance > 0.1) {
                sprintf(valBuf, "%.1f", lastResistance);
                strcpy(subBuf1, "Resistor Detectado");
                strcpy(subBuf2, "Tol: +/- 1%");
            } else if (lastVoltage > 0.1) {
                sprintf(valBuf, "%.3f", lastVoltage);
                strcpy(subBuf1, "Diodo Detectado");
                strcpy(subBuf2, "Uf: mV");
            } else {
                sprintf(valBuf, "---");
                strcpy(subBuf1, "Insira o componente");
                strcpy(subBuf2, "Auto-Detec: Ativa");
            }
            break;
        default:
            title = "ANALISANDO...";
            sprintf(valBuf, "---");
            break;
    }

    graphics_draw_header(title);

    // Box de Visualização do Componente (Esquerda do Valor)
    tft.fillRoundRect(12, 55, 65, 95, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(12, 55, 65, 95, V_RADIUS_MD, color);
    
    // Icone
    switch (currentAppState) {
        case STATE_MEASURE_RESISTOR:  icon = ICON_RESISTOR; break;
        case STATE_MEASURE_CAPACITOR: icon = ICON_CAPACITOR; break;
        case STATE_MEASURE_DIODE:     icon = ICON_DIODE; break;
        case STATE_MEASURE_TRANSISTOR: icon = ICON_TRANSISTOR_NPN; break;
        case STATE_MEASURE_INDUCTOR:  icon = ICON_INDUCTOR; break;
        case STATE_MEASURE_LED:       icon = ICON_LED; break;
        case STATE_MULTIMETER: {
            MultimeterReading mr = multimeter_get_last_reading();
            icon = (mr.mode == MMODE_CONTINUITY) ? ICON_CONTINUITY : ICON_MULTIMETER;
            break;
        }
        case STATE_THERMAL_PROBE:     icon = ICON_TEMP; break;
        case STATE_MEASURE_GENERIC:
            if (lastCapacitance > 0.01) icon = ICON_CAPACITOR;
            else if (lastResistance > 0.1) icon = ICON_RESISTOR;
            else if (lastVoltage > 0.1) icon = ICON_DIODE;
            else icon = ICON_AUTO;
            break;
        default:                      icon = ICON_SETTINGS; break;
    }
    draw_bitmap_icon(icon, 28, 87); 

    // Painel Principal (Valor Central)
    tft.fillRoundRect(85, 52, 225, 105, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(85, 52, 225, 105, V_RADIUS_LG, color);
    
    // Valor principal (Escalonamento Inteligente por comprimento)
    int8_t valLen = strlen(valBuf);
    uint8_t fontSize = 5;
    if (valLen > 8) fontSize = 3;
    else if (valLen > 6) fontSize = 4;
    if (currentAppState == STATE_MEASURE_TRANSISTOR) fontSize = 3;
    
    int16_t valWidth = valLen * 6 * fontSize;
    int16_t valX = 197 - valWidth/2;
    int16_t valY = (fontSize >= 5) ? 90 : 96;

    uint16_t valueShadow = color_mix(V_BG_DARK, color, 210);
    draw_text_5x7(tft, valX + 1, valY + 1, valBuf, valueShadow, fontSize);
    draw_text_5x7(tft, valX, valY, valBuf, color, fontSize);
    
    // Unidade (Estilizada)
    const char* unit = "";
    if (currentAppState == STATE_MEASURE_RESISTOR || (currentAppState == STATE_MEASURE_GENERIC && lastResistance > 0.1)) unit = "ohm";
    else if (currentAppState == STATE_MEASURE_CAPACITOR || (currentAppState == STATE_MEASURE_GENERIC && lastCapacitance > 0.01)) unit = "uF";
    else if (currentAppState == STATE_MULTIMETER) {
        MultimeterReading mr = multimeter_get_last_reading();
        unit = mr.unit;
    }
    else if (currentAppState == STATE_MULTIMETER || (currentAppState == STATE_MEASURE_GENERIC && lastVoltage > 0.1)) unit = "V";
    else if (currentAppState == STATE_THERMAL_PROBE) unit = "C";

    if (strlen(unit) > 0) {
        draw_text_5x7(tft, 285, 130, unit, V_TEXT_SUB, 1);
    }

    // Boxes de Informação Secundária
    tft.fillRoundRect(12, 162, 144, 45, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(12, 162, 144, 45, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 22, 180, subBuf1, V_TEXT_SUB, 1);

    tft.fillRoundRect(164, 162, 144, 45, V_RADIUS_MD, V_BG_SURFACE);
    tft.drawRoundRect(164, 162, 144, 45, V_RADIUS_MD, V_DIVIDER);
    draw_text_5x7(tft, 174, 180, subBuf2, V_TEXT_SUB, 1);
    
    // BOTÕES ADICIONAIS PARA TESTE AUTO
    if (currentAppState == STATE_MEASURE_GENERIC && (lastResistance > 0.1 || lastCapacitance > 0.01 || lastVoltage > 0.1)) {
        // Botão Testar Novamente
        tft.fillRoundRect(10, 210, 145, 25, 4, 0x0210);
        tft.drawRoundRect(10, 210, 145, 25, 4, TFT_WHITE);
        draw_text_5x7(tft, 35, 218, "TESTAR NOVAMENTE", TFT_WHITE, 1);
        
        // Botão Salvar
        tft.fillRoundRect(165, 210, 145, 25, 4, C_SUCCESS);
        tft.drawRoundRect(165, 210, 145, 25, 4, TFT_WHITE);
        draw_text_5x7(tft, 205, 218, "SALVAR", TFT_WHITE, 1);
    }
    
    // BOTÃO DE AÇÃO PARA CALIBRAÇÃO
    if (currentAppState == STATE_CALIBRATION) {
        tft.fillRoundRect(80, 210, 160, 25, 4, C_PURPLE);
        tft.drawRoundRect(80, 210, 160, 25, 4, TFT_WHITE);
        draw_text_5x7(tft, 115, 218, "CALIBRAR AGORA", TFT_WHITE, 1);
    }
    
    // OVERLAY DE DESCARGA (Requisito 2)
    if (isDischarging) {
        tft.fillRect(0, 0, 320, 240, 0x0000); // Overlay preto
        tft.fillRoundRect(40, 70, 240, 100, 10, THEME_SURFACE);
        tft.drawRoundRect(40, 70, 240, 100, 10, THEME_CYAN);
        
        draw_text_5x7(tft, 65, 90, "DESCARREGANDO CAPACITOR", THEME_CYAN, 1);
        
        // Barra de progresso profissional
        tft.drawRect(60, 130, 200, 15, THEME_DIVIDER);
        tft.fillRect(62, 132, 196 * dischargeProgress, 11, THEME_CYAN);
        
        char perc[10];
        sprintf(perc, "%d%%", (int)(dischargeProgress * 100));
        draw_text_5x7(tft, 145, 150, perc, THEME_TEXT_HIGH, 1);
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
    draw_text_5x7(tft, 100, 231, "FIRMWARE V4.0", V_TEXT_SUB, 1);
    
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
    sprintf(buf, "Uptime: %02lu:%02lu:%02lu", up/3600, (up%3600)/60, up%60);
    draw_text_5x7(tft, 35, 145, buf, V_TEXT_SUB, 1);
    
    sprintf(buf, "TEMP: %.1f C | VRef: 1.1V", (temprature_sens_read() - 32) / 1.8);
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
        "Calibracao Probe", "SD: Recarregar", "Limpar Historico",
        "Reset de Fabrica", "Informacoes CPU"
    };
    
    const char* themeNames[] = { "Verde", "Azul", "Laranja", "Roxo" };
    
    char bVal[16], sVal[16], aVal[16];
    sprintf(bVal, "%d%%", (deviceSettings.backlight * 100) / 255);
    strcpy(sVal, deviceSettings.soundEnabled ? "LIGADO" : "MUDO");
    strcpy(aVal, deviceSettings.autoSleep ? "5 min" : "DESL.");

    const char* vals[] = { 
        bVal, 
        sVal, 
        aVal, 
        deviceSettings.unitsMetric ? "Metrico" : "Imperial",
        themeNames[deviceSettings.themeIdx % 4],
        deviceSettings.darkMode ? "DARK" : "LIGHT",
        "OK", "SCAN", "CLEAR", "WARN", "VER" 
    };
    
    int16_t y_start = 55 + uiScrollY;
    
    for(int i=0; i<11; i++) {
        int16_t y = y_start + i * 45;
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
    int barH = 30;
    int barY = 50 + (abs(uiScrollY) * (150 - barH) / 330); 
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
            if (x < 160) { // Testar Novamente
                buzzer_click();
                lastResistance = 0; lastCapacitance = 0; lastVoltage = 0;
                needsScreenRedraw = true;
                return true;
            } else { // Salvar
                buzzer_click();
                char name[20];
                if (lastCapacitance > 0.01) strcpy(name, "Capacitor");
                else if (lastResistance > 0.1) strcpy(name, "Resistor");
                else strcpy(name, "Diodo");
                
                update_recent_tests(name, (lastCapacitance+lastResistance+lastVoltage), "BOM");
                needsScreenRedraw = true;
                return true;
            }
        }

        // Botão Calibrar Agora
        if (currentAppState == STATE_CALIBRATION && x >= 70 && x <= 250 && y >= 200 && y <= 236) {
            buzzer_click();
            // Lógica de calibração aqui (ex: zerar offsets)
            probeOffsetResistance = lastResistance;
            probeOffsetCapacitance = lastCapacitance;
            deviceSettings.calibrated = true;
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
                if (uiScrollY < -330) uiScrollY = -330;
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
        int16_t deltaX = lastTouchX - touchStartX;
        int16_t deltaY = lastTouchY - touchStartY;

        if (isDialogActive) {
            // Lógica de botões do diálogo (Sim/Não) no release
            if (lastTouchY > 140 && lastTouchY < 180) {
                if (lastTouchX > 40 && lastTouchX < 150) { // NÃO
                    isDialogActive = false;
                    needsScreenRedraw = true;
                } else if (lastTouchX > 170 && lastTouchX < 280) { // SIM
                    if (dialogTargetIdx == 8) {
                        logger_clear();
                        Serial.println("HISTORICO APAGADO!");
                    }
                    if (dialogTargetIdx == 9) {
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
                int8_t itemIdx = relativeY / 45;
                
                if (itemIdx >= 0 && itemIdx < 11) {
                    buzzer_click();
                    
                    // Feedback visual: Pisca o item
                    LOCK_TFT();
                    tft.drawRoundRect(20, 55 + uiScrollY + itemIdx * 45, 280, 40, 6, TFT_WHITE);
                    UNLOCK_TFT();
                    delay(80);
                    
                    // Ações específicas
                    switch(itemIdx) {
                        case 0: // Brilho em passos de 20%
                            if (deviceSettings.backlight >= 255) deviceSettings.backlight = 51;
                            else deviceSettings.backlight += 51; 
                            ledcWrite(PIN_TFT_BL, deviceSettings.backlight);
                            break;
                        case 1: // Som
                            deviceSettings.soundEnabled = !deviceSettings.soundEnabled;
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
                        case 6: // Calibração
                            currentAppState = STATE_CALIBRATION;
                            break;
                        case 7: // SD Scan
                            sdCardError = !logger_init();
                            break;
                        case 8: // Limpar Histórico
                            isDialogActive = true;
                            dialogTargetIdx = 8;
                            draw_confirmation_dialog("Limpar todo o historico?");
                            break;
                        case 9: // Reset
                            isDialogActive = true;
                            dialogTargetIdx = 9;
                            draw_confirmation_dialog("RESTAURAR PADRAO DE FABRICA?");
                            break;
                        case 10: // CPU Info
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
        ledcWrite(PIN_TFT_BL, 0); // Desliga via PWM
        backlightOn = false;
    }
}

static void backlight_on() {
    ledcWrite(PIN_TFT_BL, deviceSettings.backlight);
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
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    graphics_draw_header("INFO TECNICA CPU");
    
    tft.fillRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_BG_SURFACE);
    tft.drawRoundRect(15, 55, 290, 165, V_RADIUS_LG, V_BG_HIGHLIGHT);
    
    char buf[64];
    draw_text_5x7(tft, 35, 75, "PROCESSADOR: ESP32-S3 CORE", V_CYAN_ELECTRIC, 1);
    
    sprintf(buf, "CHIP REVISION: %d", ESP.getChipRevision());
    draw_text_5x7(tft, 35, 90, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "CPU FREQ: %d MHz", getCpuFrequencyMhz());
    draw_text_5x7(tft, 35, 105, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "FLASH: %d MB (80MHz)", ESP.getFlashChipSize() / (1024*1024));
    draw_text_5x7(tft, 35, 120, buf, V_TEXT_MAIN, 1);
    
    sprintf(buf, "SDK: %s", ESP.getSdkVersion());
    draw_text_5x7(tft, 35, 135, buf, V_TEXT_SUB, 1);
    
    sprintf(buf, "DIE TEMP: %.1f C", (temprature_sens_read() - 32) / 1.8);
    draw_text_5x7(tft, 35, 150, buf, V_WARNING, 1);
    
    sprintf(buf, "MIN HEAP: %d KB", ESP.getMinFreeHeap()/1024);
    draw_text_5x7(tft, 35, 165, buf, V_TEXT_MAIN, 1);

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
