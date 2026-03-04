#include "measurements.h"
#include "buttons.h"
#include "buzzer.h"
#include "globals.h"
#include "leds.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Array de strings para os nomes das medições
const char *measurementNames[] = {
    F("Capacitor + ESR"), F("Resistor"),      F("Diodo / LED"),
    F("Transistor"),      F("Indutor"),       F("Voltimetro DC"),
    F("Freq. Counter"),   F("PWM Generator"), F("Optoacoplador"),
    F("Cable Tester"),    F("Ponte Retif."),  F("Auto Detect"),
    F("Continuidade")};

const int NUM_MEASUREMENTS =
    sizeof(measurementNames) / sizeof(measurementNames[0]);

int currentMeasurementItem = 0;

void display_measurement_stub(const char *measurementName) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(measurementName);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println(F("Em Desenvolvimento..."));
  tft.setCursor(10, 60);
  tft.println(F("Pressione BACK para voltar."));
  set_green_led(false);
  set_red_led(false);
  flash_both_leds(
      200); // LEDs piscando rápido para indicar medição em andamento (stub)
}

void measure_capacitor() {
  Serial.println(F("Medindo Capacitor..."));
  // Implementação real de medição de capacitor
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(F("Capacitor + ESR"));
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println(F("Em Desenvolvimento..."));
  tft.setCursor(10, 60);
  tft.println(F("Pressione BACK para voltar."));
  set_green_led(false);
  set_red_led(false);
  flash_both_leds(200);
}

void measure_resistor() {
  Serial.println(F("Medindo Resistor..."));
  // Implementação real de medição de resistor
  float resistance = measure_resistance_value();

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(F("Resistor: "));
  tft.print(resistance, 1);
  tft.println(F(" Ohm"));

  // Julgamento da qualidade
  const char *judgment = get_judgment(resistance, 100.0, 5.0); // 100 Ohm ±5%
  tft.setCursor(10, 40);
  if (strcmp(judgment, "Good") == 0) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  } else if (strcmp(judgment, "Average") == 0) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
  }
  tft.println(judgment);

  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, 60);
  tft.println(F("Pressione BACK para voltar."));

  // Logging
  log_measurement("Resistor", resistance, currentTemperature, judgment);

  set_green_led(strcmp(judgment, "Good") == 0);
  set_red_led(strcmp(judgment, "Bad") == 0);
}

void measure_diode() {
  Serial.println(F("Medindo Diodo / LED..."));
  display_measurement_stub(F("Diodo / LED"));
}

void measure_transistor() {
  Serial.println(F("Medindo Transistor..."));
  display_measurement_stub(F("Transistor"));
}

void measure_inductor() {
  Serial.println(F("Medindo Indutor..."));
  display_measurement_stub(F("Indutor"));
}

void measure_voltmeter_dc() {
  Serial.println(F("Medindo Voltimetro DC..."));
  display_measurement_stub(F("Voltimetro DC"));
}

void measure_frequency_counter() {
  Serial.println(F("Medindo Frequency Counter..."));
  display_measurement_stub(F("Freq. Counter"));
}

void generate_pwm() {
  Serial.println(F("Gerando PWM..."));
  display_measurement_stub(F("PWM Generator"));
}

void test_optocoupler() {
  Serial.println(F("Testando Optoacoplador..."));
  display_measurement_stub(F("Optoacoplador"));
}

void test_cable_continuity() {
  Serial.println(F("Testando Cabo / Continuidade..."));
  display_measurement_stub(F("Cable Tester"));
}

void test_bridge_rectifier() {
  Serial.println(F("Testando Ponte Retificadora..."));
  display_measurement_stub(F("Ponte Retif."));
}

void auto_detect_component() {
  Serial.println(F("Auto Detect Componente..."));
  display_measurement_stub(F("Auto Detect"));
}

