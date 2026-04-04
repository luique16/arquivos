/**
 * @file entradaSaida.c
 * @brief Implementação das funções de leitura e escrita do arquivo binário.
 */

#include "../include/io.h"
#include "../include/definicoes.h"

#include <stdio.h>

long rrnParaOffset(int rrn) {
    return (long)TAMANHO_CABECALHO + (long)rrn * TAMANHO_REGISTRO;
}

FILE *abrirArquivoBin(const char *nomeArquivo, const char *modo) {
    FILE *bin = fopen(nomeArquivo, modo);
    if (bin == NULL) {
        return NULL;
    }

    return bin;
}

void fecharArquivoBin(FILE *bin) {
    if (bin == NULL) return;

    /* Grava status consistente antes de fechar */
    char statusConsistente = STATUS_CONSISTENTE;
    fseek(bin, 0, SEEK_SET);
    fwrite(&statusConsistente, sizeof(char), 1, bin);
    fclose(bin);
}

void marcarInconsistente(FILE *bin) {
    if (bin == NULL) return;

    char statusInconsistente = STATUS_INCONSISTENTE;
    fseek(bin, 0, SEEK_SET);
    fwrite(&statusInconsistente, sizeof(char), 1, bin);
    fflush(bin);
}

void escreverCabecalho(FILE *bin, CabecalhoArquivo *cabecalho) {
    fseek(bin, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, bin);
    fwrite(&cabecalho->topo, sizeof(int),  1, bin);
    fwrite(&cabecalho->proxRRN, sizeof(int),  1, bin);
    fwrite(&cabecalho->nroEstacoes, sizeof(int),  1, bin);
    fwrite(&cabecalho->nroParesEstacao, sizeof(int),  1, bin);
}

void lerCabecalho(FILE *bin, CabecalhoArquivo *cabecalho) {
    fseek(bin, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, bin);
    fread(&cabecalho->topo, sizeof(int),  1, bin);
    fread(&cabecalho->proxRRN, sizeof(int),  1, bin);
    fread(&cabecalho->nroEstacoes, sizeof(int),  1, bin);
    fread(&cabecalho->nroParesEstacao, sizeof(int),  1, bin);
}

void escreverRegistro(FILE *bin, RegistroDados *reg, int rrn) {
    long offsetInicio = rrnParaOffset(rrn);
    fseek(bin, offsetInicio, SEEK_SET);

    /* Campos fixos — escritos campo a campo */
    fwrite(&reg->removido, sizeof(char), 1, bin);
    fwrite(&reg->proximo, sizeof(int), 1, bin);
    fwrite(&reg->codEstacao, sizeof(int), 1, bin);
    fwrite(&reg->codLinha, sizeof(int), 1, bin);
    fwrite(&reg->codProxEstacao, sizeof(int), 1, bin);
    fwrite(&reg->distProxEstacao, sizeof(int), 1, bin);
    fwrite(&reg->codLinhaIntegra, sizeof(int), 1, bin);
    fwrite(&reg->codEstIntegra, sizeof(int), 1, bin);

    /* Campo variável: nomeEstacao */
    fwrite(&reg->tamNomeEstacao,  sizeof(int), 1, bin);
    if (reg->tamNomeEstacao > 0) {
        fwrite(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, bin);
    }

    /* Campo variável: nomeLinha */
    fwrite(&reg->tamNomeLinha,    sizeof(int),  1, bin);
    if (reg->tamNomeLinha > 0) {
        fwrite(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, bin);
    }

    /* Preenche bytes restantes com LIXO até completar TAMANHO_REGISTRO */
    long posicaoAtual = ftell(bin);
    int  bytesEscritos = (int)(posicaoAtual - offsetInicio);
    int  bytesLixo = TAMANHO_REGISTRO - bytesEscritos;

    char caractereLixo = LIXO;
    for (int i = 0; i < bytesLixo; i++) {
        fwrite(&caractereLixo, sizeof(char), 1, bin);
    }
}

void lerRegistro(FILE *bin, RegistroDados *reg, int rrn) {
    fseek(bin, rrnParaOffset(rrn), SEEK_SET);

    /* Campos fixos */
    fread(&reg->removido, sizeof(char), 1, bin);
    fread(&reg->proximo, sizeof(int), 1, bin);
    fread(&reg->codEstacao, sizeof(int), 1, bin);
    fread(&reg->codLinha, sizeof(int), 1, bin);
    fread(&reg->codProxEstacao, sizeof(int), 1, bin);
    fread(&reg->distProxEstacao, sizeof(int), 1, bin);
    fread(&reg->codLinhaIntegra, sizeof(int), 1, bin);
    fread(&reg->codEstIntegra, sizeof(int), 1, bin);

    /* Campo variável: nomeEstacao */
    fread(&reg->tamNomeEstacao, sizeof(int), 1, bin);
    if (reg->tamNomeEstacao > 0) {
        fread(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, bin);
    }
    reg->nomeEstacao[reg->tamNomeEstacao] = '\0'; /* garante terminador */

    /* Campo variável: nomeLinha */
    fread(&reg->tamNomeLinha, sizeof(int), 1, bin);
    if (reg->tamNomeLinha > 0) {
        fread(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, bin);
    }
    reg->nomeLinha[reg->tamNomeLinha] = '\0'; /* garante terminador */
}

void inicializarRegistro(RegistroDados *reg) {
    reg->removido = REGISTRO_ATIVO;
    reg->proximo = INTEIRO_NULO;
    reg->codEstacao = INTEIRO_NULO;
    reg->codLinha = INTEIRO_NULO;
    reg->codProxEstacao = INTEIRO_NULO;
    reg->distProxEstacao = INTEIRO_NULO;
    reg->codLinhaIntegra = INTEIRO_NULO;
    reg->codEstIntegra = INTEIRO_NULO;
    reg->tamNomeEstacao = 0;
    reg->nomeEstacao[0] = '\0';
    reg->tamNomeLinha = 0;
    reg->nomeLinha[0] = '\0';
}
