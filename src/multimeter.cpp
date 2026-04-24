// ============================================================================
// Component Tester PRO v3.0 — Módulo Multímetro AC/DC (CYD Edition)
// ============================================================================
// Implementação dos modos de multímetro:
//   1. Tensão AC (ZMPT101B) — True RMS com amostragem de 1000 pontos
//   2. Tensão DC (INA219)   — Leitura do barramento I2C
//   3. Corrente DC (INA219) — Via resistor shunt
//   4. Potência DC (INA219) — Cálculo P = V × I
// ============================================================================

#include <Arduino.h>
#include "multimeter.h"
#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "drawings.h"
#include "globals.h"
#include "leds.h"
#include "utils.h"
#include "menu.h"
#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <math.h>

extern TFT_eSPI tft;

// ============================================================================
// OBJETOS DOS SENSORES
// ============================================================================
// INA219 no barramento I2C secundário (GPIO 27 SDA, GPIO 22 SCL)
Adafruit_INA219 ina219(INA219_I2C_ADDR);
bool ina219_ok = false; // Flag de inicialização bem-sucedida

// ============================================================================
// VARIÁVEIS INTERNAS
// ============================================================================
static int currentMultiMode = 0; // Modo atual do multímetro

// Nomes dos modos para o menu
static const char* multiModeNames[] = {
  "Tensao AC (RMS)",
  "Tensao DC",
  "Corrente DC",
  "Potencia DC"
};

// ============================================================================
// INICIALIZAÇÃO
// ============================================================================
void multimeter_init() {
  // Inicializa o I2C com pinos customizados da CYD
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  // Tenta inicializar o INA219
  if (ina219.begin()) {
    ina219_ok = true;
    // Configuração de alta sensibilidade (±0.4A, ±16V)
    ina219.setCalibration_16V_400mA();
    LOG_SERIAL_F("INA219 OK");
  } else {
    ina219_ok = false;
    LOG_SERIAL_F("INA219 ERRO! Verificar conexao I2C");
  }
}

// ============================================================================
// MENU DE SELEÇÃO DO MODO
// ============================================================================
void draw_multimeter_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Multimetro", UI_COLOR_YELLOW);

  tft.setTextSize(1);
  for (int i = 0; i < MULTI_MODE_COUNT; i++) {
    int y = 65 + i * 24;

    if (i == currentMultiMode) {
      tft.fillRoundRect(15, y - 4, tft.width() - 30, 20, 4, UI_COLOR_HILIGHT);
      tft.setTextColor(UI_COLOR_TEXT);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }

    tft.setCursor(25, y);
    tft.print(i + 1);
    tft.print(F(". "));
    tft.print(multiModeNames[i]);

    // Indicador de status do INA219 nos modos DC
    if (i >= MULTI_DC_VOLTAGE && !ina219_ok) {
      tft.setTextColor(UI_COLOR_RED);
      tft.print(F(" [N/C]")); // Não conectado
    }
  }

  // Instrução de navegação
  tft.setCursor(20, tft.height() - 45);
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setTextSize(1);
  tft.print(F("UP/DOWN: Navegar | OK: Medir | <: Voltar"));

  draw_footer_modern();
}

