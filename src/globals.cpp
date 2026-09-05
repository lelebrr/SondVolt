// ============================================================================
// Sondvolt v3.0 - Variaveis Globais (Implementacao)
// Descricao: Variaveis globais compartilhadas entre modulos
// Versao: CYD Edition para ESP32-2432S028R
// ============================================================================

#include "globals.h"
#include "config.h"
#include "database.h"
#include "theme.h"

// ============================================================================
// ESTADO DA APLICACAO
// ============================================================================
volatile AppState currentAppState = STATE_SPLASH;
volatile AppState previousAppState = STATE_SPLASH;

// ============================================================================
// CONTROLE DE TEMPO
// ============================================================================
unsigned long lastActivityMs = 0;
unsigned long backlightOffTimeMs = 0;
bool backlightOn = true;

// ============================================================================
// MEDICOES ATUAIS
// ============================================================================
float lastResistance = 0.0f;
float lastCapacitance = 0.0f;
float lastInductance = 0.0f;
float lastVoltage = 0.0f;
float lastCurrent = 0.0f;
float lastTemperature = 0.0f;
bool isDischarging = false;
float dischargeProgress = 0.0f;

ComparatorRef referenceComp = {0.0f, COMP_UNKNOWN, "", false};

// ============================================================================
// ESTATISTICAS
// ============================================================================
uint32_t totalMeasurements = 0;
uint32_t goodMeasurements = 0;
uint32_t badMeasurements = 0;

// ============================================================================
// CALIBRACAO DOS PROBES
// ============================================================================
float probeOffsetResistance = 0.0f;
float probeOffsetCapacitance = 0.0f;

// ============================================================================
// SD CARD
// ============================================================================
bool sdCardPresent = false;
bool sdCardError = false;

// ============================================================================
// BANCO DE DADOS NA RAM
// ============================================================================
ComponentDatabase componentDB;

// ============================================================================
// DISPLAY TFT
// ============================================================================
bool tftInitialized = false;

// ============================================================================
// CONFIGURACOES DO DISPOSITIVO
// ============================================================================
DeviceSettings deviceSettings = {
    .backlight = 200,
    .darkMode = true,
    .silentMode = false,
    .autoSleep = true,
    .autoSleepMs = BACKLIGHT_TIMEOUT,
    .soundEnabled = true,
    .calibrated = false,
    .zmptScaleFactor = ZMPT_DEFAULT_GAIN,
    .themeColor = 0x07FF,   // ciano; COLOR_PRIMARY e variavel, nao constante
    .unitsMetric = true,
    .themeIdx = 0,
    .showGrid = true,
    .animations = true,
    .autoSaveHistory = true,
    .confirmActions = true,
    .expertMode = false,
    .languageIdx = 0,
    .strongBeep = false,
    .wifiEnabled = false,       // desligado por padrao: economiza energia
    .webServerEnabled = true,
    .otaEnabled = true,
    .scopeRateIdx = 3,          // 100 kSPS
    .thermalPalette = 0,        // ferro
    .pairTolerance = 5.0f
};

LogEntry recentTests[6] = {0};
void update_recent_tests(const char* name, float value, const char* status) {
    for (int i = 5; i > 0; i--) recentTests[i] = recentTests[i-1];

    recentTests[0].timestamp = millis();
    recentTests[0].value     = value;

    // snprintf sempre termina a string; strncpy nao, quando a origem enche o
    // destino. Depois disso qualquer strlen() lia fora do buffer.
    snprintf(recentTests[0].componentName, sizeof(recentTests[0].componentName),
             "%s", name ? name : "?");
    snprintf(recentTests[0].status, sizeof(recentTests[0].status),
             "%s", status ? status : "");
}

// ============================================================================
// SISTEMA DE CORES DINAMICAS
// ============================================================================
uint16_t clr_back    = 0x0863;
uint16_t clr_surf    = 0x10C4;
uint16_t clr_text    = 0xFFFF;
uint16_t clr_dim     = 0xAD55;
uint16_t clr_primary = 0x07FF;

void colors_update() {
    // A paleta agora vive em theme.cpp. Esta funcao continua existindo porque
    // varias telas a chamam, mas o que ela faz e delegar - nao ha mais duas
    // definicoes de cor concorrendo.
    ThemeAccent accent = (deviceSettings.themeIdx < TH_ACCENT_COUNT)
                       ? (ThemeAccent)deviceSettings.themeIdx
                       : TH_ACCENT_TEAL;

    theme_apply(deviceSettings.darkMode, accent);

    // As variaveis clr_* sobrevivem para o codigo que ainda as usa via
    // config.h. Elas espelham a paleta ativa.
    clr_back    = th_bg0;
    clr_surf    = th_bg1;
    clr_text    = th_txHi;
    clr_dim     = th_txLo;
    clr_primary = th_accent;

    deviceSettings.themeColor = th_accent;
}
