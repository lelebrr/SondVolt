// ============================================================================
// Sondvolt v5.0 - Servicos de Rede (implementacao)
// ============================================================================

#include "netsvc.h"
#include "config.h"
#include "globals.h"
#include "logger.h"
#include "jobs.h"
#include "multimeter.h"
#include "measurements.h"
#include "diagnostics.h"
#include "display_globals.h"
#include "display_mutex.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <SdFat.h>
#include <time.h>

extern SdFat sd;

// ----------------------------------------------------------------------------
// Estado
// ----------------------------------------------------------------------------
static NetStatus    gStatus;
static WebServer    gServer(NET_HTTP_PORT);
static Preferences  gPrefs;
static bool         gServerStarted = false;
static bool         gOtaStarted    = false;
static bool         gOtaRunning    = false;
static uint8_t      gOtaProgress   = 0;
static bool         gTimeValid     = false;
static uint32_t     gManualEpoch   = 0;    // base definida a mao
static uint32_t     gManualMillis  = 0;

static char gSsid[33]     = { 0 };
static char gPassword[65] = { 0 };

// ============================================================================
// PAGINA WEB
// ============================================================================
// HTML servido direto da flash. Sem framework, sem CDN: a pagina precisa
// abrir mesmo quando o aparelho esta em modo ponto de acesso, sem internet.

static const char kIndexHtml[] PROGMEM = R"HTML(<!doctype html>
<html lang="pt-BR"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Sondvolt</title><style>
:root{--bg:#0d1117;--surf:#161b22;--line:#30363d;--tx:#e6edf3;--dim:#8b949e;--ac:#2dd4bf}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--tx);font:15px/1.5 system-ui,sans-serif}
header{padding:16px;border-bottom:1px solid var(--line);display:flex;
 justify-content:space-between;align-items:baseline}
h1{margin:0;font-size:18px;letter-spacing:.5px}
.tag{color:var(--dim);font-size:12px}
main{padding:16px;max-width:760px;margin:0 auto}
.card{background:var(--surf);border:1px solid var(--line);border-radius:10px;
 padding:16px;margin-bottom:14px}
.card h2{margin:0 0 12px;font-size:13px;text-transform:uppercase;
 letter-spacing:1px;color:var(--ac)}
.big{font-size:34px;font-weight:600;font-variant-numeric:tabular-nums}
.unit{font-size:15px;color:var(--dim);margin-left:6px}
table{width:100%;border-collapse:collapse;font-size:13px}
td{padding:5px 0;border-bottom:1px solid var(--line)}
td:last-child{text-align:right;color:var(--dim)}
tr:last-child td{border-bottom:none}
a.btn{display:inline-block;padding:9px 16px;background:var(--ac);color:#04231f;
 border-radius:7px;text-decoration:none;font-weight:600;font-size:13px;
 margin-right:8px;margin-top:4px}
.muted{color:var(--dim);font-size:13px}
</style></head><body>
<header><h1>SONDVOLT</h1><span class="tag" id="fw">--</span></header>
<main>
<div class="card"><h2>Leitura ao vivo</h2>
<div><span class="big" id="val">--</span><span class="unit" id="unit"></span></div>
<div class="muted" id="mode">aguardando...</div></div>

<div class="card"><h2>Estado</h2>
<table><tbody id="st"></tbody></table></div>

<div class="card"><h2>Arquivos</h2>
<a class="btn" href="/log.csv">Baixar historico</a>
<a class="btn" href="/trabalho.csv">Trabalho atual</a>
<p class="muted" id="job"></p></div>
</main>
<script>
function row(k,v){return '<tr><td>'+k+'</td><td>'+v+'</td></tr>';}
async function tick(){
 try{
  const r=await fetch('/api/status'); const d=await r.json();
  document.getElementById('val').textContent=d.value;
  document.getElementById('unit').textContent=d.unit;
  document.getElementById('mode').textContent=d.mode;
  document.getElementById('fw').textContent='v'+d.fw;
  document.getElementById('st').innerHTML=
    row('Hora',d.time)+row('Ligado ha',d.uptime)+row('RAM livre',d.heap+' KB')+
    row('Cartao SD',d.sd)+row('Medicoes',d.count)+row('WiFi',d.rssi+' dBm');
  document.getElementById('job').textContent=
    d.job? ('Trabalho ativo: '+d.job) : 'Nenhum trabalho ativo.';
 }catch(e){document.getElementById('mode').textContent='sem conexao';}
}
tick(); setInterval(tick,1500);
</script></body></html>)HTML";

// ----------------------------------------------------------------------------
// Manipuladores
// ----------------------------------------------------------------------------

