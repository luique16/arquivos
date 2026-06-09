/**
 * @file arvoreb.c
 * @brief Implementação das funções das estruturas da árvore B definida em arvoreb.h
 */

#include "../include/definicoes.h"
#include "../include/arvoreb.h"

#include <stdio.h>

/* Função para ler o cabeçalho do arquivo da árvore B */
CabecalhoArvB * lerCabecalho(FILE *fp) {
    CabecalhoArvB *cabecalho = malloc(sizeof(CabecalhoArvB));    // Aloca o espaço para a struct do cabeçalho
    if (cabecalho == NULL) return cabecalho;

    // Ler os itens do cabeçalho
    fread(&cabecalho->status, sizeof(char), 1, fp);
    fread(&cabecalho->noRaiz, sizeof(int), 1, fp);
    fread(&cabecalho->topo, sizeof(int), 1, fp);
    fread(&cabecalho->proxRRN, sizeof(int), 1, fp);
    fread(&cabecalho->nroNos, sizeof(int), 1, fp);

    return cabecalho;
}

/* 
Função auxiliar de lerPagina, para ler uma única entrada, ou seja, um único par (Chave de busca, Byte offset) 
OBS: a função assume que o ponteiro do arquivo já está na posição correta quando ela é chamada!
*/
Entrada lerEntrada(FILE *fp) {
    // Seria melhor retornar um ponteiro? retornar a struct por valor copia os valores para o destino?
}

/* Função para ler uma página (nó) da árvore B */
Pagina * lerPagina(FILE *fp, int rrn) {
    // Alocar o espaço para a página
    Pagina *pagina = malloc(sizeof(Pagina));
    if (pagina == NULL) return pagina;

    // Colocar o ponteiro de arquivo na posição correta
    fseek(fp, (long) (TAMANHO_CABECALHO_ARVB + rrn * TAMANHO_PAGINA_DISCO), SEEK_SET);

    // Ler os campos da página
    fread(&pagina->removido, sizeof(char), 1, fp);
    fread(&pagina->proximo, sizeof(int), 1, fp);
    fread(&pagina->tipoNo, sizeof(int), 1, fp);
    fread(&pagina->nroChaves, sizeof(int), 1, fp);

    // Ler para o vetor de pares (Chave de busca, Byte offset)
    for (int i = 0; i < ORDEM_ARVB - 1; i++) {
        pagina->pares[i] = lerEntrada(fp); // NÃO FINALIZADO
    }

    // Ler para o vetor de descendentes
    for (int i = 0; i < ORDEM_ARVB; i++) {
        fread(&(pagina->descendentes[i]), sizeof(int), 1, fp); // Ler um descendente Pn (int)
    }

    return pagina;
}