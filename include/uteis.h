/**
 * @file uteis.h
 * @brief Declarações de funções e estruturas auxiliares.
 */

#ifndef UTEIS_H
#define UTEIS_H

#include "definicoes.h"
#include "arvoreb.h"
#include <stdbool.h>

/**
 * @brief Imprime os dados de um registro.
 *
 * Imprime registros com campos possívelmente nulos na saída padrão.
 */
void imprimirRegistro(RegistroDados *reg);

/**
 * @brief Imprime todos os registros ativos do arquivo
 *
 * Lê e imprime todos registros não-removidos do arquivo binário, uma linha por registro. Mensagem de erro se nenhum encontrado.
 */
void imprimirTodosRegistros(FILE *bin, CabecalhoArquivo *cabecalho);

/**
 * @brief Estrutura para armazenar validações (where).
 *
 * Estrutura para armazenar validações de campos.
 */
typedef struct {
    char* campo;    /* Nome do campo */
    char* valor;    /* Valor do campo na forma de string */
} Validacao;

/**
 * @brief Converte string (nulo ou não) para inteiro.
 *
 * Converte string nulo para inteiro.
 * Retorna INTEIRO_NULO se string for nulo.
 */
int stringNulavelParaInteiro(char* string);

/**
 * @brief Valida os campos de um registro.
 *
 *  Valida os campos de um registro com base em uma lista de validações.
 *  Retorna verdadeiro se todos os campos forem válidos.
 */
bool validarCampos(RegistroDados* reg, Validacao* validacoes, int qtdValidacoes);

/**
 * @brief Realiza a busca por registros no arquivo binário
 * 
 * Executa n buscas consecutivas, cada uma recebendo um ou mais campos e seus respectivos valores como entrada.
 * Ao encontrar um registro compatível, executa uma função, passada como ponteiro em uma espécie de callback.
 */
void busca(FILE *bin, CabecalhoArquivo *cabecalho, int n, void (*acao)(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn, Validacao *atualizacoes, int qtdCampos));

/**
 * @brief Faz a remoção lógica de um registro
 * 
 * Dado um ponteiro para um registro, marca ele como logicamente removido e atualiza o campo proximo para o próximo valor da pilha de remoção.
 * Além disso, grava essa alteração no arquivo binário.
 */
void removerLogic(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn, Validacao *, int);

/**
 * @brief Wrapper da função imprimirRegistro
 * 
 * Wrapper para que a função imprimirRegistro possa ser usada como callback dentro de busca sem ficar poluída com parâmetros desnecessários
 */
void acaoImprimir(FILE *, CabecalhoArquivo *, RegistroDados *reg, int, Validacao *, int);

/**
 * @brief Atualizar o número de estações e pares de estações no cabeçalho
 * 
 * Percorre o arquivo e conta o número de estações e pares de estações sem repetição.
 * Atualiza nroEstacoes e nroParesEstacao no cabeçalho.
 */
void atualizarCabecalho(FILE *bin, CabecalhoArquivo *cabecalho);

/**
 * @brief Atualizar o registro com campos e valores informados pelo usuário
 * 
 * Pede o input do número p e, em seguida, pede o input de p pares de campos e valores.
 * Atualiza cada um dos campos do registro em questão com os novos valores
 */
void atualizarRegistro(FILE *bin, CabecalhoArquivo *, RegistroDados *reg, int rrn, Validacao *atualizacoes, int qtdCampos);

void lerRegistroStdin(RegistroDados *reg);

/**
 * @brief Imprime uma linha do resultado da junção.
 *
 * Formato: codEstacao_A nomeEstacao_A nomeLinha_A codProxEstacao_A nomeEstacao_B
 */
void imprimirResultadoJuncao(RegistroDados *regA, RegistroDados *regB);

/**
 * @brief Funções de comparação para serem usadas na ordenação
 */
int compararPorCodEstacao(const void *a, const void *b);
int compararPorCodProxEstacao(const void *a, const void *b);

#endif
