// ============================================================================
// Sondvolt v3.0 — Utilitários (Header)
// ============================================================================
#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <stdint.h>

// ============================================================================
// STRINGS — Sem String class
// ============================================================================
bool str_is_empty(const char* s);
void str_trim(char* s);
void str_upper(char* s);
void str_lower(char* s);

// ============================================================================
// MATH — Funções inline
// ============================================================================
inline float clamp_float(float v, float mn, float mx);
inline int clamp_int(int v, int mn, int mx);
inline float round_to(float v, float step);

// ============================================================================
// TEMPO — Funções inline
// ============================================================================
inline unsigned long elapsed_since(unsigned long start);
inline bool has_elapsed(unsigned long start, unsigned long period);

// ============================================================================
// ADC — Funções de leitura
// ============================================================================
inline uint16_t adc_read_raw_p1();
inline uint16_t adc_read_raw_zmpt();
inline float adc_to_volts(uint16_t raw);

// ============================================================================
// DEBOUNCE — Classe Debouncer
// ============================================================================
class Debouncer {
private:
    unsigned long _last_time;
    uint8_t _state;
    uint8_t _pin;
    uint8_t _threshold;
    
public:
    Debouncer();
    void begin(uint8_t pin, uint8_t threshold = 1);
    inline bool pressed();
    inline bool is_down() const;
};

// ============================================================================
// MOVING AVERAGE — Filtro de média móvel
// ============================================================================
template <uint8_t N>
class MovingAverage {
private:
    float _buffer[N];
    uint8_t _idx;
    uint8_t _count;
    float _sum;
    
public:
    MovingAverage();
    inline float update(float value);
    inline float get() const;
    inline void reset();
    inline uint8_t count() const;
};

// ============================================================================
// LOW-PASS FILTER — Filtro IIR
// ============================================================================
class LowPass {
private:
    float _value;
    float _alpha;
    
public:
    LowPass();
    void set_alpha(float a);
    inline float update(float input);
    inline float get() const;
    inline void reset(float v = 0.0f);
};

#endif