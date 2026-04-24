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
#define ONEWIRE_BUS_PIN A3 // Moved from 4 to A3 to avoid conflict with SD_CS

// Limites de temperatura para a sonda térmica (em graus Celsius)
#define TEMP_NORMAL_THRESHOLD 70.0
#define TEMP_HOT_THRESHOLD 90.0
#define TEMP_DANGER_THRESHOLD 110.0

// Cores Modernas (Formato RGB565 para ILI9341)
#define UI_COLOR_BG       0x000F // Navy Blue
#define UI_COLOR_ACCENT   0x07FF // Cyan
#define UI_COLOR_TEXT     0xFFFF // White
#define UI_COLOR_HILIGHT  0x03EF // Medium Blue
#define UI_COLOR_RED      0xF800
#define UI_COLOR_GREEN    0x07E0
#define UI_COLOR_GREY     0x7BEF

// Layout do Menu
#define MENU_GRID_COLS    2
#define MENU_GRID_ROWS    3
#define MENU_ICON_SIZE    32
#define STATUS_BAR_HEIGHT 28

#endif // CONFIG_H