static void handle_index() {
    gStatus.clients++;
    gServer.send_P(200, "text/html; charset=utf-8", kIndexHtml);
}

static void handle_status_json() {
    MultimeterReading mr = multimeter_get_last_reading();
    SystemHealth      h  = diag_get_health();

    char value[24] = "--";
    const char* unit = "";
    const char* mode = "em espera";

    if (currentAppState == STATE_MULTIMETER && mr.valid) {
        multimeter_format_value(mr.value, value, sizeof(value));
        unit = mr.unitAbbrev ? mr.unitAbbrev : "";
        mode = multimeter_mode_name(mr.mode);
    } else {
        AnalysisResult r = measurements_get_last_result();
        if (r.valid) {
            strncpy(value, r.valueText, sizeof(value) - 1);
            value[sizeof(value) - 1] = '\0';
            mode = r.label;
        }
    }

    char when[24];
    net_format_datetime(when, sizeof(when));

    char uptime[24];
    uint32_t s = h.uptimeMs / 1000UL;
    snprintf(uptime, sizeof(uptime), "%luh %lumin",
             (unsigned long)(s / 3600UL), (unsigned long)((s % 3600UL) / 60UL));

    char body[512];
    snprintf(body, sizeof(body),
        "{\"fw\":\"%s\",\"value\":\"%s\",\"unit\":\"%s\",\"mode\":\"%s\","
        "\"time\":\"%s\",\"uptime\":\"%s\",\"heap\":%lu,\"sd\":\"%s\","
        "\"count\":%lu,\"rssi\":%d,\"job\":\"%s\"}",
        FW_VERSION, value, unit, mode, when, uptime,
        (unsigned long)(h.freeHeap / 1024),
        sdCardPresent ? "presente" : "ausente",
        (unsigned long)diag_get_usage().totalMeasurements,
        (int)gStatus.rssi,
        jobs_has_active() ? jobs_active_name() : "");

    gServer.sendHeader("Cache-Control", "no-store");
    gServer.send(200, "application/json", body);
}

// Envia um arquivo do cartao em blocos, sem carregar tudo na RAM.
static void stream_sd_file(const char* path, const char* downloadName) {
    if (!sdCardPresent) {
        gServer.send(503, "text/plain", "Cartao SD ausente");
        return;
    }

    LOCK_TFT();
    FsFile f = sd.open(path, FILE_READ);
    if (!f) {
        UNLOCK_TFT();
        gServer.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }

    char disposition[64];
    snprintf(disposition, sizeof(disposition), "attachment; filename=%s",
             downloadName);
    gServer.sendHeader("Content-Disposition", disposition);
    gServer.setContentLength(f.size());
    gServer.send(200, "text/csv; charset=utf-8", "");

    // Blocos de 512 bytes: cabe folgado na RAM e casa com o setor do cartao.
    uint8_t buffer[512];
    int n;
    while ((n = f.read()) >= 0) {
        size_t got = 0;
        buffer[got++] = (uint8_t)n;
        while (got < sizeof(buffer) && (n = f.read()) >= 0) {
            buffer[got++] = (uint8_t)n;
        }
        gServer.client().write(buffer, got);
        if (n < 0) break;
    }
    f.close();
    UNLOCK_TFT();
    gStatus.clients++;
}

static void handle_log_csv() {
    stream_sd_file(LOG_FILE_PATH, "sondvolt_historico.csv");
}

static void handle_job_csv() {
    const JobInfo* job = jobs_active();
    if (!job) {
        gServer.send(404, "text/plain", "Nenhum trabalho ativo");
        return;
    }
    char path[64];
    snprintf(path, sizeof(path), "%s/%s/MEDICOES.CSV", JOBS_ROOT_DIR,
             job->dirName);
    stream_sd_file(path, "trabalho.csv");
}

static void handle_not_found() {
    gServer.send(404, "text/plain", "Nao encontrado");
}

// ============================================================================
// CICLO DE VIDA
// ============================================================================

void net_init() {
    memset(&gStatus, 0, sizeof(gStatus));
    gStatus.state = NET_OFF;

    if (gPrefs.begin("net", true)) {
        gPrefs.getBytes("ssid", gSsid,     sizeof(gSsid) - 1);
        gPrefs.getBytes("pass", gPassword, sizeof(gPassword) - 1);
        gPrefs.end();
    }
    strncpy(gStatus.ssid, gSsid, sizeof(gStatus.ssid) - 1);
}

