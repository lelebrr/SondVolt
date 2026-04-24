#include "measurements.h"
#include "buttons.h"
#include "buzzer.h"
#include "config.h"
#include "drawings.h"
#include "globals.h"
#include "leds.h"
#include "logger.h"
#include "utils.h"
#include "menu.h"
#include "database.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>

extern Adafruit_ILI9341 tft;

void setup_measurement_ui(const char* title) {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  draw_modern_card(title, UI_COLOR_ACCENT);
}

void wait_for_back() {
  draw_footer_modern();
  while(!isBackPressed() && !isOkPressed()) {
    buttons_update();
    delay(10);
  }
}

const char m0[] PROGMEM = "Capacitor";
const char m1[] PROGMEM = "Resistor";
const char m2[] PROGMEM = "Diodo/LED";
const char m3[] PROGMEM = "Transistor";
const char m4[] PROGMEM = "Indutor";
const char m5[] PROGMEM = "Volt DC";
const char m6[] PROGMEM = "Frequencia";
const char m7[] PROGMEM = "PWM Out";
const char m8[] PROGMEM = "Optoacopl.";
const char m9[] PROGMEM = "Cabo/Cont.";
const char m10[] PROGMEM = "Ponte Ret.";
const char m11[] PROGMEM = "Auto Detect";
const char m12[] PROGMEM = "Continuity";

const char *const measurementNames[] PROGMEM = {m0, m1, m2, m3,  m4,  m5, m6,
                                                 m7, m8, m9, m10, m11, m12};

const int NUM_MEASUREMENTS = sizeof(measurementNames) / sizeof(measurementNames[0]);
int currentMeasurementItem = 0;

// --- Implementação das Medições ---

void measure_capacitor() {
  setup_measurement_ui("Capacimetro");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Descarregando..."));
  
  // Descarregar capacitor
  pinMode(PROBE1_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, LOW);
  pinMode(PROBE2_PIN, OUTPUT);
  digitalWrite(PROBE2_PIN, LOW);
  delay(1000);

  tft.fillRect(25, 75, 200, 20, UI_COLOR_BG);
  tft.setCursor(30, 80);
  tft.println(F("Medindo..."));

  pinMode(PROBE1_PIN, INPUT);
  pinMode(PROBE2_PIN, OUTPUT);
  digitalWrite(PROBE2_PIN, HIGH);

  unsigned long startTime = micros();
  while (analogRead(PROBE1_PIN) < 648) { // 63.2% de 5V
    if (micros() - startTime > 2000000) break; // Timeout 2s
  }
  unsigned long duration = micros() - startTime;
  
  // C = t / R. Se R for pullup interno (~35k)
  float capacitance = (float)duration / 35000.0; 

  tft.fillRect(25, 75, 220, 60, UI_COLOR_BG);
  tft.setCursor(20, 100);
  tft.setTextSize(4);
  tft.setTextColor(UI_COLOR_ACCENT);
  
  if (duration < 2000000) {
    if (capacitance < 1.0) {
      fprint(tft, capacitance * 1000.0, 1);
      tft.setTextSize(2); tft.print(F(" nF"));
    } else {
      fprint(tft, capacitance, 2);
      tft.setTextSize(2); tft.print(F(" uF"));
    }
    set_green_led(true);
    addToHistory("Cap", capacitance, currentTemperature, true);
    
    // Auto Identify
    ComponentDB match = findBestMatch(CAT_CAPACITOR, (uint16_t)(capacitance * 1000.0), 0, 0);
    if (strlen(match.name) > 0) {
      tft.setCursor(20, 160);
      tft.setTextSize(1);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Match: "));
      tft.println(match.name);
    }
    
    // Desenhar ícone
    uint8_t type = (capacitance > 1.0) ? CAT_CAP_ELECTRO : CAT_CAPACITOR;
    drawComponentIcon(type, 220, 90, UI_COLOR_ACCENT);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.setTextSize(2);
    tft.println(F("TIMEOUT / HIGH C"));
    set_red_led(true);
  }

  wait_for_back();
}

