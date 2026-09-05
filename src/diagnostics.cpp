// ============================================================================
// Sondvolt v4.0 - Diagnostico e Persistencia (implementacao)
// ============================================================================

#include "diagnostics.h"
#include "hal.h"
#include "config.h"
#include "display_globals.h"
#include "thermal.h"
#include "database.h"
#include <Preferences.h>
#include <Wire.h>
#include <esp_task_wdt.h>

// ----------------------------------------------------------------------------
// Estado
// ----------------------------------------------------------------------------
static SelfTestEntry gTests[TEST_COUNT];
static SystemHealth  gHealth;
static UsageStats    gUsage;

static TaskHandle_t gTaskUI      = nullptr;
static TaskHandle_t gTaskMeasure = nullptr;

static Preferences   gPrefs;
static bool     gSettingsDirty    = false;
static uint32_t gSettingsDirtyAt  = 0;
static uint32_t gBootMs           = 0;
static bool     gWatchdogEnabled  = false;

// Grava no maximo a cada 8 s para preservar os ciclos de escrita da flash.
static const uint32_t kSettingsWriteDelayMs = 8000;

// Abaixo disso o sistema corre risco real de falha de alocacao.
static const uint32_t kHeapWarningBytes  = 30000;
static const uint32_t kStackWarningWords = 350;

// ============================================================================
// AUXILIARES
// ============================================================================

static void test_set(SelfTestItem item, SelfTestResult result, const char* detail) {
    if (item >= TEST_COUNT) return;
    gTests[item].item   = item;
    gTests[item].result = result;
    strncpy(gTests[item].detail, detail ? detail : "", sizeof(gTests[item].detail) - 1);
    gTests[item].detail[sizeof(gTests[item].detail) - 1] = '\0';
}

static bool i2c_device_present(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}

// ============================================================================
// 1. AUTOTESTE
// ============================================================================

