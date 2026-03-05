#include "thermal.h"
#include "buzzer.h"
#include "globals.h"
#include "leds.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;

// Definição dos objetos OneWire e DallasTemperature
OneWire oneWireBus(ONEWIRE_BUS_PIN);
DallasTemperature sensors(&oneWireBus);

// Variáveis para controle da leitura da temperatura
unsigned long lastTempReadMillis = 0;
const long tempReadInterval = 500; // Leitura a cada 500ms

// Inicializa a sonda térmica
void thermal_init() { sensors.begin(); }

// Manipula o estado da sonda térmica
void thermal_handle() {
  if (currentMillis - lastTempReadMillis >= tempReadInterval) {
    lastTempReadMillis = currentMillis;
    currentTemperature = read_temperature();
    check_temperature_alerts(currentTemperature);

    // Exibe a temperatura na tela
    tft.fillRect(0, 0, tft.width(), tft.height(),
                 ILI9341_BLACK); // Limpa a tela
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print(F("Temp: "));
    tft.print(currentTemperature, 1);
    tft.println(F(" C"));

    tft.setTextSize(1);
    tft.setCursor(10, 40);
    if (currentTemperature < TEMP_NORMAL_THRESHOLD) {
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.println(F("Normal"));
    } else if (currentTemperature < TEMP_HOT_THRESHOLD) {
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.println(F("Quente! Cuidado"));
    } else if (currentTemperature < TEMP_DANGER_THRESHOLD) {
      tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
      tft.println(F("Muito Quente! Perigo"));
    } else {
      tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
      tft.println(F("PERIGO! DESLIGUE!"));
    }
  }
}

// Lê a temperatura do sensor DS18B20
float read_temperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

// Verifica alertas de temperatura e aciona LEDs/buzzer
void check_temperature_alerts(float temp) {
  if (temp >= TEMP_DANGER_THRESHOLD) {
    set_red_led(true); // LED vermelho fixo
    play_long_beep();  // Beep longo
  } else if (temp >= TEMP_HOT_THRESHOLD) {
    flash_red_led_fast(); // LED vermelho piscando rápido
  } else {
    set_red_led(false);
    // Se a temperatura voltar ao normal, parar o beep se estiver tocando
    stop_beep();
  }
}