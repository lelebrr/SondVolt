// ============================================================================
// Sondvolt v3.1 — Sistema de Segurança Elétrica
// Descrição: Proteção completa contra tensões perigosas (220V AC)
// Versão: CYD Edition para ESP32-2432S028R
// ============================================================================
#ifndef SAFETY_H
#define SAFETY_H

#include <stdint.h>
#include <Arduino.h>

// ============================================================================
// CONSTANTES DE SEGURANÇA ELÉTRICA
// ============================================================================

// Limiares de tensão perigosa (volts)
#ifndef SAFETY_VOLTAGE_DANGER_AC
#define SAFETY_VOLTAGE_DANGER_AC      50.0f       // Tensão AC considerada perigosa
#endif
#define SAFETY_VOLTAGE_220V_MIN    180.0f       // Mínimo para 220V
#ifndef SAFETY_VOLTAGE_220V_MAX
#define SAFETY_VOLTAGE_220V_MAX   250.0f       // Máximo para 220V
#endif
#define SAFETY_VOLTAGE_110V_MIN   90.0f        // Mínimo para 110V
#ifndef SAFETY_VOLTAGE_110V_MAX
#define SAFETY_VOLTAGE_110V_MAX   130.0f       // Máximo para 110V
#endif

// Limiares de tensão DC perigosa
#ifndef SAFETY_VOLTAGE_DC_DANGER
#define SAFETY_VOLTAGE_DC_DANGER  60.0f       // DC acima de 60V é perigoso
#endif

// Tempo de bloqueio após detecção perigosa (milissegundos)
#ifndef SAFETY_LOCKOUT_MS
#define SAFETY_LOCKOUT_MS         10000UL     // 10 segundos de bloqueio
#endif
#define SAFETY_LOCKOUT_WARNING_MS 5000UL    //Aviso 5 segundos antes de desbloquear

// Limiar de curto-circuito (ohms)
#define SAFETY_SHORT_CIRCUIT      1.0f        // Abaixo de 1Ω = curto

// ============================================================================
// CONSTANTES DE ALERTA VISUAL
// ============================================================================

// Cores de alerta (RGB565)
#ifndef SAFETY_COLOR_DANGER
#define SAFETY_COLOR_DANGER       0xF800      // Vermelho forte
#endif
#ifndef SAFETY_COLOR_WARNING
#define SAFETY_COLOR_WARNING      0xFD20      // Laranja
#endif
#ifndef SAFETY_COLOR_SAFE
#define SAFETY_COLOR_SAFE        0x07E0       // Verde
#endif
#ifndef SAFETY_COLOR_ALERT_BG
#define SAFETY_COLOR_ALERT_BG     0x9800      // Vermelho escuro
#endif

// Mensagens de alerta
#ifndef SAFETY_MSG_DANGER_220V
#define SAFETY_MSG_DANGER_220V      " PERIGO! 220V DETECTADO "
#endif
#ifndef SAFETY_MSG_DANGER_HIGH
#define SAFETY_MSG_DANGER_HIGH      " PERIGO! TENSÃO ALTA "
#endif
#define SAFETY_MSG_REMOVE          " REMOVA AS PONTEIRAS IMEDIATAMENTE "
#define SAFETY_MSG_CONFIRM       " CONFIRME QUE INSTALOU FUSÍVEL E VARISTOR? "
#define SAFETY_MSG_LOCKOUT        " EQUIPAMENTO BLOQUEADO "
#define SAFETY_MSG_WAIT          " AGUARDE... "
#define SAFETY_MSG_CHECK_PROBES  " VERIFIQUE AS PONTEIRAS "

// ============================================================================
// CONSTANTES DE ALERTA SONORO
// ============================================================================

// Frequências de alerta (Hz)
#ifndef SAFETY_BEEP_DANGER
#define SAFETY_BEEP_DANGER         2000        // Beep perigo (agudo)
#endif
#ifndef SAFETY_BEEP_WARNING
#define SAFETY_BEEP_WARNING       1000        // Beep aviso (médio)
#endif
#ifndef SAFETY_BEEP_CONFIRM
#define SAFETY_BEEP_CONFIRM      880         // Beep confirmação
#endif
#ifndef SAFETY_BEEP_OK
#define SAFETY_BEEP_OK            523         // Beep OK (grave)
#endif

// Durações de beep (ms)
#ifndef SAFETY_BEEP_DURATION_FAST
#define SAFETY_BEEP_DURATION_FAST 100         // Beep rápido
#endif
#define SAFETY_BEEP_DURATION_SLOW 500         // Beep lento
#define SAFETY_BEEP_DURATION_LONG 1000        // Beep longo

// ============================================================================
// CONSTANTES DE LED
// ============================================================================

// Intervalos de flash (ms)
#define SAFETY_LED_FLASH_FAST     100         // Flash rápido (perigo)
#define SAFETY_LED_FLASH_MED      300         // Flash médio (aviso)
#define SAFETY_LED_FLASH_SLOW     500         // Flash lento (OK)

// ============================================================================
// ESTADOS DE SEGURANÇA
// ============================================================================

enum SafetyState {
    SAFETY_STATE_SAFE = 0,           // Seguro para medir
    SAFETY_STATE_CHECKING = 1,        // Verificando tensões
    SAFETY_STATE_DANGER = 2,         // Perigo detectado
    SAFETY_STATE_LOCKOUT = 3,         // Equipamento bloqueado
    SAFETY_STATE_CONFIRMATION = 4,   // Aguardando confirmação
    SAFETY_STATE_WARNING = 5           // Modo aviso
};