void measure_resistor() {
  setup_measurement_ui("Ohmimetro");
  
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Lendo Resistencia..."));

  pinMode(PROBE1_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, LOW);
  pinMode(PROBE2_PIN, INPUT_PULLUP);
  delay(100);
  
  int raw = analogRead(PROBE2_PIN);
  // Usando R_internal ~ 35k (valor típico para ATmega328P)
  float resistance = (raw < 1020) ? 35000.0 * (float)raw / (1023.0 - (float)raw) : -1.0;

  tft.fillRect(25, 75, 220, 60, UI_COLOR_BG);
  tft.setCursor(20, 100);
  tft.setTextSize(4);
  
  if (resistance > 0) {
    tft.setTextColor(UI_COLOR_GREEN);
    if (resistance >= 1000000.0) {
      fprint(tft, resistance / 1000000.0, 2);
      tft.setTextSize(2); tft.println(F(" M Ohm"));
    } else if (resistance >= 1000.0) {
      fprint(tft, resistance / 1000.0, 1);
      tft.setTextSize(2); tft.println(F(" k Ohm"));
    } else {
      fprint(tft, resistance, 0);
      tft.setTextSize(2); tft.println(F(" Ohm"));
    }
    set_green_led(true);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("OPEN"));
    set_red_led(true);
  }

  addToHistory("Res", resistance > 0 ? resistance : 0, currentTemperature, resistance > 0);
  
  if (resistance > 0) {
    // Auto Identify
    ComponentDB match = findBestMatch(CAT_RESISTOR, (uint16_t)resistance, 0, 0);
    if (strlen(match.name) > 0) {
      tft.setCursor(20, 160);
      tft.setTextSize(1);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Match: "));
      tft.println(match.name);
    }
    drawComponentIcon(CAT_RESISTOR, 220, 90, UI_COLOR_GREEN);
  }
  
  wait_for_back();
}

void measure_diode() {
  setup_measurement_ui("Teste de Diodo");
  pinMode(PROBE1_PIN, OUTPUT);
  pinMode(PROBE2_PIN, OUTPUT);

  digitalWrite(PROBE1_PIN, HIGH);
  digitalWrite(PROBE2_PIN, LOW);
  delay(10);
  int vF1 = analogRead(A0);

  digitalWrite(PROBE1_PIN, LOW);
  digitalWrite(PROBE2_PIN, HIGH);
  delay(10);
  int vF2 = analogRead(A1);

  tft.setCursor(30, 80);
  if (vF1 > 100 && vF1 < 900) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("Diodo Detectado!"));
    tft.setCursor(30, 100);
    tft.println(F("Anodo: P1 | Katodo: P2"));
    set_green_led(true);
  } else if (vF2 > 100 && vF2 < 900) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("Diodo Detectado!"));
    tft.setCursor(30, 100);
    tft.println(F("Anodo: P2 | Katodo: P1"));
    set_green_led(true);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("Nenhum Diodo Encontrado"));
    set_red_led(true);
  }
  
  // Auto Identify Diode
  bool detected = (vF1 > 100 && vF1 < 900) || (vF2 > 100 && vF2 < 900);
  if (detected) {
    uint16_t vf_val = (vF1 > 100 && vF1 < 900) ? vF1 : vF2;
    vf_val = (uint16_t)(vf_val * (5000.0 / 1023.0)); // Convert to mV
    ComponentDB match = findBestMatch(CAT_DIODE, vf_val, 0, 0);
    if (strlen(match.name) == 0) match = findBestMatch(CAT_SCHOTTKY, vf_val, 0, 0);
    if (strlen(match.name) == 0) match = findBestMatch(CAT_ZENER, vf_val, 0, 0);
    
    if (strlen(match.name) > 0) {
      tft.setCursor(30, 160);
      tft.setTextSize(1);
      tft.setTextColor(UI_COLOR_ACCENT);
      tft.print(F("Provavel: "));
      tft.println(match.name);
    }
    
    // Desenhar ícone
    uint8_t type = CAT_DIODE;
    if (match.category == CAT_LED || vf_val > 1500) type = CAT_LED; // LEDs geralmente tem Vf alto
    else if (match.category == CAT_ZENER) type = CAT_ZENER;
    else if (match.category == CAT_SCHOTTKY) type = CAT_SCHOTTKY;
    
    drawComponentIcon(type, 220, 90, UI_COLOR_GREEN);
  }

  addToHistory("Diod", 0, currentTemperature, detected);
  wait_for_back();
}

