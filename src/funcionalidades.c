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
    FILE *bin = abrirArquivoBin(nomeArquivo, "wb+");
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
    lerRegistroCSV(csv, &reg); /* Ignora primeira linha do CSV (cabeçalho) */
    inicializarRegistro(&reg);
    while (lerRegistroCSV(csv, &reg)) {
        escreverRegistro(bin, &reg, cabecalho.proxRRN);
        cabecalho.proxRRN++;
        inicializarRegistro(&reg); /* Limpa registro para a próxima leitura */
    }

    /* Atualiza o nroEstacoes e nroParesEstacao no cabeçalho */
    atualizarCabecalho(bin, &cabecalho);

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
    int encontrou = 0;
    for (int rrn = 0; rrn < cabecalho.proxRRN; rrn++) {
        RegistroDados reg;
        lerRegistro(bin, &reg, rrn);

        /* Ignora registros removidos */
        if (reg.removido == REGISTRO_ATIVO) {
            imprimirRegistro(&reg);
            encontrou = 1;
        }
    }

    /* Imprime mensagem de erro se nenhum registro for encontrado */
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
    // atualizarCabecalho(bin, &cabecalho);

    // Atualizar o cabeçalho no arquivo após as remoções
    escreverCabecalho(bin, &cabecalho);

    fecharArquivoBin(bin);

    BinarioNaTela(nomeArquivo); //exibe o binário após fechar
    return;
}
