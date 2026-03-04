#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// Funções utilitárias
void calibrate_probes();
void set_offsets();
void handle_timeout();
unsigned long safe_millis();

// Modos de operação
void toggle_dark_mode();
void toggle_silent_mode();

// Timeout functionality
void start_timeout(unsigned long duration);
bool is_timeout_active();

// Hold functionality
void toggle_hold();
bool is_hold_active();

// Contadores
void increment_measurement_count();
void increment_faulty_count();
void get_measurement_stats(unsigned long *total, unsigned long *faulty);
void reset_counters();

#endif // UTILS_H
