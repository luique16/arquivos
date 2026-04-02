/**
 * @file funcionalidades.c
 * @brief Implementações das seis funcionalidades do programa.
 */

#include "../include/funcionalidades.h"
#include "../include/io.h"
#include "../include/definicoes.h"
#include "../include/csv.h"
#include "../include/fornecidas.h"

#include <stdio.h>

void funcionalidade1() {
    char nomeCSV[256];
    char nomeArquivo[256];

    scanf("%s %s", nomeCSV, nomeArquivo);

    FILE *csv = fopen(nomeCSV, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *bin = abrirArquivoBin(nomeArquivo, "wb");
    if (bin == NULL) {
        fclose(csv);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Inicializa e escreve cabeçalho provisório (padrão) */
    CabecalhoArquivo cabecalho;
    cabecalho.status = STATUS_INCONSISTENTE;
    cabecalho.topo = INTEIRO_NULO;
    cabecalho.proxRRN = 0;
    cabecalho.nroEstacoes = 0;
    cabecalho.nroParesEstacao = 0;

    escreverCabecalho(bin, &cabecalho);


    /* Lê CSV e grava cada registro */
    RegistroDados reg;
    lerRegistroCSV(csv, &reg); /* Pula primeira linha do CSV (cabeçalho) */
    inicializarRegistro(&reg);
    while (lerRegistroCSV(csv, &reg)) {
        escreverRegistro(bin, &reg, cabecalho.proxRRN);

        cabecalho.proxRRN++;

        /* ISSO AQUI ESTÁ ERRADO (ler o documento) */
        cabecalho.nroEstacoes++;
        cabecalho.nroParesEstacao++;

        inicializarRegistro(&reg);
    }

    /* Atualiza cabeçalho final e fecha consistentemente */
    escreverCabecalho(bin, &cabecalho);
    fecharArquivoBin(bin);
    fclose(csv);

    BinarioNaTela(nomeArquivo); /* exibe binário após fechar */
}
