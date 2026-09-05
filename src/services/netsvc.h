// ============================================================================
// Sondvolt v5.0 - Servicos de Rede
// ============================================================================
// Arquivo : netsvc.h
//
// O ESP32 da CYD tem WiFi de fabrica e ate a v4.0 ele nunca foi ligado.
// Este modulo aproveita isso para tres coisas que mudam o uso do aparelho:
//
//   1. RELOGIO DE VERDADE (NTP). Ate agora o log gravava millis() desde o
//      boot, o que e inutil num historico de conserto. Com a hora certa, o
//      relatorio entregue ao cliente tem data.
//   2. PAGINA WEB. Leitura ao vivo, historico e download do CSV pelo
//      navegador do celular, sem tirar o cartao do aparelho.
//   3. OTA. Atualizar o firmware sem cabo USB.
//
// Custo: cerca de 180 KB de flash. A placa tem 2,6 MB livres.
//
// Politica de conexao
// -------------------
// O WiFi so liga se o usuario habilitar nos ajustes. Se as credenciais
// falharem, o aparelho sobe um ponto de acesso proprio (SSID "Sondvolt")
// para voce configurar pelo celular. Nunca fica tentando conectar em
// segundo plano gastando energia e tempo de CPU.
//
// Bluetooth
// ---------
// Nao esta incluido de proposito. O stack BT classico consome ~300 KB de
// flash e disputa o mesmo radio do WiFi. Entre os dois, o WiFi entrega mais
// (pagina web + OTA + NTP contra apenas um canal serial). Ficaria caro para
// pouco retorno.
// ============================================================================

#ifndef NETSVC_H
#define NETSVC_H

#include <Arduino.h>

// ----------------------------------------------------------------------------
// Situacao da conexao
// ----------------------------------------------------------------------------
enum NetState {
    NET_OFF = 0,         // desligado por opcao do usuario
    NET_CONNECTING,      // tentando entrar na rede configurada
    NET_CONNECTED,       // conectado como cliente
    NET_AP_MODE,         // ponto de acesso proprio (fallback)
    NET_FAILED           // tentou e nao conseguiu
};

struct NetStatus {
    NetState state;
    char     ssid[33];
    char     ip[16];
    int8_t   rssi;            // dBm, negativo
    bool     timeValid;       // hora sincronizada por NTP
    bool     serverRunning;
    bool     otaReady;
    uint32_t clients;         // conexoes atendidas desde o boot
    uint32_t uptimeMs;
};

#define NET_AP_SSID       "Sondvolt"
#define NET_AP_PASSWORD   "sondvolt123"   // minimo de 8 caracteres para WPA2
#define NET_HOSTNAME      "sondvolt"
#define NET_CONNECT_TIMEOUT_MS 12000
#define NET_NTP_SERVER    "pool.ntp.org"
#define NET_TZ_OFFSET_SEC (-3 * 3600)     // horario de Brasilia
#define NET_HTTP_PORT     80

// ============================================================================
// CICLO DE VIDA
// ============================================================================

// Prepara o modulo sem ligar o radio. Chamar no setup().
void net_init();

// Liga o WiFi e tenta conectar com as credenciais gravadas. Se falhar, sobe
// o ponto de acesso proprio. Bloqueia por ate NET_CONNECT_TIMEOUT_MS.
bool net_start();

// Desliga o radio por completo e libera a memoria do stack.
void net_stop();

// Atende requisicoes web e OTA. Chamar periodicamente quando a rede estiver
// ligada; e barato quando nao ha nada pendente.
void net_update();

// Situacao atual.
NetStatus net_get_status();
bool      net_is_connected();

// ============================================================================
// CREDENCIAIS
// ============================================================================

// Grava SSID e senha na NVS (namespace proprio, separado das configuracoes).
bool net_set_credentials(const char* ssid, const char* password);

// Le o SSID gravado. Devolve "" se nao houver nenhum.
const char* net_get_ssid();

// Apaga as credenciais.
void net_clear_credentials();

// ============================================================================
// RELOGIO
// ============================================================================

// Sincroniza com o servidor NTP. Exige conexao ativa.
bool net_sync_time();

// Verdadeiro se a hora ja foi sincronizada nesta sessao.
bool net_time_valid();

// Segundos desde 1970. Devolve 0 se a hora nao for confiavel - nunca
// inventa um valor, porque um carimbo de tempo errado num laudo e pior que
// carimbo nenhum.
uint32_t net_epoch_now();

// Formata a data e hora atuais como "DD/MM AA HH:MM".
// Se nao houver hora valida, escreve "sem relogio".
void net_format_datetime(char* out, size_t outLen);

// Formata apenas a hora, "HH:MM:SS".
void net_format_time(char* out, size_t outLen);

// Define a hora manualmente, para quem nao usa WiFi. Os valores seguem o
// calendario normal (ano com quatro digitos, mes de 1 a 12).
void net_set_time_manual(uint16_t year, uint8_t month, uint8_t day,
                         uint8_t hour, uint8_t minute, uint8_t second);

// ============================================================================
// OTA
// ============================================================================

// Habilita a atualizacao pela rede. A senha protege contra gravacao acidental
// por outra pessoa na mesma rede.
bool net_ota_begin(const char* password = "sondvolt");

// Verdadeiro enquanto uma atualizacao esta em andamento (a interface deve
// mostrar o progresso e nao aceitar toques).
bool net_ota_in_progress();

// Progresso de 0 a 100.
uint8_t net_ota_progress();

#endif // NETSVC_H