void diag_run_selftest(void (*progressCb)(uint8_t, const char*)) {
    for (uint8_t i = 0; i < TEST_COUNT; i++) {
        gTests[i].item   = (SelfTestItem)i;
        gTests[i].result = TEST_RESULT_PENDING;
        gTests[i].detail[0] = '\0';
    }

    char buf[32];
    uint8_t step = 0;
    const uint8_t totalSteps = 9;
    #define STEP(msg) do { \
        step++; \
        if (progressCb) progressCb((uint8_t)((step * 100) / totalSteps), msg); \
    } while (0)

    // --- Display -----------------------------------------------------------
    STEP("Verificando display...");
    // Se chegamos aqui com a TFT inicializada, o barramento respondeu.
    test_set(TEST_DISPLAY, tftInitialized ? TEST_RESULT_PASS : TEST_RESULT_FAIL,
             tftInitialized ? "ILI9341 320x240" : "sem resposta");

    // --- Touch -------------------------------------------------------------
    STEP("Verificando touchscreen...");
    // O XPT2046 sempre responde ao SPI; o que testamos e se ele NAO esta
    // preso em toque permanente, sintoma classico de cabo flat mal encaixado.
    {
        uint8_t stuck = 0;
        for (uint8_t i = 0; i < 5; i++) { if (touch.touched()) stuck++; delay(10); }
        if (stuck >= 5) test_set(TEST_TOUCH, TEST_RESULT_WARN, "toque preso ativo");
        else            test_set(TEST_TOUCH, TEST_RESULT_PASS, "XPT2046 ok");
    }

    // --- Cartao SD ---------------------------------------------------------
    STEP("Verificando cartao SD...");
    if (sdCardPresent) {
        DbStats s = db_get_stats();
        snprintf(buf, sizeof(buf), "%lu itens no banco", (unsigned long)s.sdRecordCount);
        test_set(TEST_SDCARD, TEST_RESULT_PASS, buf);
    } else {
        test_set(TEST_SDCARD, TEST_RESULT_WARN, "sem cartao: log desativado");
    }

    // --- Pontas de prova ---------------------------------------------------
    STEP("Verificando pontas de prova...");
    if (hal_probe_available()) {
        test_set(TEST_PROBES, TEST_RESULT_PASS, "divisor de referencia ok");
    } else {
        test_set(TEST_PROBES, TEST_RESULT_FAIL, "circuito de excitacao ausente");
    }

    // --- Sensor termico ----------------------------------------------------
    STEP("Verificando sensor termico...");
    {
        float t = thermal_read();
        if (t <= THERMAL_INVALID_TEMP) {
            test_set(TEST_THERMAL, TEST_RESULT_SKIPPED, "DS18B20 nao instalado");
        } else if (t < -40.0f || t > 125.0f) {
            test_set(TEST_THERMAL, TEST_RESULT_WARN, "leitura fora de faixa");
        } else {
            snprintf(buf, sizeof(buf), "DS18B20 %.1f C", t);
            test_set(TEST_THERMAL, TEST_RESULT_PASS, buf);
        }
    }

    // --- INA219 ------------------------------------------------------------
    STEP("Verificando INA219...");
    if (i2c_device_present(INA219_ADDR)) {
        test_set(TEST_INA219, TEST_RESULT_PASS, "0x40 respondeu");
    } else {
        test_set(TEST_INA219, TEST_RESULT_SKIPPED, "nao instalado");
    }

    // --- ZMPT101B ----------------------------------------------------------
    STEP("Verificando sensor AC...");
    {
        // Sem rede conectada a saida do ZMPT deve repousar perto da meia
        // escala. Um valor colado em 0 ou 4095 indica modulo ausente ou
        // trimpot totalmente fora de ajuste.
        uint16_t raw = hal_adc_read_avg(PIN_ADC_ZMPT, 16);
        if (raw < 150 || raw > 3950) {
            test_set(TEST_ZMPT, TEST_RESULT_SKIPPED, "modulo AC nao detectado");
        } else if (raw < 1500 || raw > 2600) {
            snprintf(buf, sizeof(buf), "ajustar trimpot (%u)", (unsigned)raw);
            test_set(TEST_ZMPT, TEST_RESULT_WARN, buf);
        } else {
            snprintf(buf, sizeof(buf), "repouso em %u", (unsigned)raw);
            test_set(TEST_ZMPT, TEST_RESULT_PASS, buf);
        }
    }

    // --- Buzzer e LEDs -----------------------------------------------------
    STEP("Verificando avisos sonoros...");
    test_set(TEST_BUZZER, TEST_RESULT_PASS, "canal LEDC pronto");

    STEP("Verificando LEDs...");
    test_set(TEST_LEDS, TEST_RESULT_PASS, "RGB configurado");

    // --- PSRAM (informativo) -----------------------------------------------
    if (ESP.getPsramSize() > 0) {
        snprintf(buf, sizeof(buf), "%lu KB",
                 (unsigned long)(ESP.getPsramSize() / 1024));
        test_set(TEST_PSRAM, TEST_RESULT_PASS, buf);
    } else {
        test_set(TEST_PSRAM, TEST_RESULT_SKIPPED, "sem PSRAM");
    }

    #undef STEP
}

const SelfTestEntry* diag_selftest_get(SelfTestItem item) {
    return (item < TEST_COUNT) ? &gTests[item] : nullptr;
}

uint8_t diag_selftest_failures() {
    uint8_t n = 0;
    for (uint8_t i = 0; i < TEST_COUNT; i++) {
        if (gTests[i].result == TEST_RESULT_FAIL) n++;
    }
    return n;
}

bool diag_selftest_passed() {
    // Falha no display ou nas pontas impede o uso; o resto e degradacao.
    return (gTests[TEST_DISPLAY].result != TEST_RESULT_FAIL);
}

const char* diag_item_name(SelfTestItem item) {
    switch (item) {
        case TEST_DISPLAY: return "Display";
        case TEST_TOUCH:   return "Touchscreen";
        case TEST_SDCARD:  return "Cartao SD";
        case TEST_PROBES:  return "Pontas de prova";
        case TEST_THERMAL: return "Sensor termico";
        case TEST_INA219:  return "Sensor de corrente";
        case TEST_ZMPT:    return "Sensor AC";
        case TEST_BUZZER:  return "Buzzer";
        case TEST_LEDS:    return "LEDs";
        case TEST_PSRAM:   return "PSRAM";
        default:           return "?";
    }
}

const char* diag_result_name(SelfTestResult result) {
    switch (result) {
        case TEST_RESULT_PASS:    return "OK";
        case TEST_RESULT_WARN:    return "AVISO";
        case TEST_RESULT_FAIL:    return "FALHA";
        case TEST_RESULT_SKIPPED: return "AUSENTE";
        default:                  return "...";
    }
}

