// ============================================================================
// Sondvolt v4.0 - Camada de Abstracao de Hardware (implementacao)
// ============================================================================

#include "hal.h"
#include "display_globals.h"

// ----------------------------------------------------------------------------
// Estado interno
// ----------------------------------------------------------------------------

struct BusState {
    SemaphoreHandle_t mutex;
    bool     busy;
    uint8_t  pin;            // pino disputado
    uint8_t  rivalPin;       // pino do "dono anterior" a restaurar
    bool     rivalWasOn;     // estado salvo do rival
};

static BusState gBus[HAL_BUS_COUNT];
static bool  gLedState[40] = { false };
static float gAdcVref = 3.30f;
static bool  gProbeAvailable = false;
static bool  gHalReady = false;

// ----------------------------------------------------------------------------
// Compatibilidade LEDC (Arduino-ESP32 2.x usa canais, 3.x usa pinos)
// ----------------------------------------------------------------------------

void hal_pwm_attach(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t bits) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    (void)channel;
    ledcAttach(pin, freq, bits);
#else
    ledcSetup(channel, freq, bits);
    ledcAttachPin(pin, channel);
#endif
}

void hal_pwm_write(uint8_t pin, uint8_t channel, uint32_t duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    (void)channel;
    ledcWrite(pin, duty);
#else
    (void)pin;
    ledcWrite(channel, duty);
#endif
}

void hal_pwm_tone(uint8_t pin, uint8_t channel, uint32_t freq) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    (void)channel;
    ledcWriteTone(pin, freq);
#else
    (void)pin;
    ledcWriteTone(channel, freq);
#endif
}

void hal_pwm_stop(uint8_t pin, uint8_t channel) {
    hal_pwm_tone(pin, channel, 0);
    hal_pwm_write(pin, channel, 0);
}

// ----------------------------------------------------------------------------
// LEDs
// ----------------------------------------------------------------------------

void hal_led_write(uint8_t pin, bool on) {
    if (pin >= 40) return;
    gLedState[pin] = on;
    digitalWrite(pin, on ? LED_ACTIVE_LEVEL : LED_IDLE_LEVEL);
}

bool hal_led_state(uint8_t pin) {
    return (pin < 40) ? gLedState[pin] : false;
}

// ----------------------------------------------------------------------------
// Arbitragem de pinos compartilhados
// ----------------------------------------------------------------------------

static void bus_configure(HalBus bus, uint8_t pin, uint8_t rivalPin) {
    gBus[bus].mutex      = xSemaphoreCreateMutex();
    gBus[bus].busy       = false;
    gBus[bus].pin        = pin;
    gBus[bus].rivalPin   = rivalPin;
    gBus[bus].rivalWasOn = false;
}

bool hal_bus_acquire(HalBus bus, uint32_t timeoutMs) {
    if (bus >= HAL_BUS_COUNT) return false;
    BusState& b = gBus[bus];
    if (!b.mutex) return true;   // HAL ainda nao inicializada: segue em frente

    if (xSemaphoreTake(b.mutex, pdMS_TO_TICKS(timeoutMs)) != pdTRUE) {
        return false;
    }
    b.busy = true;

    // Se o pino tem um rival (LED), apaga o LED e memoriza para restaurar.
    if (b.rivalPin != 0xFF) {
        b.rivalWasOn = hal_led_state(b.rivalPin);
        if (b.rivalWasOn) {
            digitalWrite(b.rivalPin, LED_IDLE_LEVEL);
        }
        // Da tempo do pino estabilizar antes do proximo uso (OneWire e
        // sensivel a transitorios de nivel).
        delayMicroseconds(50);
    }
    return true;
}

void hal_bus_release(HalBus bus) {
    if (bus >= HAL_BUS_COUNT) return;
    BusState& b = gBus[bus];
    if (!b.mutex) return;

    // Restaura o LED que estava aceso antes de tomarmos o pino.
    if (b.rivalPin != 0xFF && b.rivalWasOn) {
        pinMode(b.rivalPin, OUTPUT);
        digitalWrite(b.rivalPin, LED_ACTIVE_LEVEL);
    }
    b.busy = false;
    xSemaphoreGive(b.mutex);
}

bool hal_bus_is_busy(HalBus bus) {
    return (bus < HAL_BUS_COUNT) ? gBus[bus].busy : false;
}

// ----------------------------------------------------------------------------
// ADC
// ----------------------------------------------------------------------------

void hal_adc_set_vref(float vref) {
    if (vref > 2.5f && vref < 3.9f) gAdcVref = vref;
}

float hal_adc_vref() { return gAdcVref; }

uint16_t hal_adc_read(uint8_t pin) {
    int v = analogRead(pin);
    if (v < 0) v = 0;
    if (v > ADC_MAX_COUNT) v = ADC_MAX_COUNT;
    return (uint16_t)v;
}

