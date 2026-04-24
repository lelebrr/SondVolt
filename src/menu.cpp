// ============================================================================
// Component Tester PRO v3.0 — Menu Principal (CYD Edition)
// ============================================================================
// Menu grid 2x3 com 7 itens (adicionado Multímetro).
// Navegação via touchscreen (primário) ou botões físicos (fallback).
// ============================================================================

#include "menu.h"
#include "buttons.h"
#include "buzzer.h"
#include "drawings.h"
#include "measurements.h"
#include "multimeter.h"
#include "utils.h"
#include "config.h"
#include <TFT_eSPI.h>
#include <Arduino.h>

// ============================================================================
// ITENS DO MENU PRINCIPAL
// ============================================================================
// Grid 2x4 (expandido para incluir o novo Multímetro)
MenuItem menuItems[] = {
  {"Medir",        STATE_MEASURING},
  {"Termica",      STATE_THERMAL_PROBE},
  {"Scanner",      STATE_SCANNER},
  {"Multimetro",   STATE_MULTIMETER},    // NOVO: AC/DC via ZMPT+INA219
  {"Historico",    STATE_HISTORY},
  {"Estatisticas", STATE_STATS},
  {"Ajustes",      STATE_SETTINGS}
};

const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);

// ============================================================================
// INICIALIZAÇÃO DO MENU
// ============================================================================
void menu_init() {
  totalMenuItems = NUM_MENU_ITEMS;
  currentMenuItem = 0;
  draw_menu();
}

// ============================================================================
// HANDLER DO MENU (loop)
// ============================================================================
void menu_handle() {
  buttons_update();

  int cols = MENU_GRID_COLS;
  int row = currentMenuItem / cols;
  int col = currentMenuItem % cols;

  // --- Navegação por botões ou touch ---
  if (isUpPressed()) {
    row--;
    if (row < 0) row = (NUM_MENU_ITEMS - 1) / cols;
    currentMenuItem = row * cols + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isDownPressed()) {
    row++;
    if (row > (NUM_MENU_ITEMS - 1) / cols) row = 0;
    currentMenuItem = row * cols + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isLeftPressed()) {
    col--;
    if (col < 0) col = cols - 1;
    currentMenuItem = row * cols + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isRightPressed()) {
    col++;
    if (col >= cols) col = 0;
    currentMenuItem = row * cols + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }

  // --- Seleção ---
  if (isOkPressed()) {
    play_beep(100);
    currentAppState = menuItems[currentMenuItem].targetState;
    tft.fillScreen(UI_COLOR_BG);
    draw_status_bar();

    switch (currentAppState) {
      case STATE_MEASURING:     draw_measurements_menu(); break;
      case STATE_THERMAL_PROBE: break; // Handled in loop
      case STATE_SCANNER:       draw_scanner(); break;
      case STATE_MULTIMETER:    draw_multimeter_menu(); break; // NOVO
      case STATE_HISTORY:       draw_history(); break;
      case STATE_STATS:         draw_stats(); break;
      case STATE_SETTINGS:      draw_settings_menu(); break;
      case STATE_ABOUT:         draw_about_screen(); break;
      default: break;
    }
  }
}

// ============================================================================
// DESENHO DO MENU PRINCIPAL (Grid)
// ============================================================================
void draw_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();

  // Grid layout (adaptado para 7 itens em grid 2x4)
  int startX = 5;
  int startY = STATUS_BAR_HEIGHT + 6;
  int spacingX = 160;
  int spacingY = 52; // Levemente menor para caber 4 linhas

  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    int r = i / MENU_GRID_COLS;
    int c = i % MENU_GRID_COLS;
    int x = startX + c * spacingX;
    int y = startY + r * spacingY;

    draw_grid_item(x, y, menuItems[i].text, i, (i == currentMenuItem));
  }

  draw_footer_modern();
}

// ============================================================================
// MENU DE CONFIGURAÇÕES
// ============================================================================
void draw_settings_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Configuracoes", UI_COLOR_ACCENT);

  const char *settings[] = {
    "Calibrar Probes",
    "Modo Escuro",
    "Silencioso",
    "Timeout",
    "Backlight",
    "Calibrar Touch",
    "Sobre o CT"
  };
  tft.setTextSize(1);
  for (int i = 0; i < 7; i++) {
    tft.setCursor(20, 60 + i * 20);
    tft.setTextColor(UI_COLOR_TEXT);
    tft.print(i + 1);
    tft.print(F(". "));
    tft.println(settings[i]);
  }
  draw_footer_modern();
}

