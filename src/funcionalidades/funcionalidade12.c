#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

void funcionalidade12() {
    /* Le os dois arquivos de dados, os dois campos e o arquivo de indice */
    char nomeArquivo1[256];
    char nomeCampo1[64];
    char nomeArquivo2[256];
    char nomeCampo2[64];
    char nomeArquivoIndice[256];
    scanf("%s %s %s %s %s", nomeArquivo1, nomeCampo1, nomeArquivo2, nomeCampo2, nomeArquivoIndice);
    (void)nomeCampo1;
    (void)nomeCampo2;

    /* Abre e valida o arquivo do loop unico (A) */
    FILE *fpA = fopen(nomeArquivo1, "rb");
    if (fpA == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabA;
    lerCabecalho(fpA, &cabA);
    if (cabA.status == STATUS_INCONSISTENTE) {
        fclose(fpA);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Abre e valida o arquivo interno (B), de onde sera lido o registro final */
    FILE *fpB = fopen(nomeArquivo2, "rb");
    if (fpB == NULL) {
        fclose(fpA);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabB;
    lerCabecalho(fpB, &cabB);
    if (cabB.status == STATUS_INCONSISTENTE) {
        fclose(fpA);
        fclose(fpB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Abre e valida o arquivo de indice da arvore-B */
    FILE *fpIndice = fopen(nomeArquivoIndice, "rb");
    if (fpIndice == NULL) {
        fclose(fpA);
        fclose(fpB);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArvB cabIndice = lerCabecalhoArvB(fpIndice);
    if (cabIndice.status == STATUS_INCONSISTENTE) {
        fclose(fpA);
        fclose(fpB);
        fclose(fpIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Percorre A e busca o registro de B usando o indice */
    int encontrou = 0;
    for (int rrnA = 0; rrnA < cabA.proxRRN; rrnA++) {
        RegistroDados regA;
        lerRegistro(fpA, &regA, rrnA);

        /* Registros removidos ou sem codProxEstacao nao entram na juncao */
        if (regA.removido == REGISTRO_REMOVIDO || regA.codProxEstacao == INTEIRO_NULO) {
            continue;
        }

        /* Busca a chave codProxEstacao de A no indice de codEstacao de B */
        long offsetB;
        if (!buscarNaArvore(fpIndice, cabIndice, regA.codProxEstacao, &offsetB)) {
            continue;
        }

        /* Converte o byte-offset retornado pelo indice para RRN */
        int rrnB = (int)(offsetB - TAMANHO_CABECALHO) / TAMANHO_REGISTRO;
        if (rrnB < 0 || rrnB >= cabB.proxRRN) {
            continue;
        }

        RegistroDados regB;
        lerRegistro(fpB, &regB, rrnB);

        /* So imprime quando o registro de B esta ativo e satisfaz a condicao */
        if (regB.removido == REGISTRO_ATIVO && regA.codProxEstacao == regB.codEstacao) {
            imprimirResultadoJuncao(&regA, &regB);
            encontrou = 1;
        }
    }

    /* Se nenhum par for encontrado, imprime a mensagem padrao */
    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(fpA);
    fclose(fpB);
    fclose(fpIndice);
}