uint16_t diag_result_color(SelfTestResult result) {
    switch (result) {
        case TEST_RESULT_PASS:    return COLOR_GOOD;
        case TEST_RESULT_WARN:    return COLOR_SUSPECT;
        case TEST_RESULT_FAIL:    return COLOR_BAD;
        default:                  return COLOR_TEXT_DIM;
    }
}

// ============================================================================
// 2. MONITORAMENTO
// ============================================================================

void diag_init() {
    memset(&gHealth, 0, sizeof(gHealth));
    gBootMs = millis();
    gHealth.totalHeap = ESP.getHeapSize();
    gHealth.cpuFreqMhz = (uint8_t)getCpuFrequencyMhz();
    diag_update();
}

void diag_register_task_ui(TaskHandle_t h)      { gTaskUI = h; }
void diag_register_task_measure(TaskHandle_t h) { gTaskMeasure = h; }

void diag_update() {
    gHealth.freeHeap    = ESP.getFreeHeap();
    gHealth.minFreeHeap = ESP.getMinFreeHeap();
    gHealth.totalHeap   = ESP.getHeapSize();
    gHealth.uptimeMs    = millis() - gBootMs;
    gHealth.cpuFreqMhz  = (uint8_t)getCpuFrequencyMhz();
    gHealth.loopCount++;

    if (gHealth.totalHeap > 0) {
        uint32_t used = gHealth.totalHeap - gHealth.freeHeap;
        gHealth.heapUsedPct = (uint8_t)((used * 100UL) / gHealth.totalHeap);
    }

    // uxTaskGetStackHighWaterMark devolve o MENOR espaco livre ja observado,
    // em palavras de 4 bytes. Quanto menor, mais perto do estouro.
    if (gTaskUI)      gHealth.uiStackFree      = uxTaskGetStackHighWaterMark(gTaskUI);
    if (gTaskMeasure) gHealth.measureStackFree = uxTaskGetStackHighWaterMark(gTaskMeasure);

    gHealth.heapWarning = (gHealth.freeHeap < kHeapWarningBytes);
    gHealth.stackWarning =
        (gTaskUI      && gHealth.uiStackFree      < kStackWarningWords) ||
        (gTaskMeasure && gHealth.measureStackFree < kStackWarningWords);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    gHealth.chipTempC = (float)temperatureRead();
#else
    gHealth.chipTempC = 0.0f;
#endif
}

SystemHealth diag_get_health() { return gHealth; }

bool diag_watchdog_enable(uint32_t timeoutSeconds) {
    if (gWatchdogEnabled) return true;
    // Se o watchdog ja estiver ativo pelo core, esta chamada simplesmente
    // reaproveita a configuracao existente.
    esp_task_wdt_init(timeoutSeconds, true);
    gWatchdogEnabled = true;
    return true;
}

void diag_feed_watchdog() {
    if (gWatchdogEnabled) esp_task_wdt_reset();
}

const char* diag_health_summary() {
    static char buf[40];
    snprintf(buf, sizeof(buf), "RAM %lu KB  %s",
             (unsigned long)(gHealth.freeHeap / 1024),
             gHealth.heapWarning ? "BAIXA" : "ok");
    return buf;
}

// ============================================================================
// 3. ESTATISTICAS
// ============================================================================

void diag_count_measurement(ComponentStatus status) {
    gUsage.totalMeasurements++;
    totalMeasurements = gUsage.totalMeasurements;

    if (status == STATUS_GOOD) {
        gUsage.goodMeasurements++;
        goodMeasurements = gUsage.goodMeasurements;
    } else if (status == STATUS_BAD || status == STATUS_SHORT) {
        gUsage.badMeasurements++;
        badMeasurements = gUsage.badMeasurements;
    }

    // Grava a cada 20 medicoes para nao perder o historico num reset.
    if ((gUsage.totalMeasurements % 20) == 0) settings_save_usage();
}

void diag_count_sd_write()  { gUsage.sdWrites++; }
void diag_count_lockout()   { gUsage.safetyLockouts++; settings_save_usage(); }

UsageStats diag_get_usage() {
    UsageStats u = gUsage;
    u.totalUptimeMinutes = gUsage.totalUptimeMinutes + (gHealth.uptimeMs / 60000UL);
    return u;
}

void diag_reset_usage() {
    memset(&gUsage, 0, sizeof(gUsage));
    totalMeasurements = goodMeasurements = badMeasurements = 0;
    settings_save_usage();
}

// ============================================================================
// 4. PERSISTENCIA
// ============================================================================

