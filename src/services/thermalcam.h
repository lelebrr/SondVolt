// ============================================================================
// Sondvolt v5.0 - Camera Termica MLX90640
// ============================================================================
// Arquivo : thermalcam.h
//
// O que e
// -------
// Matriz de 32x24 termopilhas que enxerga temperatura sem contato. Numa
// bancada, serve para achar o componente que esta esquentando numa placa
// antes de tocar nele - o que muitas vezes aponta o defeito direto.
//
// Restricoes honestas nesta placa
// -------------------------------
// 1. O MLX90640 mora no mesmo barramento I2C que o INA219 e o expansor, e
//    esse barramento divide pinos com a excitacao das pontas. Enquanto a
//    camera estiver capturando, as medicoes de componente ficam suspensas.
// 2. Um quadro tem 768 pixels de float, ou seja 3 KB, e a biblioteca precisa
//    de mais 1,7 KB de parametros de calibracao. Sao quase 5 KB de RAM
//    permanentemente ocupados. Cabe, mas nao e de graca.
// 3. A 400 kHz o sensor entrega cerca de 2 quadros por segundo. Para 4 ou 8
//    FPS seria preciso subir o I2C para 1 MHz, o que o INA219 tolera mal.
//    Ficamos em 400 kHz e 2 FPS: mais lento, porem estavel.
// 4. A tela tem 320x240 e a matriz e 32x24 - exatamente 10x menor. A
//    interpolacao bilinear suaviza, mas nao cria detalhe que o sensor nao
//    capturou.
// ============================================================================

#ifndef THERMALCAM_H
#define THERMALCAM_H

#include <Arduino.h>

#define TCAM_COLS        32
#define TCAM_ROWS        24
#define TCAM_PIXELS      (TCAM_COLS * TCAM_ROWS)

// Paletas de cor disponiveis.
enum ThermalPalette {
    TCAM_PALETTE_IRON = 0,   // preto-vermelho-amarelo-branco (classica)
    TCAM_PALETTE_RAINBOW,    // azul-verde-vermelho (mais contraste)
    TCAM_PALETTE_GRAY,       // escala de cinza (melhor para fotografar)
    TCAM_PALETTE_COUNT
};

struct ThermalFrame {
    float    pixels[TCAM_PIXELS];   // temperatura em graus Celsius
    float    minC;
    float    maxC;
    float    avgC;
    float    centerC;               // ponto central, o "mira" da tela
    uint16_t hotIndex;              // pixel mais quente
    uint16_t coldIndex;
    bool     valid;
    uint32_t timestampMs;
};

// ============================================================================
// CICLO DE VIDA
// ============================================================================

// Procura o sensor e carrega os parametros de calibracao da EEPROM interna
// dele. Essa leitura demora quase 1 segundo e por isso e feita uma vez so.
bool tcam_init();

// Verdadeiro se o sensor respondeu e foi configurado.
bool tcam_present();

// Refaz a deteccao.
bool tcam_redetect();

// Libera a memoria dos parametros. Chamar ao sair do modo camera se a RAM
// estiver apertada.
void tcam_release();

// ============================================================================
// CAPTURA
// ============================================================================

// Le um quadro completo. Bloqueia por cerca de 500 ms a 2 FPS.
// Devolve false se o sensor nao responder.
bool tcam_read_frame(ThermalFrame* out);

// Ultimo quadro lido com sucesso.
const ThermalFrame* tcam_last_frame();

// Taxa de quadros configurada.
void  tcam_set_refresh_hz(uint8_t hz);   // aceita 1, 2, 4 ou 8
uint8_t tcam_get_refresh_hz();

// ============================================================================
// APRESENTACAO
// ============================================================================

// Cor RGB565 para uma temperatura, dentro da faixa min..max informada.
uint16_t tcam_temp_to_color(float celsius, float minC, float maxC,
                            ThermalPalette palette);

void           tcam_set_palette(ThermalPalette p);
ThermalPalette tcam_get_palette();
const char*    tcam_palette_name(ThermalPalette p);

// Desenha o quadro na tela com interpolacao bilinear, dentro do retangulo
// indicado. Inclui a mira central e o marcador do ponto mais quente.
void tcam_draw(const ThermalFrame* frame, int16_t x, int16_t y,
               int16_t w, int16_t h);

// Desenha a barra de escala de cores com os limites em graus.
void tcam_draw_scale(const ThermalFrame* frame, int16_t x, int16_t y,
                     int16_t w, int16_t h);

// Salva o quadro atual como CSV no cartao, para analise no computador.
bool tcam_save_snapshot(const char* name);

#endif // THERMALCAM_H
