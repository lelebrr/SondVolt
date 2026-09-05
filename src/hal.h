// ============================================================================
// Sondvolt v4.0 - Camada de Abstracao de Hardware (HAL)
// ============================================================================
// Arquivo : hal.h
// Objetivo: Isolar do resto do firmware tudo que e especifico do ESP32 e da
//           CYD: compatibilidade da API LEDC entre o core 2.x e o 3.x,
//           arbitragem dos pinos compartilhados (Rev A), leitura de ADC com
//           sobreamostragem e correcao de nao-linearidade.
//
// Por que isto existe
// -------------------
// Na revisao A da placa tres pinos tem dois donos:
//     GPIO4  -> LED vermelho   e  barramento OneWire (DS18B20)
//     GPIO17 -> LED azul       e  dreno de descarga de capacitor
//     GPIO27 -> I2C SDA        e  excitacao das pontas de prova
// Sem arbitragem, acender um LED corrompe a leitura do sensor e vice-versa.
// As funcoes hal_bus_* garantem exclusao mutua e restauram o estado anterior.
// ============================================================================

#ifndef HAL_H
#define HAL_H

#include <Arduino.h>
#include "pins.h"

// ----------------------------------------------------------------------------
// Recursos que disputam pinos na Rev A
// ----------------------------------------------------------------------------
enum HalBus {
    HAL_BUS_ONEWIRE = 0,   // GPIO4  (vs LED vermelho)
    HAL_BUS_DISCHARGE,     // GPIO17 (vs LED azul)
    HAL_BUS_PROBE_DRIVE,   // GPIO27 (vs I2C SDA)
    HAL_BUS_I2C,           // GPIO27/22
    HAL_BUS_COUNT
};

// ----------------------------------------------------------------------------
// Inicializacao
// ----------------------------------------------------------------------------

// Configura ADC, LEDC, pinos de saida e o mutex de arbitragem.
// Deve ser a PRIMEIRA chamada de hardware do setup().
void hal_init();

// Verdadeiro se o hardware de excitacao das pontas foi detectado.
// Quando falso, as medidas de resistencia/capacitancia ficam indisponiveis
// em vez de devolver numeros inventados.
bool hal_probe_available();

// Forca o resultado da deteccao (usado pelo autoteste e pelos ajustes).
void hal_probe_set_available(bool available);

// ----------------------------------------------------------------------------
// Arbitragem de pinos compartilhados
// ----------------------------------------------------------------------------

// Toma posse de um recurso compartilhado. Salva o estado do pino, desliga o
// dono anterior e devolve true quando o barramento estiver livre para uso.
// Bloqueia por no maximo timeoutMs.
bool hal_bus_acquire(HalBus bus, uint32_t timeoutMs = 250);

// Devolve o recurso e restaura o estado anterior do pino (por exemplo,
// reacende o LED que estava ligado antes da leitura do DS18B20).
void hal_bus_release(HalBus bus);

// Verdadeiro se o barramento esta tomado por alguem neste momento.
bool hal_bus_is_busy(HalBus bus);

// ----------------------------------------------------------------------------
// PWM / LEDC compativel com Arduino-ESP32 2.x e 3.x
// ----------------------------------------------------------------------------
void hal_pwm_attach(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t bits);
void hal_pwm_write(uint8_t pin, uint8_t channel, uint32_t duty);
void hal_pwm_tone(uint8_t pin, uint8_t channel, uint32_t freq);
void hal_pwm_stop(uint8_t pin, uint8_t channel);

// ----------------------------------------------------------------------------
// ADC
// ----------------------------------------------------------------------------

// Leitura simples ja corrigida (conta bruta 0..4095).
uint16_t hal_adc_read(uint8_t pin);

// Media de N leituras descartando o maior e o menor valor (filtro de mediana
// aparada). Remove picos de ruido tipicos do ADC do ESP32.
uint16_t hal_adc_read_avg(uint8_t pin, uint8_t samples = 16);

// Converte conta bruta em volts aplicando a curva de correcao do ESP32.
// O ADC do ESP32 e visivelmente nao-linear nas extremidades; esta funcao
// aplica a compensacao empirica recomendada pela Espressif.
float hal_adc_to_volts(uint16_t raw);

// Le direto em volts (leitura + media + conversao).
float hal_adc_read_volts(uint8_t pin, uint8_t samples = 16);

// Tensao de referencia efetiva medida/estimada do ADC.
float hal_adc_vref();
void  hal_adc_set_vref(float vref);

// ----------------------------------------------------------------------------
// LEDs (respeitando o anodo comum da CYD)
// ----------------------------------------------------------------------------
void hal_led_write(uint8_t pin, bool on);
bool hal_led_state(uint8_t pin);

// ----------------------------------------------------------------------------
// Touchscreen
// ----------------------------------------------------------------------------

// Le o touch e ja devolve as coordenadas em pixels da tela (0..319, 0..239).
// Devolve false se nao houver toque valido. Esta e a UNICA conversao de
// coordenadas do projeto: antes existiam duas, com constantes diferentes,
// e as telas discordavam sobre onde o dedo estava.
bool hal_touch_read(uint16_t* outX, uint16_t* outY);

// Espera por um toque ate timeoutMs. Devolve false se o tempo esgotar.
bool hal_touch_wait(uint16_t* outX, uint16_t* outY, uint32_t timeoutMs);

// Espera o dedo sair da tela (evita um toque ser lido duas vezes).
void hal_touch_wait_release(uint32_t timeoutMs = 1500);

// ----------------------------------------------------------------------------
// Utilidades
// ----------------------------------------------------------------------------

// Nome legivel do pino, usado nas telas de diagnostico.
const char* hal_pin_name(uint8_t pin);

// Descreve o conflito de um pino, ou nullptr se ele for exclusivo.
const char* hal_pin_conflict(uint8_t pin);

#endif // HAL_H
