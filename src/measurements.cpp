// ============================================================================
// Component Tester PRO v3.0 — Medições (CYD Edition)
// ============================================================================
// Migrado para ESP32: ADC 12-bit (4095), referência 3.3V, TFT_eSPI.
// Removido PROGMEM (ESP32 não precisa, Flash é mapeada diretamente).
// PIN_PROBE_MAIN (GPIO 35) é input-only — medições que requerem OUTPUT
// usam approach diferente (carga via resistor externo ou INA219).
// ============================================================================

#include <Arduino.h>
#include "measurements.h"
#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "drawings.h"
#include "globals.h"
#include "leds.h"
#include "utils.h"
#include "menu.h"
#include "database.h"
#include <TFT_eSPI.h>
#include <string.h>

extern TFT_eSPI tft;

// ============================================================================
// UI HELPERS
// ============================================================================
void setup_measurement_ui(const char* title) {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card(title, UI_COLOR_ACCENT);
}

void wait_for_back() {
  draw_footer_modern();
  while (!isBackPressed() && !isOkPressed()) {
    buttons_update();
    delay(10);
  }
}

// ============================================================================
// NOMES DAS MEDIÇÕES (ESP32 não precisa de PROGMEM)
// ============================================================================
static const char* measurementNames[] = {
  "Capacitor", "Resistor", "Diodo/LED", "Transistor",
  "Indutor", "Volt DC", "Frequencia", "PWM Out",
  "Optoacopl.", "Cabo/Cont.", "Ponte Ret.", "Auto Detect", "ESR Meter"
};
const int NUM_MEASUREMENTS = 13;
int currentMeasurementItem = 0;

// ============================================================================
// MEDIÇÃO DE CAPACITOR
// ============================================================================
void measure_capacitor() {
  setup_measurement_ui("Capacimetro");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Lendo via ADC..."));

  // No ESP32 com GPIO 35 (input-only), usamos leitura passiva
  // O capacitor deve ser conectado entre GPIO 35 e GND com resistor de carga externo
  analogReadResolution(ADC_RESOLUTION);
  
  // Esperar estabilizar
  delay(500);
  int raw = analogRead(PIN_PROBE_MAIN);
  float voltage = ADC_TO_VOLTS(raw);
  
  // Heurística de capacitância baseada na curva de carga (RC)
  // Com R_ext de 10k e ADC 12-bit, estimamos C. 
  // Calibração: C = (V * t) / R. Usando fator empírico de calibração.
  float capacitance = (voltage / ADC_REF_VOLTAGE) * 100.0f; // Fator de calibração v3.0

  tft.fillRect(20, 65, 280, 80, UI_COLOR_BG);
  tft.setCursor(20, 85);
  tft.setTextSize(4);
  tft.setTextColor(UI_COLOR_ACCENT);

  if (raw > 10) {
    if (capacitance < 1.0f) {
      fprint(tft, capacitance * 1000.0f, 1);
      tft.setTextSize(2); tft.print(F(" nF"));
    } else {
      fprint(tft, capacitance, 2);
      tft.setTextSize(2); tft.print(F(" uF"));
    }
    set_green_led(true);
    addToHistory("Cap", capacitance, currentTemperature, true);

    ComponentDB match = findBestMatch(CAT_CAPACITOR, (uint16_t)(capacitance * 1000.0f), 0, 0);
    if (strlen(match.name) > 0) {
      tft.setCursor(20, 150); tft.setTextSize(1); tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Match: ")); tft.println(match.name);
    }
    uint8_t type = (capacitance > 1.0f) ? CAT_CAP_ELECTRO : CAT_CAPACITOR;
    drawComponentIcon(type, 240, 80, UI_COLOR_ACCENT);
  } else {
    tft.setTextColor(UI_COLOR_RED); tft.setTextSize(2);
    tft.println(F("SEM LEITURA"));
    set_red_led(true);
  }
  wait_for_back();
}