// ============================================================================
// HANDLER PRINCIPAL (chamado no loop)
// ============================================================================
void multimeter_handle() {
  buttons_update();

  if (isUpPressed()) {
    currentMultiMode = (currentMultiMode - 1 + MULTI_MODE_COUNT) % MULTI_MODE_COUNT;
    draw_multimeter_menu();
    play_beep(50);
  }
  if (isDownPressed()) {
    currentMultiMode = (currentMultiMode + 1) % MULTI_MODE_COUNT;
    draw_multimeter_menu();
    play_beep(50);
  }

  if (isOkPressed()) {
    play_beep(100);
    switch (currentMultiMode) {
      case MULTI_AC_VOLTAGE: measure_ac_voltage(); break;
      case MULTI_DC_VOLTAGE: measure_dc_voltage(); break;
      case MULTI_DC_CURRENT: measure_dc_current(); break;
      case MULTI_DC_POWER:   measure_dc_power();   break;
    }
    draw_multimeter_menu(); // Volta ao menu após medição
  }

  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// ============================================================================
// 1. MEDIÇÃO DE TENSÃO AC — ZMPT101B (True RMS)
// ============================================================================
// Algoritmo True RMS:
//   1. Amostrar N pontos do ADC durante ~2 ciclos de 60Hz (~33ms)
//   2. Subtrair o offset DC (ponto zero = meio da escala ADC)
//   3. Calcular Vrms = sqrt(sum(Vi²)/N) * fator_de_escala
// ============================================================================
void measure_ac_voltage() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Voltimetro AC", UI_COLOR_YELLOW);

  tft.setCursor(30, 70);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.setTextSize(1);
  tft.println(F("Amostrando sinal AC..."));

  // Configurar ADC para alta velocidade
  analogReadResolution(ADC_RESOLUTION);

  bool measuring = true;
  while (measuring) {
    // --- Amostragem True RMS ---
    float sumSquares = 0.0f;
    int numSamples = ZMPT_SAMPLES;

    for (int i = 0; i < numSamples; i++) {
      int raw = analogRead(PIN_ZMPT_AC);
      float centered = (float)(raw - ZMPT_ZERO_POINT); // Remover offset DC
      sumSquares += centered * centered;
      delayMicroseconds(20); // ~50kHz de amostragem
    }

    // Cálculo do RMS
    float rmsRaw = sqrt(sumSquares / (float)numSamples);
    float vrms = rmsRaw * deviceSettings.zmptScaleFactor / (float)ADC_MAX_VALUE;

    // Tensão de pico
    float vpeak = vrms * 1.414f;

    // --- Exibição na tela ---
    tft.fillRect(20, 65, 280, 130, UI_COLOR_BG);

    // Valor principal (grande)
    tft.setCursor(30, 80);
    tft.setTextSize(4);
    if (vrms > 1.0f) {
      tft.setTextColor(UI_COLOR_YELLOW);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }
    fprint(tft, vrms, 1);
    tft.setTextSize(2);
    tft.print(F(" Vac"));

    // Indicador "True RMS"
    tft.setCursor(30, 120);
    tft.setTextSize(1);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(F("True RMS | Vpk: "));
    fprint(tft, vpeak, 1);
    tft.print(F("V"));

    // Frequência estimada (aproximada)
    tft.setCursor(30, 140);
    tft.setTextColor(UI_COLOR_GREY);
    tft.print(F("Sensor: ZMPT101B | GPIO "));
    tft.print(PIN_ZMPT_AC);

    // Ícone de onda senoidal
    for (int i = 0; i < 60; i++) {
      int px = 230 + i;
      int py = 90 + (int)(15.0f * sin(i * 0.2f));
      tft.drawPixel(px, py, UI_COLOR_YELLOW);
    }

    // Label RMS
    tft.setCursor(230, 115);
    tft.setTextColor(UI_COLOR_YELLOW);
    tft.print(F("~AC~"));

    draw_footer_modern();

    // Adicionar ao histórico
    addToHistory("VacRMS", vrms, currentTemperature, vrms > 0.5f);

    // Aguardar input (loop de atualização contínua)
    unsigned long refreshTimer = millis();
    while (millis() - refreshTimer < 1000) {
      buttons_update();
      if (isBackPressed()) {
        measuring = false;
        break;
      }
      delay(10);
    }
  }
}

