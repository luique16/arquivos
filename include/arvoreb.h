/**
 * @file arvoreb.h
 * @brief Definição constantes, estruturas de dados e funções relacionados à árvore B.
 */

#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>
#include <stdbool.h>

/* Constantes */
#define TAMANHO_CABECALHO_ARVB  17
#define ORDEM_ARVB              4
#define TAMANHO_PAGINA_DISCO    53
#define NO_FOLHA                -1
#define NO_RAIZ                 0
#define NO_INTERMEDIARIO        1
#define MINIMO_CHAVES           ((ORDEM_ARVB / 2) - 1)
#define REM_NAO_ENCONTRADO      0
#define REM_OK                  1
#define REM_UNDERFLOW           2

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
 * @brief Armazena uma chave de busca e a referência para o registro no
 * arquivo de dados correspondente.
 *
 * O campo de referência guarda o byte offset do registro no arquivo de dados.
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

/**
 * @brief Resultado de split (para inserção).
 */
typedef struct {
    int houveSplit;   /* 1 se houve split, 0 caso contrário */
    int chaveSobe;    /* Chave promovida para o nó pai */
    int prSobe;       /* Byte offset no arquivo de dados correspondente */
    int rrnDireito;   /* RRN do novo nó criado à direita */
} ResultadoSplit;

/**
 * @brief struct para armazenar o par da página pai e o filho seguido (para ser usado na remoção como uma pilha que guarda o caminho seguido)
 */
typedef struct {
    int rrnPagina; /* O nó em que estava */
    int indiceFilho; /* O índice i de pagina.descendentes[i], o filho para o qual foi seguido */
} EntradaCaminho;


/* === Funções de inicialização === */

/**
 * @brief Cria e retorna um nó com todos os campos no estado padrão.
 * @return Pagina com campos zerados/inicializados.
 */
Pagina noVazio(void);

/**
 * @brief Cria o cabeçalho inicial para um arquivo de índice recém-criado.
 * @return CabecalhoArvB com status '0' e demais campos zerados.
 */
CabecalhoArvB cabecalhoArvoreVazio(void);

/**
 * @brief Obtém o RRN para um novo nó, reaproveitando da pilha se possível.
 * @param fp Ponteiro para o arquivo de índice.
 * @param cab Ponteiro para o cabeçalho (atualiza topo/proxRRN).
 * @return RRN do nó alocado.
 */
int alocarNo(FILE *fp, CabecalhoArvB *cab);

/* === Funções de busca === */

/**
 * @brief Busca uma chave na árvore-B.
 * @param fp     Ponteiro para o arquivo de índice.
 * @param cab    Cabeçalho da árvore-B.
 * @param chave  Chave a ser buscada.
 * @param offsetEncontrado Ponteiro para armazenar o offset encontrado.
 * @return 1 se encontrada, 0 caso contrário.
 */
int buscarNaArvore(FILE *fp, CabecalhoArvB cab, int chave, long *offsetEncontrado);

/**
 * @brief Encontra a sucessora imediata de uma chave não-folha.
 * @param fp          Ponteiro para o arquivo de índice.
 * @param rrnFilhoDir RRN do filho direito da chave.
 * @param chaveSuc    Ponteiro para armazenar a chave sucessora.
 * @param prSuc       Ponteiro para armazenar o PR da sucessora.
 * @param rrnFolhaSuc Ponteiro para armazenar o RRN da folha onde está a sucessora.
 */
void encontrarSucessora(FILE *fp, int rrnFilhoDir, int *chaveSuc, int *prSuc, int *rrnFolhaSuc);

/* === Funções de inserção === */

/**
 * @brief Insere uma chave em um nó que tem espaço disponível.
 * @param no           Ponteiro para o nó.
 * @param chave        Chave a ser inserida.
 * @param pr           Byte offset no arquivo de dados.
 * @param rrnFilhoDir  RRN do filho direito da chave.
 */
void inserirNaoPropagado(Pagina *no, int chave, int pr, int rrnFilhoDir);

