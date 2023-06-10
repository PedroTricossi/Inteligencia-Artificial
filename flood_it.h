#ifndef __FLOOD_IT__
#define __FLOOD_IT__

typedef struct 
{
    int flood;
    int cor;
}t_cell;


typedef struct 
{
    int lin;
    int col;
    int cor;
    t_cell **tab;
}t_tabuleiro;


t_tabuleiro le_tabuleiro();

int verifica_borda(t_tabuleiro *tab, int *x, int *y);

void printa_tabuleiro(t_tabuleiro *tab);

void pinta_tabuleiro(t_tabuleiro *tab, int x, int y, int *new_color);

void printa_tabuleiro(t_tabuleiro *tab);

void pinta_flood(t_tabuleiro *tab, int *new_color);
#endif