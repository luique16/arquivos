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
    return (no->nroChaves < MINIMO_CHAVES);
}

/* Recebe o indice do filho à esquerda, que será redistribuido com o da sua direita */
void redistribuir(FILE *fp, Pagina *pai, int indiceFilhoEsq) {
    Pagina filhoEsq = lerNo(fp, pai->descendentes[indiceFilhoEsq]);
    Pagina filhoDir = lerNo(fp, pai->descendentes[indiceFilhoEsq + 1]);

    /* Juntar todas as chaves e descendentes em arrays temporarios */
    int totalChaves = filhoEsq.nroChaves + 1 + filhoDir.nroChaves;
    Entrada chaves[totalChaves];
    int totalDesc = totalChaves + 1; /* Um descendente a mais do que nroChaves */
    int descendentes[totalDesc];
    /* Adicionar as chaves do filho da esquerda */
    int i = 0;
    for (i; i < filhoEsq.nroChaves; i++) {
        chaves[i] = filhoEsq.pares[i];
        descendentes[i] = filhoEsq.descendentes[i]; /* Insere o descendente da esquerda */
    }
    /* Adicionar a chave separadora e o último descendente de filhoEsq */
    chaves[i] = pai->pares[indiceFilhoEsq];
    descendentes[i] = filhoEsq.descendentes[i];
    i++;
    /* Adicionar as chaves do filho da direita */
    for (int j = 0; j < filhoDir.nroChaves; i++, j++) {
        chaves[i] = filhoDir.pares[j];
        descendentes[i] = filhoDir.descendentes[j]; /* Insere o descendente da esquerda */
    }
    /* Adicionar o último descendente do filhoDir */
    descendentes[i] = filhoDir.descendentes[filhoDir.nroChaves];

    /* Distruibuir o mais igualmente possível */
    int meio = totalChaves / 2; /* índice do meio, pega o menor da direita */

    /* Reconstruindo filhoEsq com as primeiras nEsq chaves*/
    for (int i = 0; i < meio; i++) {
        filhoEsq.pares[i] = chaves[i];
        filhoEsq.descendentes[i] = descendentes[i];
    }

    /* Último descendente da esquerda e sobe o promovido */
    filhoEsq.descendentes[meio] = descendentes[meio];
    pai->pares[indiceFilhoEsq] = chaves[meio];

    /* Reconstruindo filhoDir com as chaves de meio+1 até a última */
    int nDir = 0;
    for (int i = meio+1; i < totalChaves; i++, nDir++) {
        filhoDir.pares[nDir] = chaves[i];
        filhoDir.descendentes[nDir] = descendentes[i];
    }

    /* Último descendente da direita */
    filhoDir.descendentes[nDir] = descendentes[totalChaves];

    /* Atualizar os nroChaves */
    filhoEsq.nroChaves = meio;
    filhoDir.nroChaves = totalChaves - meio - 1;

    /* Limpar as posições antigas (se um dos filhos ficou menor do que antes) */
    for (int i = filhoDir.nroChaves; i < ORDEM_ARVB - 1; i++) {
        filhoDir.pares[i].chave  = -1;
        filhoDir.pares[i].offset = -1;
        filhoDir.descendentes[i + 1] = NULO_RRN;
    }

    for (int i = filhoEsq.nroChaves; i < ORDEM_ARVB - 1; i++) {
        filhoEsq.pares[i].chave  = -1;
        filhoEsq.pares[i].offset = -1;
        filhoEsq.descendentes[i + 1] = NULO_RRN;
    }

    /* Escrever os filhos no disco, o pai é escrito no chamaor */
    escreverNo(fp, pai->descendentes[indiceFilhoEsq], filhoEsq);
    escreverNo(fp, pai->descendentes[indiceFilhoEsq+1], filhoDir);

    return;
}

void removerLogicPagina(FILE *fp, CabecalhoArvB *cab, int rrn) {
    int removido = REGISTRO_REMOVIDO;

    //atualizar os valores relacionados à pilha dos removidos
    int proximo = cab->topo;
    cab->topo = rrn;

    //atualizar o início da página no arquivo, não compensa usar escreverNo para mudar apenas os campos iniciais
    long offsetInicio = rrnParaOffset(rrn);
    fseek(fp, offsetInicio, SEEK_SET);
    fwrite(&removido, sizeof(char), 1, fp);
    fwrite(&proximo, sizeof(int), 1, fp);

    //atualizar o nroNos no cabeçalho
    cab->nroNos--;

    return;
}

