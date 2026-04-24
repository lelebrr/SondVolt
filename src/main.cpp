// ============================================================================
// Component Tester PRO v3.0 — Main (CYD Edition)
// ============================================================================
// Plataforma: ESP32-2432S028R (Cheap Yellow Display)
// Display: TFT 2.8" ILI9341 320x240 (integrado, driver TFT_eSPI)
// Touch: XPT2046 resistivo (integrado)
// SD Card: MicroSD no HSPI (separado do TFT)
// Persistência: NVS via Preferences
// ============================================================================

#include "globals.h"
#include "buttons.h"
#include "buzzer.h"
#include "leds.h"
#include "menu.h"
#include "multimeter.h"
#include "measurements.h"
#include "thermal.h"
#include "database.h"
#include <TFT_eSPI.h>
#include <Preferences.h>
#include <Arduino.h>

// ============================================================================
// INSTÂNCIAS GLOBAIS
// ============================================================================
// O TFT_eSPI é configurado via build_flags no platformio.ini
TFT_eSPI tft = TFT_eSPI();

// NVS (Non-Volatile Storage)
Preferences preferences;

// ============================================================================
// VARIÁVEIS GLOBAIS (definições das extern em globals.h)
// ============================================================================
enum AppState currentAppState = STATE_SPLASH;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
bool greenLedState = false;
bool redLedState = false;
float currentTemperature = 0.0;
int currentMenuItem = 0;
int totalMenuItems = 0;

