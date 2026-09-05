// ============================================================================
// Sondvolt v5.0 - Sistema de Trabalhos
// ============================================================================
// Arquivo : jobs.h
//
// O que resolve
// -------------
// Ate agora todas as medicoes caiam num unico arquivo corrido. Numa bancada
// real voce atende varios aparelhos por dia, e depois precisa saber qual
// componente pertencia a qual conserto.
//
// Um "trabalho" e uma pasta no cartao com o nome do cliente ou do aparelho.
// Enquanto ele estiver ativo, toda medicao vai para o log dele. No fim,
// gera um relatorio em texto para entregar junto com o aparelho.
//
// Estrutura no cartao
// -------------------
//   /TRABALHOS/
//       /LIQUID_JOAO/
//           JOB.INF        cabecalho: nome, cliente, aparelho, datas
//           MEDICOES.CSV   uma linha por medicao
//           NOTAS.TXT      observacoes livres do tecnico
//           RELATOR.TXT    relatorio gerado ao finalizar
//       /TV_SAMSUNG_32/
//           ...
//
// Os nomes de pasta sao limitados a 8 caracteres maiusculos por causa do
// FAT de nomes curtos, que e o que o SdFat usa por padrao. O nome completo
// digitado pelo usuario fica guardado dentro do JOB.INF.
// ============================================================================

#ifndef JOBS_H
#define JOBS_H

#include <Arduino.h>
#include "types.h"
#include "analysis.h"

#define JOBS_ROOT_DIR      "/TRABALHOS"
#define JOB_MAX_NAME       28      // nome completo, guardado no JOB.INF
#define JOB_MAX_DIR        8       // nome da pasta no cartao (FAT 8.3)
#define JOB_MAX_LISTED     32      // maximo de trabalhos listados na tela

// ----------------------------------------------------------------------------
// Situacao de um trabalho
// ----------------------------------------------------------------------------
enum JobStatus {
    JOB_STATUS_OPEN = 0,     // em andamento
    JOB_STATUS_DONE,         // finalizado, relatorio gerado
    JOB_STATUS_CANCELLED     // abandonado
};

// ----------------------------------------------------------------------------
// Cabecalho de um trabalho (o conteudo do JOB.INF)
// ----------------------------------------------------------------------------
struct JobInfo {
    char      dirName[JOB_MAX_DIR + 1];   // pasta no cartao
    char      name[JOB_MAX_NAME + 1];     // nome completo digitado
    char      customer[JOB_MAX_NAME + 1]; // cliente
    char      device[JOB_MAX_NAME + 1];   // aparelho
    uint32_t  createdEpoch;               // 0 se nao havia relogio
    uint32_t  createdMillis;
    uint32_t  measurementCount;
    uint32_t  goodCount;
    uint32_t  badCount;
    JobStatus status;
    bool      valid;
};

// ============================================================================
// CICLO DE VIDA
// ============================================================================

// Cria a pasta raiz se preciso e recarrega o trabalho que estava ativo antes
// do ultimo desligamento (o nome fica guardado na NVS).
void jobs_init();

// Verdadeiro se o sistema pode operar (exige cartao SD montado).
bool jobs_available();

// ============================================================================
// TRABALHO ATIVO
// ============================================================================

// Cria um trabalho novo e o torna ativo. O nome da pasta e derivado do nome
// digitado; se ja existir, um sufixo numerico e acrescentado.
// Devolve false se nao houver cartao ou se o nome for invalido.
bool jobs_create(const char* name, const char* customer, const char* device);

// Abre um trabalho existente pela pasta.
bool jobs_open(const char* dirName);

// Fecha o trabalho ativo sem finalizar (as medicoes ficam gravadas).
void jobs_close();

// Finaliza: marca como concluido e gera o relatorio.
bool jobs_finish();

// Trabalho ativo, ou nullptr se nao houver nenhum.
const JobInfo* jobs_active();

// Verdadeiro se existe trabalho ativo.
bool jobs_has_active();

// Nome curto do trabalho ativo, para a barra de status. "" se nao houver.
const char* jobs_active_name();

// ============================================================================
// REGISTRO DE MEDICOES
// ============================================================================

// Grava uma medicao no log do trabalho ativo. Se nao houver trabalho ativo,
// devolve false e quem chamou deve gravar no log geral.
bool jobs_record(const AnalysisResult& result);

// Versao manual, para grandezas que nao vem do motor de analise
// (tensao do multimetro, temperatura, ripple).
bool jobs_record_value(const char* label, float value, const char* unit,
                       ComponentStatus status);

// Acrescenta uma nota livre ao NOTAS.TXT do trabalho ativo.
bool jobs_add_note(const char* text);

// ============================================================================
// LISTAGEM
// ============================================================================

// Varre a pasta raiz e preenche a lista interna. Devolve quantos achou.
uint8_t jobs_scan();

// Quantidade encontrada na ultima varredura.
uint8_t jobs_count();

// Cabecalho de um trabalho listado, ou nullptr se o indice for invalido.
const JobInfo* jobs_at(uint8_t index);

// Nome do trabalho listado, para desenhar a lista.
const char* jobs_name_at(uint8_t index);

// Apaga um trabalho e tudo que esta dentro dele. Nao ha desfazer.
bool jobs_delete(const char* dirName);

// ============================================================================
// RELATORIO
// ============================================================================

// Gera o RELATOR.TXT do trabalho indicado (ou do ativo, se dirName for nulo).
bool jobs_generate_report(const char* dirName);

// Caminho do relatorio gerado, para exibir ao usuario.
const char* jobs_report_path();

// Texto legivel da situacao.
const char* jobs_status_text(JobStatus status);

#endif // JOBS_H
