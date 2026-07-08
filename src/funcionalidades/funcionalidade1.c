#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"

#include <stdio.h>

void funcionalidade1() {
    /* Nomes dos arquivos */
    char nomeCSV[256];
    char nomeArquivo[256];

    scanf("%s %s", nomeCSV, nomeArquivo);

    /* Abre o arquivo CSV */
    FILE *csv = fopen(nomeCSV, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Abre o arquivo binário */
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

    /* Lê CSV e grava cada registro, acumulando dados para contagem */
    RegistroDados reg;
    lerRegistroCSV(csv, &reg); /* Ignora primeira linha do CSV (cabeçalho) */
    inicializarRegistro(&reg);

    char **nomesEst = NULL;
    int *pA = NULL, *pB = NULL;
    int qtdReg = 0;

    while (lerRegistroCSV(csv, &reg)) {
        qtdReg++;
        nomesEst = realloc(nomesEst, qtdReg * sizeof(char*));
        pA = realloc(pA, qtdReg * sizeof(int));
        pB = realloc(pB, qtdReg * sizeof(int));

        nomesEst[qtdReg - 1] = reg.tamNomeEstacao > 0 ? strdup(reg.nomeEstacao) : NULL;
        pA[qtdReg - 1] = reg.codEstacao;
        pB[qtdReg - 1] = reg.codProxEstacao;

        escreverRegistro(bin, &reg, cabecalho.proxRRN);
        cabecalho.proxRRN++;
        inicializarRegistro(&reg); /* Limpa registro para a próxima leitura */
    }

    /* Conta estações distintas por nome */
    int nEst = 0;
    for (int i = 0; i < qtdReg; i++) {
        if (nomesEst[i] == NULL) continue;
        nEst++;
        for (int j = i + 1; j < qtdReg; j++) {
            if (nomesEst[j] != NULL &&
                strcmp(nomesEst[i], nomesEst[j]) == 0) {
                free(nomesEst[j]);
                nomesEst[j] = NULL;
            }
        }
    }

    /* Conta pares (A,B) distintos - (A,B) e (B,A) são o mesmo par */
    int nPar = 0;
    for (int i = 0; i < qtdReg; i++) {
        if (pA[i] == INTEIRO_NULO || pB[i] == INTEIRO_NULO) continue;
        nPar++;
        for (int j = i + 1; j < qtdReg; j++) {
            if (pA[j] == INTEIRO_NULO || pB[j] == INTEIRO_NULO) continue;
            if ((pA[i] == pA[j] && pB[i] == pB[j]) ||
                (pA[i] == pB[j] && pB[i] == pA[j])) {
                pA[j] = INTEIRO_NULO;
                pB[j] = INTEIRO_NULO;
            }
        }
    }

    /* Limpeza */
    for (int i = 0; i < qtdReg; i++) free(nomesEst[i]);
    free(nomesEst);
    free(pA);
    free(pB);

    cabecalho.nroEstacoes = nEst;
    cabecalho.nroParesEstacao = nPar;

    /* Escreve cabeçalho e fecha consistentemente */
    escreverCabecalho(bin, &cabecalho);
    fecharArquivoBin(bin);
    fclose(csv);

    BinarioNaTela(nomeArquivo); /* exibe binário após fechar */
}