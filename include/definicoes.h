/**
 * @file definicoes.h
 * @brief Definição de tipos, constantes e estruturas de dados do projeto.
 */

#ifndef DEFINICOES_H
#define DEFINICOES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constantes */
#define TAMANHO_CABECALHO    17
#define TAMANHO_REGISTRO     80
#define LIXO                 '$'
#define INTEIRO_NULO         -1
#define TAMANHO_MAX_NOME     50 /* Nomes garantidamente menores que 50 */

/* Status do arquivo */
#define STATUS_INCONSISTENTE '0'
#define STATUS_CONSISTENTE   '1'

/* Remoção de registros */
#define REGISTRO_REMOVIDO    '1'
#define REGISTRO_ATIVO       '0'

/**
 * @brief Cabeçalho do arquivo binário (17 bytes fixos no disco).
 *
 * Layout no disco:
 *   byte 0     : status (char)
 *   bytes 1–4  : topo (int)
 *   bytes 5–8  : proxRRN (int)
 *   bytes 9–12 : nroEstacoes (int)
 *   bytes 13–16: nroParesEstacao (int)
 */
typedef struct {
    char status;         /* STATUS_CONSISTENTE ou STATUS_INCONSISTENTE */
    int topo;            /* RRN do topo da pilha de removidos (-1 se vazia) */
    int proxRRN;         /* Próximo RRN disponível (inicia em 0) */
    int nroEstacoes;     /* Quantidade de estações distintas no arquivo */
    int nroParesEstacao; /* Quantidade de pares (codEstacao, codProxEstacao) */
} CabecalhoArquivo;

/**
 * @brief Registro de dados (80 bytes fixos no disco).
 *
 * Formato:
 *   byte 0       : removido (char)
 *   bytes 1–4    : proximo (int)
 *   bytes 5–8    : codEstacao (int)
 *   bytes 9–12   : codLinha (int)
 *   bytes 13–16  : codProxEstacao (int)
 *   bytes 17–20  : distProxEstacao (int)
 *   bytes 21–24  : codLinhaIntegra (int)
 *   bytes 25–28  : codEstIntegra (int)
 *   bytes 29–32  : tamNomeEstacao (int)
 *   bytes 33–... : nomeEstacao (sem '\0')
 *   próx. 4 bytes: tamNomeLinha (int)
 *   próx. n bytes: nomeLinha (sem '\0')
 *   restante     : LIXO ('$')
 */
typedef struct {
    char removido;                      /* REGISTRO_REMOVIDO ou REGISTRO_ATIVO */
    int  proximo;                       /* RRN do próximo removido na pilha (-1 se último) */
    int  codEstacao;                    /* Código da estação (-1 se nulo) */
    int  codLinha;                      /* Código da linha */
    int  codProxEstacao;                /* Código da próxima estação (-1 se nulo) */
    int  distProxEstacao;               /* Distância para a próxima estação */
    int  codLinhaIntegra;               /* Código da linha de integração (-1 se nulo) */
    int  codEstIntegra;                 /* Código da estação de integração (-1 se nulo) */
    int  tamNomeEstacao;                /* Tamanho em bytes de nomeEstacao */
    char nomeEstacao[TAMANHO_MAX_NOME]; /* Nome da estação (sem '\0') */
    int  tamNomeLinha;                  /* Tamanho em bytes de nomeLinha */
    char nomeLinha[TAMANHO_MAX_NOME];   /* Nome da linha (sem '\0') */
} RegistroDados;

#endif
