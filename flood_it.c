#include <stdio.h>
#include <stdlib.h>
#include "flood_it.h"

// Cria um tabuleiro de flood-it usando como base as informacoes do enunciado do trabalho
t_tabuleiro le_tabuleiro(){
    char filename[1024];
    int i, j;
    t_tabuleiro tab;
    FILE* tabuleiro;

    scanf("%s", filename);

    tabuleiro = fopen(filename, "r");

    fscanf(tabuleiro, "%d%d%d", &tab.lin, &tab.col, &tab.cor);

    // aloca um vetor de LIN ponteiros para linhas
    tab.tab = malloc (tab.lin * sizeof (t_cell*)) ;
    
    // aloca um vetor com todos os elementos da matriz
    tab.tab[0] = malloc (tab.lin * tab.col * sizeof (t_cell)) ;
    
    // ajusta os demais ponteiros de linhas (i > 0)
    for (i=1; i < tab.lin; i++)
    tab.tab[i] = tab.tab[0] + i * tab.col ;
    
    // percorre a matriz
    for (i=0; i < tab.lin; i++)
        for (j=0; j < tab.col; j++){
            fscanf(tabuleiro, "%d", &tab.tab[i][j].cor);
            tab.tab[i][j].flood = 0;
        }
        
    fclose(tabuleiro);

    return tab;
}

// Verifica se a jogada esta dentro do tabuleiro. Retorna 0 se estiver, 1 caso contrario
int verifica_borda(t_tabuleiro *tab, int *x, int *y){
    if(*x < 0 || *x > (tab->col - 1) || *y < 0 || *y > (tab->lin - 1))
        return 1;
    
    return 0;
}

// Pinta as pecas ja conectadas da nova cor da jogada
void pinta_flood(t_tabuleiro *tab, int *new_color){
        for(int i=0; i<tab->lin;i++)
            for(int j=0;j<tab->col;j++){
                if(tab->tab[i][j].flood == 1){
                    tab->tab[i][j].cor = *new_color;
                    tab->tab[i][j].flood = 0;
                }
                if(tab->tab[i][j].flood == 2){
                    tab->tab[i][j].flood = 0;
                }
                }
}

// pinta o tavuleiro com a nova jogada
void pinta_tabuleiro(t_tabuleiro *tab, int x, int y, int *new_color){
    if(verifica_borda(tab, &x, &y)) return;
    if(tab->tab[x][y].cor != *new_color || tab->tab[x][y].flood == 1) return;

    tab->tab[x][y].flood = 1;

    pinta_tabuleiro(tab, x+1,y, new_color);
    pinta_tabuleiro(tab, x-1,y, new_color);
    pinta_tabuleiro(tab, x,y+1, new_color);
    pinta_tabuleiro(tab, x,y-1, new_color);
}

// Printa o Tabuleiro
void printa_tabuleiro(t_tabuleiro *tab){
    for(int i=0; i<tab->lin;i++){
        for(int j=0;j<tab->col;j++)
            printf(" %d ", tab->tab[i][j].cor);
        
        printf("\n");
    }
}