enum SafetyAlertLevel {
    SAFETY_ALERT_NONE = 0,            // Sem alerta
    SAFETY_ALERT_LOW = 1,            // Alerta baixo (50-100V)
    SAFETY_ALERT_MEDIUM = 2,          // Alerta médio (100-180V)
    SAFETY_ALERT_HIGH = 3,           // Alerta alto (220V detectado)
    SAFETY_ALERT_CRITICAL = 4        // Alerta crítico (curto-circuito)
};

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

// Estado atual do sistema de segurança
typedef struct {
    SafetyState state;                   // Estado atual
    SafetyAlertLevel alertLevel;         // Nível de alerta
    float lastDetectedVoltage;          //Última tensão detectada
    bool hasFuseInstalled;              // Usuário confirmou fusível
    bool hasVaristorInstalled;          // Usuário confirmou varistor
    unsigned long lockoutEndTime;       // Quando termina o bloqueio
    unsigned long lastCheckTime;        // Última verificação
    uint8_t dangerCount;                // Contador de detecções perigosas
    bool safetyAcknowledged;           // Usuário reconheceu o aviso
} SafetyStatus;

// Resultado da verificação de segurança
typedef struct {
    bool isSafe;                        // É seguro continuar
    float detectedVoltage;             // Tensão detectada
    bool isAcDanger;                   // É tensão AC perigosa
    bool isDcDanger;                   // É tensão DC perigosa
    bool isShortCircuit;                // É curto-circuito
    SafetyAlertLevel alertLevel;        // Nível de alerta
    const char* message;               // Mensagem de alerta
} SafetyCheckResult;

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

// Estado de segurança global (definido em safety.cpp)
extern SafetyStatus safetyStatus;

// Flags de configuração de segurança
extern bool safetyCheckEnabled;
extern bool safetySoundEnabled;
extern bool safetyLedEnabled;
extern bool safetyAutoLockoutEnabled;

// ============================================================================
// PROTÓTIPOS — INICIALIZAÇÃO
// ============================================================================

// Inicializa o sistema de segurança
void safety_init();

// Reseta o estado de segurança
void safety_reset();

// ============================================================================
// PROTÓTIPOS — VERIFICAÇÃO
// ============================================================================

// Verifica tensão nas pontas de prova (retorna resultado completo)
SafetyCheckResult safety_check_voltage(float voltage);

// Verifica tensão AC perigosa (retorna true se perigoso)
bool safety_is_ac_danger(float voltage);

// Verifica tensão DC perigosa (retorna true se perigoso)
bool safety_is_dc_danger(float voltage);

// Verifica curto-circuito (retorna true se curto)
bool safety_is_short_circuit(float resistance);

// ============================================================================
// PROTÓTIPOS — ALERTAS
// ============================================================================

// Ativa alerta de perigo (visuais + sonoros)
void safety_trigger_alert(SafetyAlertLevel level);

// Ativa modo piscante de LED vermelho
void safety_alert_led_flash(bool enable);

// Ativa alerta sonoro de perigo
void safety_alert_sound_danger();

// Ativa alerta sonoro de confirmação
void safety_alert_sound_confirm();

// Para todos os alertas
void safety_alert_stop();

// ============================================================================
// PROTÓTIPOS — TELA DE PERIGO
// ============================================================================

// Desenha tela de perigo (220V detectado)
void safety_draw_danger_screen(const char* message, float voltage);

// Desenha tela de bloqueio
void safety_draw_lockout_screen(unsigned long remainingMs);

// Desenha tela de confirmação de segurança
void safety_draw_confirm_screen();

// Desenha tela de verificação (pré-medição)
void safety_draw_check_screen();

// ============================================================================
// PROTÓTIPOS — BLOQUEIO
// ============================================================================

// Ativa bloqueio de segurança
void safety_activate_lockout();

// Desativa bloqueio de segurança
void safety_deactivate_lockout();

// Verifica se está em bloqueio (retorna true se bloqueado)
bool safety_is_locked_out();

// Verifica se pode continuar (retorna true se seguro)
bool safety_can_proceed();

// Registra reconhecimento do aviso de segurança
void safety_acknowledge_warning();

// ============================================================================
// PROTÓTIPOS — SPLASH DE SEGURANÇA
// ============================================================================

// Desenha splash screen de segurança na inicialização
void safety_draw_splash();

// Exibe splash de segurança animado
void safety_show_splash_animated();

// ============================================================================
// PROTÓTIPOS — MENU
// ============================================================================

// Verifica segurança antes de entrar no modo multímetro
// Retorna true se pode prosseguir, false se bloqueado
bool safety_check_before_multimeter();

// Verifica segurança antes de medir rede elétrica (AC)
// Retorna true se confirmado, false se cancelado
bool safety_confirm_electrical_measurement();

// ============================================================================
// PROTÓTIPOS — ATUALIZAÇÃO
// ============================================================================

// Atualiza o sistema de segurança (chamar no loop principal)
void safety_update();

// ============================================================================
// PROTÓTIPOS — DETECÇÃO AUTOMÁTICA
// ============================================================================

//Realiza detecção automática de tensão perigosa nas pontas de prova
//Retorna resultado completo
SafetyCheckResult safety_detect_danger();

// ============================================================================
// PROTÓTIPOS — CONFIGURAÇÃO
// ============================================================================

// Ativa/desativa verificação automática
void safety_set_check_enabled(bool enabled);

// Ativa/desativa som de alerta
void safety_set_sound_enabled(bool enabled);

// Ativa/desativa LED de alerta
void safety_set_led_enabled(bool enabled);

// Ativa/desativa bloqueio automático
void safety_set_lockout_enabled(bool enabled);

#endif // SAFETY_H