void test_continuity_buzzer() {
  Serial.println(F("Testando Continuidade com Buzzer..."));
  display_measurement_stub(F("Continuidade"));
  play_beep(500); // Exemplo de uso do buzzer
}

// Desenha o menu de medições
void draw_measurements_menu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(TL_DATUM);

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    if (i == currentMeasurementItem) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.setCursor(10, 10 + i * 20);
    tft.println(measurementNames[i]);
  }
  // Rodapé para navegação
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.fillRect(0, tft.height() - 20, tft.width(), 20, TFT_DARKGREY);
  tft.setCursor(5, tft.height() - 15);
  tft.print(F("UP/DW: Scroll | OK: Iniciar | BCK: Voltar"));
}

// Manipula a seleção e execução das medições
void measurements_handle() {
  buttons_update();

  if (isUpPressed()) {
    currentMeasurementItem--;
    if (currentMeasurementItem < 0) {
      currentMeasurementItem = NUM_MEASUREMENTS - 1;
    }

    // Funções auxiliares de medição
    float measure_resistance_value() {
      // Implementação simplificada de medição de resistor
      // Em um projeto real, isso envolveria circuito de medição com ADC
      analogRead(PROBE1_PIN); // Descarga do capacitor
      delay(10);

      // Carregar capacitor através do resistor
      pinMode(PROBE1_PIN, OUTPUT);
      digitalWrite(PROBE1_PIN, HIGH);
      pinMode(PROBE2_PIN, INPUT);
      delayMicroseconds(100);

      // Medir a tensão através do tempo de descarga
      pinMode(PROBE1_PIN, INPUT);
      unsigned long startTime = micros();
      int count = 0;
      while (analogRead(PROBE1_PIN) > 512 && count < 10000) {
        count++;
        delayMicroseconds(10);
      }
      unsigned long dischargeTime = micros() - startTime;

      // Calcular resistência (fórmula simplificada)
      float resistance = (dischargeTime * 0.001) * 1000; // Ohm aproximado

      return constrain(resistance, 1.0,
                       1000000.0); // Limitar entre 1 Ohm e 1 MOhm
    }

    const char *get_judgment(float measured, float expected,
                             float tolerance_percent) {
      float tolerance = expected * (tolerance_percent / 100.0);
      float min_good = expected - tolerance;
      float max_good = expected + tolerance;
      float min_average = expected - tolerance * 2;
      float max_average = expected + tolerance * 2;

      if (measured >= min_good && measured <= max_good) {
        return "Good";
      } else if (measured >= min_average && measured <= max_average) {
        return "Average";
      } else {
        return "Bad";
      }
    }
    draw_measurements_menu();
  }
  if (isDownPressed()) {
    currentMeasurementItem++;
    if (currentMeasurementItem >= NUM_MEASUREMENTS) {
      currentMeasurementItem = 0;
    }
    draw_measurements_menu();
  }
  if (isOkPressed()) {
    // Executa a função de medição selecionada
    switch (currentMeasurementItem) {
    case 0:
      measure_capacitor();
      break;
    case 1:
      measure_resistor();
      break;
    case 2:
      measure_diode();
      break;
    case 3:
      measure_transistor();
      break;
    case 4:
      measure_inductor();
      break;
    case 5:
      measure_voltmeter_dc();
      break;
    case 6:
      measure_frequency_counter();
      break;
    case 7:
      generate_pwm();
      break;
    case 8:
      test_optocoupler();
      break;
    case 9:
      test_cable_continuity();
      break;
    case 10:
      test_bridge_rectifier();
      break;
    case 11:
      auto_detect_component();
      break;
    case 12:
      test_continuity_buzzer();
      break;
    }
  }
  if (isBackPressed()) {
    currentAppState = STATE_MENU; // Volta para o menu principal
    tft.fillScreen(TFT_BLACK);
    // É importante redesenhar o menu principal ao voltar
    // menu_init(); // Isso pode resetar a seleção do menu, melhor chamar
    // draw_menu()
  }
}