void handle_settings_menu() {
  static int currentSettingsItem = 0;
  const int NUM_SETTINGS_ITEMS = 7;

  if (isUpPressed()) {
    currentSettingsItem = (currentSettingsItem - 1 + NUM_SETTINGS_ITEMS) % NUM_SETTINGS_ITEMS;
    draw_settings_menu_with_highlights(currentSettingsItem);
    play_beep(50);
  }
  if (isDownPressed()) {
    currentSettingsItem = (currentSettingsItem + 1) % NUM_SETTINGS_ITEMS;
    draw_settings_menu_with_highlights(currentSettingsItem);
    play_beep(50);
  }

  if (isOkPressed()) {
    play_beep(100);
    switch (currentSettingsItem) {
      case 0: calibrate_probes(); break;
      case 1:
        deviceSettings.darkMode = !deviceSettings.darkMode;
        saveSettings();
        break;
      case 2:
        deviceSettings.silentMode = !deviceSettings.silentMode;
        saveSettings();
        break;
      case 3:
        deviceSettings.timeoutDuration = (deviceSettings.timeoutDuration == 30000) ? 60000 : 30000;
        saveSettings();
        break;
      case 4:
        // Ciclar backlight: 50 → 100 → 150 → 200 → 255 → 50
        deviceSettings.backlight += 50;
        if (deviceSettings.backlight < 50) deviceSettings.backlight = 50;
        if (deviceSettings.backlight > 255) deviceSettings.backlight = 50;
        ledcWrite(PIN_TFT_BL, deviceSettings.backlight);
        saveSettings();
        break;
      case 5:
        calibrate_touch();
        return;
      case 6:
        draw_about_screen();
        return;
    }
    draw_settings_menu_with_highlights(currentSettingsItem);
  }

  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

void draw_settings_menu_with_highlights(int highlighted) {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Configuracoes", UI_COLOR_ACCENT);

  const char *settings[] = {
    "Calibrar Probes",
    "Modo Escuro",
    "Silencioso",
    "Timeout",
    "Backlight",
    "Calibrar Touch",
    "Sobre o CT"
  };

  // Valores atuais das configurações
  const char *values[] = {
    "",
    deviceSettings.darkMode ? "[ON]" : "[OFF]",
    deviceSettings.silentMode ? "[ON]" : "[OFF]",
    deviceSettings.timeoutDuration == 30000 ? "[30s]" : "[60s]",
    "",  // Backlight será mostrado inline
    "",
    ""
  };

  tft.setTextSize(1);
  for (int i = 0; i < 7; i++) {
    int yPos = 60 + i * 20;
    tft.setCursor(20, yPos);

    if (i == highlighted) {
      tft.fillRoundRect(15, yPos - 4, tft.width() - 30, 18, 4, UI_COLOR_HILIGHT);
      tft.setTextColor(UI_COLOR_TEXT);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }

    tft.print(i + 1);
    tft.print(F(". "));
    tft.print(settings[i]);

    // Mostrar valor atual
    if (strlen(values[i]) > 0) {
      tft.print(F(" "));
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(values[i]);
    }

    // Backlight: mostrar valor numérico
    if (i == 4) {
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F(" ["));
      tft.print(deviceSettings.backlight);
      tft.print(F("]"));
    }
  }

  draw_footer_modern();
}

// ============================================================================
// TELA "SOBRE"
// ============================================================================
void draw_about_screen() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Sobre o Dispositivo", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(20, 60);  tft.println(F("Component Tester PRO"));
  tft.setCursor(20, 75);  tft.print(F("Versao: ")); tft.println(F(FW_VERSION " (" FW_CODENAME ")"));

  tft.setCursor(20, 95);  tft.println(F("Hardware: ESP32-2432S028R"));
  tft.setCursor(20, 110); tft.println(F("Display: ILI9341 2.8\" 320x240"));
  tft.setCursor(20, 125); tft.println(F("Touch: XPT2046 Resistivo"));
  tft.setCursor(20, 140); tft.println(F("CPU: ESP32 Dual Core 240MHz"));
  tft.setCursor(20, 155);  tft.println(F("RAM: 520KB | Flash: 4MB"));
  
  tft.setCursor(20, 180);
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.println(F("Desenvolvido por Leandro"));

  tft.setCursor(20, 195);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("github.com/lelebrr/Component_Tester"));

  draw_footer_modern();

  // Aguardar input para voltar
  while (!isBackPressed() && !isOkPressed()) {
    buttons_update();
    delay(10);
  }
  currentAppState = STATE_MENU;
  draw_menu();
}

