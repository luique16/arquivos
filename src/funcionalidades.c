/**
 * @file funcionalidades.c
 * @brief Implementações das seis funcionalidades do programa.
 */

#include "../include/funcionalidades.h"
#include "../include/io.h"
#include "../include/definicoes.h"
#include "../include/csv.h"
#include "../include/fornecidas.h"
#include "../include/uteis.h"

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

    /* Listas para definir cabeçalho final */
    int tamanhoListas = 10; /* Tamanho inicial das listas */
    char** nomesEstacoes = malloc(sizeof(char*)*tamanhoListas);
    int* parEstacaoA = malloc(sizeof(int)*tamanhoListas);
    int* parEstacaoB = malloc(sizeof(int)*tamanhoListas);

    /* Lê CSV e grava cada registro */
    RegistroDados reg;
    lerRegistroCSV(csv, &reg); /* Ignora primeira linha do CSV (cabeçalho) */
    inicializarRegistro(&reg);
    while (lerRegistroCSV(csv, &reg)) {
        escreverRegistro(bin, &reg, cabecalho.proxRRN);

        cabecalho.proxRRN++;

        if (cabecalho.proxRRN == tamanhoListas) {
            /* Se lista cheia, aumenta tamanho */
            tamanhoListas += 10;
            nomesEstacoes = realloc(nomesEstacoes, sizeof(char*)*tamanhoListas);
            parEstacaoA = realloc(parEstacaoA, sizeof(int)*tamanhoListas);
            parEstacaoB = realloc(parEstacaoB, sizeof(int)*tamanhoListas);
        }

        /* Adiciona estação ao final da lista */
        nomesEstacoes[cabecalho.nroEstacoes] = strdup(reg.nomeEstacao);
        parEstacaoA[cabecalho.nroEstacoes] = reg.codEstacao;
        parEstacaoB[cabecalho.nroEstacoes] = reg.codProxEstacao;
        cabecalho.nroEstacoes++;

        inicializarRegistro(&reg);
    }

    /* Subtrai contagem de estações se houver repetição de nome */
    int contagem = cabecalho.nroEstacoes;
    for (int i = 0; i < contagem; i++) {
        if(nomesEstacoes[i] == NULL) {
            continue;
        }

        for (int j = i+1; j < contagem; j++) {
            if (nomesEstacoes[j] == NULL) {
                continue;
            }

            if (strcmp(nomesEstacoes[i], nomesEstacoes[j]) == 0) {
                cabecalho.nroEstacoes--;
                free(nomesEstacoes[j]);
                nomesEstacoes[j] = NULL;
            }
        }
    }

    /* Soma pares de estações */
    for (int i = 0; i < contagem; i++) {
        if (parEstacaoA[i] == INTEIRO_NULO || parEstacaoB[i] == INTEIRO_NULO) {
            continue;
        }

        for (int j = i+1; j < contagem; j++) {
            if (parEstacaoA[j] == INTEIRO_NULO || parEstacaoB[j] == INTEIRO_NULO) {
                continue;
            }

            if ((parEstacaoA[i] == parEstacaoA[j] && parEstacaoB[i] == parEstacaoB[j])
            || (parEstacaoA[i] == parEstacaoB[j] && parEstacaoB[i] == parEstacaoA[j])) {
                parEstacaoA[j] = INTEIRO_NULO;
                parEstacaoB[j] = INTEIRO_NULO;
                continue;
            }
        }

        cabecalho.nroParesEstacao++;
    }

    /* Limpeza de memória */
    for (int i = 0; i < contagem; i++) {
        if (nomesEstacoes[i] != NULL) {
            free(nomesEstacoes[i]);
        }
    }
    free(nomesEstacoes);
    free(parEstacaoA);
    free(parEstacaoB);

    /* Atualiza cabeçalho final e fecha consistentemente */
    escreverCabecalho(bin, &cabecalho);
    fecharArquivoBin(bin);
    fclose(csv);

    BinarioNaTela(nomeArquivo); /* exibe binário após fechar */
}

void funcionalidade2() {
    char nomeArquivo[256];

    scanf("%s", nomeArquivo);

    FILE *bin = abrirArquivoBin(nomeArquivo, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    int encontrou = 0;

    /* Percorre todos os RRNs sequencialmente */
    for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++) {
        RegistroDados reg;
        lerRegistro(bin, &reg, rrn);

        if (reg.removido == REGISTRO_ATIVO) {
            imprimirRegistro(&reg);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(bin);
}

void funcionalidade3() {
    char nomeArquivo[256];
    scanf("%s", nomeArquivo);

    int n;
    scanf("%d", &n);

    FILE *bin = abrirArquivoBin(nomeArquivo, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //a partir daqui, foi modularizado na função busca(FILE *bin, CabecalhoArquivo cabecalho, int n, void (*acao)(tipos da função acao))
    busca(bin, &cabecalho, n, acaoImprimir);

    fclose(bin);
}

void funcionalidade4() {
    char nomeArquivo[256];
    scanf("%s", nomeArquivo);

    int n;
    scanf("%d", &n);

    FILE *bin = abrirArquivoBin(nomeArquivo, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    /* Define status do cabeçalho como inconsistente para segurança */
    marcarInconsistente(bin);

    //assim como na funcionalidade 3, usamos a função busca, mas dessa vez passando a função removerLogic() como parâmetro
    busca(bin, &cabecalho, n, removerLogic);

    /* Atualiza nroEstacoes e nroParesEstacao no cabeçalho */
    atualizarCabecalho(bin, &cabecalho);

    //atualizar o cabeçalho no arquivo após as remoções
    escreverCabecalho(bin, &cabecalho);

    fecharArquivoBin(bin);

    BinarioNaTela(nomeArquivo); //exibe o binário após fechar
    return;
}
