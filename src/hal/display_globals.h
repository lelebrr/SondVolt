// ============================================================================
// Display globals para ESP32-2432S028R (CYD) - RESTAURADO PARA TFT_eSPI
// ============================================================================
#ifndef DISPLAY_GLOBALS_H
#define DISPLAY_GLOBALS_H

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SdFat.h>
#include <SPI.h>
#include "config.h"

// Barramento compartilhado entre TFT e SD (VSPI)
extern SPIClass spiTFT_SD;

// Display global
extern TFT_eSPI tft;

// Touch global
extern XPT2046_Touchscreen touch;

// SD global
extern SdFat sd;

// SPI bus dedicado para o Touch (HSPI)
extern SPIClass touchSPI;

// Mutex para sincronização de acesso (UI vs Measurements)
extern SemaphoreHandle_t g_tft_mutex;

#endif