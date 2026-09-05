// ============================================================================
// Sondvolt v4.0 - Sonda Termica (DS18B20)
// ============================================================================
// Arquivo : thermal.h
// ============================================================================

#ifndef THERMAL_H
#define THERMAL_H

#include <Arduino.h>

// Detecta o sensor no barramento OneWire e o configura em 12 bits.
// Pode ser chamada novamente para redetectar uma sonda plugada a quente.
void thermal_init();

// Leitura sincrona: bloqueia por ate 800 ms enquanto o DS18B20 converte.
// Devolve THERMAL_INVALID_TEMP se nao houver sensor.
float thermal_read();

// Leitura assincrona, propria para o laco de medicao. Dispara a conversao
// numa chamada e coleta o resultado na seguinte. Devolve true quando um
// valor novo ficou disponivel.
bool thermal_update();

// Ultimo valor valido lido.
float thermal_get_last();

// Presenca e validade.
bool    thermal_present();
bool    thermal_is_valid();
uint8_t thermal_device_count();

// Faixas de alerta.
bool thermal_is_warning();
bool thermal_is_critical();

// Texto e cor para a interface.
const char* thermal_status_text();
uint16_t    thermal_status_color();

#endif // THERMAL_H
