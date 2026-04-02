/**
 * @file csv.c
 * @brief Implementação da leitura do arquivo CSV de entrada.
 */

#include "../include/csv.h"
#include <stdio.h>

/**
 * @brief Lê um campo do CSV até encontrar vírgula, newline ou EOF.
 *
 * @param fp      Arquivo CSV aberto para leitura.
 * @param destino Buffer que receberá o campo lido.
 * @param tamanho Tamanho máximo do buffer.
 * @return O caractere delimitador encontrado (',' '\n' ou EOF).
 */
int lerCampoCSV(FILE *fp, char *destino, int tamanho) {
    int i = 0;
    int c;

    while ((c = fgetc(fp)) != ',' && c != '\n' && c != EOF) {
        if (i < tamanho - 1) {
            destino[i++] = (char)c;
        }
    }
    destino[i] = '\0';

    return c;
}

/**
 * @brief Converte um campo lido do CSV para inteiro.
 *
 * Se o campo estiver vazio (campo nulo no CSV: ,,) retorna NULO_INTEIRO.
 *
 * @param campo String lida do CSV.
 * @return Valor inteiro ou NULO_INTEIRO se vazio.
 */
int campoParaInteiro(char *campo) {
    int inteiro = atoi(campo);

    if (inteiro == 0 && campo[0] != '0') {
        return INTEIRO_NULO;
    }

    return inteiro;
}

int lerRegistroCSV(FILE *csv, RegistroDados *reg) {
    char buffer[TAMANHO_MAX_NOME + 4];
    
    /* Verifica se é fim de arquivo */
    if (feof(csv)) return 0;

    /* Campo 1: codEstacao */
    int delimitador = lerCampoCSV(csv, buffer, sizeof(buffer));
    if (delimitador == EOF && buffer[0] == '\0') return 0;
    reg->codEstacao = campoParaInteiro(buffer);

    /* Campo 2: nomeEstacao */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->tamNomeEstacao = (int)strlen(buffer);
    strncpy(reg->nomeEstacao, buffer, TAMANHO_MAX_NOME);

    /* Campo 3: codLinha */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->codLinha = campoParaInteiro(buffer);

    /* Campo 4: nomeLinha */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->tamNomeLinha = (int)strlen(buffer);
    strncpy(reg->nomeLinha, buffer, TAMANHO_MAX_NOME);

    /* Campo 5: codProxEstacao */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->codProxEstacao = campoParaInteiro(buffer);

    /* Campo 6: distProxEstacao */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->distProxEstacao = campoParaInteiro(buffer);

    /* Campo 7: codLinhaIntegra */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->codLinhaIntegra = campoParaInteiro(buffer);

    /* Campo 8: codEstIntegra (último da linha) */
    lerCampoCSV(csv, buffer, sizeof(buffer));
    reg->codEstIntegra = campoParaInteiro(buffer);

    return 1;
}