/* Recebe o índice do filho à esquerda, que será concatenado com o da sua direita (o da direita será destruído) */
void concatenarFilhos(FILE *fp, CabecalhoArvB *cab, Pagina *pai, int indiceFilhoEsq) {
    int rrnEsq = pai->descendentes[indiceFilhoEsq];
    int rrnDir = pai->descendentes[indiceFilhoEsq+1];
    Pagina filhoEsq = lerNo(fp, rrnEsq);
    Pagina filhoDir = lerNo(fp, rrnDir);

    int i = filhoEsq.nroChaves;

    /* Descer o separador do pai para o filho da esquerda */
    filhoEsq.pares[i] = pai->pares[indiceFilhoEsq];
    i++;

    /* Copiar chaves e descendentes de filhoDir para filhoEsq */
    for (int j = 0; j < filhoDir.nroChaves; j++, i++) {
        filhoEsq.pares[i] = filhoDir.pares[j];
        filhoEsq.descendentes[i] = filhoDir.descendentes[j]; /* Copia também o descendente da esquerda do atual*/
    }
    /* Último descendente do filhoDir */
    filhoEsq.descendentes[i] = filhoDir.descendentes[filhoDir.nroChaves];

    filhoEsq.nroChaves = i; /* Atualizar o nroChaves */

    /* Remover (por shiftada para esquerda) o separador do pai e o ponteiro para filhoDir */
    for (int j = indiceFilhoEsq; j < pai->nroChaves - 1; j++) {
        pai->pares[j] = pai->pares[j+1];
        pai->descendentes[j+1] = pai->descendentes[j+2];
    }
    /* Limpar os valores que ficaram no final */
    pai->pares[pai->nroChaves - 1].chave   = -1;
    pai->pares[pai->nroChaves - 1].offset  = -1;
    pai->descendentes[pai->nroChaves]      = NULO_RRN;
    pai->nroChaves--;

    /* Remover logicamente o nó do filhoDir */
    removerLogicPagina(fp, cab, rrnDir);

    /* Escrever o novo nó de filhoEsq, o pai é escrito pelo chamador */
    escreverNo(fp, pai->descendentes[indiceFilhoEsq], filhoEsq);
}

int tratarUnderflow(FILE *fp, CabecalhoArvB *cab, Pagina *pai, int indiceFilhoComUnderflow) {
    fprintf(stderr, "tratarUnderflow: indiceFilho=%d pai->nroChaves=%d\n", indiceFilhoComUnderflow, pai->nroChaves);
    /* Tentar redistribuir pelo irmão da esquerda */
    if (indiceFilhoComUnderflow > 0) {
        /* Ler só o nroChaves do irmão da esquerda(para não precisar carregar a página inteira ainda) */
        int nroChavesIrmEsq;
        fseek(fp, rrnParaOffset(pai->descendentes[indiceFilhoComUnderflow-1]) /* início da página */ + 1*sizeof(char) /* removido */ + 2*sizeof(int) /* proximo, tipoNo */, SEEK_SET);
        fread(&nroChavesIrmEsq, sizeof(int), 1, fp);

        /* Se tem uma chave sobrando */
        if (nroChavesIrmEsq >= MINIMO_CHAVES + 1) {
            redistribuir(fp, pai, indiceFilhoComUnderflow - 1);
            return REM_OK;
        }
    }

    /* Tentar redistribuir pelo irmão da direita */
    if (indiceFilhoComUnderflow < pai->nroChaves) {
        /* Ler só o nroChaves do irmão da esquerda(para não precisar carregar a página inteira ainda) */
        int nroChavesIrmDir;
        fseek(fp, rrnParaOffset(pai->descendentes[indiceFilhoComUnderflow]) /* início da página */ + 1*sizeof(char) /* removido */ + 2*sizeof(int) /* proximo, tipoNo */, SEEK_SET);
        fread(&nroChavesIrmDir, sizeof(int), 1, fp);

        /* Se tem uma chave sobrando */
        if (nroChavesIrmDir >= MINIMO_CHAVES + 1) {
            redistribuir(fp, pai, indiceFilhoComUnderflow);
            return REM_OK;
        }
    }

    /* Não redistribuiu: aplica concatenação */
    if (indiceFilhoComUnderflow > 0) {
        /* concatena com o irmão da esquerda */
        concatenarFilhos(fp, cab, pai, indiceFilhoComUnderflow - 1);
    } else {
        /* concatena com o irmão da direita */
        concatenarFilhos(fp, cab, pai, indiceFilhoComUnderflow);
    }

    /* checar se o pai perdeu uma chave na concatenação e subir na recursão */
    return checkUnderflow(pai) ? REM_UNDERFLOW : REM_OK;
}

/* Função interna recursiva, trabalha a partir de um RRN */
int removerRec(FILE *fp, CabecalhoArvB *cab, int rrnAtual, int alvo) {
    fprintf(stderr, "removerRec: rrnAtual=%d alvo=%d\n", rrnAtual, alvo);
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
        int rrnRaizAntiga = cab->noRaiz;
        if (raiz.nroChaves == 0 && raiz.tipoNo != NO_FOLHA) {
            cab->noRaiz = raiz.descendentes[0];
        }
        if (rrnRaizAntiga != NULO_RRN)
            removerLogicPagina(fp, cab, rrnRaizAntiga);
    }

    return (resultado != REM_NAO_ENCONTRADO);
}