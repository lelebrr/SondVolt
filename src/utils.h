// ============================================================================
// Component Tester PRO v3.0 — Utilitários (CYD Edition)
// ============================================================================
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Print.h>

// Funções utilitárias
void fprint(Print &p, float val, int prec);   // Print float com precisão
void showBackMsg();                            // Mensagem "BACK para sair"
void calibrate_probes();                       // Calibração das sondas
void set_offsets();                             // Exibir offsets atuais
void handle_timeout();                         // Gerenciar timeout de inatividade
unsigned long safe_millis();                   // Millis thread-safe
void toggle_dark_mode();                       // Toggle modo escuro
void toggle_silent_mode();                     // Toggle modo silencioso
void start_timeout(unsigned long duration);    // Iniciar timeout
bool is_timeout_active();                      // Verificar timeout
void toggle_hold();                            // Toggle modo hold
bool is_hold_active();                         // Verificar hold
void increment_measurement_count();            // Incrementar contadores
void increment_faulty_count();
void get_measurement_stats(unsigned long *total, unsigned long *faulty);
void reset_counters();

#endif // UTILS_H