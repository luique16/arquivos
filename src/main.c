/**
 * @file main.c
 * @brief Ponto de entrada do programa programaTrab.
 *
 * Lê o identificador da funcionalidade da entrada e chama a função correspondente.
 */

#include "../include/funcionalidades.h"

#include <stdio.h>

int main(void) {
    int funcionalidade;

    /* A primeira entrada é sempre o identificador da funcionalidade */
    scanf("%d", &funcionalidade);

    switch (funcionalidade) {
        case 1:
            funcionalidade1();
            break;
        case 2:
            funcionalidade2();
            break;
        case 3:
            funcionalidade3();
            break;
        default:
            fprintf(stderr, "Falha no processamento do arquivo.\n");
            return 1;
    }

    return 0;
}
