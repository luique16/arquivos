/**
 * @file arvoreb.c
 * @brief Implementação do TAD da árvore B definida em arvoreb.h
 */

#include "../include/definicoes.h"
#include "../include/arvoreb.h"

#include <stdio.h>

ArvoreB * criarArvB() {
    ArvoreB *arvb = malloc(sizeof(ArvoreB));    // Aloca o espaço para a struct da árvore B
    if (arvb == NULL) return arvb;

    // Preencher o cabeçalho como vazio
    arvb->cabecalho.status = STATUS_INCONSISTENTE;
    arvb->cabecalho.noRaiz = -1;
    arvb->cabecalho.topo = -1;
    arvb->cabecalho.proxRRN = 0;
    arvb->cabecalho.nroNos = 0;

    // Criar o vetor de registros
}