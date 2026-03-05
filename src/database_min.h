#ifndef DATABASE_MIN_H
#define DATABASE_MIN_H

#include <Arduino.h>
#include <stdint.h>

struct ComponentDB {
  const char *name;
  uint8_t category;
  uint16_t value1;
  uint16_t min1;
  uint16_t max1;
  uint16_t value2;
  uint8_t pinout[3];
  const char *description;
  const char *common_use;
  uint8_t typical_esr;
};

#define CAT_BJT_NPN 1
#define CAT_BJT_PNP 2
#define CAT_MOSFET_N 3
#define CAT_MOSFET_P 4
#define CAT_DIODE 5
#define CAT_ZENER 6
#define CAT_SCHOTTKY 7
#define CAT_LED 8
#define CAT_CAPACITOR 9
#define CAT_RESISTOR 10
#define CAT_INDUCTOR 11
#define CAT_OPTOCOUPLER 12
#define CAT_CRYSTAL 13
#define CAT_POTENTIOMETER 14
#define CAT_FUSE 15
#define CAT_VARISTOR 16
#define CAT_NTC 17
#define CAT_TRIAC 18
#define CAT_SCR 19
#define CAT_REGULATOR 20
#define CAT_RELAY 21
#define CAT_SENSOR 22
#define CAT_OTHER 23

extern const ComponentDB PROGMEM bjt_npn_db[];
extern const uint16_t bjt_npn_count;

#endif