// ============================================================================
// 2. MEDIÇÃO DE TENSÃO DC — INA219 (Bus Voltage)
// ============================================================================
void measure_dc_voltage() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Voltimetro DC", UI_COLOR_GREEN);

  if (!ina219_ok) {
    tft.setCursor(30, 90);
    tft.setTextColor(UI_COLOR_RED);
    tft.setTextSize(2);
    tft.println(F("INA219 N/C!"));
    tft.setTextSize(1);
    tft.setCursor(30, 120);
    tft.println(F("Verifique conexao I2C"));
    tft.setCursor(30, 135);
    tft.print(F("SDA: GPIO ")); tft.print(PIN_I2C_SDA);
    tft.print(F(" | SCL: GPIO ")); tft.println(PIN_I2C_SCL);

    // Fallback: leitura via ADC direto no probe
    tft.setCursor(30, 160);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.println(F("Usando ADC direto (Probe)..."));

    int raw = analogRead(PIN_PROBE_MAIN);
    float voltage = ADC_TO_VOLTS(raw);

    tft.setCursor(30, 180);
    tft.setTextSize(3);
    tft.setTextColor(UI_COLOR_GREEN);
    fprint(tft, voltage, 3);
    tft.setTextSize(2);
    tft.print(F(" V"));

    addToHistory("Vdc", voltage, currentTemperature, true);
    draw_footer_modern();

    // Aguardar voltar
    while (!isBackPressed() && !isOkPressed()) {
      buttons_update();
      delay(10);
    }
    return;
  }

  // --- Leitura contínua via INA219 ---
  bool measuring = true;
  while (measuring) {
    float busVoltage = ina219.getBusVoltage_V();
    float shuntVoltage = ina219.getShuntVoltage_mV();
    float loadVoltage = busVoltage + (shuntVoltage / 1000.0f);

    tft.fillRect(20, 65, 280, 130, UI_COLOR_BG);

    // Valor principal
    tft.setCursor(30, 80);
    tft.setTextSize(4);
    tft.setTextColor(UI_COLOR_GREEN);
    fprint(tft, loadVoltage, 3);
    tft.setTextSize(2);
    tft.print(F(" Vdc"));

    // Detalhes
    tft.setTextSize(1);
    tft.setCursor(30, 125);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(F("Bus: ")); fprint(tft, busVoltage, 3); tft.print(F("V"));
    tft.setCursor(30, 140);
    tft.print(F("Shunt: ")); fprint(tft, shuntVoltage, 2); tft.print(F("mV"));

    // Indicador visual (barra)
    int barW = (int)(loadVoltage * 16.0f); // 16V = barra cheia (256px)
    if (barW > 256) barW = 256;
    tft.drawRect(30, 165, 260, 12, UI_COLOR_GREY);
    tft.fillRect(31, 166, barW, 10, UI_COLOR_GREEN);
    tft.setCursor(30, 180);
    tft.setTextColor(UI_COLOR_GREY);
    tft.print(F("0V            8V           16V"));

    draw_footer_modern();

    addToHistory("Vdc", loadVoltage, currentTemperature, true);

    // Refresh a cada 500ms
    unsigned long refreshTimer = millis();
    while (millis() - refreshTimer < 500) {
      buttons_update();
      if (isBackPressed()) {
        measuring = false;
        break;
      }
      delay(10);
    }
  }
}

// ============================================================================
// 3. MEDIÇÃO DE CORRENTE DC — INA219 (Shunt Current)
// ============================================================================
void measure_dc_current() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Amperimetro DC", UI_COLOR_ACCENT);

  if (!ina219_ok) {
    tft.setCursor(30, 90);
    tft.setTextColor(UI_COLOR_RED);
    tft.setTextSize(2);
    tft.println(F("INA219 N/C!"));
    tft.setTextSize(1);
    tft.setCursor(30, 120);
    tft.println(F("Modulo INA219 necessario"));
    draw_footer_modern();

    while (!isBackPressed() && !isOkPressed()) {
      buttons_update();
      delay(10);
    }
    return;
  }

  bool measuring = true;
  while (measuring) {
    float current_mA = ina219.getCurrent_mA();
    float current_A = current_mA / 1000.0f;

    tft.fillRect(20, 65, 280, 130, UI_COLOR_BG);

    // Valor principal
    tft.setCursor(30, 80);
    tft.setTextSize(4);

    if (abs(current_mA) < 1.0f) {
      // µA range
      tft.setTextColor(UI_COLOR_GREY);
      fprint(tft, current_mA * 1000.0f, 0);
      tft.setTextSize(2);
      tft.print(F(" uA"));
    } else if (abs(current_mA) < 1000.0f) {
      // mA range
      tft.setTextColor(UI_COLOR_ACCENT);
      fprint(tft, current_mA, 2);
      tft.setTextSize(2);
      tft.print(F(" mA"));
    } else {
      // A range
      tft.setTextColor(UI_COLOR_RED);
      fprint(tft, current_A, 3);
      tft.setTextSize(2);
      tft.print(F(" A"));
    }

    // Direção da corrente
    tft.setTextSize(1);
    tft.setCursor(30, 130);
    tft.setTextColor(UI_COLOR_ACCENT);
    if (current_mA >= 0) {
      tft.print(F("Direcao: >>> (positiva)"));
    } else {
      tft.print(F("Direcao: <<< (reversa)"));
    }

    // Indicador de alerta
    tft.setCursor(30, 150);
    if (abs(current_mA) > 350.0f) {
      tft.setTextColor(UI_COLOR_RED);
      tft.print(F("ALERTA: Proximo ao limite!"));
      set_red_led(true);
    } else {
      tft.setTextColor(UI_COLOR_GREEN);
      tft.print(F("Status: Dentro da faixa"));
      set_green_led(true);
    }

    draw_footer_modern();

    addToHistory("Idc", current_mA, currentTemperature, abs(current_mA) < 400.0f);

    unsigned long refreshTimer = millis();
    while (millis() - refreshTimer < 500) {
      buttons_update();
      if (isBackPressed()) {
        measuring = false;
        break;
      }
      delay(10);
    }
  }
  set_red_led(false);
  set_green_led(false);
}

