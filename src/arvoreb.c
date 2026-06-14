#include "../include/arvoreb.h"
#include "../include/io.h"
#include "../include/definicoes.h"
#include <stdio.h>
#include <stdlib.h>

Pagina noVazio(void) {
    Pagina no;
    no.removido  = '0';
    no.proximo   = NULO_RRN;
    no.tipoNo    = NO_FOLHA;   /* nó folha por padrão */
    no.nroChaves = 0;
    for (int i = 0; i < MAX_CHAVES; i++) {
        no.pares[i].chave  = NULO_RRN;
        no.pares[i].offset = NULO_RRN;
    }
    for (int i = 0; i < MAX_FILHOS; i++) {
        no.descendentes[i] = NULO_RRN;
    }
    return no;                 /* retorna nó zerado */
}

CabecalhoArvB cabecalhoArvoreVazio(void) {
    CabecalhoArvB cab;
    cab.status  = '0';          /* marcado inconsistente */
    cab.noRaiz  = NULO_RRN;     /* árvore vazia */
    cab.topo    = NULO_RRN;     /* pilha vazia */
    cab.proxRRN = 0;
    cab.nroNos  = 0;
    return cab;                 /* cabeçalho inicial */
}

int alocarNo(FILE *fp, CabecalhoArvB *cab) {
    if (cab->topo != NULO_RRN) {  /* reaproveita da pilha de nós removidos */
        int rrn = cab->topo;
        Pagina no = lerNo(fp, rrn);
        cab->topo = no.proximo;   /* avança topo da pilha */
        return rrn;
    }
    int rrn = cab->proxRRN;       /* pilha vazia: usa próximo disponível */
    cab->proxRRN++;
    return rrn;
}

int buscarNo(FILE *fp, int rrnAtual, int chave, long *offsetEncontrado) {
    if (rrnAtual == NULO_RRN) return 0;  /* não encontrado */

    Pagina no = lerNo(fp, rrnAtual);

    for (int i = 0; i < no.nroChaves; i++) {
        if (chave == no.pares[i].chave) {
            *offsetEncontrado = no.pares[i].offset;
            return 1;                    /* chave encontrada neste nó */
        }
        if (chave < no.pares[i].chave) {
            return buscarNo(fp, no.descendentes[i], chave, offsetEncontrado);  /* desce pelo filho esquerdo */
        }
    }
    /* chave maior que todas: desce pelo último filho */
    return buscarNo(fp, no.descendentes[no.nroChaves], chave, offsetEncontrado);
}

int buscarNaArvore(FILE *fp, CabecalhoArvB cab, int chave, long *offsetEncontrado) {
    return buscarNo(fp, cab.noRaiz, chave, offsetEncontrado);
}

void encontrarSucessora(FILE *fp, int rrnFilhoDir, int *chaveSuc, int *prSuc, int *rrnFolhaSuc) {
    int rrnAtual = rrnFilhoDir;
    while (1) {
        Pagina no = lerNo(fp, rrnAtual);
        if (no.descendentes[0] == NULO_RRN) {  /* folha: primeiro par é o sucessor */
            *chaveSuc    = no.pares[0].chave;
            *prSuc       = no.pares[0].offset;
            *rrnFolhaSuc = rrnAtual;
            return;
        }
        rrnAtual = no.descendentes[0];          /* desce sempre à esquerda */
    }
}

void inserirNaoPropagado(Pagina *no, int chave, int pr, int rrnFilhoDir) {
    int i = no->nroChaves - 1;
    while (i >= 0 && chave < no->pares[i].chave) {  /* desloca para abrir espaço */
        no->pares[i+1].chave  = no->pares[i].chave;
        no->pares[i+1].offset = no->pares[i].offset;
        no->descendentes[i+2]  = no->descendentes[i+1];
        i--;
    }
    no->pares[i+1].chave  = chave;   /* insere na posição correta */
    no->pares[i+1].offset = pr;
    no->descendentes[i+2]  = rrnFilhoDir;
    no->nroChaves++;
}

