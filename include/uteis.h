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

#endif
