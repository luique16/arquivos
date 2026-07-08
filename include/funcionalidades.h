/**
 * @file funcionalidades.h
 * @brief Declarações das funcionalidades do programa.
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

/**
 * @brief [7] CREATE INDEX — Cria índice árvore-B para arquivo de dados.
 *
 * Lê o arquivo de dados e insere cada registro ativo no índice,
 * usando codEstacao como chave de busca.
 */
void funcionalidade7();

/**
 * @brief [8] SELECT WITH INDEX — Busca registros usando índice.
 *
 * Se codEstacao for critério, usa índice para busca; caso contrário,
 * faz busca sequencial. Exibe registros encontrados.
 */
void funcionalidade8();

/**
 * @brief [9] INSERT WITH INDEX — Insere registros e atualiza índice.
 *
 * Insere novos registros no arquivo de dados com reaproveitamento
 * e insere as chaves no índice árvore-B.
 */
void funcionalidade9();

/**
 * @brief [10] DELETE WITH INDEX — Remove registros e atualiza índice.
 *
 * Remove logicamente registros que satisfaçam critérios e remove
 * as chaves correspondentes do índice árvore-B.
 */
void funcionalidade10();

/**
 * @brief [11] NESTED LOOP JOIN — Junção por loop aninhado.
 *
 * Percorre os dois arquivos de dados em loop aninhado e imprime
 * os pares que satisfazem a condição codProxEstacao = codEstacao.
 */
void funcionalidade11();

/**
 * @brief [12] SINGLE LOOP JOIN — Junção usando índice árvore-B.
 *
 * Percorre o arquivo externo e usa o indice arvore-B para localizar
 * o registro correspondente no arquivo interno pelo campo codEstacao.
 */
void funcionalidade12();

/**
 * @brief [13] ORDER BY — Ordena os registros ativos em um novo arquivo.
 *
 * Le todos os registros ativos para memoria, ordena por codEstacao
 * ou codProxEstacao e grava o resultado em arquivo binario de saida.
 */
void funcionalidade13();

/**
 * @brief [14] SORT MERGE JOIN - Junção por ordenação e intercalação dos registros.
 * 
 * Carrega os registros de ambos os arquivos na memória e os ordena.
 * Depois, faz a junção de A e B por intercalação, imprimindo os registros correspondentes.
 */
void funcionalidade14();

#endif
