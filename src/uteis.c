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

        if (acao == atualizarRegistro) {

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

        if (acao == acaoImprimir) {
            printf("\n");
        }
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

void atualizarCabecalho(FILE *bin, CabecalhoArquivo *cabecalho) {
    int total = cabecalho->proxRRN;
 
    char **nomesEstacoes = malloc(sizeof(char *) * total);
    int  *parEstacaoA    = malloc(sizeof(int)    * total);
    int  *parEstacaoB    = malloc(sizeof(int)    * total);
 
    /* Lê todos os registros, ignorando removidos */
    RegistroDados reg;
    for (int i = 0; i < total; i++) {
        lerRegistro(bin, &reg, i);
 
        if (reg.removido == REGISTRO_REMOVIDO) {
            nomesEstacoes[i] = NULL;
            parEstacaoA[i] = INTEIRO_NULO;
            parEstacaoB[i] = INTEIRO_NULO;
            continue;
        }
 
        nomesEstacoes[i] = reg.tamNomeEstacao > 0 ? strdup(reg.nomeEstacao) : NULL;
        parEstacaoA[i] = reg.codEstacao;
        parEstacaoB[i] = reg.codProxEstacao;
    }
 
    /* Conta estações distintas por nome */
    int contadorEstacoes = 0;
    for (int i = 0; i < total; i++) {
        if (nomesEstacoes[i] == NULL) continue;
    
        contadorEstacoes++;
    
        for (int j = i + 1; j < total; j++) {
            if (nomesEstacoes[j] != NULL &&
                strcmp(nomesEstacoes[i], nomesEstacoes[j]) == 0) {
                free(nomesEstacoes[j]);
                nomesEstacoes[j] = NULL;
            }
        }
    }
 
    /* Conta pares (A,B) distintos — (A,B) e (B,A) são o mesmo par */
    int contadorPares = 0;
    for (int i = 0; i < total; i++) {
        if (parEstacaoA[i] == INTEIRO_NULO || parEstacaoB[i] == INTEIRO_NULO) continue;
 
        contadorPares++;
 
        for (int j = i + 1; j < total; j++) {
            if (parEstacaoA[j] == INTEIRO_NULO || parEstacaoB[j] == INTEIRO_NULO) continue;
 
            if ((parEstacaoA[i] == parEstacaoA[j] && parEstacaoB[i] == parEstacaoB[j]) ||
                (parEstacaoA[i] == parEstacaoB[j] && parEstacaoB[i] == parEstacaoA[j])) {
                parEstacaoA[j] = INTEIRO_NULO;
                parEstacaoB[j] = INTEIRO_NULO;
            }
        }
    }
 
    /* Limpeza */
    for (int i = 0; i < total; i++) free(nomesEstacoes[i]);
    free(nomesEstacoes);
    free(parEstacaoA);
    free(parEstacaoB);
 
    cabecalho->nroEstacoes = contadorEstacoes;
    cabecalho->nroParesEstacao = contadorPares;
}

void coletarRRNs(int *vet, int tam, int rrn) {
    
}

Validacao *getAtualizacoes(int qtdCampos) {
    Validacao *atualizacoes = malloc(qtdCampos * sizeof(Validacao)); // Vetor que guarda os novos pares {campo, valor}

    for (int c = 0; c < qtdCampos; c++) {
        atualizacoes[c].campo = malloc(sizeof(char)*TAMANHO_MAX_NOME);
        atualizacoes[c].valor = malloc(sizeof(char)*TAMANHO_MAX_NOME);
        scanf("%s", atualizacoes[c].campo);

        if (strcmp(atualizacoes[c].campo, "nomeEstacao") == 0
        ||  strcmp(atualizacoes[c].campo, "nomeLinha") == 0) {
            ScanQuoteString(atualizacoes[c].valor);
        } else {
            scanf("%s", atualizacoes[c].valor);
        }
    }

    return atualizacoes;
}

// Função auxiliar de atualizarRegistro que atualiza os valores dos campos de um único registro
void atualizarCampos(RegistroDados *reg, Validacao *atualizacoes, int qtdCampos) {
    for (int i = 0; i < qtdCampos; i++) {
        if (strcmp(atualizacoes[i].campo, "codEstacao") == 0) {
            reg->codEstacao = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "codLinha") == 0) {
            reg->codLinha = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "codProxEstacao") == 0) {
            reg->codProxEstacao = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "distProxEstacao") == 0) {
            reg->distProxEstacao = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "codLinhaIntegra") == 0) {
            reg->codLinhaIntegra = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "codEstIntegra") == 0) {
            reg->codEstIntegra = stringNulavelParaInteiro(atualizacoes[i].valor);
        } else if (strcmp(atualizacoes[i].campo, "nomeEstacao") == 0) {
            strcpy(reg->nomeEstacao, atualizacoes[i].valor);
            reg->tamNomeEstacao = strlen(reg->nomeEstacao);
        } else if (strcmp(atualizacoes[i].campo, "nomeLinha") == 0) {
            strcpy(reg->nomeLinha, atualizacoes[i].valor);
            reg->tamNomeLinha = strlen(reg->nomeLinha);
        }
    }
}

void atualizarRegistro(FILE *bin, RegistroDados *reg, Validacao *atualizacoes, int rrn) {
    int qtdCampos;
    scanf("%d");

    getAtualizacoes(qtdCampos);
    
    atualizarCampos(reg, atualizacoes, qtdCampos);
    escreverRegistro(bin, reg, rrn); // Reescreve no arquivo o registro, que agora tem os valores atualizados

    // Liberar a memória
    for (int c = 0; c < qtdCampos; c++) {
        free(atualizacoes[c].campo);
        free(atualizacoes[c].valor);
    }

    free(atualizacoes);
}