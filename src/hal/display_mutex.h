#ifndef DISPLAY_MUTEX_H
#define DISPLAY_MUTEX_H

#include <Arduino.h>

// Mutex global para acesso ao display TFT
// Impede que múltiplas tarefas (Safety, Display, Loop) acessem o SPI ao mesmo tempo
extern SemaphoreHandle_t g_tft_mutex;

// Cria o mutex. Chamar no inicio do setup(), antes de qualquer desenho.
// Ate a v5.0 ele era criado num inicializador estatico global: funcionava na
// pratica, mas se a criacao falhasse as macros viravam no-op silencioso e o
// barramento ficava sem protecao NENHUMA - pior que travar.
void display_mutex_init();

// Envolvidos em do/while(0) de proposito. Como `if` nu, um
//     if (cond) LOCK_TFT(); else ...
// faria o `else` casar com o `if` de dentro da macro, em vez do de fora -
// o classico dangling-else, que o compilador aceita sem reclamar.
#define LOCK_TFT()                                              \
    do {                                                        \
        if (g_tft_mutex) {                                      \
            xSemaphoreTakeRecursive(g_tft_mutex, portMAX_DELAY);\
        }                                                       \
    } while (0)

#define UNLOCK_TFT()                                            \
    do {                                                        \
        if (g_tft_mutex) {                                      \
            xSemaphoreGiveRecursive(g_tft_mutex);               \
        }                                                       \
    } while (0)

#endif