// ============================================================================
// HISTÓRICO DE MEDIÇÕES
// ============================================================================
void draw_history() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Historico", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  if (historyCount == 0) {
    tft.setCursor(30, 80);
    tft.setTextColor(UI_COLOR_GREY);
    tft.println(F("Nenhum registro encontrado"));
  } else {
    int startIdx = (historyCount < HISTORY_SIZE) ? 0 : historyIndex;
    int maxToShow = min(historyCount, 8); // Mais linhas com a tela maior

    for (int i = 0; i < maxToShow; i++) {
      int idx = (startIdx + i) % HISTORY_SIZE;
      tft.setCursor(20, 58 + i * 18);
      tft.setTextColor(measurementHistory[idx].isGood ? UI_COLOR_GREEN : UI_COLOR_RED);
      tft.print(measurementHistory[idx].name);
      tft.print(F(": "));
      tft.setTextColor(UI_COLOR_TEXT);
      fprint(tft, measurementHistory[idx].value, 1);

      // Temperatura no momento da medição
      tft.setTextColor(UI_COLOR_GREY);
      tft.print(F("  ("));
      tft.print(measurementHistory[idx].temp, 0);
      tft.print(F("C)"));
    }

    // Indicador de scroll se houver mais registros
    if (historyCount > maxToShow) {
      tft.setCursor(20, 58 + maxToShow * 18);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("... e mais "));
      tft.print(historyCount - maxToShow);
      tft.print(F(" registros"));
    }
  }

  draw_footer_modern();
}

void handle_history() {
  buttons_update();
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// ============================================================================
// SCANNER AUTOMÁTICO
// ============================================================================
void draw_scanner() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Scanner Automatico", UI_COLOR_ACCENT);

  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Pressione OK para iniciar"));
  tft.setCursor(30, 100);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("Conecte o componente na"));
  tft.setCursor(30, 115);
  tft.print(F("probe (GPIO "));
  tft.print(PIN_PROBE_MAIN);
  tft.println(F(") antes."));

  draw_footer_modern();
}

void handle_scanner() {
  buttons_update();
  static bool scanning = false;

  if (!scanning && isOkPressed()) {
    scanning = true;
    play_beep(100);
    tft.fillRect(20, 70, 260, 100, UI_COLOR_BG);
    tft.setCursor(30, 80);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.println(F("Escaneando..."));
  }

  if (scanning) {
    auto_detect_component();
    delay(1000);
    scanning = false;
    draw_scanner();
  }

  if (isBackPressed()) {
    scanning = false;
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// ============================================================================
// ESTATÍSTICAS DE USO
// ============================================================================
void draw_stats() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Estatisticas", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(30, 70);
  tft.print(F("Total Medicoes: "));
  tft.println(deviceSettings.totalMeasurements);

  tft.setCursor(30, 90);
  tft.print(F("Componentes OK: "));
  tft.println(deviceSettings.totalMeasurements - deviceSettings.faultyMeasurements);

  tft.setCursor(30, 110);
  tft.setTextColor(UI_COLOR_RED);
  tft.print(F("Defeituosos: "));
  tft.println(deviceSettings.faultyMeasurements);

  // Taxa de falha com barra visual
  if (deviceSettings.totalMeasurements > 0) {
    float pct = (float)deviceSettings.faultyMeasurements / deviceSettings.totalMeasurements * 100.0f;

    tft.setCursor(30, 140);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(F("Taxa de Falha: "));
    tft.print(pct, 1);
    tft.println(F("%"));

    // Barra de progresso visual
    tft.drawRect(30, 160, 220, 12, UI_COLOR_GREY);
    int barW = (int)(pct * 2.2f);
    if (barW > 218) barW = 218;
    tft.fillRect(31, 161, barW, 10, UI_COLOR_RED);

    // Labels da barra
    tft.setCursor(30, 175);
    tft.setTextColor(UI_COLOR_GREY);
    tft.print(F("0%"));
    tft.setCursor(230, 175);
    tft.print(F("100%"));
  }

  // Info do sistema
  tft.setCursor(30, 200);
  tft.setTextColor(UI_COLOR_GREY);
  tft.print(F("RAM livre: "));
  tft.print(ESP.getFreeHeap() / 1024);
  tft.print(F("KB | Uptime: "));
  tft.print(millis() / 60000);
  tft.print(F("min"));

  draw_footer_modern();
}

void handle_stats() {
  buttons_update();
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}