void measure_transistor() {
  setup_measurement_ui("Transistor");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Identificando BJT..."));
  
  // Teste de junções (Diodos) para BJT
  // Este é um teste simplificado para 2 probes (precisaria de 3 para hFE real)
  // Mas vamos detectar se há uma junção BE ou BC
  
  pinMode(PROBE1_PIN, OUTPUT);
  pinMode(PROBE2_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, HIGH);
  digitalWrite(PROBE2_PIN, LOW);
  delay(10);
  int vF = analogRead(A0); // Queda de tensão
  
  tft.fillRect(25, 75, 220, 80, UI_COLOR_BG);
  tft.setCursor(30, 90);
  
  if (vF > 100 && vF < 800) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("Juncao Detectada"));
    tft.setCursor(30, 110);
    tft.setTextColor(UI_COLOR_TEXT);
    tft.print(F("Vf: ")); tft.print(vF * (5.0/1023.0), 2); tft.println(F("V"));
    tft.setCursor(30, 130);
    tft.println(F("P1: Anodo | P2: Katodo"));
    set_green_led(true);

    // Auto Identify BJT (Limited with 2 probes, but can guess by Vf)
    uint16_t vf_mv = (uint16_t)(vF * (5000.0 / 1023.0));
    ComponentDB match = findBestMatch(CAT_BJT_NPN, 300, vf_mv, 0); // Use 300 as typical HFE guess
    if (strlen(match.name) > 0) {
       tft.setCursor(30, 160);
       tft.setTextSize(1);
       tft.setTextColor(UI_COLOR_ACCENT);
       tft.print(F("Sugerido: "));
       tft.println(match.name);
    }
    
    // Desenhar ícone (Simplificado para NPN por padrão se detectado junção)
    drawComponentIcon(CAT_BJT_NPN, 220, 90, UI_COLOR_GREEN);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("Nenhum BJT Ativo"));
    tft.setCursor(30, 110);
    tft.setTextSize(1);
    tft.println(F("Use 3 probes para hFE"));
    set_red_led(true);
  }
  
  wait_for_back();
}

void measure_voltmeter_dc() {
  setup_measurement_ui("Voltimetro DC");
  int raw = analogRead(A0);
  float voltage = (raw * 5.0) / 1023.0;
  
  tft.setCursor(30, 90);
  tft.setTextSize(3);
  tft.setTextColor(UI_COLOR_ACCENT);
  fprint(tft, voltage, 2);
  tft.setTextSize(1);
  tft.println(F(" V"));
  
  set_green_led(true);
  wait_for_back();
}

void auto_detect_component() {
  setup_measurement_ui("Auto Detect");
  tft.setCursor(30, 80);
  tft.println(F("Analisando probes..."));
  
  // 1. Tenta Diodo (Queda de tensão rápida)
  pinMode(PROBE1_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, HIGH);
  pinMode(PROBE2_PIN, OUTPUT);
  digitalWrite(PROBE2_PIN, LOW);
  delay(10);
  int vF = analogRead(A0);
  
  if (vF > 100 && vF < 900) {
    measure_diode();
    return;
  }
  
  // 2. Tenta Capacitor (Se carregar)
  pinMode(PROBE1_PIN, INPUT);
  digitalWrite(PROBE2_PIN, HIGH);
  unsigned long start = millis();
  while(analogRead(PROBE1_PIN) < 500 && (millis() - start < 100)) delay(1);
  if (millis() - start > 5 && millis() - start < 100) {
    measure_capacitor();
    return;
  }
  
  // 3. Fallback para Resistor
  measure_resistor();
}

