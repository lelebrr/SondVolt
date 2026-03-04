#include "measurements.h"
#include "globals.h"
#include "buttons.h"
#include "leds.h"
#include "buzzer.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Array de strings para os nomes das medições
const char* measurementNames[] = {
    F("Capacitor + ESR"),
    F("Resistor"),
    F("Diodo / LED"),
    F("Transistor"),
    F("Indutor"),
    F("Voltimetro DC"),
    F("Freq. Counter"),
    F("PWM Generator"),
    F("Optoacoplador"),
    F("Cable Tester"),
    F("Ponte Retif."),
    F("Auto Detect"),
    F("Continuidade")
};

const int NUM_MEASUREMENTS = sizeof(measurementNames) / sizeof(measurementNames[0]);

int currentMeasurementItem = 0;

void display_measurement_stub(const char* measurementName) {
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
    flash_both_leds(200); // LEDs piscando rápido para indicar medição em andamento (stub)
}

void measure_capacitor() {
    Serial.println(F("Medindo Capacitor..."));
    display_measurement_stub(F("Capacitor + ESR"));
}

void measure_resistor() {
    Serial.println(F("Medindo Resistor..."));
    display_measurement_stub(F("Resistor"));
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
            case 0: measure_capacitor(); break;
            case 1: measure_resistor(); break;
            case 2: measure_diode(); break;
            case 3: measure_transistor(); break;
            case 4: measure_inductor(); break;
            case 5: measure_voltmeter_dc(); break;
            case 6: measure_frequency_counter(); break;
            case 7: generate_pwm(); break;
            case 8: test_optocoupler(); break;
            case 9: test_cable_continuity(); break;
            case 10: test_bridge_rectifier(); break;
            case 11: auto_detect_component(); break;
            case 12: test_continuity_buzzer(); break;
        }
    }
    if (isBackPressed()) {
        currentAppState = STATE_MENU; // Volta para o menu principal
        tft.fillScreen(TFT_BLACK);
        // É importante redesenhar o menu principal ao voltar
        // menu_init(); // Isso pode resetar a seleção do menu, melhor chamar draw_menu()
    }
}