/**
 * @brief Realiza o split de um nó cheio ao receber uma nova chave.
 * @param fp                     Ponteiro para o arquivo de índice.
 * @param cab                    Ponteiro para o cabeçalho.
 * @param rrnEsq                 RRN do nó que sofrerá split.
 * @param noEsq                  Nó que está cheio.
 * @param novaChave              Nova chave a ser inserida.
 * @param novoPR                 PR da nova chave.
 * @param rrnFilhoDirDaNovaChave RRN do filho direito da nova chave.
 * @return ResultadoSplit com dados da chave promovida.
 */
ResultadoSplit splitNo(FILE *fp, CabecalhoArvB *cab,
                       int rrnEsq, Pagina noEsq,
                       int novaChave, int novoPR, int rrnFilhoDirDaNovaChave);

/**
 * @brief Insere recursivamente uma chave na subárvore.
 * @param fp       Ponteiro para o arquivo de índice.
 * @param cab      Ponteiro para o cabeçalho.
 * @param rrnAtual RRN do nó atual na recursão.
 * @param chave    Chave a ser inserida.
 * @param pr       Byte offset no arquivo de dados.
 * @return ResultadoSplit indicando se houve promoção.
 */
ResultadoSplit inserirRecursivo(FILE *fp, CabecalhoArvB *cab,
                                int rrnAtual, int chave, int pr);

/**
 * @brief Ponto de entrada público para inserção na árvore-B.
 * @param fp    Ponteiro para o arquivo de índice.
 * @param cab   Ponteiro para o cabeçalho.
 * @param chave Chave a ser inserida.
 * @param pr    Byte offset no arquivo de dados.
 */
void inserirNaArvore(FILE *fp, CabecalhoArvB *cab, int chave, int pr);

/* === Funções de remoção === */

/**
 * @brief Retorna o índice do filho dentro do pai.
 * @param pai      Nó pai.
 * @param rrnFilho RRN do filho procurado.
 * @return Índice do filho, ou -1 se não encontrado.
 */
int encontrarPosicaoFilho(Pagina pai, int rrnFilho);

/**
 * @brief Remove a chave na posição pos de um nó folha.
 * @param no  Ponteiro para o nó folha.
 * @param pos Índice da chave a remover.
 */
void removerDaFolha(Pagina *no, int pos);

/**
 * @brief Checa se tem underflow
 */
bool checkUnderflow(Pagina *no);

/**
 * @brief Redistribui chaves do irmão direito para o filho em underflow.
 * @param fp          Ponteiro para o arquivo de índice.
 * @param pai         Ponteiro para o nó pai.
 * @param posFilhoEsq Índice do filho em underflow no pai.
 * @param filhoEsq    Ponteiro para o filho em underflow.
 * @param rrnFilhoEsq RRN do filho em underflow.
 * @param irmaoDir    Ponteiro para o irmão direito.
 * @param rrnIrmaoDir RRN do irmão direito.
 */
void redistribuir(FILE *fp, Pagina *pai, int indiceFilhoEsq);

/**
 * @brief Concatena filho em underflow com irmão direito.
 * @param fp          Ponteiro para o arquivo de índice.
 * @param cab         Ponteiro para o cabeçalho.
 * @param pai         Ponteiro para o nó pai.
 * @param posFilhoEsq Índice do filho em underflow no pai.
 * @param filhoEsq    Ponteiro para o filho em underflow.
 * @param rrnFilhoEsq RRN do filho em underflow.
 * @param irmaoDir    Ponteiro para o irmão direito.
 * @param rrnIrmaoDir RRN do irmão direito.
 */
void concatenarFilhos(FILE *fp, CabecalhoArvB *cab, Pagina *pai, int indiceFilhoEsq);

/**
 * @brief Remove uma chave da árvore recursivamente, tratando underflow.
 * @param fp       Ponteiro para o arquivo de índice.
 * @param cab      Ponteiro para o cabeçalho.
 * @param rrnAtual RRN do nó atual na recursão.
 * @param chave    Chave a ser removida.
 * @return 1 se removida com sucesso, 0 se não encontrada.
 */
int removerDaArvore(FILE *fp, CabecalhoArvB *cab, int chave);

#endif