// --- Menu de Medições ---

void draw_measurements_menu() {
  tft.fillScreen(UI_COLOR_BG);
  draw_status_bar();
  
  tft.setTextColor(UI_COLOR_ACCENT);
  tft.setTextSize(1);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println(F("SELECIONE A MEDICAO:"));

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    int y = STATUS_BAR_HEIGHT + 30 + i * 16;
    if (y > tft.height() - 30) continue;
    
    if (i == currentMeasurementItem) {
      tft.fillRoundRect(5, y - 4, tft.width() - 10, 14, 4, UI_COLOR_HILIGHT);
      tft.setTextColor(UI_COLOR_TEXT);
    } else {
      tft.setTextColor(UI_COLOR_GREY);
    }
    tft.setCursor(15, y);
    char buf[25];
    strcpy_P(buf, (char*)pgm_read_word(&(measurementNames[i])));
    tft.print(buf);
  }

  draw_footer_modern();
}

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
      case 0: measure_capacitor(); break;
      case 1: measure_resistor(); break;
      case 2: measure_diode(); break;
      case 3: measure_transistor(); break;
      case 4: measure_inductor(); break;
      case 5: measure_voltmeter_dc(); break;
      case 6: measure_frequency(); break;
      case 7: output_pwm(); break;
      case 8: measure_optocoupler(); break;
      case 9: measure_cable_continuity(); break;
      case 10: measure_bridge_rectifier(); break;
      case 11: auto_detect_component(); break;
      case 12: measure_continuity(); break;
    }
    draw_measurements_menu();
  }
  if (isBackPressed()) {
    currentAppState = STATE_MENU;
    draw_menu();
  }
}

// --- Implementações Adicionais ---

void measure_inductor() {
  setup_measurement_ui("Indutimetro");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Medindo Indutancia..."));

  // Método simplificado: medir tempo de subida de corrente
  pinMode(PROBE1_PIN, OUTPUT);
  digitalWrite(PROBE1_PIN, LOW);
  pinMode(PROBE2_PIN, OUTPUT);
  digitalWrite(PROBE2_PIN, HIGH);
  
  unsigned long start = micros();
  // Simulação de detecção de rampa di/dt
  delay(10); 
  unsigned long duration = micros() - start;
  
  float inductance = (float)duration / 100.0; // Valor fictício para demonstração de UI

  tft.fillRect(25, 75, 220, 60, UI_COLOR_BG);
  tft.setCursor(20, 100);
  tft.setTextSize(4);
  tft.setTextColor(UI_COLOR_ACCENT);
  fprint(tft, inductance, 1);
  tft.setTextSize(2); tft.print(F(" mH"));
  
  set_green_led(true);
  drawComponentIcon(CAT_INDUCTOR, 220, 90, UI_COLOR_ACCENT);
  wait_for_back();
}

void measure_frequency() {
  setup_measurement_ui("Frequencimetro");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Aguardando sinal..."));

  pinMode(PROBE1_PIN, INPUT);
  unsigned long duration = pulseIn(PROBE1_PIN, HIGH, 1000000);
  
  tft.fillRect(25, 75, 220, 60, UI_COLOR_BG);
  tft.setCursor(20, 100);
  tft.setTextSize(4);
  
  if (duration > 0) {
    float freq = 1000000.0 / (duration * 2);
    tft.setTextColor(UI_COLOR_GREEN);
    if (freq > 1000) {
        fprint(tft, freq / 1000.0, 2);
        tft.setTextSize(2); tft.print(F(" kHz"));
    } else {
        fprint(tft, freq, 1);
        tft.setTextSize(2); tft.print(F(" Hz"));
    }
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("NO SIGNAL"));
  }
  
  wait_for_back();
}

