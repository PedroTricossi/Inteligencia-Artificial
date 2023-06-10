#include <stdio.h>
#include <stdlib.h>
#include "flood_it.h"
#include "ia.h"

int main(){
    t_tabuleiro tab;
    int cor, x, y, i;
    int quadrados_faltantes;
    t_jogada melhor_jogada;
    tab = le_tabuleiro();

    // printa_tabuleiro(&tab);

    quadrados_faltantes = (tab.col * tab.lin) - 1;
    i = 0;

    while(i < 5){

        pinta_flood(&tab, &melhor_jogada.cor);

        fprintf(stdout, "MELHOR COR: %d -- MELHOR QUINA: %c \n", melhor_jogada.cor, melhor_jogada.quina);

        pinta_tabuleiro(&tab, x, y, &melhor_jogada.cor);
        i++;
        quadrados_faltantes = melhor_jogada.quadrado;
    }
    
    printa_tabuleiro(&tab);

    return 0;
}