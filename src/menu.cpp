#include "menu.h"
#include "buttons.h"
#include "buzzer.h"
#include "drawings.h"
#include "measurements.h"
#include "utils.h"
#include "config.h"
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

MenuItem menuItems[] = {
  {"Medir", STATE_MEASURING},
  {"Termica", STATE_THERMAL_PROBE},
  {"Scanner", STATE_SCANNER},
  {"Historico", STATE_HISTORY},
  {"Estatisticas", STATE_STATS},
  {"Ajustes", STATE_SETTINGS}
};

const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);

void menu_init() {
  totalMenuItems = NUM_MENU_ITEMS;
  currentMenuItem = 0;
  draw_menu();
}

void menu_handle() {
  buttons_update();

  int row = currentMenuItem / MENU_GRID_COLS;
  int col = currentMenuItem % MENU_GRID_COLS;

  if (isUpPressed()) {
    row--;
    if (row < 0) row = (NUM_MENU_ITEMS - 1) / MENU_GRID_COLS;
    currentMenuItem = row * MENU_GRID_COLS + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isDownPressed()) {
    row++;
    if (row > (NUM_MENU_ITEMS - 1) / MENU_GRID_COLS) row = 0;
    currentMenuItem = row * MENU_GRID_COLS + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isLeftPressed()) {
    col--;
    if (col < 0) col = MENU_GRID_COLS - 1;
    currentMenuItem = row * MENU_GRID_COLS + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }
  if (isRightPressed()) {
    col++;
    if (col >= MENU_GRID_COLS) col = 0;
    currentMenuItem = row * MENU_GRID_COLS + col;
    if (currentMenuItem >= NUM_MENU_ITEMS) currentMenuItem = NUM_MENU_ITEMS - 1;
    draw_menu();
    play_beep(50);
  }

  if (isOkPressed()) {
    play_beep(100);
    currentAppState = menuItems[currentMenuItem].targetState;
    tft.fillScreen(UI_COLOR_BG);
    draw_status_bar();
    
    switch(currentAppState) {
      case STATE_MEASURING: draw_measurements_menu(); break;
      case STATE_THERMAL_PROBE: break; // Handled in loop
      case STATE_SCANNER: draw_scanner(); break;
      case STATE_HISTORY: draw_history(); break;
      case STATE_STATS: draw_stats(); break;
      case STATE_SETTINGS: draw_settings_menu(); break;
      case STATE_ABOUT: draw_about_screen(); break;
      default: break;
    }
  }
}

void draw_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  
  // Padding para o grid
  int startX = 5;
  int startY = STATUS_BAR_HEIGHT + 10;
  int spacingX = 160;
  int spacingY = 62;

  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    int r = i / MENU_GRID_COLS;
    int c = i % MENU_GRID_COLS;
    int x = startX + c * spacingX;
    int y = startY + r * spacingY;
    
    draw_grid_item(x, y, menuItems[i].text, i, (i == currentMenuItem));
  }
  
  draw_footer_modern();
}

void draw_settings_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Configuracoes", UI_COLOR_ACCENT);

  const char *settings[] = {"Calibrar Probes", "Modo Escuro", "Silencioso", "Timeout", "Sobre o CT"};
  tft.setTextSize(1);
  for (int i = 0; i < 5; i++) {
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
  const int NUM_SETTINGS_ITEMS = 5;

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
      case 1: deviceSettings.darkMode = !deviceSettings.darkMode; break;
      case 2: deviceSettings.silentMode = !deviceSettings.silentMode; break;
      case 3: deviceSettings.timeoutDuration = (deviceSettings.timeoutDuration == 30000) ? 60000 : 30000; break;
      case 4: 
        draw_about_screen();
        return; // draw_about_screen will eventually return and call draw_menu
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

  const char *settings[] = {"Calibrar Probes", "Modo Escuro", "Silencioso", "Timeout", "Sobre o CT"};
  tft.setTextSize(1);
  for (int i = 0; i < 5; i++) {
    tft.setCursor(20, 60 + i * 22);
    if (i == highlighted) {
      tft.fillRoundRect(15, 58 + i * 22, tft.width() - 30, 18, 4, UI_COLOR_HILIGHT);
      tft.setTextColor(UI_COLOR_TEXT);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }
    tft.print(i + 1);
    tft.print(F(". "));
    tft.println(settings[i]);
  }
  draw_footer_modern();
}

void draw_about_screen() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Sobre o Dispositivo", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(20, 60); tft.println(F("Component Tester PRO"));
  tft.setCursor(20, 75); tft.println(F("Versao: 2.1.0 (Elite)"));
  tft.setCursor(20, 95); tft.println(F("Hardware: Arduino Uno"));
  tft.setCursor(20, 110); tft.println(F("Display: ILI9341 320x240"));
  
  tft.setCursor(20, 140); 
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.println(F("Desenvolvido por Leandro"));
  
  draw_footer_modern();
  
  while(!isBackPressed() && !isOkPressed()) {
    buttons_update();
    delay(10);
  }
  currentAppState = STATE_MENU;
  draw_menu();
}

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
    for (int i = 0; i < historyCount && i < 6; i++) {
      int idx = (startIdx + i) % HISTORY_SIZE;
      tft.setCursor(20, 60 + i * 18);
      tft.setTextColor(measurementHistory[idx].isGood ? UI_COLOR_GREEN : UI_COLOR_RED);
      tft.print(measurementHistory[idx].name);
      tft.print(F(": "));
      tft.setTextColor(UI_COLOR_TEXT);
      fprint(tft, measurementHistory[idx].value, 1);
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

void draw_scanner() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Scanner Automático", UI_COLOR_ACCENT);
  
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Pressione OK para iniciar"));
  tft.setCursor(30, 100);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("Conecte o componente nas"));
  tft.setCursor(30, 115);
  tft.println(F("probes antes de comecar."));
  
  draw_footer_modern();
}

void handle_scanner() {
  buttons_update();
  static bool scanning = false;
  
  if (!scanning && isOkPressed()) {
    scanning = true;
    play_beep(100);
    tft.fillRect(20, 70, 200, 100, UI_COLOR_BG);
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

void draw_stats() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Estatisticas", UI_COLOR_ACCENT);

  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(30, 70); tft.print(F("Total Medicoes: ")); tft.println(deviceSettings.totalMeasurements);
  tft.setCursor(30, 90); tft.print(F("Componentes OK: ")); tft.println(deviceSettings.totalMeasurements - deviceSettings.faultyMeasurements);
  tft.setCursor(30, 110); 
  tft.setTextColor(UI_COLOR_RED);
  tft.print(F("Defeituosos: ")); tft.println(deviceSettings.faultyMeasurements);

  if (deviceSettings.totalMeasurements > 0) {
    float pct = (float)deviceSettings.faultyMeasurements / deviceSettings.totalMeasurements * 100.0;
    tft.setCursor(30, 140);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(F("Taxa de Falha: "));
    tft.print(pct, 1);
    tft.println(F("%"));
    
    // Pequena barra de progresso visual
    tft.drawRect(30, 160, 200, 10, UI_COLOR_GREY);
    int barW = (int)(pct * 2.0);
    tft.fillRect(31, 161, barW, 8, UI_COLOR_RED);
  }

  draw_footer_modern();
}

void handle_stats() {
  buttons_update();
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}