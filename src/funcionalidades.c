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
    char nomeArquivoDados[256];
    char nomeArquivoIndice[256];
    scanf("%s %s", nomeArquivoDados, nomeArquivoIndice);

    int n;
    scanf("%d", &n);

    FILE *fpDados = fopen(nomeArquivoDados, "r+b");
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
    marcarInconsistente(fpDados);

    FILE *fpIndice = fopen(nomeArquivoIndice, "r+b");

    CabecalhoArvB cabIndice;
    cabIndice = lerCabecalhoArvB(fpIndice);
    if (cabIndice.status == STATUS_INCONSISTENTE) {
        fclose(fpDados);
        fclose(fpIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    cabIndice.status = STATUS_INCONSISTENTE;
    escreverCabecalhoArvB(fpIndice, cabIndice);

    for (int i = 0; i < n; i++) {
        RegistroDados novoReg;
        inicializarRegistro(&novoReg);

        char *leitura = malloc(TAMANHO_MAX_NOME);

        scanf("%s", leitura);
        novoReg.codEstacao = stringNulavelParaInteiro(leitura);

        ScanQuoteString(novoReg.nomeEstacao);
        novoReg.tamNomeEstacao = strlen(novoReg.nomeEstacao);

        scanf("%s", leitura);
        novoReg.codLinha = stringNulavelParaInteiro(leitura);

        ScanQuoteString(novoReg.nomeLinha);
        novoReg.tamNomeLinha = strlen(novoReg.nomeLinha);

        scanf("%s", leitura);
        novoReg.codProxEstacao = stringNulavelParaInteiro(leitura);

        scanf("%s", leitura);
        novoReg.distProxEstacao = stringNulavelParaInteiro(leitura);

        scanf("%s", leitura);
        novoReg.codLinhaIntegra = stringNulavelParaInteiro(leitura);

        scanf("%s", leitura);
        novoReg.codEstIntegra = stringNulavelParaInteiro(leitura);

        free(leitura);

        /* verifica se codEstacao já existe no índice */
        if (novoReg.codEstacao != INTEIRO_NULO) {
            long offsetExistente;
            if (buscarNaArvore(fpIndice, cabIndice, novoReg.codEstacao, &offsetExistente)) {
                continue;  /* chave já existe: não insere */
            }
        }

        int rrnDestino;
        if (cabDados.topo != INTEIRO_NULO) {
            rrnDestino = cabDados.topo;
            RegistroDados regRemovido;
            lerRegistro(fpDados, &regRemovido, rrnDestino);
            cabDados.topo = regRemovido.proximo;
        } else {
            rrnDestino = cabDados.proxRRN;
            cabDados.proxRRN++;
        }

        if (novoReg.codEstacao != INTEIRO_NULO && novoReg.codProxEstacao != INTEIRO_NULO) {
            cabDados.nroParesEstacao++;
        }

        novoReg.removido = REGISTRO_ATIVO;
        novoReg.proximo  = INTEIRO_NULO;

        int offset = (int)rrnParaOffset(rrnDestino);
        escreverRegistro(fpDados, &novoReg, rrnDestino);

        inserirNaArvore(fpIndice, &cabIndice, novoReg.codEstacao, offset);
    }

    atualizarCabecalho(fpDados, &cabDados);
    escreverCabecalho(fpDados, &cabDados);
    fecharArquivoBin(fpDados);

    cabIndice.status = STATUS_CONSISTENTE;
    escreverCabecalhoArvB(fpIndice, cabIndice);
    fclose(fpIndice);

    BinarioNaTela(nomeArquivoDados);
    BinarioNaTela(nomeArquivoIndice);
}