MeasurementHistory measurementHistory[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;

Settings deviceSettings = {
  0.0f,           // offset1
  0.0f,           // offset2
  false,          // darkMode
  false,          // silentMode
  DEFAULT_TIMEOUT_MS, // timeoutDuration
  0,              // totalMeasurements
  0,              // faultyMeasurements
  DEFAULT_BACKLIGHT,  // backlight
  ZMPT_SCALE_FACTOR,  // zmptScaleFactor
  {0, 0, 0, 0, 0}     // touchCalibration (zeros = sem calibração)
};

// ============================================================================
// PERSISTÊNCIA VIA NVS (Preferences)
// ============================================================================
// O ESP32 usa o NVS (Flash interna particionada).
// Muito mais robusto e com endurance de ~100k ciclos por chave.

void loadSettings() {
  preferences.begin("ct_settings", true); // Modo somente-leitura

  deviceSettings.offset1           = preferences.getFloat("offset1", 0.0f);
  deviceSettings.offset2           = preferences.getFloat("offset2", 0.0f);
  deviceSettings.darkMode          = preferences.getBool("darkMode", false);
  deviceSettings.silentMode        = preferences.getBool("silentMode", false);
  deviceSettings.timeoutDuration   = preferences.getULong("timeout", DEFAULT_TIMEOUT_MS);
  deviceSettings.totalMeasurements = preferences.getULong("totalMeas", 0);
  deviceSettings.faultyMeasurements= preferences.getULong("faultyMeas", 0);
  deviceSettings.backlight         = preferences.getUChar("backlight", DEFAULT_BACKLIGHT);
  deviceSettings.zmptScaleFactor   = preferences.getFloat("zmptScale", ZMPT_SCALE_FACTOR);
  
  // Carregar blob de calibração do touch
  preferences.getBytes("touchCal", deviceSettings.touchCalibration, sizeof(deviceSettings.touchCalibration));

  preferences.end();

  Serial.println(F("Settings carregados do NVS"));
}

void saveSettings() {
  preferences.begin("ct_settings", false); // Modo leitura/escrita

  preferences.putFloat("offset1", deviceSettings.offset1);
  preferences.putFloat("offset2", deviceSettings.offset2);
  preferences.putBool("darkMode", deviceSettings.darkMode);
  preferences.putBool("silentMode", deviceSettings.silentMode);
  preferences.putULong("timeout", deviceSettings.timeoutDuration);
  preferences.putULong("totalMeas", deviceSettings.totalMeasurements);
  preferences.putULong("faultyMeas", deviceSettings.faultyMeasurements);
  preferences.putUChar("backlight", deviceSettings.backlight);
  preferences.putFloat("zmptScale", deviceSettings.zmptScaleFactor);
  
  // Salvar blob de calibração do touch
  preferences.putBytes("touchCal", deviceSettings.touchCalibration, sizeof(deviceSettings.touchCalibration));

  preferences.end();
}

// ============================================================================
// HISTÓRICO DE MEDIÇÕES
// ============================================================================
void addToHistory(const char* name, float value, float temp, bool isGood) {
  strncpy(measurementHistory[historyIndex].name, name, MAX_MEASUREMENT_NAME - 1);
  measurementHistory[historyIndex].name[MAX_MEASUREMENT_NAME - 1] = '\0';
  measurementHistory[historyIndex].value = value;
  measurementHistory[historyIndex].temp = temp;
  measurementHistory[historyIndex].isGood = isGood;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  if (historyCount < HISTORY_SIZE) historyCount++;

  deviceSettings.totalMeasurements++;
  if (!isGood) deviceSettings.faultyMeasurements++;
  saveSettings();
}

// ============================================================================
// CONTROLE DO BACKLIGHT (PWM via LEDC)
// ============================================================================
static void backlight_init() {
  // Configura canal LEDC para o backlight do TFT
  ledcAttach(PIN_TFT_BL, LEDC_FREQ_BL, LEDC_RESOLUTION);
  ledcWrite(PIN_TFT_BL, deviceSettings.backlight);
  Serial.print(F("Backlight: "));
  Serial.println(deviceSettings.backlight);
}

static void backlight_set(uint8_t level) {
  ledcWrite(PIN_TFT_BL, level);
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // --- Serial ---
  Serial.begin(SERIAL_BAUD);
  Serial.println(F(""));
  Serial.println(F("========================================="));
  Serial.println(F("  CT PRO v3.0 — CYD Edition"));
  Serial.println(F("  ESP32-2432S028R (Cheap Yellow Display)"));
  Serial.println(F("========================================="));

  // --- TFT Display ---
  tft.init();
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(UI_COLOR_BG);

  // --- Backlight ---
  backlight_init();

  // --- LEDs ---
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);

  // --- Buzzer ---
  buzzer_init();

  // --- Botões e Touch ---
  buttons_init();

  // --- ADC ---
  analogReadResolution(ADC_RESOLUTION); // 12 bits (0-4095)
  // O ESP32 ADC tem não-linearidade natural, mas funciona bem para nossos propósitos

  // =========================================================================
  // SPLASH SCREEN v3.0 — CYD Edition (com barra de progresso animada)
  // =========================================================================
  tft.fillScreen(UI_COLOR_BG);

  // Logo CT PRO com efeito de brilho expansivo
  for (int i = 0; i < 5; i++) {
    tft.drawRoundRect(80 - i, 50 - i, 160 + i * 2, 60 + i * 2, 10,
                      0x0005 + i * 0x0100);
    delay(40);
  }

  // Título principal
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setTextSize(4);
  tft.setCursor(90, 65);
  tft.print(F("CT PRO"));

  // Subtítulo
  tft.setTextSize(1);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setCursor(85, 108);
  tft.print(F("CYD Edition v3.0 (ESP32)"));

  // Info da placa
  tft.setCursor(75, 125);
  tft.setTextColor(UI_COLOR_GREY);
  tft.print(F("ESP32-WROOM-32 | 240MHz | 520KB"));

  // --- Barra de Progresso Animada ---
  int barW = 220;
  int barH = 12;
  int barX = (tft.width() - barW) / 2;
  int barY = 175;

  tft.drawRoundRect(barX - 2, barY - 2, barW + 4, barH + 4, 4, UI_COLOR_HILIGHT);

  for (int i = 0; i <= 100; i += 2) {
    int currentW = (barW * i) / 100;
    tft.fillRoundRect(barX, barY, currentW, barH, 2, UI_COLOR_ACCENT);

    // Texto de progresso
    tft.fillRect(barX + barW / 2 - 25, barY + 18, 70, 12, UI_COLOR_BG);
    tft.setCursor(barX + barW / 2 - 15, barY + 18);
    tft.setTextColor(UI_COLOR_TEXT);
    tft.setTextSize(1);
    tft.print(i);
    tft.print(F("%"));

    // Ações de inicialização em pontos específicos
    if (i == 10) {
      tft.setCursor(barX, barY + 32);
      tft.setTextColor(UI_COLOR_GREY);
      tft.print(F("Iniciando SD Card..."));

      if (db_init_sd()) {
        tft.setTextColor(UI_COLOR_GREEN);
        tft.print(F(" OK"));
      } else {
        tft.setTextColor(UI_COLOR_RED);
        tft.print(F(" ERRO"));
      }
    }

    if (i == 30) {
      tft.fillRect(barX, barY + 32, 280, 12, UI_COLOR_BG);
      tft.setCursor(barX, barY + 32);
      tft.setTextColor(UI_COLOR_GREY);
      tft.print(F("Carregando database..."));

      if (db_load_index()) {
        tft.setTextColor(UI_COLOR_GREEN);
        tft.print(F(" OK"));
      } else {
        tft.setTextColor(UI_COLOR_ORANGE);
        tft.print(F(" N/A"));
      }
    }

    if (i == 50) {
      tft.fillRect(barX, barY + 32, 280, 12, UI_COLOR_BG);
      tft.setCursor(barX, barY + 32);
      tft.setTextColor(UI_COLOR_GREY);
      tft.print(F("Carregando settings..."));
      loadSettings();
      backlight_set(deviceSettings.backlight); // Aplicar backlight salvo
      tft.setTextColor(UI_COLOR_GREEN);
      tft.print(F(" OK"));
    }

    if (i == 70) {
      tft.fillRect(barX, barY + 32, 280, 12, UI_COLOR_BG);
      tft.setCursor(barX, barY + 32);
      tft.setTextColor(UI_COLOR_GREY);
      tft.print(F("Iniciando sensores..."));
      thermal_init();
      multimeter_init();
      tft.setTextColor(UI_COLOR_GREEN);
      tft.print(F(" OK"));
    }

    if (i == 90) {
      tft.fillRect(barX, barY + 32, 280, 12, UI_COLOR_BG);
      tft.setCursor(barX, barY + 32);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Sistema pronto!"));
    }

    delay(15);
  }

  // Beep de boot finalizado
  play_beep(200);
  delay(400);

  // --- Transição para o menu principal ---
  currentAppState = STATE_MENU;
  menu_init();
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================
void loop() {
  currentMillis = millis();
  buttons_update();
  buzzer_update();

  switch (currentAppState) {
    case STATE_SPLASH:
      // Splash é tratado no setup(), não precisa de nada aqui
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
      // About é modal (handled dentro do draw_about_screen)
      break;

    case STATE_HISTORY:
      handle_history();
      break;

    case STATE_SCANNER:
      handle_scanner();
      break;

    case STATE_STATS:
      handle_stats();
      break;

    case STATE_MULTIMETER:
      multimeter_handle();
      break;

    case STATE_TOUCH_CALIBRATE:
      // A função calibrate_touch é bloqueante e muda o estado ao terminar
      calibrate_touch();
      break;
  }

  update_leds();
}