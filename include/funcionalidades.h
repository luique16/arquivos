/**
 * @file funcionalidades.h
 * @brief Declarações das seis funcionalidades do programa.
 */

#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include "definicoes.h"

/**
 * @brief [1] CREATE TABLE — Lê CSV e grava registros no arquivo binário.
 *
 * Lê o nome do arquivo CSV de entrada e o nome do arquivo binário de saída.
 * Chama binarioNaTela após fechar o arquivo.
 */
void funcionalidade1();

/**
 * @brief [2] SELECT * — Lista todos os registros ativos do arquivo.
 *
 * Percorre o arquivo sequencialmente. Registros removidos são ignorados.
 * Exibe "Registro inexistente." se o arquivo estiver vazio.
 * Exibe "Falha no processamento do arquivo." em caso de erro.
 */
void funcionalidade2();

#endif
