// ============================================================================
// Sondvolt v3.2 — Tela Inicial (Splash)
// ============================================================================
// Arquivo: splash.cpp
// Descricao: Animacao de inicializacao profissional
// ============================================================================

#include "graphics.h"
#include "display_mutex.h"
#include "display_globals.h"
#include "config.h"
#include "visual.h"
#include "fonts.h"

void draw_splash_screen() {
    LOCK_TFT();
    tft.fillScreen(V_BG_DARK);
    
    // Efeito de brilho Neon ao fundo (Ciano/Azul)
    for(int r = 120; r > 0; r -= 15) {
        uint16_t c = color_mix(V_DEEP_BLUE, V_BG_DARK, (r * 255) / 120);
        tft.fillCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 20, r, c);
    }
    
    // Nome do Projeto (Estilizado com Sombra)
    draw_text_5x7(tft, SCREEN_WIDTH/2 - (8*6*4)/2 + 2, 82, "SONDVOLT", 0x0000, 4);
    draw_text_5x7(tft, SCREEN_WIDTH/2 - (8*6*4)/2, 80, "SONDVOLT", V_CYAN_ELECTRIC, 4);
    
    // Slogan Neon
    draw_text_5x7(tft, SCREEN_WIDTH/2 - (20*6)/2, 125, "THE FUTURE OF MEASUREMENTS", V_NEON_GREEN, 1);
    
    // Animacao de barra de progresso Neon
    int16_t pbW = 240;
    int16_t pbH = 8;
    int16_t pbX = (SCREEN_WIDTH - pbW) / 2;
    int16_t pbY = SCREEN_HEIGHT - 65;
    
    tft.drawRoundRect(pbX, pbY, pbW, pbH, 4, V_BG_HIGHLIGHT);
    
    for(int i = 0; i <= 100; i += 4) {
        int16_t fillW = (pbW - 4) * i / 100;
        tft.fillRoundRect(pbX + 2, pbY + 2, fillW, pbH - 4, 2, V_CYAN_ELECTRIC);
        
        // Texto dinâmico de carregamento
        if (i % 20 == 0) {
            tft.fillRect(0, SCREEN_HEIGHT - 45, 320, 15, V_BG_DARK);
            const char* msg = (i < 40) ? "LOADING KERNEL..." : (i < 80) ? "CONFIGURING SENSORS..." : "SYSTEM READY!";
            int16_t msgX = 160 - (strlen(msg) * 3);
            draw_text_5x7(tft, msgX, SCREEN_HEIGHT - 45, msg, V_TEXT_SUB, 1);
        }
        
        delay(25);
    }
    
    draw_text_5x7(tft, SCREEN_WIDTH/2 - (15*6)/2, SCREEN_HEIGHT - 25, "v4.0 PRO - 2026", V_BG_HIGHLIGHT, 1);
    
    UNLOCK_TFT();
    delay(400); 
}