// ============================================================================
// 4. MEDIÇÃO DE POTÊNCIA DC — INA219 (Power = V × I)
// ============================================================================
void measure_dc_power() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card("Wattimetro DC", UI_COLOR_ORANGE);

  if (!ina219_ok) {
    tft.setCursor(30, 90);
    tft.setTextColor(UI_COLOR_RED);
    tft.setTextSize(2);
    tft.println(F("INA219 N/C!"));
    draw_footer_modern();

    while (!isBackPressed() && !isOkPressed()) {
      buttons_update();
      delay(10);
    }
    return;
  }

  bool measuring = true;
  while (measuring) {
    float busVoltage = ina219.getBusVoltage_V();
    float current_mA = ina219.getCurrent_mA();
    float power_mW = ina219.getPower_mW();
    float shuntVoltage = ina219.getShuntVoltage_mV();
    float loadVoltage = busVoltage + (shuntVoltage / 1000.0f);

    tft.fillRect(20, 65, 280, 135, UI_COLOR_BG);

    // Potência (valor principal)
    tft.setCursor(30, 75);
    tft.setTextSize(3);
    tft.setTextColor(UI_COLOR_ORANGE);
    if (power_mW >= 1000.0f) {
      fprint(tft, power_mW / 1000.0f, 3);
      tft.setTextSize(2);
      tft.print(F(" W"));
    } else {
      fprint(tft, power_mW, 1);
      tft.setTextSize(2);
      tft.print(F(" mW"));
    }

    // Detalhes (tensão e corrente)
    tft.setTextSize(2);
    tft.setCursor(30, 110);
    tft.setTextColor(UI_COLOR_GREEN);
    fprint(tft, loadVoltage, 2);
    tft.setTextSize(1);
    tft.print(F(" V"));

    tft.setTextSize(2);
    tft.setCursor(160, 110);
    tft.setTextColor(UI_COLOR_ACCENT);
    fprint(tft, current_mA, 1);
    tft.setTextSize(1);
    tft.print(F(" mA"));

    // Fórmula visual
    tft.setCursor(30, 145);
    tft.setTextColor(UI_COLOR_GREY);
    tft.setTextSize(1);
    tft.print(F("P = V x I  |  Sensor: INA219"));

    // Mini gráfico de barras (V e I lado a lado)
    int vBar = (int)(loadVoltage * 8.0f);  // 16V = 128px
    int iBar = (int)(current_mA * 0.32f);  // 400mA = 128px
    if (vBar > 128) vBar = 128;
    if (iBar > 128) iBar = 128;

    tft.setCursor(30, 165); tft.setTextColor(UI_COLOR_GREEN); tft.print(F("V:"));
    tft.drawRect(50, 163, 130, 10, UI_COLOR_GREY);
    tft.fillRect(51, 164, vBar, 8, UI_COLOR_GREEN);

    tft.setCursor(30, 180); tft.setTextColor(UI_COLOR_ACCENT); tft.print(F("I:"));
    tft.drawRect(50, 178, 130, 10, UI_COLOR_GREY);
    tft.fillRect(51, 179, iBar, 8, UI_COLOR_ACCENT);

    draw_footer_modern();

    addToHistory("Pdc", power_mW, currentTemperature, true);

    unsigned long refreshTimer = millis();
    while (millis() - refreshTimer < 500) {
      buttons_update();
      if (isBackPressed()) {
        measuring = false;
        break;
      }
      delay(10);
    }
  }
}
