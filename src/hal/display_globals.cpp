#include "display_globals.h"
#include "display_mutex.h"
#include <SPI.h>

// Barramento compartilhado (TFT + SD)
SPIClass spiTFT_SD(VSPI);

// Instanciação do objeto TFT usando TFT_eSPI
TFT_eSPI tft = TFT_eSPI();

// SPI bus dedicado para o Touch (HSPI na CYD)
SPIClass touchSPI(HSPI);

// Instanciação do Touch (XPT2046)
XPT2046_Touchscreen touch(PIN_TOUCH_CS);

// Instanciação do SD
SdFat sd;

// Mutex de acesso ao display. Criado explicitamente em display_mutex_init(),
// nao mais num inicializador estatico: assim a falha e detectavel.
SemaphoreHandle_t g_tft_mutex = nullptr;

void display_mutex_init() {
    if (g_tft_mutex) return;
    g_tft_mutex = xSemaphoreCreateRecursiveMutex();

    // Sem o mutex nao ha como serializar o SPI entre as duas tarefas. Seguir
    // em frente daria corrupcao intermitente de tela, dificil de diagnosticar.
    if (!g_tft_mutex) {
        Serial.println(F("[SYS] FALHA CRITICA: mutex do display nao criado"));
        delay(100);
        ESP.restart();
    }
}
