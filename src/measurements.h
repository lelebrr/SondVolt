#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include "globals.h"
#include <Arduino.h>

// Funções para medição de componentes
void measure_capacitor();
void measure_resistor();
void measure_diode();
void measure_transistor();
void measure_inductor();
void measure_voltmeter_dc();
void measure_frequency();
void output_pwm();
void measure_optocoupler();
void measure_cable_continuity();
void measure_bridge_rectifier();
void auto_detect_component();
void measure_continuity();
void measurements_handle();
void draw_measurements_menu();

#endif // MEASUREMENTS_H
