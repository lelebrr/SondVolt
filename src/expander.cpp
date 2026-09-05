// ============================================================================
// Sondvolt v5.0 - Expansor de Linhas de Controle (implementacao)
// ============================================================================

#include "expander.h"
#include "hal.h"
#include "config.h"
#include <Wire.h>

static bool    gPresent = false;
static uint8_t gState   = 0x00;   // espelho local das 8 linhas

// ----------------------------------------------------------------------------
// Acesso ao chip
// ----------------------------------------------------------------------------
// O PCF8574 nao tem registradores: escrever um byte define as 8 saidas, ler
// um byte devolve as 8 entradas. Simples assim.

static bool pcf_write(uint8_t value) {
    if (!hal_bus_acquire(HAL_BUS_I2C, 100)) return false;

    Wire.beginTransmission(PCF8574_ADDR);
    Wire.write(value);
    bool ok = (Wire.endTransmission() == 0);

    hal_bus_release(HAL_BUS_I2C);

    if (ok) gState = value;
    else    gPresent = false;   // sumiu do barramento: nao insista
    return ok;
}

// ============================================================================
// CICLO DE VIDA
// ============================================================================

bool expander_init() {
    if (!hal_bus_acquire(HAL_BUS_I2C, 200)) {
        gPresent = false;
        return false;
    }

    Wire.beginTransmission(PCF8574_ADDR);
    gPresent = (Wire.endTransmission() == 0);
    hal_bus_release(HAL_BUS_I2C);

    if (gPresent) {
        // Comeca com tudo desligado. Em especial o boost: deixar 12 V ligado
        // sem ninguem esperando por eles e um jeito rapido de queimar algo.
        gState = 0x00;
        pcf_write(0x00);
        LOG_SERIAL_F("[EXP] PCF8574 detectado em 0x20");
    } else {
        LOG_SERIAL_F("[EXP] Expansor ausente: recursos da placa Bancada "
                     "desativados");
    }
    return gPresent;
}

bool expander_present()  { return gPresent; }
bool expander_redetect() { return expander_init(); }

// ============================================================================
// CONTROLE
// ============================================================================

bool expander_write(ExpanderLine line, bool level) {
    if (!gPresent) return false;
    if ((uint8_t)line > 7) return false;

    uint8_t next = gState;
    if (level) next |=  (uint8_t)(1u << (uint8_t)line);
    else       next &= (uint8_t)~(1u << (uint8_t)line);

    if (next == gState) return true;   // nada a fazer
    return pcf_write(next);
}

bool expander_read(ExpanderLine line) {
    if (!gPresent || (uint8_t)line > 7) return false;
    return (gState & (1u << (uint8_t)line)) != 0;
}

bool expander_write_all(uint8_t mask) {
    if (!gPresent) return false;
    return pcf_write(mask);
}

uint8_t expander_state() { return gState; }

void expander_all_off() {
    if (gPresent) pcf_write(0x00);
}

// ============================================================================
// ATALHOS DE ALTO NIVEL
// ============================================================================

bool expander_boost_enable(bool on) {
    if (!expander_write(EXP_LINE_BOOST, on)) return false;
    if (on) {
        // O MT3608 leva alguns milissegundos para a saida subir e estabilizar.
        // Medir antes disso da resultado errado e nao ha aviso nenhum.
        vTaskDelay(pdMS_TO_TICKS(60));
    }
    return true;
}

bool expander_ripple_coupling(bool on) {
    if (!expander_write(EXP_LINE_RIPPLE, on)) return false;
    if (on) {
        // O capacitor de acoplamento precisa carregar ate o ponto de
        // polarizacao antes da leitura fazer sentido.
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    return true;
}

bool expander_siggen_output(bool on) {
    return expander_write(EXP_LINE_SIGGEN, on);
}

bool expander_scope_attenuator(bool tenX) {
    if (!expander_write(EXP_LINE_ATTEN, tenX)) return false;
    vTaskDelay(pdMS_TO_TICKS(5));   // acomodacao do divisor
    return true;
}

bool expander_curve_sense(bool on) {
    return expander_write(EXP_LINE_CURVE, on);
}

bool expander_probe_isolate(bool isolated) {
    return expander_write(EXP_LINE_ISOLATE, isolated);
}

const char* expander_line_name(ExpanderLine line) {
    switch (line) {
        case EXP_LINE_BOOST:   return "Fonte 12V";
        case EXP_LINE_RIPPLE:  return "Acopl. ripple";
        case EXP_LINE_SIGGEN:  return "Saida gerador";
        case EXP_LINE_ATTEN:   return "Atenuador 10x";
        case EXP_LINE_CURVE:   return "Shunt da curva";
        case EXP_LINE_ISOLATE: return "Isolar pontas";
        case EXP_LINE_AUX1:    return "Reserva 1";
        case EXP_LINE_AUX2:    return "Reserva 2";
        default:               return "?";
    }
}