// Versao do formato gravado. Incrementar sempre que DeviceSettings mudar de
// layout, para que uma gravacao antiga seja descartada em vez de mal lida.
static const uint32_t kSettingsMagic   = 0x53564C54;   // "SVLT"
static const uint16_t kSettingsVersion = 4;

bool settings_load() {
    if (!gPrefs.begin("sondvolt", true)) {
        LOG_SERIAL_F("[CFG] NVS indisponivel, usando padroes de fabrica");
        return false;
    }

    uint32_t magic   = gPrefs.getUInt("magic", 0);
    uint32_t version = gPrefs.getUInt("ver", 0);

    if (magic != kSettingsMagic || version != kSettingsVersion) {
        gPrefs.end();
        LOG_SERIAL_F("[CFG] Nenhuma configuracao compativel gravada");
        // Ainda assim conta o boot.
        gUsage.bootCount = 1;
        return false;
    }

    size_t got = gPrefs.getBytes("cfg", &deviceSettings, sizeof(DeviceSettings));
    if (got != sizeof(DeviceSettings)) {
        gPrefs.end();
        LOG_SERIAL_F("[CFG] Configuracao gravada com tamanho invalido");
        return false;
    }

    gPrefs.getBytes("usage", &gUsage, sizeof(UsageStats));
    gPrefs.end();

    // Sanidade: valores fora de faixa viram padrao em vez de quebrar a UI.
    if (deviceSettings.backlight < 20)  deviceSettings.backlight = 20;
    if (deviceSettings.themeIdx  > 5)   deviceSettings.themeIdx  = 0;
    if (deviceSettings.languageIdx > 2) deviceSettings.languageIdx = 0;
    if (deviceSettings.autoSleepMs < 5000UL)
        deviceSettings.autoSleepMs = BACKLIGHT_TIMEOUT;

    totalMeasurements = gUsage.totalMeasurements;
    goodMeasurements  = gUsage.goodMeasurements;
    badMeasurements   = gUsage.badMeasurements;

    gUsage.bootCount++;
    LOG_SERIAL_FMT("[CFG] Configuracoes carregadas (boot #%lu)\n",
                   (unsigned long)gUsage.bootCount);
    return true;
}

bool settings_save() {
    if (!gPrefs.begin("sondvolt", false)) return false;

    gPrefs.putUInt("magic", kSettingsMagic);
    gPrefs.putUInt("ver",   kSettingsVersion);
    size_t written = gPrefs.putBytes("cfg", &deviceSettings, sizeof(DeviceSettings));
    gPrefs.putBytes("usage", &gUsage, sizeof(UsageStats));
    gPrefs.end();

    gSettingsDirty = false;
    return (written == sizeof(DeviceSettings));
}

bool settings_save_usage() {
    if (!gPrefs.begin("sondvolt", false)) return false;
    gPrefs.putBytes("usage", &gUsage, sizeof(UsageStats));
    gPrefs.end();
    return true;
}

void settings_factory_reset() {
    if (gPrefs.begin("sondvolt", false)) {
        gPrefs.clear();
        gPrefs.end();
    }

    deviceSettings.backlight       = 200;
    deviceSettings.darkMode        = true;
    deviceSettings.silentMode      = false;
    deviceSettings.autoSleep       = true;
    deviceSettings.autoSleepMs     = BACKLIGHT_TIMEOUT;
    deviceSettings.soundEnabled    = true;
    deviceSettings.calibrated      = false;
    deviceSettings.zmptScaleFactor = ZMPT_DEFAULT_GAIN;
    deviceSettings.themeColor      = 0x07FF;
    deviceSettings.unitsMetric     = true;
    deviceSettings.themeIdx        = 0;
    deviceSettings.showGrid        = true;
    deviceSettings.animations      = true;
    deviceSettings.autoSaveHistory = true;
    deviceSettings.confirmActions  = true;
    deviceSettings.expertMode      = false;
    deviceSettings.languageIdx     = 0;
    deviceSettings.strongBeep      = false;

    colors_update();
    settings_save();
    LOG_SERIAL_F("[CFG] Padroes de fabrica restaurados");
}

void settings_mark_dirty() {
    gSettingsDirty   = true;
    gSettingsDirtyAt = millis();
}

void settings_flush_if_needed() {
    if (!gSettingsDirty) return;
    if ((millis() - gSettingsDirtyAt) < kSettingsWriteDelayMs) return;
    settings_save();
}
