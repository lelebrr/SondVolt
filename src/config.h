#ifndef CONFIG_H
#define CONFIG_H

// Definições de pinos
#define PROBE1_PIN A0
#define PROBE2_PIN A1

#define BTN_UP_PIN 2
#define BTN_DOWN_PIN 3
#define BTN_LEFT_PIN 5
#define BTN_RIGHT_PIN 6
#define BTN_OK_PIN 7
#define BTN_BACK_PIN 8

#define BUZZER_PIN 9
#define LED_GREEN_PIN 10
#define LED_RED_PIN 11

#define TFT_CS_PIN 12
#define TFT_DC_PIN 13
#define TFT_RST_PIN A2

#define SD_CS_PIN 4
#define ONEWIRE_BUS_PIN                                                        \
  4 // Mesmo pino que SD_CS - compartilhado com resistor pull-up 4.7kΩ

// Limites de temperatura para a sonda térmica (em graus Celsius)
#define TEMP_NORMAL_THRESHOLD 70.0
#define TEMP_HOT_THRESHOLD 90.0
#define TEMP_DANGER_THRESHOLD 110.0

#endif // CONFIG_H
