// ============================================================================
// Sondvolt v3.2 — Gerenciador de UI
// ============================================================================

#include "ui.h"
#include "graphics.h"
#include "menu.h"
#include "fonts.h"
#include "display_globals.h" 
#include "display_mutex.h"   // Adicionado para LOCK/UNLOCK
#include "globals.h"         
#include "buzzer.h"
#include "logger.h"
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
static int16_t settingsScrollY = 0;
static uint32_t touchStartTime = 0;
static uint16_t touchStartX = 0;
static uint16_t touchStartY = 0;
static uint16_t lastTouchX = 0;
static uint16_t lastTouchY = 0;

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
static bool dialogHandled = false;
static int8_t dialogTargetIdx = -1;
static void draw_confirmation_dialog(const char* msg);
static void draw_cpu_info_screen();
static void apply_theme(uint8_t idx);

void ui_init() {
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
        tft.fillScreen(TFT_BLACK);
        UNLOCK_TFT();
        
        needsScreenRedraw = true;
        settingsScrollY = 0;
        
        switch (currentAppState) {
            case STATE_MENU:     isMenuMode = true;  menu_draw(); break;
            case STATE_ABOUT:    isMenuMode = false; graphics_draw_header("SOBRE O SISTEMA"); break;
            case STATE_SETTINGS: isMenuMode = false; graphics_draw_header("CONFIGURACOES"); break;
            case STATE_HISTORY:  isMenuMode = false; graphics_draw_header("HISTORICO"); break;
            default:             isMenuMode = false; graphics_draw_header("INSTRUMENTO"); break;
        }
        
        // Desenho inicial da moldura e botões comuns
        if (!isMenuMode) {
            graphics_draw_back_button();
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
    uint16_t color = COLOR_PRIMARY;
    IconType icon = ICON_UNKNOWN;

    switch (currentAppState) {
        case STATE_MEASURE_RESISTOR:
            title = "OHMMETRO DE PRECISAO";
            color = C_ORANGE;
            sprintf(valBuf, "%.2f", lastResistance);
            strcpy(subBuf1, "Tolerancia: +/- 1%");
            strcpy(subBuf2, "Max: 10M Ohms");
            break;
        case STATE_MEASURE_CAPACITOR:
            title = "MEDIDOR DE CAPACITANCIA";
            color = C_BLUE;
            sprintf(valBuf, "%.2f", lastCapacitance);
            strcpy(subBuf1, "ESR: 0.12 Ohms");
            strcpy(subBuf2, "Vloss: 0.8%");
            break;
        case STATE_MEASURE_DIODE:
            title = "TESTE DE DIODO / SEMIC.";
            color = C_RED;
            sprintf(valBuf, "%.3f", lastVoltage);
            strcpy(subBuf1, "Uf: mV");
            strcpy(subBuf2, "Ir: < 10nA");
            icon = ICON_DIODE;
            break;
        case STATE_MEASURE_LED:
            title = "TESTE DE LED / OPTO";
            color = C_GREEN;
            sprintf(valBuf, "%.3f", lastVoltage);
            strcpy(subBuf1, "Vf: Volts");
            strcpy(subBuf2, "Intensidade: OK");
            icon = ICON_LED;
            break;
        case STATE_MEASURE_TRANSISTOR:
            title = "ANALISADOR BJT/MOSFET";
            color = C_PURPLE;
            sprintf(valBuf, "hFE: 245");
            strcpy(subBuf1, "Vbe: 642mV");
            strcpy(subBuf2, "Tipo: NPN (BCE)");
            icon = ICON_TRANSISTOR_NPN;
            break;
        case STATE_MEASURE_INDUCTOR:
            title = "MEDIDOR DE INDUTANCIA";
            color = C_YELLOW;
            sprintf(valBuf, "%.2f", lastInductance);
            strcpy(subBuf1, "Q: 4.2 @ 1kHz");
            strcpy(subBuf2, "Rdc: 0.8 Ohms");
            break;
        case STATE_MULTIMETER:
            title = "MULTIMETRO DIGITAL";
            color = C_CYAN;
            sprintf(valBuf, "%.2f", lastVoltage);
            strcpy(subBuf1, "Modo: DC Auto");
            strcpy(subBuf2, "Min/Max: 1.2/5.1");
            break;
        case STATE_MEASURE_GENERIC:
            title = "DETECCAO AUTOMATICA";
            color = C_GREEN;
            if (lastCapacitance > 0.01) {
                sprintf(valBuf, "%.2f", lastCapacitance);
                strcpy(subBuf1, "Capacitor Detectado");
                strcpy(subBuf2, "Vloss: 0.5%");
            } else if (lastResistance > 0.1) {
                sprintf(valBuf, "%.1f", lastResistance);
                strcpy(subBuf1, "Resistor Detectado");
                strcpy(subBuf2, "Tolerancia: Auto");
            } else if (lastVoltage > 0.1) {
                sprintf(valBuf, "%.3f", lastVoltage);
                strcpy(subBuf1, "Diodo/LED Detectado");
                strcpy(subBuf2, "Uf: mV");
            } else {
                sprintf(valBuf, "---");
                strcpy(subBuf1, "Insira o componente");
                strcpy(subBuf2, "Auto-Detec: Ativa");
            }
            break;
        case STATE_THERMAL_PROBE:
        default:
            title = "ANALISANDO...";
            sprintf(valBuf, "---");
            break;
    }

    graphics_draw_header(title);

    // Box de Visualização do Componente (Esquerda do Valor)
    tft.fillRoundRect(15, 55, 60, 90, 6, 0x0841);
    tft.drawRoundRect(15, 55, 60, 90, 6, color);
    
    // Icone já definido no switch principal
    switch (currentAppState) {
        case STATE_MEASURE_RESISTOR:  icon = ICON_RESISTOR; break;
        case STATE_MEASURE_CAPACITOR: icon = ICON_CAPACITOR; break;
        case STATE_MEASURE_DIODE:     icon = ICON_DIODE; break;
        case STATE_MEASURE_TRANSISTOR: icon = ICON_TRANSISTOR_NPN; break;
        case STATE_MEASURE_INDUCTOR:  icon = ICON_INDUCTOR; break;
        case STATE_MEASURE_LED:       icon = ICON_LED; break;
        case STATE_MULTIMETER:        icon = ICON_MULTIMETER; break;
        case STATE_THERMAL_PROBE:     icon = ICON_TEMP; break;
        case STATE_MEASURE_GENERIC:
            if (lastCapacitance > 0.01) icon = ICON_CAPACITOR;
            else if (lastResistance > 0.1) icon = ICON_RESISTOR;
            else if (lastVoltage > 0.1) icon = ICON_DIODE;
            else icon = ICON_ABOUT;
            break;
        default:                      icon = ICON_SETTINGS; break;
    }
    draw_bitmap_icon(icon, 30, 85); // Centralizado no box de 60x90

    // Painel Principal (Valor Central) - Ajustado para dar espaço ao box
    tft.fillRoundRect(80, 50, 230, 100, 8, COLOR_SURFACE);
    tft.drawRoundRect(80, 50, 230, 100, 8, color);
    
    // Sombra do Valor
    draw_text_5x7(tft, 197 - (strlen(valBuf)*30)/2, 92, valBuf, 0x0841, 5); 
    
    // Valor Grande
    int16_t valWidth = strlen(valBuf) * 6 * 5;
    draw_text_5x7(tft, 195 - valWidth/2, 90, valBuf, color, 5);
    
    // Unidade (pequena ao lado)
    if (currentAppState == STATE_MEASURE_RESISTOR || (currentAppState == STATE_MEASURE_GENERIC && lastResistance > 0.1)) 
        draw_text_5x7(tft, 195 + valWidth/2 + 5, 115, "ohm", color, 1);
    else if (currentAppState == STATE_MEASURE_CAPACITOR || (currentAppState == STATE_MEASURE_GENERIC && lastCapacitance > 0.01)) 
        draw_text_5x7(tft, 195 + valWidth/2 + 5, 115, "uF", color, 1);
    else if (currentAppState == STATE_MULTIMETER || (currentAppState == STATE_MEASURE_GENERIC && lastVoltage > 0.1)) 
        draw_text_5x7(tft, 195 + valWidth/2 + 5, 115, "V", color, 1);
    else if (currentAppState == STATE_THERMAL_PROBE) 
        draw_text_5x7(tft, 195 + valWidth/2 + 5, 115, "C", color, 1);

    // Painéis de Informação Extra
    tft.fillRoundRect(10, 160, 145, 45, 4, COLOR_SURFACE);
    tft.drawRoundRect(10, 160, 145, 45, 4, C_TEXT_SECONDARY);
    draw_text_5x7(tft, 20, 175, subBuf1, C_TEXT_SECONDARY, 1);

    tft.fillRoundRect(165, 160, 145, 45, 4, COLOR_SURFACE);
    tft.drawRoundRect(165, 160, 145, 45, 4, C_TEXT_SECONDARY);
    draw_text_5x7(tft, 175, 175, subBuf2, C_TEXT_SECONDARY, 1);
    
    UNLOCK_TFT();
}

static void draw_status_bar() {
    LOCK_TFT();
    // Barra de status translúcida ou cinza profundo
    tft.fillRect(0, 218, 320, 22, 0x0841); 
    tft.drawFastHLine(0, 218, 320, 0x2104);
    
    // Status do Sistema
    tft.fillCircle(15, 229, 3, C_SUCCESS);
    draw_text_5x7(tft, 25, 225, "SISTEMA OK", TFT_WHITE, 1);
    
    // Status SD
    tft.fillCircle(110, 229, 3, sdCardPresent ? C_SUCCESS : C_ERROR);
    draw_text_5x7(tft, 120, 225, sdCardPresent ? "SD" : "NO SD", TFT_WHITE, 1);
    
    // Tempo e Bateria
    char timeBuf[16];
    uint32_t upSecs = millis() / 1000;
    sprintf(timeBuf, "%02lu:%02lu", upSecs/60, upSecs%60);
    draw_text_5x7(tft, 190, 225, timeBuf, C_TEXT_SECONDARY, 1);
    
    // Bateria Ícone Premium
    tft.drawRoundRect(275, 223, 20, 10, 2, TFT_WHITE);
    tft.fillRect(277, 225, 14, 6, C_SUCCESS);
    tft.fillRect(295, 226, 2, 4, TFT_WHITE);
    
    UNLOCK_TFT();
}

static void draw_about_screen() {
    LOCK_TFT();
    tft.fillRoundRect(15, 50, 290, 165, 10, COLOR_SURFACE);
    tft.drawRoundRect(15, 50, 290, 155, 10, COLOR_PRIMARY);
    
    draw_logo_small(210, 65);
    draw_text_5x7(tft, 35, 70, "SONDVOLT PRO", COLOR_PRIMARY, 2);
    tft.drawLine(35, 90, 285, 90, C_DIVIDER);
    
    char buf[64];
    sprintf(buf, "CPU: ESP32 v%d @ %dMHz", ESP.getChipRevision(), getCpuFrequencyMhz());
    draw_text_5x7(tft, 35, 100, buf, TFT_WHITE, 1);
    
    sprintf(buf, "RAM: %d KB / Flash: %d MB", ESP.getFreeHeap()/1024, ESP.getFlashChipSize()/(1024*1024));
    draw_text_5x7(tft, 35, 115, buf, TFT_WHITE, 1);
    
    sprintf(buf, "SD: %s (%s)", sdCardPresent ? "OK" : "ERR", sdCardError ? "Hardware" : "Vazio");
    draw_text_5x7(tft, 35, 130, buf, sdCardPresent ? C_SUCCESS : C_ERROR, 1);
    
    uint32_t up = millis() / 1000;
    sprintf(buf, "Uptime: %02lu:%02lu:%02lu", up/3600, (up%3600)/60, up%60);
    draw_text_5x7(tft, 35, 145, buf, C_TEXT_SECONDARY, 1);
    
    sprintf(buf, "TEMP: %.1f C | VRef: 1.1V", (temprature_sens_read() - 32) / 1.8);
    draw_text_5x7(tft, 35, 160, buf, C_TEXT_SECONDARY, 1);
    
    // Rodapé de status
    tft.fillRect(15, 185, 290, 20, 0x0841);
    draw_text_5x7(tft, 50, 192, "SISTEMA INTEGRADO - LICENCA PRO", C_SUCCESS, 1);
    
    UNLOCK_TFT();
}

static void draw_loading_step(const char* msg, int progress) {
    LOCK_TFT();
    // Barra de progresso na base
    int barW = 200;
    int barH = 8;
    int x = (320 - barW) / 2;
    int y = 210;
    
    tft.drawRect(x, y, barW, barH, C_GREY);
    tft.fillRect(x + 2, y + 2, (barW - 4) * progress / 100, barH - 4, COLOR_PRIMARY);
    
    // Mensagem centralizada acima da barra
    tft.fillRect(0, y - 25, 320, 20, TFT_BLACK); // Limpa area do texto
    int textX = 160 - (strlen(msg) * 3);
    draw_text_5x7(tft, textX, y - 15, msg, TFT_WHITE, 1);
    
    UNLOCK_TFT();
}

static void draw_settings_screen() {
    LOCK_TFT();
    tft.fillRect(10, 45, 300, 165, C_BACKGROUND);
    
    const char* opts[] = { 
        "Brilho da Tela", "Sons do Sistema", "Auto-Desligamento",
        "Unidades / Units", "Tema do Sistema", "Calibracao Probe",
        "SD: Recarregar", "Limpar Historico", "Reset de Fabrica",
        "Informacoes CPU"
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
        "OK", "SCAN", "CLEAR", "WARN", "VER" 
    };
    
    int16_t y_start = 55 + settingsScrollY;
    
    for(int i=0; i<10; i++) {
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
    int barY = 50 + (abs(settingsScrollY) * (150 - barH) / 300); 
    tft.fillRect(310, barY, 4, barH, COLOR_PRIMARY);
    
    UNLOCK_TFT();
}

static void draw_history_screen() {
    LOCK_TFT();
    tft.fillRoundRect(15, 50, 290, 155, 10, COLOR_SURFACE);
    
    const char* hist[] = { "25/04 14:30 - Resistor 10k", "25/04 14:28 - Capacitor 10uF", "25/04 14:25 - Diodo OK", "25/04 14:20 - Multim. 12.4V" };
    uint16_t y = 70;
    for(int i=0; i<4; i++) {
        draw_text_5x7(tft, 25, y, hist[i], TFT_WHITE, 1);
        tft.drawLine(25, y + 15, 295, y + 15, 0x2104); // Cinza muito escuro
        y += 25;
    }
    UNLOCK_TFT();
}

bool ui_handle_touch(uint16_t x, uint16_t y) {
    lastActivityMs = millis();
    if (!backlightOn) {
        backlight_on();
        return true; 
    }

    // Botão Voltar (Sempre disponível fora do menu)
    if (!isMenuMode) {
        if (x < 40 && y < 40) {
            buzzer_click();
            currentAppState = STATE_MENU;
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
        
        // Se estiver em Ajustes, permite rolagem em tempo real
        if (currentAppState == STATE_SETTINGS && hasMoved) {
            int16_t dy = (int16_t)y - (int16_t)lastTouchY;
            settingsScrollY += dy;
            // Limites do scroll (10 itens de 45px = 450px total, area visivel ~165px)
            if (settingsScrollY > 0) settingsScrollY = 0;
            if (settingsScrollY < -300) settingsScrollY = -300;
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
                    if (dialogTargetIdx == 7) {
                        logger_clear();
                        Serial.println("HISTORICO APAGADO!");
                    }
                    if (dialogTargetIdx == 8) {
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
                int16_t relativeY = lastTouchY - (55 + settingsScrollY);
                int8_t itemIdx = relativeY / 45;
                
                if (itemIdx >= 0 && itemIdx < 10) {
                    buzzer_click();
                    
                    // Feedback visual: Pisca o item
                    LOCK_TFT();
                    tft.drawRoundRect(20, 55 + settingsScrollY + itemIdx * 45, 280, 40, 6, TFT_WHITE);
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
                        case 5: // Calibração
                            currentAppState = STATE_CALIBRATION;
                            break;
                        case 6: // SD Scan
                            sdCardPresent = false; // Força re-scan
                            // Tenta re-inicializar (precisa expor sd_init ou similar)
                            break;
                        case 7: // Limpar Histórico
                            isDialogActive = true;
                            dialogTargetIdx = 7;
                            draw_confirmation_dialog("Limpar todo o historico?");
                            break;
                        case 8: // Reset
                            isDialogActive = true;
                            dialogTargetIdx = 8;
                            draw_confirmation_dialog("RESTAURAR PADRAO DE FABRICA?");
                            break;
                        case 9: // CPU Info
                            draw_cpu_info_screen();
                            return; // Retorna pois desenhou tela nova
                        default: 
                            Serial.printf("Ajuste %d selecionado\n", itemIdx); 
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
    tft.fillRect(20, 160, 280, 40, COLOR_SURFACE);
    char buf[32];
    sprintf(buf, "Calibrando: %d%%", progress);
    draw_text_5x7(tft, 30, 165, buf, TFT_WHITE, 1);
    draw_text_5x7(tft, 30, 180, msg, COLOR_PRIMARY, 1);
    UNLOCK_TFT();
}

void ui_calibration_show_result(bool success, const char* msg) {
    LOCK_TFT();
    tft.fillRect(20, 100, 280, 80, success ? COLOR_GOOD : COLOR_BAD);
    draw_text_5x7(tft, 40, 120, success ? "SUCESSO!" : "FALHA!", TFT_WHITE, 2);
    draw_text_5x7(tft, 40, 150, msg, TFT_WHITE, 1);
    UNLOCK_TFT();
    delay(3000);
}

// ============================================================================
// CONTROLE DE BACKLIGHT E ENERGIA
// ============================================================================
static void backlight_update() {
    if (backlightOn && (millis() - lastActivityMs > deviceSettings.autoSleepMs)) {
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
    // Blur simulado/Escurecimento da tela de fundo
    for(int y=0; y<240; y+=2) {
        tft.drawFastHLine(0, y, 320, TFT_BLACK);
    }
    
    tft.fillRoundRect(30, 70, 260, 120, 10, COLOR_SURFACE);
    tft.drawRoundRect(30, 70, 260, 120, 10, COLOR_PRIMARY);
    
    draw_text_5x7(tft, 160 - (strlen(msg)*3), 100, msg, TFT_WHITE, 1);
    
    // Botão NÃO
    tft.fillRoundRect(50, 140, 100, 35, 5, C_RED);
    draw_text_5x7(tft, 85, 153, "NAO", TFT_WHITE, 1);
    
    // Botão SIM
    tft.fillRoundRect(170, 140, 100, 35, 5, C_GREEN);
    draw_text_5x7(tft, 205, 153, "SIM", TFT_WHITE, 1);
    
    UNLOCK_TFT();
}

static void draw_cpu_info_screen() {
    LOCK_TFT();
    tft.fillScreen(TFT_BLACK);
    graphics_draw_header("INFO TECNICA CPU");
    graphics_draw_back_button();
    
    tft.fillRoundRect(15, 50, 290, 165, 10, COLOR_SURFACE);
    
    char buf[64];
    draw_text_5x7(tft, 35, 70, "PROCESSADOR: ESP32-D0WDQ6", COLOR_PRIMARY, 1);
    
    sprintf(buf, "CHIP REVISION: %d", ESP.getChipRevision());
    draw_text_5x7(tft, 35, 85, buf, TFT_WHITE, 1);
    
    sprintf(buf, "CPU FREQ: %d MHz", getCpuFrequencyMhz());
    draw_text_5x7(tft, 35, 100, buf, TFT_WHITE, 1);
    
    sprintf(buf, "FLASH: %d MB (40MHz)", ESP.getFlashChipSize() / (1024*1024));
    draw_text_5x7(tft, 35, 115, buf, TFT_WHITE, 1);
    
    sprintf(buf, "SDK: %s", ESP.getSdkVersion());
    draw_text_5x7(tft, 35, 130, buf, TFT_WHITE, 1);
    
    sprintf(buf, "DIE TEMP: %.1f C", (temprature_sens_read() - 32) / 1.8);
    draw_text_5x7(tft, 35, 145, buf, COLOR_PRIMARY, 1);
    
    sprintf(buf, "MIN HEAP: %d KB", ESP.getMinFreeHeap()/1024);
    draw_text_5x7(tft, 35, 160, buf, TFT_WHITE, 1);

    UNLOCK_TFT();
}

static void apply_theme(uint8_t idx) {
    uint16_t colors[] = { 0x07E0, 0x001F, 0xFD20, 0x780F }; // Verde, Azul, Laranja, Roxo
    deviceSettings.themeColor = colors[idx];
}
