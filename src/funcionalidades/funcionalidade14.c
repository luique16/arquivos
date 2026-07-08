#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

/* Função auxiliar que carrega todos os registros de um arquivo de dados para um vetor já alocado */
int carregarRegistros(FILE *fp, int proxRRN, RegistroDados *vetor) {
    int qtd = 0;

    for (int rrn = 0; rrn < proxRRN; rrn++) {
        /* checar se o registro atual foi removido */
        char removido;
        long offset = rrnParaOffset(rrn);
        fseek(fp, offset, SEEK_SET);
        fread(&removido, sizeof(char), 1, fp);
        if (removido == REGISTRO_REMOVIDO) continue;

        /* adicionar o registro no vetor */
        fseek(fp, offset, SEEK_SET);
        lerRegistro(fp, &vetor[qtd], rrn);
        qtd++;
    }

    return qtd;
}

void funcionalidade14() {
    /* Lê os dois arquivos e os dois campos da condição de junção */
    char nomeArquivo1[256];
    char nomeCampo1[64];
    char nomeArquivo2[256];
    char nomeCampo2[64];
    scanf("%s %s %s %s", nomeArquivo1, nomeCampo1, nomeArquivo2, nomeCampo2);

    /* Abre e valida o primeiro arquivo (A) */
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

    /* Trazer os registros do arquivo A para a memória em um vetor */
    RegistroDados *registrosA = malloc(cabA.proxRRN * sizeof(RegistroDados));
    int qtdA = carregarRegistros(fpA, cabA.proxRRN, registrosA);

    /* Ordenar usando qsort() */
    if (strcmp(nomeCampo1, "codEstacao") == 0) {
        qsort(registrosA, qtdA, sizeof(RegistroDados), compararPorCodEstacao);
    } else if (strcmp(nomeCampo1, "codProxEstacao") == 0) {
        qsort(registrosA, qtdA, sizeof(RegistroDados), compararPorCodProxEstacao);
    }

    /* Abre e valida o segundo arquivo (B) */
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

    /* Trazer os registros do arquivo B para a memória em um vetor */
    RegistroDados *registrosB = malloc(cabB.proxRRN * sizeof(RegistroDados));
    int qtdB = carregarRegistros(fpB, cabB.proxRRN, registrosB);

    /* Ordenar usando qsort() */
    if (strcmp(nomeCampo2, "codEstacao") == 0) {
        qsort(registrosB, qtdB, sizeof(RegistroDados), compararPorCodEstacao);
    } else if (strcmp(nomeCampo2, "codProxEstacao") == 0) {
        qsort(registrosB, qtdB, sizeof(RegistroDados), compararPorCodProxEstacao);
    }

    /* Fechar arquivos, não vamos mais precisar deles */
    fclose(fpA);
    fclose(fpB);

    /* Intercalação */
    int i = 0, j = 0;
    int encontrou = 0;

    while (i < qtdA && j < qtdB) {
        int valA = registrosA[i].codProxEstacao;
        int valB = registrosB[j].codEstacao;

        /* Tratar valores inválidos */
        if (valA == INTEIRO_NULO) {
            i++;
            continue;
        }
        if (valB == INTEIRO_NULO) {
            j++;
            continue;
        }

        if (valA == valB) {
            imprimirResultadoJuncao(&registrosA[i], &registrosB[j]);
            encontrou = 1;
            i++;
            j++;  /* ambos avançam, pois B não tem coEstacao duplicados */

        } else if (valA < valB) {
            i++;
        } else {
            j++;
        }
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    /* Liberar a memória */
    free(registrosA);
    free(registrosB);
}
