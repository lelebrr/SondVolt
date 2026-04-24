#ifndef DISPLAY_MUTEX_H
#define DISPLAY_MUTEX_H

#include <Arduino.h>

// Mutex global para acesso ao display TFT
// Impede que múltiplas tarefas (Safety, Display, Loop) acessem o SPI ao mesmo tempo
extern SemaphoreHandle_t g_tft_mutex;

#define LOCK_TFT()   if (g_tft_mutex) xSemaphoreTakeRecursive(g_tft_mutex, portMAX_DELAY)
#define UNLOCK_TFT() if (g_tft_mutex) xSemaphoreGiveRecursive(g_tft_mutex)

#endif
