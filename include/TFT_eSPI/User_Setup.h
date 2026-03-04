// User_Setup.h for Component Tester PRO v2.0
// Configuração para Arduino Uno R3 e display TFT ILI9341 (SPI)

#define ILI9341_DRIVER

// Define os pinos SPI para o Arduino Uno
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_CS   10  // Chip Select para TFT (D10)
#define TFT_DC   9   // Data/Command para TFT (D9)
#define TFT_RST  A2  // Reset para TFT (A2)

// Rotação do display (landscape)
#define TFT_ROTATION 1 // 0 = Portrait, 1 = Landscape, 2 = Invert Portrait, 3 = Invert Landscape

// Define se o SD Card compartilha o barramento SPI
#define SPI_TOUCH_SCREEN // Embora não haja touch, esta define o uso do SPI para outros dispositivos

// Define o pino CS para o SD Card
#define SD_CS 4 // Chip Select para SD Card (D4)

// Otimizações de fonte (opcional, pode ser ajustado conforme necessário)
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1190 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3030 bytes in FLASH
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH
#define LOAD_FONT6  // Font 6. Large 48 pixel high font, needs ~26006 bytes in FLASH
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2676 bytes in FLASH
#define LOAD_FONT8  // Font 8. Large 75 pixel high font, needs ~3256 bytes in FLASH
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts

#define SMOOTH_FONT