uint16_t hal_adc_read_avg(uint8_t pin, uint8_t samples) {
    if (samples < 3) samples = 3;
    if (samples > 64) samples = 64;

    uint32_t sum = 0;
    uint16_t vmin = 0xFFFF, vmax = 0;

    for (uint8_t i = 0; i < samples; i++) {
        uint16_t v = hal_adc_read(pin);
        sum += v;
        if (v < vmin) vmin = v;
        if (v > vmax) vmax = v;
        delayMicroseconds(60);
    }
    // Mediana aparada: descarta o extremo de cima e o de baixo.
    sum -= vmin;
    sum -= vmax;
    return (uint16_t)(sum / (samples - 2));
}

float hal_adc_to_volts(uint16_t raw) {
    // O ADC do ESP32 satura por baixo (~0.1 V) e por cima (~3.1 V) e tem um
    // erro sistematico de ate 6% no meio da escala. A correcao polinomial
    // abaixo e a aproximacao empirica amplamente usada na comunidade e reduz
    // o erro para menos de 1% na faixa util de 0.15 V a 3.10 V.
    if (raw == 0) return 0.0f;
    if (raw >= ADC_MAX_COUNT) return gAdcVref;

    const float x = (float)raw;
    // Curva de correcao (Espressif AN + medicoes de bancada)
    float corrected = -0.000000000000016f * x * x * x * x
                    +  0.000000000118171f * x * x * x
                    -  0.000000301211691f * x * x
                    +  0.001109019271794f * x
                    +  0.034143524634089f;

    if (corrected < 0.0f) corrected = 0.0f;
    // Reescala para a Vref real medida na placa.
    corrected *= (gAdcVref / 3.30f);
    if (corrected > gAdcVref) corrected = gAdcVref;
    return corrected;
}

float hal_adc_read_volts(uint8_t pin, uint8_t samples) {
    return hal_adc_to_volts(hal_adc_read_avg(pin, samples));
}

// ----------------------------------------------------------------------------
// Deteccao do circuito de excitacao das pontas
// ----------------------------------------------------------------------------

bool hal_probe_available()               { return gProbeAvailable; }
void hal_probe_set_available(bool avail) { gProbeAvailable = avail; }

// Verifica se o pino de excitacao realmente influencia a ponta 1. Se o
// divisor nao estiver montado, a leitura nao muda quando alternamos o drive.
static bool probe_hardware_detect() {
    pinMode(PIN_PROBE_DRIVE, OUTPUT);

    digitalWrite(PIN_PROBE_DRIVE, LOW);
    delay(5);
    uint16_t low = hal_adc_read_avg(PIN_ADC_PROBE1, 8);

    digitalWrite(PIN_PROBE_DRIVE, HIGH);
    delay(5);
    uint16_t high = hal_adc_read_avg(PIN_ADC_PROBE1, 8);

    digitalWrite(PIN_PROBE_DRIVE, LOW);
    pinMode(PIN_PROBE_DRIVE, INPUT);   // deixa em alta impedancia

    // Com o divisor montado e as pontas em aberto, HIGH leva a ponta perto de
    // 3.3 V e LOW perto de 0 V. Uma diferenca grande confirma a montagem.
    int delta = (int)high - (int)low;
    return (delta > 500);
}

// ----------------------------------------------------------------------------
// Touchscreen
// ----------------------------------------------------------------------------

bool hal_touch_read(uint16_t* outX, uint16_t* outY) {
    if (!touch.touched()) return false;

    TS_Point p = touch.getPoint();

    // Rejeita toques fracos: o XPT2046 gera leituras espurias quando o dedo
    // esta saindo da tela.
    if (p.z < TOUCH_MIN_PRESSURE) return false;

    long mx = map(p.x, TOUCH_RAW_X_MIN, TOUCH_RAW_X_MAX, 0, SCREEN_W_PX - 1);
    long my = map(p.y, TOUCH_RAW_Y_MIN, TOUCH_RAW_Y_MAX, 0, SCREEN_H_PX - 1);

    if (mx < 0)                mx = 0;
    if (mx > SCREEN_W_PX - 1)  mx = SCREEN_W_PX - 1;
    if (my < 0)                my = 0;
    if (my > SCREEN_H_PX - 1)  my = SCREEN_H_PX - 1;

    if (outX) *outX = (uint16_t)mx;
    if (outY) *outY = (uint16_t)my;
    return true;
}

bool hal_touch_wait(uint16_t* outX, uint16_t* outY, uint32_t timeoutMs) {
    uint32_t start = millis();
    while ((millis() - start) < timeoutMs) {
        if (hal_touch_read(outX, outY)) return true;
        vTaskDelay(pdMS_TO_TICKS(15));
    }
    return false;
}