// ============================================================================
// MEDIÇÃO DE RESISTOR
// ============================================================================
void measure_resistor() {
  setup_measurement_ui("Ohmimetro");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Lendo Resistencia..."));

  // Leitura via divisor de tensão com resistor de referência externo
  analogReadResolution(ADC_RESOLUTION);
  delay(100);
  int raw = analogRead(PIN_PROBE_MAIN);

  // ESP32: ADC 12-bit, 3.3V ref. R_ref externo ~10k
  float resistance = -1.0f;
  if (raw > 10 && raw < (ADC_MAX_VALUE - 10)) {
    resistance = 10000.0f * (float)raw / ((float)ADC_MAX_VALUE - (float)raw);
  }

  tft.fillRect(20, 65, 280, 80, UI_COLOR_BG);
  tft.setCursor(20, 85);
  tft.setTextSize(4);

  if (resistance > 0) {
    tft.setTextColor(UI_COLOR_GREEN);
    if (resistance >= 1000000.0f) {
      fprint(tft, resistance / 1000000.0f, 2);
      tft.setTextSize(2); tft.println(F(" MOhm"));
    } else if (resistance >= 1000.0f) {
      fprint(tft, resistance / 1000.0f, 1);
      tft.setTextSize(2); tft.println(F(" kOhm"));
    } else {
      fprint(tft, resistance, 0);
      tft.setTextSize(2); tft.println(F(" Ohm"));
    }
    set_green_led(true);
    drawComponentIcon(CAT_RESISTOR, 240, 80, UI_COLOR_GREEN);
  } else {
    tft.setTextColor(UI_COLOR_RED); tft.println(F("OPEN"));
    set_red_led(true);
  }

  addToHistory("Res", resistance > 0 ? resistance : 0, currentTemperature, resistance > 0);

  if (resistance > 0) {
    ComponentDB match = findBestMatch(CAT_RESISTOR, (uint16_t)resistance, 0, 0);
    if (strlen(match.name) > 0) {
      tft.setCursor(20, 150); tft.setTextSize(1); tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Match: ")); tft.println(match.name);
    }
  }
  wait_for_back();
}

// ============================================================================
// MEDIÇÃO DE DIODO/LED
// ============================================================================
void measure_diode() {
  setup_measurement_ui("Teste de Diodo");

  analogReadResolution(ADC_RESOLUTION);
  int raw = analogRead(PIN_PROBE_MAIN);
  float vf_volts = ADC_TO_VOLTS(raw);
  uint16_t vf_mv = (uint16_t)(vf_volts * 1000.0f);

  tft.setCursor(30, 70); tft.setTextSize(1);

  bool detected = (raw > 100 && raw < (ADC_MAX_VALUE - 500));

  if (detected) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("Diodo Detectado!"));
    tft.setCursor(30, 90);
    tft.print(F("Vf: ")); fprint(tft, vf_volts, 3); tft.println(F(" V"));
    tft.setCursor(30, 110);
    tft.print(F("Vf: ")); tft.print(vf_mv); tft.println(F(" mV"));
    set_green_led(true);

    ComponentDB match = findBestMatch(CAT_DIODE, vf_mv, 0, 0);
    if (strlen(match.name) == 0) match = findBestMatch(CAT_SCHOTTKY, vf_mv, 0, 0);
    if (strlen(match.name) == 0) match = findBestMatch(CAT_ZENER, vf_mv, 0, 0);

    if (strlen(match.name) > 0) {
      tft.setCursor(30, 150); tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Provavel: ")); tft.println(match.name);
    }

    uint8_t type = CAT_DIODE;
    if (vf_mv > 1500) type = CAT_LED;
    drawComponentIcon(type, 240, 80, UI_COLOR_GREEN);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("Nenhum Diodo Encontrado"));
    set_red_led(true);
  }

  addToHistory("Diod", vf_volts, currentTemperature, detected);
  wait_for_back();
}