ResultadoSplit splitNo(FILE *fp, CabecalhoArvB *cab,
                       int rrnEsq, Pagina noEsq,
                       int novaChave, int novoPR, int rrnFilhoDirDaNovaChave) {
    int tempC[MAX_CHAVES + 1];   /* vetor temporário de chaves */
    int tempPR[MAX_CHAVES + 1];  /* vetor temporário de offsets (PR) */
    int tempP[MAX_FILHOS + 1];   /* vetor temporário de descendentes */

    /* insere nova chave no vetor temporário mantendo ordenação */
    int pos = noEsq.nroChaves;
    for (int i = 0; i < noEsq.nroChaves; i++) {
        if (novaChave < noEsq.pares[i].chave) { pos = i; break; }
    }

    /* copia pares e descendentes antes de pos */
    for (int i = 0; i < pos; i++) {
        tempC[i]  = noEsq.pares[i].chave;
        tempPR[i] = noEsq.pares[i].offset;
        tempP[i]  = noEsq.descendentes[i];
    }
    tempC[pos]  = novaChave;           /* insere nova chave na posição */
    tempPR[pos] = novoPR;
    tempP[pos]  = noEsq.descendentes[pos];
    tempP[pos+1] = rrnFilhoDirDaNovaChave;
    for (int i = pos + 1; i <= noEsq.nroChaves; i++) {
        tempC[i]  = noEsq.pares[i-1].chave;
        tempPR[i] = noEsq.pares[i-1].offset;
        tempP[i+1] = noEsq.descendentes[i];
    }

    int idxSobe = (MAX_CHAVES + 1) / 2;  /* chave do meio sobe */

    /* metade esquerda do split */
    noEsq.nroChaves = idxSobe;
    for (int i = 0; i < idxSobe; i++) {
        noEsq.pares[i].chave  = tempC[i];
        noEsq.pares[i].offset = tempPR[i];
        noEsq.descendentes[i]  = tempP[i];
    }
    noEsq.descendentes[idxSobe] = tempP[idxSobe];
    for (int i = idxSobe; i < MAX_CHAVES; i++) {
        noEsq.pares[i].chave  = NULO_RRN;
        noEsq.pares[i].offset = NULO_RRN;
    }
    for (int i = idxSobe + 1; i < MAX_FILHOS; i++) {
        noEsq.descendentes[i] = NULO_RRN;
    }

    /* metade direita do split */
    Pagina noDir = noVazio();
    int qtdDir = (MAX_CHAVES + 1) - idxSobe - 1;
    noDir.nroChaves = qtdDir;
    for (int i = 0; i < qtdDir; i++) {
        noDir.pares[i].chave  = tempC[idxSobe + 1 + i];
        noDir.pares[i].offset = tempPR[idxSobe + 1 + i];
        noDir.descendentes[i]  = tempP[idxSobe + 1 + i];
    }
    noDir.descendentes[qtdDir] = tempP[idxSobe + 1 + qtdDir];
    if (noEsq.tipoNo == NO_RAIZ) {          /* raiz deixa de ser raiz */
        noEsq.tipoNo = NO_INTERMEDIARIO;
    }
    noDir.tipoNo = noEsq.tipoNo;            /* herda tipo do original */

    int rrnDir = alocarNo(fp, cab);
    cab->nroNos++;
    escreverNo(fp, rrnEsq, noEsq);
    escreverNo(fp, rrnDir, noDir);

    ResultadoSplit res;
    res.houveSplit = 1;
    res.chaveSobe  = tempC[idxSobe];
    res.prSobe     = tempPR[idxSobe];
    res.rrnDireito = rrnDir;
    return res;
}

ResultadoSplit inserirRecursivo(FILE *fp, CabecalhoArvB *cab, int rrnAtual, int chave, int pr) {
    ResultadoSplit res;
    res.houveSplit = 0;                     /* sem split por padrão */

    if (rrnAtual == NULO_RRN) {             /* chegou na folha: chave sobe */
        res.houveSplit = 1;
        res.chaveSobe  = chave;
        res.prSobe     = pr;
        res.rrnDireito = NULO_RRN;
        return res;
    }

    Pagina no = lerNo(fp, rrnAtual);

    /* encontra por onde descer na árvore */
    int pos = no.nroChaves;
    for (int i = 0; i < no.nroChaves; i++) {
        if (chave == no.pares[i].chave) {
            return res;                     /* duplicata: não insere */
        }
        if (chave < no.pares[i].chave) {
            pos = i;
            break;
        }
    }

    ResultadoSplit resFilho = inserirRecursivo(fp, cab, no.descendentes[pos], chave, pr);
    if (!resFilho.houveSplit) return resFilho;  /* sem promoção do filho */

    if (no.nroChaves < MAX_CHAVES) {            /* cabe no nó atual */
        inserirNaoPropagado(&no, resFilho.chaveSobe, resFilho.prSobe, resFilho.rrnDireito);
        escreverNo(fp, rrnAtual, no);
        res.houveSplit = 0;
        return res;
    }

    return splitNo(fp, cab, rrnAtual, no,       /* nó cheio: precisa split */
                   resFilho.chaveSobe, resFilho.prSobe, resFilho.rrnDireito);
}

void inserirNaArvore(FILE *fp, CabecalhoArvB *cab, int chave, int pr) {
    if (cab->noRaiz == NULO_RRN) {           /* primeira inserção: cria raiz */
        Pagina noRaiz = noVazio();
        noRaiz.tipoNo        = NO_FOLHA;
        noRaiz.pares[0].chave  = chave;
        noRaiz.pares[0].offset = pr;
        noRaiz.nroChaves     = 1;

        int rrn = alocarNo(fp, cab);
        cab->noRaiz = rrn;
        cab->nroNos++;
        escreverNo(fp, rrn, noRaiz);
        escreverCabecalhoArvB(fp, *cab);
        return;
    }

    ResultadoSplit res = inserirRecursivo(fp, cab, cab->noRaiz, chave, pr);

    if (res.houveSplit) {                    /* raiz antiga dividida: criar nova */
        Pagina novaRaiz = noVazio();
        novaRaiz.tipoNo        = NO_RAIZ;
        novaRaiz.nroChaves     = 1;
        novaRaiz.pares[0].chave  = res.chaveSobe;
        novaRaiz.pares[0].offset = res.prSobe;
        novaRaiz.descendentes[0] = cab->noRaiz;  /* filho esquerdo: raiz antiga */
        novaRaiz.descendentes[1] = res.rrnDireito; /* filho direito: novo nó do split */

        int rrnNovaRaiz = alocarNo(fp, cab);
        cab->noRaiz = rrnNovaRaiz;
        cab->nroNos++;
        escreverNo(fp, rrnNovaRaiz, novaRaiz);
    }

    escreverCabecalhoArvB(fp, *cab);         /* atualiza cabeçalho no disco */
}

