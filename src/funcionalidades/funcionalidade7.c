#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"
#include "../../include/arvoreb.h"

#include <stdio.h>

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
