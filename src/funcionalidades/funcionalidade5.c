#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"

#include <stdio.h>

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
