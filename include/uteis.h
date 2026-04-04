/**
 * @file uteis.h
 * @brief Declarações de funções e estruturas auxiliares.
 */

#ifndef UTEIS_H
#define UTEIS_H

#include "definicoes.h"

#include <stdbool.h>

/**
 * @brief Imprime os dados de um registro.
 *
 * Imprime registros com campos possívelmente nulos na saída padrão.
 */
void imprimirRegistro(RegistroDados *reg);

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
void busca(FILE *bin, CabecalhoArquivo *cabecalho, int n, void (*acao)(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn));

/**
 * @brief Faz a remoção lógica de um registro
 * 
 * Dado um ponteiro para um registro, marca ele como logicamente removido e atualiza o campo proximo para o próximo valor da pilha de remoção.
 * Além disso, grava essa alteração no arquivo binário.
 */
void removerLogic(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn);

/**
 * @brief Wrapper da função imprimirRegistro
 * 
 * Wrapper para que a função imprimirRegistro possa ser usada como callback dentro de busca sem ficar poluída com parâmetros desnecessários
 */
void acaoImprimir(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn);

/**
 * @brief Atualizar o número de estações no cabeçalho
 * 
 * Percorre o arquivo e conta o número de estações pelo codEstacao.
 * Atualiza nroEstacoes no cabeçalho.
 */
void contarEstacoes(FILE *bin, CabecalhoArquivo *cabecalho);
#endif
