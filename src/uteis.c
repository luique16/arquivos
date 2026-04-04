/**
 * @file uteis.c
 * @brief Implementações das funções auxiliares.
 */

#include "../include/uteis.h"
#include "../include/definicoes.h"
#include "../include/io.h"
#include "../include/fornecidas.h"
#include <string.h>

void imprimirRegistro(RegistroDados *reg) {
    /* Dados não nulos */
    printf("%d %s %d %s"
           , reg->codEstacao
           , reg->nomeEstacao
           , reg->codLinha
           , reg->nomeLinha);

    /* Dados talvez nulos */
    if (reg->codProxEstacao != INTEIRO_NULO) {
        printf(" %d %d", reg->codProxEstacao, reg->distProxEstacao);
    } else {
        printf(" NULO NULO");
    }

    if (reg->codLinhaIntegra != INTEIRO_NULO) {
        printf(" %d %d", reg->codLinhaIntegra, reg->codEstIntegra);
    } else {
        printf(" NULO NULO");
    }

    printf("\n");
}

void acaoImprimir(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn) {
    imprimirRegistro(reg);
}

int stringNulavelParaInteiro(char* string) {
    if (string == NULL || strcmp(string, "NULO") == 0) {
        return INTEIRO_NULO;
    }

    return atoi(string);
}

bool validarCampos(RegistroDados* reg, Validacao* validacoes, int qtdValidacoes) {
    for (int i = 0; i < qtdValidacoes; i++) {
        if (strcmp(validacoes[i].campo, "codEstacao") == 0) {
            if (reg->codEstacao != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "codLinha") == 0) {
            if (reg->codLinha != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "codProxEstacao") == 0) {
            if (reg->codProxEstacao != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "distProxEstacao") == 0) {
            if (reg->distProxEstacao != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "codLinhaIntegra") == 0) {
            if (reg->codLinhaIntegra != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "codEstIntegra") == 0) {
            if (reg->codEstIntegra != stringNulavelParaInteiro(validacoes[i].valor)) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "nomeEstacao") == 0) {
            if (strcmp(reg->nomeEstacao, validacoes[i].valor) != 0) {
                return false;
            }
        } else if (strcmp(validacoes[i].campo, "nomeLinha") == 0) {
            if (strcmp(reg->nomeLinha, validacoes[i].valor) != 0) {
                return false;
            }
        }
    }

    return true;
}

void busca(FILE *bin, CabecalhoArquivo *cabecalho, int n, void (*acao)(FILE *f, CabecalhoArquivo *c, RegistroDados *reg, int rrn)) {
    /* Executa n buscas consecutivas */
    for (int i = 0; i < n; i++) {
        int qtdValidacoes;
        scanf("%d", &qtdValidacoes);

        Validacao* validacoes = malloc(sizeof(Validacao)*qtdValidacoes);

        for (int c = 0; c < qtdValidacoes; c++) {
            validacoes[c].campo = malloc(sizeof(char)*TAMANHO_MAX_NOME);
            validacoes[c].valor = malloc(sizeof(char)*TAMANHO_MAX_NOME);
            scanf("%s", validacoes[c].campo);

            if (strcmp(validacoes[c].campo, "nomeEstacao") == 0
            ||  strcmp(validacoes[c].campo, "nomeLinha") == 0) {
                ScanQuoteString(validacoes[c].valor);
            } else {
                scanf("%s", validacoes[c].valor);
            }
        }

        int encontrou = 0;

        /* Percorre todos os RRNs para esta busca */
        for (int rrn = 0; rrn < cabecalho->proxRRN; rrn++) {
            RegistroDados reg;
            lerRegistro(bin, &reg, rrn);

            if (reg.removido == REGISTRO_REMOVIDO) continue;

            /* Executa validações */
            if (validarCampos(&reg, validacoes, qtdValidacoes)) {
                acao(bin, cabecalho, &reg, rrn);
                encontrou = 1;
            }
        }

        if (!encontrou) {
            printf("Registro inexistente.\n");
        }

        /* Libera memoria */
        for (int c = 0; c < qtdValidacoes; c++) {
            free(validacoes[c].campo);
            free(validacoes[c].valor);
        }
        free(validacoes);

        printf("\n");
    }
}

void removerLogic(FILE *bin, CabecalhoArquivo *cabecalho, RegistroDados *reg, int rrn) {
    reg->removido = REGISTRO_REMOVIDO;

    //atualizar os valores relacionados à pilha dos removidos
    reg->proximo = cabecalho->topo;
    cabecalho->topo = rrn;

    //atualizar o registro no arquivo, não compensa usar escreverRegistro para mudar apenas os campos iniciais
    long offsetInicio = rrnParaOffset(rrn);
    fseek(bin, offsetInicio, SEEK_SET);
    fwrite(&reg->removido, sizeof(char), 1, bin);
    fwrite(&reg->proximo, sizeof(int), 1, bin);

    return;
}