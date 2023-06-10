#include <stdio.h>
#include <stdlib.h>
#include "flood_it.h"
#include "ia.h"

int main(){
    t_tabuleiro tab;
    int cor;
    tab = le_tabuleiro();

    cor = 10;
    pinta_flood(&tab, &cor);
    pinta_tabuleiro(&tab, 0, 0, &cor);

    cor = 7;
    pinta_flood(&tab, &cor);
    pinta_tabuleiro(&tab, 0, 0, &cor);

    cor = 3;
    pinta_flood(&tab, &cor);
    pinta_tabuleiro(&tab, 0, 0, &cor);

    cor = 6;
    pinta_flood(&tab, &cor);
    pinta_tabuleiro(&tab, 0, 0, &cor);

    printa_tabuleiro(&tab);


    return 0;
}