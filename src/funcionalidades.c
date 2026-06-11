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

void funcionalidade2() {
    /* Nome do arquivo binário */
    char nomeArquivo[256];

    scanf("%s", nomeArquivo);

    /* Abre o arquivo binário */
    FILE *bin = abrirArquivoBin(nomeArquivo, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    /* Verifica se o arquivo está consistente */
    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Percorre todos os registros sequencialmente */
    imprimirTodosRegistros(bin, &cabecalho);

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

    /* Verifica se o arquivo está consistente */
    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

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

void funcionalidade5() {
    /* Nome do arquivo binário */
    char nomeArquivo[256];
    scanf("%s", nomeArquivo);

    /* Abre o arquivo binário */
    FILE *bin = abrirArquivoBin(nomeArquivo, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Número de inserções */
    int n;
    scanf("%d", &n);

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    /* Verifica se o arquivo está consistente */
    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Marca o arquivo como inconsistente para realizar alterações */
    marcarInconsistente(bin);

    /* Insere cada registro */
    for (int insercao = 0; insercao < n; insercao++) {
        RegistroDados novoReg;
        inicializarRegistro(&novoReg);

        /* Lê dados do usuário */
        char* leitura = malloc(TAMANHO_MAX_NOME);

        /* codEstacao */
        scanf("%s", leitura);
        novoReg.codEstacao = stringNulavelParaInteiro(leitura);

        /* nomeEstacao */
        ScanQuoteString(novoReg.nomeEstacao);
        novoReg.tamNomeEstacao = strlen(novoReg.nomeEstacao);

        /* codLinha */
        scanf("%s", leitura);
        novoReg.codLinha = stringNulavelParaInteiro(leitura);

        /* nomeLinha */
        ScanQuoteString(novoReg.nomeLinha);
        novoReg.tamNomeLinha = strlen(novoReg.nomeLinha);

        /* codProxEstacao */
        scanf("%s", leitura);
        novoReg.codProxEstacao = stringNulavelParaInteiro(leitura);

        /* distProxEstacao */
        scanf("%s", leitura);
        novoReg.distProxEstacao = stringNulavelParaInteiro(leitura);

        /* codLinhaIntegra */
        scanf("%s", leitura);
        novoReg.codLinhaIntegra = stringNulavelParaInteiro(leitura);

        /* codEstIntegra */
        scanf("%s", leitura);
        novoReg.codEstIntegra = stringNulavelParaInteiro(leitura);

        free(leitura);

        /* Seleciona o RRN de destino (topo da pilha ou proximo RRN) */
        int rrnDestino;

        if (cabecalho.topo != INTEIRO_NULO) {
            /* Reaproveitamento: usa o topo da pilha */
            rrnDestino = cabecalho.topo;

            RegistroDados regRemovido;
            lerRegistro(bin, &regRemovido, rrnDestino);
            cabecalho.topo = regRemovido.proximo; /* desempilha */
        } else {
            /* Pilha vazia: insere ao final */
            rrnDestino = cabecalho.proxRRN;
            cabecalho.proxRRN++;
        }

        /* Esperado nos testes: soma número de pares de estações se codEstacao e codProxEstacao não forem nulos */
        if (novoReg.codEstacao != INTEIRO_NULO && novoReg.codProxEstacao != INTEIRO_NULO) {
            cabecalho.nroParesEstacao++;
        }

        novoReg.removido = REGISTRO_ATIVO;
        novoReg.proximo  = INTEIRO_NULO;
        escreverRegistro(bin, &novoReg, rrnDestino);
    }

    /* Cabeçalho não atualizado, pois é garantido que não haverá outra alteração em nroEstacoes e nroParesEstacao */
    // atualizarCabecalho(bin, &cabecalho);

    /* Escreve cabeçalho e fecha consistentemente */
    escreverCabecalho(bin, &cabecalho);
    fecharArquivoBin(bin);

    BinarioNaTela(nomeArquivo);
}

void funcionalidade6() {
    char nomeArquivo[256];
    scanf("%s", nomeArquivo);

    // Quantidade de buscas a serem realizadas
    int n;
    scanf("%d", &n);

    FILE *bin = abrirArquivoBin(nomeArquivo, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabecalho;
    lerCabecalho(bin, &cabecalho);

    // Verificar se o arquivo está consistente
    if (cabecalho.status == STATUS_INCONSISTENTE) {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Marcar o arquivo como inconsistente, pois abrimos num modo que escreve
    marcarInconsistente(bin);

    // Assim como nas funcionalidades 3 e 4, usamos a função busca, mas dessa vez passando a função atualizarRegistro() como parâmetro
    busca(bin, &cabecalho, n, atualizarRegistro);

    // Atualiza nroEstacoes e nroParesEstacao no cabeçalho
    atualizarCabecalho(bin, &cabecalho);

    // Atualizar o cabeçalho no arquivo após as remoções
    escreverCabecalho(bin, &cabecalho);

    fecharArquivoBin(bin);

    BinarioNaTela(nomeArquivo); //exibe o binário após fechar
    return;
}
