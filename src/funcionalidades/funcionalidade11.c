#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

void funcionalidade11() {
    /* Lê os dois arquivos e os dois campos da condição de junção */
    char nomeArquivo1[256];
    char nomeCampo1[64];
    char nomeArquivo2[256];
    char nomeCampo2[64];
    scanf("%s %s %s %s", nomeArquivo1, nomeCampo1, nomeArquivo2, nomeCampo2);
    (void)nomeCampo1;
    (void)nomeCampo2;

    /* Abre e valida o arquivo do loop externo (A) */
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

    /* Abre e valida o arquivo do loop interno (B) */
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

    /* Executa juncao por loop aninhado: para cada registro de A, varre B inteiro */
    int encontrou = 0;
    for (int rrnA = 0; rrnA < cabA.proxRRN; rrnA++) {
        RegistroDados regA;
        lerRegistro(fpA, &regA, rrnA);

        /* Registros removidos ou sem codProxEstacao nao participam da juncao */
        if (regA.removido == REGISTRO_REMOVIDO || regA.codProxEstacao == INTEIRO_NULO) {
            continue;
        }

        for (int rrnB = 0; rrnB < cabB.proxRRN; rrnB++) {
            RegistroDados regB;
            lerRegistro(fpB, &regB, rrnB);
            if (regB.removido == REGISTRO_REMOVIDO) {
                continue;
            }

            /* Condição de juncao: A.codProxEstacao = B.codEstacao */
            if (regA.codProxEstacao == regB.codEstacao) {
                imprimirResultadoJuncao(&regA, &regB);
                encontrou = 1;
                break;
            }
        }
    }

    /* Se nenhum par satisfizer a condição, imprime a mensagem padrao */
    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(fpA);
    fclose(fpB);
}