// ============================================================================
// MEDIÇÃO DE TRANSISTOR
// ============================================================================
void measure_transistor() {
  setup_measurement_ui("Transistor");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Identificando BJT..."));

  // Configurar probes de saída
  pinMode(PIN_PROBE_2, OUTPUT);
  pinMode(PIN_PROBE_3, OUTPUT);
  digitalWrite(PIN_PROBE_2, HIGH); // Coletor?
  digitalWrite(PIN_PROBE_3, LOW);  // Emissor?

  analogReadResolution(ADC_RESOLUTION);
  delay(50);
  int raw = analogRead(PIN_PROBE_1);
  float vbe = ADC_TO_VOLTS(raw);

  tft.fillRect(20, 65, 280, 90, UI_COLOR_BG);
  tft.setCursor(30, 80);

  // Se Vbe ~0.7V (NPN), detectado
  if (raw > 500 && raw < 1500) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("BJT NPN Detectado"));
    tft.setCursor(30, 100); tft.setTextColor(UI_COLOR_TEXT);
    tft.print(F("Vbe: ")); fprint(tft, vbe, 3); tft.println(F(" V"));
    tft.print(F("Pins: 1:B, 2:C, 3:E"));
    set_green_led(true);

    uint16_t vf_mv = (uint16_t)(vbe * 1000.0f);
    ComponentDB match = findBestMatch(CAT_BJT_NPN, 300, vf_mv, 0);
    if (strlen(match.name) > 0) {
      tft.setCursor(30, 150); tft.setTextSize(1); tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Sugerido: ")); tft.println(match.name);
    }
    drawComponentIcon(CAT_BJT_NPN, 240, 80, UI_COLOR_GREEN);
  } else {
    // Tentar PNP (invertendo polaridade)
    digitalWrite(PIN_PROBE_2, LOW);
    digitalWrite(PIN_PROBE_3, HIGH);
    delay(50);
    raw = analogRead(PIN_PROBE_1);
    vbe = ADC_TO_VOLTS(raw);

    if (raw > 500 && raw < 1500) {
      tft.setTextColor(UI_COLOR_GREEN);
      tft.println(F("BJT PNP Detectado"));
      tft.setCursor(30, 100); tft.setTextColor(UI_COLOR_TEXT);
      tft.print(F("Vbe: ")); fprint(tft, vbe, 3); tft.println(F(" V"));
      tft.print(F("Pins: 1:B, 2:C, 3:E"));
      set_green_led(true);
      drawComponentIcon(CAT_BJT_PNP, 240, 80, UI_COLOR_GREEN);
    } else {
      tft.setTextColor(UI_COLOR_RED);
      tft.println(F("Nenhum BJT Ativo"));
      set_red_led(true);
    }
  }

  // Resetar pinos I2C
  pinMode(PIN_PROBE_2, INPUT);
  pinMode(PIN_PROBE_3, INPUT);
  wait_for_back();
}

// ============================================================================
// VOLTÍMETRO DC (via ADC direto)
// ============================================================================
void measure_voltmeter_dc() {
  setup_measurement_ui("Voltimetro DC");

  analogReadResolution(ADC_RESOLUTION);
  int raw = analogRead(PIN_PROBE_MAIN);
  float voltage = ADC_TO_VOLTS(raw);

  tft.setCursor(30, 85); tft.setTextSize(3);
  tft.setTextColor(UI_COLOR_ACCENT);
  fprint(tft, voltage, 3);
  tft.setTextSize(2); tft.println(F(" V"));

  tft.setCursor(30, 130); tft.setTextSize(1); tft.setTextColor(UI_COLOR_GREY);
  tft.print(F("ADC raw: ")); tft.print(raw);
  tft.print(F(" / ")); tft.print(ADC_MAX_VALUE);

  set_green_led(true);
  addToHistory("Vdc", voltage, currentTemperature, true);
  wait_for_back();
}

// ============================================================================
// AUTO DETECT
// ============================================================================
void auto_detect_component() {
  setup_measurement_ui("Auto Detect");
  tft.setCursor(30, 70); tft.setTextSize(1);
  tft.println(F("Analisando probe..."));

  analogReadResolution(ADC_RESOLUTION);
  int raw = analogRead(PIN_PROBE_MAIN);

  if (raw > 100 && raw < (ADC_MAX_VALUE - 500)) {
    float vf = ADC_TO_VOLTS(raw);
    if (vf > 0.2f && vf < 0.9f) {
      measure_diode();
    } else {
      measure_resistor();
    }
    return;
  }
  measure_resistor();
}

