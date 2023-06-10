#ifndef __IA__
#define __IA__
#include "flood_it.h"

typedef struct
{
    char quina;
    int cor;
    int quadrado;
}t_jogada;


t_jogada calcula_melhor_jogada(t_tabuleiro *tab, int *num_restante);

#endif