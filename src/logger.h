// ============================================================================
// Sondvolt v4.0 - Registro de Medicoes no Cartao SD
// ============================================================================
// Arquivo : logger.h
// Formato : CSV separado por ponto e virgula, com cabecalho de colunas.
//           tempo_ms;hh:mm:ss;componente;valor;unidade;status
// ============================================================================

#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include "types.h"

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
};

// Monta o cartao, cria o cabecalho e rotaciona o arquivo se preciso.
// Tenta 10 MHz e cai para 4 MHz automaticamente em cartoes lentos.
bool logger_init();
void logger_close();
bool logger_is_ready();

// Grava uma linha de medicao. Respeita deviceSettings.autoSaveHistory.
bool logger_write(const char* component, float value, const char* unit,
                  const char* status);

// Mesma coisa, convertendo o enum de status em texto.
bool logger_write_result(const char* component, float value, const char* unit,
                         ComponentStatus status);

// Le as ultimas maxEntries medicoes para a tela de historico.
// Devolve quantas foram efetivamente carregadas.
uint8_t logger_get_recent(HistoryItem* buffer, uint8_t maxEntries);

// Total de linhas de medicao no arquivo.
uint32_t logger_count_entries();

// Apaga o historico e recria o cabecalho.
void logger_clear();

// Exporta um relatorio legivel para /RELATOR/<jobName>.TXT.
bool logger_export_report(const char* jobName);

// Log de diagnostico na serial.
void logger_log(LogLevel level, const char* message);

#endif // LOGGER_H