void hal_touch_wait_release(uint32_t timeoutMs) {
    uint32_t start = millis();
    while (touch.touched() && (millis() - start) < timeoutMs) {
        vTaskDelay(pdMS_TO_TICKS(15));
    }
    vTaskDelay(pdMS_TO_TICKS(40));   // debounce mecanico
}

// ----------------------------------------------------------------------------
// Nomes e conflitos (telas de diagnostico)
// ----------------------------------------------------------------------------

const char* hal_pin_name(uint8_t pin) {
    switch (pin) {
        case PIN_TFT_CS:       return "TFT CS";
        case PIN_TFT_DC:       return "TFT DC";
        case PIN_TFT_BL:       return "Backlight";
        case PIN_TOUCH_CS:     return "Touch CS";
        case PIN_SD_CS:        return "SD CS";
        case PIN_ADC_PROBE1:   return "Ponta 1";
        case PIN_ADC_PROBE2:   return "Ponta 2";
        case PIN_ADC_ZMPT:     return "ZMPT101B";
        case PIN_PROBE_DRIVE:  return "Drive pontas";
        case PIN_LED_RED:      return "LED vermelho";
        case PIN_LED_GREEN:    return "LED verde";
        case PIN_LED_BLUE:     return "LED azul";
        case PIN_BUZZER:       return "Buzzer";
        default:               return "GPIO";
    }
}

const char* hal_pin_conflict(uint8_t pin) {
#if SONDVOLT_HW_REV == 0
    if (pin == PIN_LED_RED)  return "compartilhado com OneWire";
    if (pin == PIN_LED_BLUE) return "compartilhado com descarga";
    if (pin == PIN_I2C_SDA)  return "compartilhado com drive das pontas";
    if (pin == PIN_ADC_ZMPT) return "compartilhado com IRQ do touch";
#endif
    (void)pin;
    return nullptr;
}

// ----------------------------------------------------------------------------
// Inicializacao
// ----------------------------------------------------------------------------

void hal_init() {
    if (gHalReady) return;

    // --- ADC ---------------------------------------------------------------
    analogReadResolution(ADC_RESOLUTION_BITS);
    // Atenuacao maxima para cobrir a faixa completa de 0 a ~3.3 V.
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    analogSetPinAttenuation(PIN_ADC_PROBE1, ADC_ATTEN_DB_12);
    analogSetPinAttenuation(PIN_ADC_PROBE2, ADC_ATTEN_DB_12);
    analogSetPinAttenuation(PIN_ADC_ZMPT,   ADC_ATTEN_DB_12);
#else
    analogSetPinAttenuation(PIN_ADC_PROBE1, ADC_ATTEN_DB_11);
    analogSetPinAttenuation(PIN_ADC_PROBE2, ADC_ATTEN_DB_11);
    analogSetPinAttenuation(PIN_ADC_ZMPT,   ADC_ATTEN_DB_11);
#endif
    pinMode(PIN_ADC_PROBE1, INPUT);
    pinMode(PIN_ADC_PROBE2, INPUT);
    pinMode(PIN_ADC_ZMPT,   INPUT);

    // --- LEDs (anodo comum: HIGH apaga) ------------------------------------
    pinMode(PIN_LED_RED,   OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE,  OUTPUT);
    digitalWrite(PIN_LED_RED,   LED_IDLE_LEVEL);
    digitalWrite(PIN_LED_GREEN, LED_IDLE_LEVEL);
    digitalWrite(PIN_LED_BLUE,  LED_IDLE_LEVEL);

    // --- Descarga de capacitor ---------------------------------------------
    pinMode(PIN_CAP_DISCHARGE, OUTPUT);
    digitalWrite(PIN_CAP_DISCHARGE, LOW);

    // --- Arbitragem --------------------------------------------------------
#if ONEWIRE_SHARED_WITH_LED
    bus_configure(HAL_BUS_ONEWIRE, PIN_ONEWIRE, PIN_LED_RED);
#else
    bus_configure(HAL_BUS_ONEWIRE, PIN_ONEWIRE, 0xFF);
#endif
#if CAP_DISCHARGE_SHARED_WITH_LED
    bus_configure(HAL_BUS_DISCHARGE, PIN_CAP_DISCHARGE, PIN_LED_BLUE);
#else
    bus_configure(HAL_BUS_DISCHARGE, PIN_CAP_DISCHARGE, 0xFF);
#endif
    bus_configure(HAL_BUS_PROBE_DRIVE, PIN_PROBE_DRIVE, 0xFF);
    bus_configure(HAL_BUS_I2C,         PIN_I2C_SDA,     0xFF);

    // --- Deteccao do circuito de medicao ------------------------------------
    gProbeAvailable = probe_hardware_detect();

    gHalReady = true;
}
