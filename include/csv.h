/**
 * @file csv.h
 * @brief Funções de leitura do arquivo CSV de entrada.
 */

#ifndef CSV_H
#define CSV_H

#include "definicoes.h"

/**
 * @brief Lê uma linha do CSV e preenche um RegistroDados.
 *
 * O separador de campos é vírgula (,). A primeira linha do arquivo
 * contém os nomes das colunas e deve ser ignorada. Campos nulos são
 * representados por espaço em branco no CSV.
 *
 * @param csv Ponteiro para o arquivo CSV aberto para leitura.
 * @param reg Ponteiro para a struct que receberá os dados lidos.
 * @return 1 se um registro foi lido com sucesso, 0 em caso de EOF ou erro.
 */
int lerRegistroCSV(FILE *csv, RegistroDados *reg);

#endif