static void start_services() {
    if (!gServerStarted) {
        gServer.on("/",              handle_index);
        gServer.on("/api/status",    handle_status_json);
        gServer.on("/log.csv",       handle_log_csv);
        gServer.on("/trabalho.csv",  handle_job_csv);
        gServer.onNotFound(handle_not_found);
        gServer.begin();
        gServerStarted = true;
        gStatus.serverRunning = true;
        LOG_SERIAL_FMT("[NET] Servidor web em http://%s/\n", gStatus.ip);
    }
    net_ota_begin();
}

bool net_start() {
    gStatus.state = NET_CONNECTING;

    WiFi.persistent(false);          // nao gravar credenciais na flash do WiFi
    WiFi.setHostname(NET_HOSTNAME);

    // --- Tentativa como cliente ------------------------------------------
    if (gSsid[0] != '\0') {
        WiFi.mode(WIFI_STA);
        WiFi.begin(gSsid, gPassword);

        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED &&
               (millis() - start) < NET_CONNECT_TIMEOUT_MS) {
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (WiFi.status() == WL_CONNECTED) {
            gStatus.state = NET_CONNECTED;
            gStatus.rssi  = (int8_t)WiFi.RSSI();
            strncpy(gStatus.ssid, gSsid, sizeof(gStatus.ssid) - 1);
            snprintf(gStatus.ip, sizeof(gStatus.ip), "%s",
                     WiFi.localIP().toString().c_str());
            LOG_SERIAL_FMT("[NET] Conectado a %s, IP %s\n",
                           gStatus.ssid, gStatus.ip);

            net_sync_time();
            start_services();
            return true;
        }
        LOG_SERIAL_F("[NET] Falha ao conectar; subindo ponto de acesso");
    }

    // --- Fallback: ponto de acesso proprio --------------------------------
    // Sem isso, um usuario que errou a senha ficaria sem nenhum caminho para
    // corrigi-la, ja que a configuracao mora justamente na rede.
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(NET_AP_SSID, NET_AP_PASSWORD)) {
        gStatus.state = NET_AP_MODE;
        strncpy(gStatus.ssid, NET_AP_SSID, sizeof(gStatus.ssid) - 1);
        snprintf(gStatus.ip, sizeof(gStatus.ip), "%s",
                 WiFi.softAPIP().toString().c_str());
        LOG_SERIAL_FMT("[NET] Ponto de acesso '%s' em %s\n",
                       NET_AP_SSID, gStatus.ip);
        start_services();
        return true;
    }

    gStatus.state = NET_FAILED;
    LOG_SERIAL_F("[NET] Nao foi possivel iniciar o WiFi");
    return false;
}

void net_stop() {
    if (gServerStarted) { gServer.stop(); gServerStarted = false; }
    if (gOtaStarted)    { ArduinoOTA.end(); gOtaStarted = false; }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    gStatus.state         = NET_OFF;
    gStatus.serverRunning = false;
    gStatus.otaReady      = false;
    gStatus.ip[0]         = '\0';
    LOG_SERIAL_F("[NET] WiFi desligado");
}

void net_update() {
    if (gStatus.state != NET_CONNECTED && gStatus.state != NET_AP_MODE) return;

    if (gServerStarted) gServer.handleClient();
    if (gOtaStarted)    ArduinoOTA.handle();

    // Atualiza o RSSI de vez em quando; consultar a cada ciclo e desperdicio.
    static uint32_t lastRssi = 0;
    if (gStatus.state == NET_CONNECTED && (millis() - lastRssi) > 5000) {
        gStatus.rssi = (int8_t)WiFi.RSSI();
        lastRssi = millis();

        // Reconecta sozinho se a rede cair.
        if (WiFi.status() != WL_CONNECTED) {
            LOG_SERIAL_F("[NET] Conexao perdida, reconectando...");
            WiFi.reconnect();
        }
    }
    gStatus.uptimeMs = millis();
}

NetStatus net_get_status() {
    gStatus.timeValid = gTimeValid;
    gStatus.otaReady  = gOtaStarted;
    return gStatus;
}

bool net_is_connected() {
    return gStatus.state == NET_CONNECTED || gStatus.state == NET_AP_MODE;
}

// ============================================================================
// CREDENCIAIS
// ============================================================================

bool net_set_credentials(const char* ssid, const char* password) {
    if (!ssid || !ssid[0]) return false;

    strncpy(gSsid,     ssid,                  sizeof(gSsid) - 1);
    strncpy(gPassword, password ? password : "", sizeof(gPassword) - 1);
    gSsid[sizeof(gSsid) - 1] = '\0';
    gPassword[sizeof(gPassword) - 1] = '\0';

    if (!gPrefs.begin("net", false)) return false;
    gPrefs.putBytes("ssid", gSsid,     strlen(gSsid) + 1);
    gPrefs.putBytes("pass", gPassword, strlen(gPassword) + 1);
    gPrefs.end();
    return true;
}