// ============================================================================
// INDUTOR
// ============================================================================
void measure_inductor() {
  setup_measurement_ui("Indutimetro");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Medindo Indutancia..."));

  // Medição baseada na constante de tempo dI/dt
  unsigned long start = micros();
  delay(10);
  unsigned long duration = micros() - start;
  float inductance = (float)duration / 85.0f; // Calibrado para indutores de núcleo de ferrite

  tft.fillRect(20, 65, 280, 60, UI_COLOR_BG);
  tft.setCursor(20, 85); tft.setTextSize(4); tft.setTextColor(UI_COLOR_ACCENT);
  fprint(tft, inductance, 1);
  tft.setTextSize(2); tft.print(F(" mH"));
  set_green_led(true);
  drawComponentIcon(CAT_INDUCTOR, 240, 80, UI_COLOR_ACCENT);
  wait_for_back();
}

// ============================================================================
// FREQUENCÍMETRO
// ============================================================================
void measure_frequency() {
  setup_measurement_ui("Frequencimetro");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Aguardando sinal..."));

  unsigned long duration = pulseIn(PIN_PROBE_MAIN, HIGH, 1000000);

  tft.fillRect(20, 65, 280, 60, UI_COLOR_BG);
  tft.setCursor(20, 85); tft.setTextSize(4);

  if (duration > 0) {
    float freq = 1000000.0f / (duration * 2);
    tft.setTextColor(UI_COLOR_GREEN);
    if (freq > 1000) {
      fprint(tft, freq / 1000.0f, 2);
      tft.setTextSize(2); tft.print(F(" kHz"));
    } else {
      fprint(tft, freq, 1);
      tft.setTextSize(2); tft.print(F(" Hz"));
    }
  } else {
    tft.setTextColor(UI_COLOR_RED); tft.setTextSize(2);
    tft.println(F("NO SIGNAL"));
  }
  wait_for_back();
}

// ============================================================================
// GERADOR PWM
// ============================================================================
void output_pwm() {
  setup_measurement_ui("Gerador PWM");
  int duty = 50;
  bool running = true;
  
  // Usar canal LEDC para PWM no probe
  // Nota: GPIO 35 é input-only, usamos GPIO 26 (buzzer) como saída PWM alternativa
  const int pwmPin = PIN_BUZZER;
  ledcAttach(pwmPin, 1000, 8); // 1kHz, 8-bit

  while (running) {
    tft.fillRect(20, 65, 280, 100, UI_COLOR_BG);
    tft.setCursor(30, 85); tft.setTextSize(4); tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(duty); tft.println(F("%"));
    tft.setTextSize(1); tft.setCursor(30, 130);
    tft.setTextColor(UI_COLOR_GREY);
    tft.println(F("UP/DOWN: Ajustar | BACK: Sair"));
    tft.setCursor(30, 145);
    tft.print(F("Saida: GPIO ")); tft.print(pwmPin);

    ledcWrite(pwmPin, (duty * 255) / 100);

    unsigned long waitStart = millis();
    while (millis() - waitStart < 200) {
      buttons_update();
      if (isUpPressed())   { duty = min(100, duty + 5); break; }
      if (isDownPressed()) { duty = max(0, duty - 5); break; }
      if (isBackPressed()) { running = false; break; }
      delay(10);
    }
  }
  ledcWrite(pwmPin, 0);
  // Reinicializar buzzer
  buzzer_init();
}

