// ============================================================================
// Component Tester PRO v3.0 — Botões e Touchscreen (CYD Edition)
// ============================================================================
// Navegação unificada: touchscreen XPT2046 como método primário,
// com suporte opcional a botões físicos via Bounce2.
//
// O touchscreen é dividido em zonas lógicas:
//   - Toque no topo:    UP
//   - Toque no fundo:   DOWN
//   - Toque à esquerda: LEFT / BACK
//   - Toque à direita:  RIGHT
//   - Toque no centro:  OK
//   - Toque no canto inferior esquerdo: BACK
// ============================================================================

#include "buttons.h"
#include "config.h"
#include "globals.h"
#include <TFT_eSPI.h>
#include <Arduino.h>

extern TFT_eSPI tft;

// ============================================================================
// VARIÁVEIS INTERNAS DO TOUCH
// ============================================================================
static unsigned long lastTouchTime = 0;
static bool touchWasPressed = false;

// Flags de ação (setadas pelo touch ou botões, consumidas pelas funções isXPressed)
static bool actionUp    = false;
static bool actionDown  = false;
static bool actionLeft  = false;
static bool actionRight = false;
static bool actionOk    = false;
static bool actionBack  = false;

// ============================================================================
// ZONAS DE TOQUE (em pixels, para tela 320x240 landscape)
// ============================================================================
// Definição das zonas de toque relativas à tela:
//
//   +----+------------+----+
//   |    |    UP       |    |
//   | L  +------------+ R  |
//   | E  |   CENTER   | I  |
//   | F  |    (OK)    | G  |
//   | T  +------------+ H  |
//   |    |   DOWN     | T  |
//   +----+-----+------+----+
//   |  BACK    |            |
//   +-----------+------------+

#define ZONE_MARGIN     50   // Margem lateral para LEFT/RIGHT
#define ZONE_TOP        60   // Altura da zona UP
#define ZONE_BOTTOM     60   // Altura da zona DOWN (a partir do fundo)
#define ZONE_BACK_W     100  // Largura da zona BACK (canto inferior esquerdo)
#define ZONE_BACK_H     40   // Altura da zona BACK

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================
void buttons_init() {
  // Se houver calibração salva, aplicar. Caso contrário, usar defaults.
  bool hasCal = false;
  for (int i = 0; i < 5; i++) {
    if (deviceSettings.touchCalibration[i] != 0) {
      hasCal = true;
      break;
    }
  }

  if (hasCal) {
    tft.setTouch(deviceSettings.touchCalibration);
    LOG_SERIAL_F("Touch carregado do NVS");
  } else {
    // Valores padrão para CYD (podem variar por unidade)
    uint16_t calData[5] = {275, 3620, 264, 3532, 1};
    tft.setTouch(calData);
    LOG_SERIAL_F("Touch usando defaults (recomendado calibrar)");
  }
}

// ============================================================================
// ATUALIZAÇÃO (chamada a cada loop)
// ============================================================================
void buttons_update() {
  // Resetar flags de ação
  actionUp    = false;
  actionDown  = false;
  actionLeft  = false;
  actionRight = false;
  actionOk    = false;
  actionBack  = false;

  // --- Leitura do Touchscreen ---
  uint16_t tx = 0, ty = 0;
  bool pressed = tft.getTouch(&tx, &ty, TOUCH_THRESHOLD);

  if (pressed && !touchWasPressed) {
    // Verificar debounce
    if (currentMillis - lastTouchTime > TOUCH_DEBOUNCE_MS) {
      lastTouchTime = currentMillis;

      // Classificar o toque em uma zona
      if (ty < ZONE_BACK_H && tx < ZONE_BACK_W) {
        // Canto inferior esquerdo = BACK
        actionBack = true;
      } else if (ty < ZONE_TOP) {
        // Zona superior = UP (nota: no TFT_eSPI com rotação, y pode ser invertido)
        actionDown = true;
      } else if (ty > (SCREEN_HEIGHT - ZONE_BOTTOM)) {
        // Zona inferior = DOWN
        actionUp = true;
      } else if (tx < ZONE_MARGIN) {
        // Zona esquerda = LEFT
        actionLeft = true;
      } else if (tx > (SCREEN_WIDTH - ZONE_MARGIN)) {
        // Zona direita = RIGHT
        actionRight = true;
      } else {
        // Centro = OK
        actionOk = true;
      }
    }
  }

  touchWasPressed = pressed;
}

// ============================================================================
// FUNÇÕES DE QUERY (consumidas uma vez por chamada)
// ============================================================================
bool isUpPressed() {
  if (actionUp) {
    actionUp = false;
    return true;
  }
  return false;
}

bool isDownPressed() {
  if (actionDown) {
    actionDown = false;
    return true;
  }
  return false;
}

bool isLeftPressed() {
  if (actionLeft) {
    actionLeft = false;
    return true;
  }
  return false;
}

bool isRightPressed() {
  if (actionRight) {
    actionRight = false;
    return true;
  }
  return false;
}

bool isOkPressed() {
  if (actionOk) {
    actionOk = false;
    return true;
  }
  return false;
}

bool isBackPressed() {
  if (actionBack) {
    actionBack = false;
    return true;
  }
  return false;
}

// ============================================================================
// CALIBRAÇÃO DO TOUCHSCREEN
// ============================================================================
void calibrate_touch() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 20);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println(F("Calibracao do Touch"));
  tft.println(F("Toque nos cantos indicados"));

  // Função nativa do TFT_eSPI para calibração
  tft.calibrateTouch(deviceSettings.touchCalibration, TFT_MAGENTA, TFT_BLACK, 15);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(20, 100);
  tft.println(F("Calibracao concluida!"));
  tft.println(F("Dados salvos no NVS."));
  
  saveSettings();
  delay(2000);
  
  currentAppState = STATE_SETTINGS;
}