const char* net_get_ssid() { return gSsid; }

void net_clear_credentials() {
    gSsid[0] = gPassword[0] = '\0';
    if (gPrefs.begin("net", false)) {
        gPrefs.clear();
        gPrefs.end();
    }
}

// ============================================================================
// RELOGIO
// ============================================================================

bool net_sync_time() {
    if (gStatus.state != NET_CONNECTED) return false;

    configTime(NET_TZ_OFFSET_SEC, 0, NET_NTP_SERVER);

    // Espera ate 6 s pela primeira resposta. O epoch so passa de 2020 quando
    // o NTP realmente respondeu; antes disso o relogio marca 1970.
    uint32_t start = millis();
    while ((millis() - start) < 6000) {
        time_t now = time(nullptr);
        if (now > 1600000000L) {          // depois de setembro de 2020
            gTimeValid = true;
            char buf[32];
            net_format_datetime(buf, sizeof(buf));
            LOG_SERIAL_FMT("[NET] Hora sincronizada: %s\n", buf);
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    LOG_SERIAL_F("[NET] NTP nao respondeu");
    return false;
}

bool net_time_valid() { return gTimeValid || gManualEpoch > 0; }

uint32_t net_epoch_now() {
    if (gTimeValid) {
        time_t now = time(nullptr);
        if (now > 1600000000L) return (uint32_t)now;
    }
    if (gManualEpoch > 0) {
        // Avanca a base manual pelo tempo decorrido desde que foi definida.
        return gManualEpoch + (millis() - gManualMillis) / 1000UL;
    }
    return 0;   // sem relogio confiavel: quem chamou decide o que fazer
}

void net_format_datetime(char* out, size_t outLen) {
    uint32_t epoch = net_epoch_now();
    if (epoch == 0) {
        snprintf(out, outLen, "sem relogio");
        return;
    }
    time_t t = (time_t)epoch;
    struct tm tmv;
    localtime_r(&t, &tmv);
    snprintf(out, outLen, "%02d/%02d %02d:%02d",
             tmv.tm_mday, tmv.tm_mon + 1, tmv.tm_hour, tmv.tm_min);
}

void net_format_time(char* out, size_t outLen) {
    uint32_t epoch = net_epoch_now();
    if (epoch == 0) { snprintf(out, outLen, "--:--:--"); return; }

    time_t t = (time_t)epoch;
    struct tm tmv;
    localtime_r(&t, &tmv);
    snprintf(out, outLen, "%02d:%02d:%02d",
             tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
}

void net_set_time_manual(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second) {
    if (year < 2020 || month < 1 || month > 12 || day < 1 || day > 31) return;

    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
    tmv.tm_year = year - 1900;
    tmv.tm_mon  = month - 1;
    tmv.tm_mday = day;
    tmv.tm_hour = hour;
    tmv.tm_min  = minute;
    tmv.tm_sec  = second;

    time_t epoch = mktime(&tmv);
    if (epoch <= 0) return;

    gManualEpoch  = (uint32_t)epoch;
    gManualMillis = millis();
    LOG_SERIAL_F("[NET] Hora definida manualmente");
}

// ============================================================================
// OTA
// ============================================================================

bool net_ota_begin(const char* password) {
    if (gOtaStarted) return true;
    if (!net_is_connected()) return false;

    ArduinoOTA.setHostname(NET_HOSTNAME);
    if (password && password[0]) ArduinoOTA.setPassword(password);

    ArduinoOTA.onStart([]() {
        gOtaRunning  = true;
        gOtaProgress = 0;
        // Uma gravacao interrompida deixa o aparelho sem firmware. Fechar o
        // cartao antes evita corromper tambem o sistema de arquivos.
        logger_close();
        LOG_SERIAL_F("[OTA] Atualizacao iniciada");
    });

    ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
        gOtaProgress = (total > 0) ? (uint8_t)((done * 100UL) / total) : 0;
    });

    ArduinoOTA.onEnd([]() {
        gOtaRunning  = false;
        gOtaProgress = 100;
        LOG_SERIAL_F("[OTA] Concluida, reiniciando");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        gOtaRunning = false;
        LOG_SERIAL_FMT("[OTA] Erro %u\n", (unsigned)error);
    });

    ArduinoOTA.begin();
    gOtaStarted      = true;
    gStatus.otaReady = true;
    LOG_SERIAL_F("[OTA] Pronto para receber atualizacao");
    return true;
}

bool    net_ota_in_progress() { return gOtaRunning; }
uint8_t net_ota_progress()    { return gOtaProgress; }
