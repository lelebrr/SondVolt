#include "menu.h"
#include "buttons.h"
#include "leds.h"

// Definição dos itens do menu
MenuItem menuItems[] = {{F("Medir Componente"), STATE_MEASURING},
                        {F("Sonda Termica"), STATE_THERMAL_PROBE},
                        {F("Configuracoes"), STATE_SETTINGS},
                        {F("Sobre"), STATE_ABOUT}};

const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);

// Inicializa o sistema de menu
void menu_init() {
  totalMenuItems = NUM_MENU_ITEMS;
  currentMenuItem = 0; // Começa no primeiro item
  draw_menu();
}

// Manipula a navegação do menu
void menu_handle() {
  buttons_update(); // Atualiza o estado dos botões

  if (isUpPressed()) {
    currentMenuItem--;
    if (currentMenuItem < 0) {
      currentMenuItem = NUM_MENU_ITEMS - 1;
    }
    draw_menu();
  }
  if (isDownPressed()) {
    currentMenuItem++;
    if (currentMenuItem >= NUM_MENU_ITEMS) {
      currentMenuItem = 0;
    }
    draw_menu();
  }
  if (isOkPressed()) {
    // Transita para o estado selecionado
    currentAppState = menuItems[currentMenuItem].targetState;
    tft.fillScreen(TFT_BLACK); // Limpa a tela ao mudar de estado
    // Dependendo do estado, pode ser necessário chamar uma função de
    // inicialização específica
    if (currentAppState == STATE_THERMAL_PROBE) {
      // thermal_init() já é chamado no setup, mas pode precisar de um reset
      // visual
    } else if (currentAppState == STATE_MEASURING) {
      draw_measurements_menu();
    } else if (currentAppState == STATE_SETTINGS) {
      draw_settings_menu();
    } else if (currentAppState == STATE_ABOUT) {
      draw_about_screen();
    }
  }
  // O botão Back pode ser usado para voltar ao menu principal de outros estados
  // Mas no menu principal, ele pode não ter função ou sair para um estado de
  // 'idle'
}

// Desenha o menu na tela TFT
void draw_menu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(TL_DATUM); // Top-Left datum

  for (int i = 0; i < NUM_MENU_ITEMS; i++) {
    if (i == currentMenuItem) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE); // Item selecionado em destaque
    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.setCursor(10, 10 + i * 20);
    tft.println(menuItems[i].text);
  }
  draw_footer();
}

// Desenha o rodapé com a legenda dos botões
void draw_footer() {
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.fillRect(0, tft.height() - 20, tft.width(), 20, TFT_DARKGREY);
  tft.setCursor(5, tft.height() - 15);
  tft.print(F("UP/DW: Scroll | OK: Select | BCK: Exit"));
}

// Desenha o menu de configurações
void draw_settings_menu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Configuracoes"));

  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println(F("1. Calibrar Probes"));
  tft.setCursor(10, 60);
  tft.println(F("2. Modo Escuro"));
  tft.setCursor(10, 80);
  tft.println(F("3. Modo Silencioso"));
  tft.setCursor(10, 100);
  tft.println(F("4. Resetar Contadores"));
  tft.setCursor(10, 120);
  tft.println(F("5. Versao Firmware"));

  draw_footer();
}

// Manipula o menu de configurações
void handle_settings_menu() {
  buttons_update();

  if (isUpPressed()) {
    // Navegação para cima
    // Implementar navegação entre itens
  }
  if (isDownPressed()) {
    // Navegação para baixo
    // Implementar navegação entre itens
  }
  if (isOkPressed()) {
    // Executa função selecionada
    // Implementar lógica de seleção
  }
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    tft.fillScreen(TFT_BLACK);
  }
}

// Desenha a tela sobre/informações
void draw_about_screen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Component Tester"));
  tft.setCursor(10, 30);
  tft.println(F("PRO v2.0"));

  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.println(F("Desenvolvido por: Leandro"));
  tft.setCursor(10, 80);
  tft.println(F("Baseado em Arduino Uno R3"));
  tft.setCursor(10, 100);
  tft.println(F("Firmware: v2.0.1"));
  tft.setCursor(10, 120);
  tft.println(F("Data: 04/03/2026"));

  draw_footer();
}