void removerDaFolha(Pagina *pagina, int pos) {
    // shift para a esquerda a partir de pos
    for (int i = pos; i + 1 < pagina->nroChaves; i++) {
        pagina->pares[i] = pagina->pares[i+1];
    }
    // limpar a última posição que ficou duplicada
    pagina->pares[pagina->nroChaves - 1].chave = -1;
    pagina->pares[pagina->nroChaves - 1].offset = -1;
    pagina->nroChaves--;
}

bool checkUnderflow(Pagina *no) {
    return (no->nroChaves < (ORDEM_ARVB / 2) - 1);
}

int tratarUnderflow(FILE *fp, CabecalhoArvB *cab, Pagina *pagina, int indiceFilhoComUnderflow) {
    // A fazer
}

/* Função interna recursiva, trabalha a partir de um RRN */
int removerRec(FILE *fp, CabecalhoArvB *cab, int rrnAtual, int alvo) {
    if (rrnAtual == NULO_RRN) return REM_NAO_ENCONTRADO;

    Pagina pagina = lerNo(fp, rrnAtual);

    /* Busca linear pela chave na página atual */
    int posEncontrado = -1;
    int idFilhoASeguir = pagina.nroChaves; /* assume maior que todos (último filho) */
    for (int i = 0; i < pagina.nroChaves; i++) {
        if (pagina.pares[i].chave == alvo) {
            posEncontrado = i;
            break;
        }
        if (alvo < pagina.pares[i].chave) {
            idFilhoASeguir = i;
            break;
        }
    }

    int resultado;

    if (posEncontrado >= 0) {
        /* Chave encontrada nesta página */
        if (pagina.tipoNo == NO_FOLHA) {
            /* Caso simples: remove direto da folha */
            removerDaFolha(&pagina, posEncontrado);
            escreverNo(fp, rrnAtual, pagina);
            return checkUnderflow(&pagina) ? REM_UNDERFLOW : REM_OK;
        } else {
            /* Nó interno: substitui pelo sucessor imediato */
            Entrada valoresSuc;
            int rrnFolhaSuc;
            encontrarSucessora(fp, pagina.descendentes[posEncontrado + 1], &valoresSuc.chave, &valoresSuc.offset, &rrnFolhaSuc);

            /* Remove o sucessor recursivamente */
            resultado = removerRec(fp, cab, pagina.descendentes[posEncontrado + 1], valoresSuc.chave);

            /* Recarrega a página pois a recursão pode ter alterado o disco */
            pagina = lerNo(fp, rrnAtual);
            pagina.pares[posEncontrado] = valoresSuc; /* Atualiza o valor do alvo pelo do sucessor que foi removido (essencialmente a mesma coisa que trocar os dois e remove o alvo) */

            /* Trata underflow no filho à direita do separador, se necessário */
            if (resultado == REM_UNDERFLOW) {
                resultado = tratarUnderflow(fp, cab, &pagina, posEncontrado + 1);
            }

            escreverNo(fp, rrnAtual, pagina);
            return checkUnderflow(&pagina) ? REM_UNDERFLOW : REM_OK;
        }
    } else {
        /* Chave não encontrada aqui, desce para o filho adequado */
        resultado = removerRec(fp, cab, pagina.descendentes[idFilhoASeguir], alvo);

        if (resultado == REM_UNDERFLOW) {
            /* Recarrega a página pois a recursão pode ter alterado o disco */
            pagina = lerNo(fp, rrnAtual);
            resultado = tratarUnderflow(fp, cab, &pagina, idFilhoASeguir);
            escreverNo(fp, rrnAtual, pagina);
        }

        return resultado;
    }
}

/* Função principal, ponto de entrada */
int removerDaArvore(FILE *fp, CabecalhoArvB *cab, int alvo) {
    int resultado = removerRec(fp, cab, cab->noRaiz, alvo);

    /* Caso especial: raiz ficou vazia após fusão */
    if (resultado == REM_UNDERFLOW) {
        Pagina raiz = lerNo(fp, cab->noRaiz);
        if (raiz.nroChaves == 0 && raiz.tipoNo != NO_FOLHA) {
            cab->noRaiz = raiz.descendentes[0];
        }
    }

    return (resultado != REM_NAO_ENCONTRADO);
}