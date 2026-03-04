#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino.h>
#include "globals.h"

// Funções para medição de componentes
void measure_capacitor();
void measure_resistor();
void measure_diode();
void measure_transistor();
void measure_inductor();
void measure_voltmeter_dc();
void measure_frequency_counter();
void generate_pwm();
void test_optocoupler();
void test_cable_continuity();
void test_bridge_rectifier();
void auto_detect_component();
void test_continuity_buzzer();

#endif // MEASUREMENTS_H
