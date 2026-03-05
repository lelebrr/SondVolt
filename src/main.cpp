#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "drawings.h"
#include "globals.h"
#include "leds.h"
#include "logger.h"
#include "menu.h"
#include "thermal.h"
#include "utils.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <SPI.h>

// Instância do display TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

// Variáveis globais (declaradas em globals.h, definidas aqui)
enum AppState currentAppState = STATE_SPLASH;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
bool greenLedState = false;
bool redLedState = false;
float currentTemperature = 0.0;
int currentMenuItem = 0;
int totalMenuItems = 0;

// Configuração do Display
#define TFT_ROTATION                                                           \
  1 // Ajuste para a orientação desejada do menu (1 = Paisagem)

// Declarando as funções definidas em outros módulos ou usadas no loop para que
// o compilador encontre
#include "measurements.h"

void setup() {
  Serial.begin(115200);
  Serial.println(F("Iniciando Component Tester PRO v2.0..."));

  // Inicializa TFT
  tft.begin();
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(ILI9341_BLACK);

  // Splash Screen
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Component Tester PRO v2.0"));
  tft.setTextSize(1);
  tft.setCursor(10, 30);
  tft.println(F("- Leandro -"));
  delay(2000);
  tft.fillScreen(ILI9341_BLACK);

  // Inicializa LEDs
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);

  // Inicializa Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Inicializa botões
  buttons_init();

  // Inicializa SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("Falha na inicialização do SD Card!"));
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.setCursor(10, 50);
    tft.println(F("SD Card Error!"));
    while (true)
      ; // Trava aqui se o SD falhar
  }
  Serial.println(F("SD Card inicializado."));

  // Inicializa sonda térmica
  thermal_init();

  // Define o estado inicial do aplicativo
  currentAppState = STATE_MENU;
}

void loop() {
  currentMillis = millis();

  // Leitura e atualização dos botões
  buttons_update();

  // Atualiza o estado do buzzer
  buzzer_update();

  switch (currentAppState) {
  case STATE_SPLASH:
    // Já tratado no setup, transita para STATE_MENU
    break;
  case STATE_MENU:
    menu_handle();
    break;
  case STATE_MEASURING:
    measurements_handle();
    break;
  case STATE_THERMAL_PROBE:
    thermal_handle();
    break;
  case STATE_SETTINGS:
    handle_settings_menu();
    break;
  case STATE_ABOUT:
    // A tela about é estática, não precisa de handle
    break;
  }

  // Atualiza LEDs com base nos estados globais
  update_leds();
}
