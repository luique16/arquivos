#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

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
    atualizarCabecalho(fpSaida, &cabSaida); /* Atualiza somente na memória */
    escreverCabecalho(fpSaida, &cabSaida);
    fecharArquivoBin(fpSaida);

    free(vetor);
    BinarioNaTela(nomeArquivoSaida);
}