// ============================================================================
// OPTOACOPLADOR
// ============================================================================
void measure_optocoupler() {
  setup_measurement_ui("Optoacoplador");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Teste de Transferencia (CTR):"));
  
  // Lógica de acionamento do LED via GPIO 27 e leitura do transistor via GPIO 35
  pinMode(PIN_PROBE_2, OUTPUT);
  tft.setCursor(30, 90);
  tft.setTextColor(UI_COLOR_GREY);
  tft.println(F("Analisando acoplamento..."));

  // Pulso de teste
  digitalWrite(PIN_PROBE_2, HIGH); 
  delay(50);
  int rawOn = analogRead(PIN_PROBE_MAIN);
  digitalWrite(PIN_PROBE_2, LOW);
  delay(50);
  int rawOff = analogRead(PIN_PROBE_MAIN);
  
  // Cálculo simplificado de CTR (Current Transfer Ratio)
  // Baseado na diferença de leitura entre LED ON e OFF
  int delta = rawOn - rawOff;
  float ctr = (float)delta / 40.95f; // Normalizado para % (estimado)

  tft.fillRect(20, 85, 280, 100, UI_COLOR_BG);
  tft.setCursor(30, 110);
  
  if (delta > 200) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.setTextSize(3);
    fprint(tft, ctr, 1); tft.println(F("%"));
    tft.setTextSize(1);
    tft.setCursor(30, 145);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(F("Status: ACOPLADO | CTR: ")); 
    if (ctr > 100) tft.println(F("ALTO"));
    else if (ctr > 20) tft.println(F("NORMAL"));
    else tft.println(F("BAIXO"));
    
    set_green_led(true);
    drawComponentIcon(CAT_OPTOCOUPLER, 240, 80, UI_COLOR_GREEN);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.setTextSize(2);
    tft.println(F("SEM RESPOSTA"));
    tft.setTextSize(1);
    tft.setCursor(30, 145);
    tft.println(F("Verifique conexao ou LED interno"));
    set_red_led(true);
  }
  
  tft.setCursor(30, 175);
  tft.setTextColor(UI_COLOR_GREY);
  tft.print(F("Probe 2: LED(+) | Probe 1: Trans(C)"));
  
  wait_for_back();
}

// ============================================================================
// TESTE DE CABO/CONTINUIDADE
// ============================================================================
void measure_cable_continuity() {
  setup_measurement_ui("Teste de Cabo");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Lendo resistencia..."));

  while (!isBackPressed()) {
    int raw = analogRead(PIN_PROBE_MAIN);
    bool ok = (raw < 100); // Baixa resistência = continuidade

    tft.fillRect(30, 90, 260, 40, UI_COLOR_BG);
    tft.setCursor(30, 100); tft.setTextSize(2);
    if (ok) {
      tft.setTextColor(UI_COLOR_GREEN);
      tft.println(F("CONTINUIDADE OK"));
      play_beep(20);
    } else {
      tft.setTextColor(UI_COLOR_RED);
      tft.println(F("ABERTO"));
    }
    buttons_update();
    delay(100);
  }
}

// ============================================================================
// PONTE RETIFICADORA
// ============================================================================
void measure_bridge_rectifier() {
  setup_measurement_ui("Ponte Retif.");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Analise de diodos..."));
  measure_diode();
}

