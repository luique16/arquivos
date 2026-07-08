#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

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
