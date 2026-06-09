/**
 * @file arvoreb.h
 * @brief Definição constantes, estruturas de dados e funções relacionados à árvore B.
 */

#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>

/* Constantes */
#define TAMANHO_CABECALHO_ARVB  17
#define ORDEM_ARVB              4
#define TAMANHO_PAGINA_DISCO    53
#define NO_FOLHA                -1
#define NO_RAIZ                 0
#define NO_INTERMEDIARIO        1

/**
 * @brief Cabeçalho do arquivo da árvore B (17 bytes fixos no disco).
 *
 * Layout no disco:
 *   byte 0     : status (char)
 *   bytes 1–4  : noRaiz (int)
 *   bytes 5–8  : topo (int)
 *   bytes 9–12 : proxRRN (int)
 *   bytes 13–16: nroNos (int)
 */
typedef struct {
    char status;    /* STATUS_CONSISTENTE ou STATUS_INCONSISTENTE */
    int noRaiz;     /* RRN do nó raiz da árvore B (-1 se ela estiver vazia) */
    int topo;       /* RRN do topo da pilha de logicamente removidos (-1 se vazia) */
    int proxRRN;    /* Próximo RRN disponível para criação de nó (inicia em 0) */
    int nroNos;     /* Quantidade de nós na árvore B */
} CabecalhoArvB;


/**
 * Estrutura auxiliar que armazena uma chave de busca e um campo de referência para o registro no arquivo de dados que corresponde a essa chave.
 * Nesse caso, o campo de referência guarda o byte offset do registro no arquivo de dados.
 */
typedef struct {
    int chave;      /* Chave de busca (-1 se estiver vazio)*/
    int offset;     /* Campo de referência para o registro no arquivo de dados que contém a chave de busca (-1 se estiver vazio) */
} Entrada;

/**
 * @brief Registro(Nó/Página) de dados.
 * No caso de ordem = 4, temos 53 bytes fixos no disco.
 *
 * Formato para ordem 4:
 *   byte 0       : removido (char)
 *   bytes 1–4    : proximo (int)
 *   bytes 5–8    : tipoNo (int)
 *   bytes 9–12   : nroChaves (int)
 * 
 *   bytes 13–16  : C1 (int)    |
 *   bytes 17–20  : Pr1 (int)   |
 *   bytes 21–24  : C2 (int)    |   na memória, serão agrupados em um vetor de Indices (pares Cn, Prn)
 *   bytes 25–28  : Pr2 (int)   |
 *   bytes 29–32  : C3 (int)    |
 *   bytes 33–36  : Pr3 (int)   |
 * 
 *   bytes 37–40  : P1 (int)    |
 *   bytes 41–44  : P2 (int)    |   na memória, serão agrupados em um vetor de descendentes (Pn)
 *   bytes 45–48  : P3 (int)    |
 *   bytes 49–52  : P4 (int)    |
 */
typedef struct {
    char removido;                      /* REGISTRO_REMOVIDO ou REGISTRO_ATIVO */
    int  proximo;                       /* RRN do próximo removido na pilha (-1 se último) */
    int  tipoNo;                        /* Tipo do nó (NO_FOLHA, NO_RAIZ ou NO_INTERMEDIARIO) */
    int  nroChaves;                     /* Quantidade de chaves presentes no nó */
    Entrada pares[ORDEM_ARVB - 1];      /* Vetor com a capacidade máxima de pares (chave, referência), que é ORDEM_ARVB - 1 */
    int descendentes[ORDEM_ARVB];       /* Vetor com a capacidade máxima de referências para sub-árvores (valor = -1 se não tiver sub-árvore) */
} Pagina;

/* Funções relacionadas às estruturas da árvore B */
CabecalhoArvB * lerCabecalho(FILE *fp);
Pagina * lerPagina(FILE *fp, int rrn);
void gravarCabecalho(FILE *fp);
void gravarPagina(FILE *fp, int rrn);

#endif
