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
#include "../include/arvoreb.h"

#include <stdio.h>
#include <stdlib.h>

static int compararPorCodEstacao(const void *a, const void *b) {
    const RegistroDados *ra = (const RegistroDados *)a;
    const RegistroDados *rb = (const RegistroDados *)b;
    return ra->codEstacao - rb->codEstacao;
}

static int compararPorCodProxEstacao(const void *a, const void *b) {
    const RegistroDados *ra = (const RegistroDados *)a;
    const RegistroDados *rb = (const RegistroDados *)b;

    /* Quando ambos forem nulos, sao equivalentes */
    if (ra->codProxEstacao == INTEIRO_NULO && rb->codProxEstacao == INTEIRO_NULO) {
        return 0;
    }

    /* Nulo vai para o fim */
    if (ra->codProxEstacao == INTEIRO_NULO) {
        return 1;
    }
    if (rb->codProxEstacao == INTEIRO_NULO) {
        return -1;
    }

    return ra->codProxEstacao - rb->codProxEstacao;
}

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
        lerRegistroStdin(&novoReg);

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

void funcionalidade7() {
    char nomeArquivoDados[256];
    char nomeArquivoIndice[256];

    scanf("%s %s", nomeArquivoDados, nomeArquivoIndice);

    /* abre arquivo de dados */
    FILE *fpDados = fopen(nomeArquivoDados, "rb");
    if (fpDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabDados;
    lerCabecalho(fpDados, &cabDados);
    if (cabDados.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* cria arquivo de índice da árvore-B */
    FILE *fpIndice = fopen(nomeArquivoIndice, "w+b");
    if (fpIndice == NULL) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArvB cabIndice = cabecalhoArvoreVazio();
    cabIndice.status = STATUS_INCONSISTENTE;      /* inconsistente durante construção */
    escreverCabecalhoArvB(fpIndice, cabIndice);

    for (int rrn = 0; rrn < cabDados.proxRRN; rrn++) {
        /* verifica se registro foi removido */
        char removido;
        long offset = rrnParaOffset(rrn);
        fseek(fpDados, offset, SEEK_SET);
        fread(&removido, sizeof(char), 1, fpDados);
        if (removido == REGISTRO_REMOVIDO) continue;

        /* insere chave (codEstacao) e offset na árvore */
        RegistroDados reg;
        fseek(fpDados, offset, SEEK_SET);
        lerRegistro(fpDados, &reg, rrn);

        int pr = (int)offset;
        inserirNaArvore(fpIndice, &cabIndice, reg.codEstacao, pr);
    }

    cabIndice.status = STATUS_CONSISTENTE;      /* marca como consistente */
    escreverCabecalhoArvB(fpIndice, cabIndice);
    fclose(fpIndice);
    fclose(fpDados);

    BinarioNaTela(nomeArquivoIndice);
}

void funcionalidade8() {
    char nomeArquivoDados[256];
    char nomeArquivoIndice[256];
    scanf("%s %s", nomeArquivoDados, nomeArquivoIndice);

    int n;
    scanf(" %d", &n);

    FILE *fpDados = fopen(nomeArquivoDados, "rb");
    if (fpDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabDados;
    lerCabecalho(fpDados, &cabDados);
    if (cabDados.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *fpIndice = fopen(nomeArquivoIndice, "r+b");
    if (fpIndice == NULL) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArvB cabIndice = lerCabecalhoArvB(fpIndice);
    if (cabIndice.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        fclose(fpIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    for (int buscaId = 0; buscaId < n; buscaId++) {
        /* lê critérios de validação */
        int qtdValidacoes;
        scanf(" %d", &qtdValidacoes);

        Validacao *validacoes = malloc(qtdValidacoes * sizeof(Validacao));
        for (int c = 0; c < qtdValidacoes; c++) {
            validacoes[c].campo = malloc(TAMANHO_MAX_NOME);
            validacoes[c].valor = malloc(TAMANHO_MAX_NOME);
            scanf("%s", validacoes[c].campo);
            if (strcmp(validacoes[c].campo, "nomeEstacao") == 0
            ||  strcmp(validacoes[c].campo, "nomeLinha") == 0) {
                ScanQuoteString(validacoes[c].valor);  /* campos string entre aspas */
            } else {
                scanf("%s", validacoes[c].valor);
            }
        }

        int usouIndice = 0;
        int encontrou = 0;

        /* tenta usar índice se filtrar por codEstacao */
        for (int v = 0; v < qtdValidacoes; v++) {
            if (strcmp(validacoes[v].campo, "codEstacao") == 0) {
                usouIndice = 1;
                long offset;
                int chave = stringNulavelParaInteiro(validacoes[v].valor);
                if (buscarNaArvore(fpIndice, cabIndice, chave, &offset)) {
                    int rrn = (int) (offset - TAMANHO_CABECALHO) / TAMANHO_REGISTRO; /* Converte offset para RRN para poder aproveitar a função lerRegistro */
                    RegistroDados reg;
                    lerRegistro(fpDados, &reg, rrn);

                    if (reg.removido == REGISTRO_ATIVO && validarCampos(&reg, validacoes, qtdValidacoes)) {
                        imprimirRegistro(&reg);
                        encontrou = 1;
                    }
                }
                break;
            }
        }

        /* sem índice: varredura sequencial com os critérios já lidos */
        if (!usouIndice) {
            int encontrados_seq = 0;
            for (int rrn = 0; rrn < cabDados.proxRRN; rrn++) {
                char removido;
                long offset = rrnParaOffset(rrn);
                fseek(fpDados, offset, SEEK_SET);
                fread(&removido, sizeof(char), 1, fpDados);
                if (removido == REGISTRO_REMOVIDO) continue;

                RegistroDados reg;
                fseek(fpDados, offset, SEEK_SET);
                lerRegistro(fpDados, &reg, rrn);

                if (validarCampos(&reg, validacoes, qtdValidacoes)) {
                    imprimirRegistro(&reg);
                    encontrados_seq = 1;
                }
            }
            encontrou = encontrados_seq;
        }

        if (!encontrou) {
            printf("Registro inexistente.\n\n");
        }

        /* libera memória das validações */
        for (int c = 0; c < qtdValidacoes; c++) {
            free(validacoes[c].campo);
            free(validacoes[c].valor);
        }
        free(validacoes);

        if (encontrou) {
            printf("\n");
        }
    }

    fclose(fpIndice);
    fclose(fpDados);
}

void funcionalidade9() {
    /* Nome dos arquivos de dados e índice */
    char nomeArquivoDados[256];
    char nomeArquivoIndice[256];
    scanf("%s %s", nomeArquivoDados, nomeArquivoIndice);

    /* Número de inserções */
    int n;
    scanf("%d", &n);

    /* Abre o arquivo de dados */
    FILE *fpDados = fopen(nomeArquivoDados, "r+b");
    if (fpDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Lê cabeçalho do arquivo de dados */
    CabecalhoArquivo cabDados;
    lerCabecalho(fpDados, &cabDados);
    if (cabDados.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Marca arquivo de dados como inconsistente */
    marcarInconsistente(fpDados);

    /* Abre o arquivo de índice */
    FILE *fpIndice = fopen(nomeArquivoIndice, "r+b");
    if (fpIndice == NULL) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Lê cabeçalho do arquivo de índice */
    CabecalhoArvB cabIndice;
    cabIndice = lerCabecalhoArvB(fpIndice);

    if (cabIndice.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        fclose(fpIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Marca arquivo de índice como inconsistente */
    cabIndice.status = STATUS_INCONSISTENTE;
    escreverCabecalhoArvB(fpIndice, cabIndice);

    for (int i = 0; i < n; i++) {
        RegistroDados novoReg;
        lerRegistroStdin(&novoReg);

        /* verifica se codEstacao já existe no índice */
        if (novoReg.codEstacao != INTEIRO_NULO) {
            long offsetExistente;
            if (buscarNaArvore(fpIndice, cabIndice, novoReg.codEstacao, &offsetExistente)) {
                continue;  /* chave já existe: não insere */
            }
        }

        /* Seleciona o RRN de destino (topo da pilha ou proxRRN) */
        int rrnDestino;
        if (cabDados.topo != INTEIRO_NULO) {
            /* Reaproveita topo da pilha de removidos */
            rrnDestino = cabDados.topo;
            RegistroDados regRemovido;
            lerRegistro(fpDados, &regRemovido, rrnDestino);
            cabDados.topo = regRemovido.proximo; /* desempilha */
        } else {
            /* Pilha vazia: insere ao final */
            rrnDestino = cabDados.proxRRN;
            cabDados.proxRRN++;
        }

        /* Incrementa contador se codEstacao e codProxEstacao não forem nulos */
        if (novoReg.codEstacao != INTEIRO_NULO && novoReg.codProxEstacao != INTEIRO_NULO) {
            cabDados.nroParesEstacao++;
        }

        /* Marca como ativo e sem próximo na pilha */
        novoReg.removido = REGISTRO_ATIVO;
        novoReg.proximo  = INTEIRO_NULO;

        /* Escreve registro no arquivo de dados */
        int offset = (int)rrnParaOffset(rrnDestino);
        escreverRegistro(fpDados, &novoReg, rrnDestino);

        /* Insere chave no índice */
        inserirNaArvore(fpIndice, &cabIndice, novoReg.codEstacao, offset);
    }

    /* Finaliza arquivo de dados */
    atualizarCabecalho(fpDados, &cabDados);
    cabDados.status = STATUS_CONSISTENTE;   /* Marcando explicitamente o arquivo de dados como consistente */
    escreverCabecalho(fpDados, &cabDados);
    fecharArquivoBin(fpDados);

    /* Finaliza arquivo de índice */
    cabIndice.status = STATUS_CONSISTENTE;
    escreverCabecalhoArvB(fpIndice, cabIndice);
    fclose(fpIndice);

    /* Exibe binários */
    BinarioNaTela(nomeArquivoDados);
    BinarioNaTela(nomeArquivoIndice);
}

void funcionalidade10() {
    char nomeArquivoDados[256];
    char nomeArquivoIndice[256];
    scanf("%s %s", nomeArquivoDados, nomeArquivoIndice);

    int n;
    scanf(" %d", &n);

    FILE *fpDados = fopen(nomeArquivoDados, "rb+"); /* Abre o arquivo de dados no modo de leitura e escrita (binário) */
    if (fpDados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Ler cabeçalho do arquivo de dados */
    CabecalhoArquivo cabDados;
    lerCabecalho(fpDados, &cabDados);
    if (cabDados.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *fpIndice = fopen(nomeArquivoIndice, "rb+"); /* Abre o arquivo de índice árvore-B no modo de leitura e escrita (binário) */
    if (fpIndice == NULL) {
        fclose(fpDados);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Ler cabeçalho do arquivo da árvore-B */
    CabecalhoArvB cabIndice = lerCabecalhoArvB(fpIndice);
    if (cabIndice.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        fclose(fpIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Marcar os dois arquivos como inconsistentes, pois vamos fazer alterações neles */
    marcarInconsistente(fpDados);
    marcarInconsistente(fpIndice);
    cabDados.status = STATUS_INCONSISTENTE;
    cabIndice.status = STATUS_INCONSISTENTE;

    for (int remId = 0; remId < n; remId++) {
        /* A partir daqui é bem semelhante à função de busca (8), mas muda o que vamos fazer ao encontrar o registro, seria bom modularizar essa parte depois */
        /* lê critérios de validação */
        int qtdValidacoes;
        scanf(" %d", &qtdValidacoes);

        Validacao *validacoes = malloc(qtdValidacoes * sizeof(Validacao));
        for (int c = 0; c < qtdValidacoes; c++) {
            validacoes[c].campo = malloc(TAMANHO_MAX_NOME);
            validacoes[c].valor = malloc(TAMANHO_MAX_NOME);
            scanf("%s", validacoes[c].campo);
            if (strcmp(validacoes[c].campo, "nomeEstacao") == 0
            ||  strcmp(validacoes[c].campo, "nomeLinha") == 0) {
                ScanQuoteString(validacoes[c].valor);  /* campos string entre aspas */
            } else {
                scanf("%s", validacoes[c].valor);
            }
        }

        int usouIndice = 0;
        int encontrou = 0;

        /* tenta usar índice se filtrar por codEstacao */
        for (int v = 0; v < qtdValidacoes; v++) {
            if (strcmp(validacoes[v].campo, "codEstacao") == 0) {
                usouIndice = 1;
                long offset;
                int chave = stringNulavelParaInteiro(validacoes[v].valor);
                if (buscarNaArvore(fpIndice, cabIndice, chave, &offset)) {
                    int rrn = (int) (offset - TAMANHO_CABECALHO) / TAMANHO_REGISTRO; /* Converte offset para RRN para poder aproveitar a função lerRegistro */
                    RegistroDados reg;
                    lerRegistro(fpDados, &reg, rrn);

                    if (reg.removido == REGISTRO_ATIVO && validarCampos(&reg, validacoes, qtdValidacoes)) {
                        int chave = reg.codEstacao;
                        removerLogic(fpDados, &cabDados, &reg, rrn, NULL, 0); /* Os últimos dois argumentos da função só existem por causa do wrapper para aproveitar a função de busca da funcionalidade 3, portanto não são necessários aqui e receberam NULL e 0 */
                        removerDaArvore(fpIndice, &cabIndice, chave);
                        encontrou = 1;
                    }
                }
                break;
            }
        }

        /* sem índice: varredura sequencial com os critérios já lidos */
        if (!usouIndice) {
            int encontrados_seq = 0;
            for (int rrn = 0; rrn < cabDados.proxRRN; rrn++) {
                char removido;
                long offset = rrnParaOffset(rrn);
                fseek(fpDados, offset, SEEK_SET);
                fread(&removido, sizeof(char), 1, fpDados); // Ler o byte para checar se está removido
                if (removido == REGISTRO_REMOVIDO) continue;

                RegistroDados reg;
                // fseek(fpDados, offset, SEEK_SET); lerRegistro já faz o fseek absoluto internamente
                lerRegistro(fpDados, &reg, rrn);

                if (validarCampos(&reg, validacoes, qtdValidacoes)) {
                    int chave = reg.codEstacao;
                    removerLogic(fpDados, &cabDados, &reg, rrn, NULL, 0); /* Os últimos dois argumentos da função só existem por causa do wrapper para aproveitar a função de busca da funcionalidade 3, portanto não são necessários aqui e receberam NULL e 0 */
                    removerDaArvore(fpIndice, &cabIndice, chave);
                    encontrados_seq = 1;
                }
            }
            encontrou = encontrados_seq;
        }

        /* libera memória das validações */
        for (int c = 0; c < qtdValidacoes; c++) {
            free(validacoes[c].campo);
            free(validacoes[c].valor);
        }
        free(validacoes);
    }

    /* Recalcular nroEstacoes e nroParesEstacao no cabeçalho dos dados (EM MEMÓRIA) */
    atualizarCabecalho(fpDados, &cabDados);

    /* Marcar os arquivos como consistentes */
    cabDados.status = STATUS_CONSISTENTE;
    cabIndice.status = STATUS_CONSISTENTE;

    /* Atualizar os cabeçalhos no disco */
    escreverCabecalho(fpDados, &cabDados);
    escreverCabecalhoArvB(fpIndice, cabIndice);

    /* Fechar os arquivos */
    fclose(fpDados);
    fclose(fpIndice);

    /* Mostrar os binários na tela */
    BinarioNaTela(nomeArquivoDados);
    BinarioNaTela(nomeArquivoIndice);
}

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

void funcionalidade13() {
    /* Le arquivo de entrada, campo de ordenacao e arquivo de saida */
    char nomeArquivoEntrada[256];
    char campoOrdenacao[64];
    char nomeArquivoSaida[256];
    scanf("%s %s %s", nomeArquivoEntrada, campoOrdenacao, nomeArquivoSaida);

    /* Abre e valida arquivo de entrada */
    FILE *fpEntrada = fopen(nomeArquivoEntrada, "rb");
    if (fpEntrada == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabEntrada;
    lerCabecalho(fpEntrada, &cabEntrada);
    if (cabEntrada.status == STATUS_INCONSISTENTE) {
        fclose(fpEntrada);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Conta registros ativos para alocar vetor em memoria */
    int nAtivos = 0;
    for (int rrn = 0; rrn < cabEntrada.proxRRN; rrn++) {
        char removido;
        fseek(fpEntrada, rrnParaOffset(rrn), SEEK_SET);
        fread(&removido, sizeof(char), 1, fpEntrada);
        if (removido == REGISTRO_ATIVO) {
            nAtivos++;
        }
    }

    RegistroDados *vetor = NULL;
    if (nAtivos > 0) {
        vetor = malloc((size_t)nAtivos * sizeof(RegistroDados));
        if (vetor == NULL) {
            fclose(fpEntrada);
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    /* Carrega os registros ativos no vetor */
    int idx = 0;
    for (int rrn = 0; rrn < cabEntrada.proxRRN; rrn++) {
        char removido;
        fseek(fpEntrada, rrnParaOffset(rrn), SEEK_SET);
        fread(&removido, sizeof(char), 1, fpEntrada);
        if (removido == REGISTRO_REMOVIDO) {
            continue;
        }

        lerRegistro(fpEntrada, &vetor[idx], rrn);
        idx++;
    }
    fclose(fpEntrada);

    /* Ordena conforme campo solicitado */
    if (strcmp(campoOrdenacao, "codEstacao") == 0) {
        qsort(vetor, (size_t)nAtivos, sizeof(RegistroDados), compararPorCodEstacao);
    } else if (strcmp(campoOrdenacao, "codProxEstacao") == 0) {
        qsort(vetor, (size_t)nAtivos, sizeof(RegistroDados), compararPorCodProxEstacao);
    } else {
        free(vetor);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* Cria arquivo de saida e grava cabecalho inicial */
    FILE *fpSaida = fopen(nomeArquivoSaida, "wb+");
    if (fpSaida == NULL) {
        free(vetor);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CabecalhoArquivo cabSaida;
    cabSaida.status = STATUS_INCONSISTENTE;
    cabSaida.topo = INTEIRO_NULO;
    cabSaida.proxRRN = nAtivos;
    cabSaida.nroEstacoes = 0;
    cabSaida.nroParesEstacao = 0;
    escreverCabecalho(fpSaida, &cabSaida);

    /* Grava vetor ordenado no arquivo de saida */
    for (int i = 0; i < nAtivos; i++) {
        vetor[i].removido = REGISTRO_ATIVO;
        vetor[i].proximo = INTEIRO_NULO;
        escreverRegistro(fpSaida, &vetor[i], i);
    }

    /* Recalcula valores do cabecalho e finaliza arquivo */
    atualizarCabecalho(fpSaida, &cabSaida);
    escreverCabecalho(fpSaida, &cabSaida);
    fecharArquivoBin(fpSaida);

    free(vetor);
    BinarioNaTela(nomeArquivoSaida);
}

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

    /* Trazer os registros do arquivo A para a memória em um vetor */
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
            printf("%d %s %s %d %s\n",
                registrosA[i].codEstacao,
                registrosA[i].nomeEstacao,
                registrosA[i].nomeLinha,
                registrosA[i].codProxEstacao,
                registrosB[j].nomeEstacao);
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