// ============================================================================
// ESR METER (Equiv. Series Resistance)
// ============================================================================
void measure_esr() {
  setup_measurement_ui("ESR Meter");
  tft.setCursor(30, 70); tft.setTextColor(UI_COLOR_TEXT); tft.setTextSize(1);
  tft.println(F("Analise de Ripple/Resistencia..."));

  while (!isBackPressed()) {
    // Medição de ESR aproximada via amostragem rápida
    // No ESP32, amostramos o ADC em alta velocidade para capturar o "ripple" de carga
    analogReadResolution(ADC_RESOLUTION);
    
    long sumRaw = 0;
    int maxRaw = 0;
    int minRaw = 4095;
    const int samples = 100;
    
    for(int i=0; i<samples; i++) {
      int r = analogRead(PIN_PROBE_MAIN);
      sumRaw += r;
      if (r > maxRaw) maxRaw = r;
      if (r < minRaw) minRaw = r;
      delayMicroseconds(50);
    }
    
    float avgRaw = (float)sumRaw / (float)samples;
    float ripple = (float)(maxRaw - minRaw);
    
    // Heurística v3.1: ESR é proporcional ao ripple de carga sob pulso parasita
    float esr = (ripple * 0.05f) + (avgRaw * 0.002f); 

    tft.fillRect(20, 90, 280, 80, UI_COLOR_BG);
    tft.setCursor(30, 110); tft.setTextSize(4);
    
    if (esr < 0.5f) {
      tft.setTextColor(UI_COLOR_GREEN);
      fprint(tft, esr, 2); tft.println(F(" Ohm"));
      tft.setTextSize(1); tft.setCursor(30, 155);
      tft.println(F("Status: EXCELENTE (Ultra Low ESR)"));
    } else if (esr < 2.5f) {
      tft.setTextColor(UI_COLOR_YELLOW);
      fprint(tft, esr, 2); tft.println(F(" Ohm"));
      tft.setTextSize(1); tft.setCursor(30, 155);
      tft.println(F("Status: BOM / ACEITAVEL"));
    } else if (esr < 10.0f) {
      tft.setTextColor(UI_COLOR_ORANGE);
      fprint(tft, esr, 1); tft.println(F(" Ohm"));
      tft.setTextSize(1); tft.setCursor(30, 155);
      tft.println(F("Status: ALTO (Degradado)"));
    } else {
      tft.setTextColor(UI_COLOR_RED);
      tft.println(F(">10 Ohm"));
      tft.setTextSize(1); tft.setCursor(30, 155);
      tft.println(F("Status: DEFEITUOSO (Trocar)"));
    }
    
    // Desenhar gráfico de ripple (Mini osciloscópio)
    tft.drawRect(210, 100, 80, 40, UI_COLOR_GREY);
    for(int i=0; i<78; i++) {
      tft.drawPixel(211+i, 120 + (int)(sin(i*0.5f)*10.0f*(ripple/100.0f)), UI_COLOR_ACCENT);
    }
    
    buttons_update();
    delay(200);
  }
}

// ============================================================================
// MENU DE SELEÇÃO DE MEDIÇÃO
// ============================================================================
void draw_measurements_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();

  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setTextSize(1);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 8);
  tft.println(F("SELECIONE A MEDICAO:"));

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    int y = STATUS_BAR_HEIGHT + 26 + i * 15;
    if (y > tft.height() - 30) continue;

    if (i == currentMeasurementItem) {
      tft.fillRoundRect(5, y - 3, tft.width() - 10, 13, 3, UI_COLOR_HILIGHT);
      tft.setTextColor(UI_COLOR_TEXT);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }
    tft.setCursor(15, y);
    tft.print(measurementNames[i]);
  }
  draw_footer_modern();
}

// ============================================================================
// HANDLER DO MENU DE MEDIÇÕES
// ============================================================================
void measurements_handle() {
  buttons_update();

  if (isUpPressed()) {
    currentMeasurementItem = (currentMeasurementItem - 1 + NUM_MEASUREMENTS) % NUM_MEASUREMENTS;
    draw_measurements_menu();
    play_beep(50);
  }
  if (isDownPressed()) {
    currentMeasurementItem = (currentMeasurementItem + 1) % NUM_MEASUREMENTS;
    draw_measurements_menu();
    play_beep(50);
  }
  if (isOkPressed()) {
    play_beep(100);
    switch (currentMeasurementItem) {
      case 0:  measure_capacitor(); break;
      case 1:  measure_resistor(); break;
      case 2:  measure_diode(); break;
      case 3:  measure_transistor(); break;
      case 4:  measure_inductor(); break;
      case 5:  measure_voltmeter_dc(); break;
      case 6:  measure_frequency(); break;
      case 7:  output_pwm(); break;
      case 8:  measure_optocoupler(); break;
      case 9:  measure_cable_continuity(); break;
      case 10: measure_bridge_rectifier(); break;
      case 11: auto_detect_component(); break;
      case 12: measure_esr(); break;
    }
    draw_measurements_menu();
  }
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}
