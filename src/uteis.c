/**
 * @file uteis.c
 * @brief Implementações das funções auxiliares.
 */

#include "../include/uteis.h"
#include "../include/definicoes.h"
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
