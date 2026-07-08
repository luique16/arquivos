#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

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