/**
 * @file io.h
 * @brief Funções de leitura e escrita para o arquivo binário.
 */

#ifndef IO_H
#define IO_H

#include "definicoes.h"

/**
 * @brief Calcula o byte offset de um registro a partir do seu RRN.
 * @param rrn Número relativo do registro (base 0).
 * @return Byte offset no arquivo.
 */
long rrnParaOffset(int rrn);

/**
 * @brief Abre um arquivo binário.
 *
 * @param nomeArquivo Caminho do arquivo.
 * @param modo Modo de abertura ("rb", "wb", "wb+", "r+b").
 * @return Ponteiro para o arquivo, ou NULL em caso de erro.
 */
FILE *abrirArquivoBin(const char *nomeArquivo, const char *modo);

/**
 * @brief Fecha o arquivo gravando STATUS_CONSISTENTE no byte 0.
 * @param bin Ponteiro para o arquivo aberto.
 */
void fecharArquivoBin(FILE *bin);

/**
 * @brief Grava STATUS_INCONSISTENTE no byte 0 do arquivo.
 * @param bin Ponteiro para o arquivo aberto.
 */
void marcarInconsistente(FILE *bin);

/**
 * @brief Escreve o cabeçalho no início do arquivo (campo a campo).
 * @param bin Ponteiro para o arquivo binário.
 * @param cabecalho Ponteiro para a struct de cabeçalho.
 */
void escreverCabecalho(FILE *bin, CabecalhoArquivo *cabecalho);

/**
 * @brief Lê o cabeçalho do início do arquivo (campo a campo).
 * @param bin Ponteiro para o arquivo binário.
 * @param cabecalho Ponteiro para a struct que receberá os dados.
 */
void lerCabecalho(FILE *bin, CabecalhoArquivo *cabecalho);

/**
 * @brief Escreve um registro no arquivo na posição do RRN informado.
 *
 * Grava campo a campo e preenche bytes restantes com LIXO ('$').
 *
 * @param bin Ponteiro para o arquivo binário.
 * @param reg Ponteiro para o registro a ser gravado.
 * @param rrn RRN de destino.
 */
void escreverRegistro(FILE *bin, RegistroDados *reg, int rrn);

/**
 * @brief Lê um registro do arquivo na posição do RRN informado.
 *
 * Lê campo a campo e garante '\0' ao final das strings.
 *
 * @param bin Ponteiro para o arquivo binário.
 * @param reg Ponteiro para a struct que receberá os dados.
 * @param rrn RRN de origem.
 */
void lerRegistro(FILE *bin, RegistroDados *reg, int rrn);

/**
 * @brief Inicializa um RegistroDados com valores padrão (tudo nulo).
 * @param reg Ponteiro para o registro a ser inicializado.
 */
void inicializarRegistro(RegistroDados *reg);

#endif
