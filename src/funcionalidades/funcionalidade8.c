#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

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
