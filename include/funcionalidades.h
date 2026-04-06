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
 * @brief [4] DELETE — Busca registros por um ou mais campos e deleta-os.
 * 
 * Utiliza a busca implementada na funcionalidade 3 para percorrer o arquivo e encontrar os registros. 
 * Ao encontrar um registro que satisfaça a busca, marca-o como logicamente removido e atualiza os valores da pilha.
 */
void funcionalidade4();

/**
 * @brief [5] INSERT INTO — Inserção com reaproveitamento via pilha.
 *
 * Se a pilha de removidos não estiver vazia, reutiliza o espaço do topo.
 * Caso contrário, insere ao final do arquivo. Executa n inserções.
 */
void funcionalidade5();

/**
 * @brief [6] UPDATE — Atualização de valores de campos nos registros.
 * 
 * Faz uma busca por registros que correspondam aos campos de valores fornecidos.
 * Recebe novos valores para campos especificados pelo usuário.
 * Atualiza os campos em questão dos registros encontrados pela busca.
 */
void funcionalidade6();
#endif
