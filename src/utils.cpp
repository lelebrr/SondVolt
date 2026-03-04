#include "utils.h"
#include "globals.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Função para calibração das ponteiras
void calibrate_probes() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println(F("Calibracao de Probes"));
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.println(F("Em Desenvolvimento..."));
    tft.setCursor(10, 60);
    tft.println(F("Pressione BACK para voltar."));
}

// Função para definir offsets
void set_offsets() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println(F("Definir Offsets"));
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.println(F("Em Desenvolvimento..."));
    tft.setCursor(10, 60);
    tft.println(F("Pressione BACK para voltar."));
}

// Função para lidar com timeouts
void handle_timeout() {
    // Implementação futura
}

// Função auxiliar para millis (se necessário, pode ser mais complexa)
void millis_helper() {
    // Implementação futura
}
