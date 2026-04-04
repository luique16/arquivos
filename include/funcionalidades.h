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

/**
 * @brief [3] SELECT WHERE — Busca registros por um ou mais campos.
 *
 * Percorre o arquivo sequencialmente. Registros removidos são ignorados.
 * Executa n buscas consecutivas, cada uma com m pares (nomeCampo, valor).
 */
void funcionalidade3();

/**
 * @brief [4] DELETE — Busca registros por um ou mais campos e deleta-os
 * 
 * Utiliza a busca implementada na funcionalidade 3 para percorrer o arquivo e encontrar os registros. 
 * Ao encontrar um registro que satisfaça a busca, marca-o como logicamente removido e atualiza os valores da pilha.
 */
void funcionalidade4();
#endif