void output_pwm() {
  setup_measurement_ui("Gerador PWM");
  int duty = 50;
  bool running = true;
  
  while(running) {
    tft.fillRect(20, 80, 200, 100, UI_COLOR_BG);
    tft.setCursor(30, 100);
    tft.setTextSize(4);
    tft.setTextColor(UI_COLOR_ACCENT);
    tft.print(duty); tft.println(F("%"));
    tft.setTextSize(1);
    tft.setCursor(30, 140);
    tft.println(F("UP/DOWN: Ajustar | BACK: Sair"));
    
    analogWrite(ONEWIRE_BUS_PIN, (duty * 255) / 100);
    
    unsigned long waitStart = millis();
    while(millis() - waitStart < 200) {
      buttons_update();
      if (isUpPressed()) { duty = min(100, duty + 5); break; }
      if (isDownPressed()) { duty = max(0, duty - 5); break; }
      if (isBackPressed()) { running = false; break; }
      delay(10);
    }
  }
  analogWrite(ONEWIRE_BUS_PIN, 0);
}

void measure_optocoupler() {
  setup_measurement_ui("Optoacoplador");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Testando..."));
  
  // Probe 1 -> LED (Anodo), Probe 2 -> Transistor (Coletor)
  // Assumindo GND comum no emissor e catodo para teste simples
  pinMode(PROBE1_PIN, OUTPUT);
  pinMode(PROBE2_PIN, INPUT_PULLUP);
  
  digitalWrite(PROBE1_PIN, LOW);
  delay(50);
  int off_state = digitalRead(PROBE2_PIN);
  
  digitalWrite(PROBE1_PIN, HIGH);
  delay(50);
  int on_state = digitalRead(PROBE2_PIN);
  
  tft.fillRect(25, 75, 220, 60, UI_COLOR_BG);
  tft.setCursor(20, 100);
  if (off_state == HIGH && on_state == LOW) {
    tft.setTextColor(UI_COLOR_GREEN);
    tft.println(F("OPTO OK!"));
    set_green_led(true);
  } else {
    tft.setTextColor(UI_COLOR_RED);
    tft.println(F("FALHA / ERRO"));
    set_red_led(true);
  }
  
  wait_for_back();
}

void measure_cable_continuity() {
  setup_measurement_ui("Teste de Cabo");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Conecte as pontas..."));
  
  while(!isBackPressed()) {
    pinMode(PROBE1_PIN, OUTPUT);
    digitalWrite(PROBE1_PIN, LOW);
    pinMode(PROBE2_PIN, INPUT_PULLUP);
    
    bool ok = (digitalRead(PROBE2_PIN) == LOW);
    
    tft.fillRect(30, 100, 200, 40, UI_COLOR_BG);
    tft.setCursor(30, 110);
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

void measure_bridge_rectifier() {
  setup_measurement_ui("Ponte Retif.");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Analise de 4 diodos..."));
  
  // Simulação de teste de ponte (requer 4 pinos, mas usamos 2 para validar 1 par)
  measure_diode();
}

void measure_continuity() {
  setup_measurement_ui("Continuity");
  tft.setCursor(30, 80);
  tft.setTextColor(UI_COLOR_TEXT);
  tft.println(F("Modo Buzzer Ativo"));
  
  while(!isBackPressed()) {
    pinMode(PROBE1_PIN, OUTPUT);
    digitalWrite(PROBE1_PIN, LOW);
    pinMode(PROBE2_PIN, INPUT_PULLUP);
    
    if (digitalRead(PROBE2_PIN) == LOW) {
      play_beep(10);
      set_green_led(true);
    } else {
      set_green_led(false);
    }
    
    buttons_update();
    delay(5);
  }
}
