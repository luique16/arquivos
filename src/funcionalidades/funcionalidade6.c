#include "../../include/funcionalidades.h"
#include "../../include/io.h"
#include "../../include/definicoes.h"
#include "../../include/csv.h"
#include "../../include/fornecidas.h"
#include "../../include/uteis.h"

#include <stdio.h>

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