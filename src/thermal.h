// ============================================================================
// Sondvolt v3.0 — Sonda Térmica DS18B20
// Descrição: Leitura da sonda de temperatura OneWire
// ============================================================================
#ifndef THERMAL_H
#define THERMAL_H

#include <stdint.h>

// ============================================================================
// PROTÓTIPOS
// ============================================================================

// Inicialização do sensor
void thermal_init();

// Leitura única (bloqueante)
float thermal_read();

// Leitura não-bloqueante (requer polling)
float thermal_read_async();

// Último valor lido (disponível sem bloquear)
float thermal_get_last();

// Verifica se há alerta de temperatura
bool thermal_has_alert();

// Limiares
bool thermal_is_warning();    // >70°C
bool thermal_is_critical();  // >90°C

// Handle da UI (loop)
void thermal_handle();

// Desenha tela de monitoramento térmico
void thermal_draw();

// Alertas sonoros
void thermal_alert_beep();

// Modo de varredura rápida
void thermal_set_resolution(uint8_t resolution);

#endif // THERMAL_H