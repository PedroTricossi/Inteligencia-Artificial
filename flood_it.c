#include <stdio.h>
#include <stdlib.h>
#include "flood_it.h"

// Cria um tabuleiro de flood-it usando como base as informacoes do enunciado do trabalho
t_tabuleiro le_tabuleiro() {
    char filename[1024];
    int i, j;
    t_tabuleiro tab;
    FILE* tabuleiro;

    // printf("Enter filename: ");
    scanf("%s", filename);

    tabuleiro = fopen(filename, "r");
    if (tabuleiro == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    if (fscanf(tabuleiro, "%d%d%d", &tab.lin, &tab.col, &tab.cor) != 3) {
        printf("Invalid file format.\n");
        exit(1);
    }

    tab.tab = malloc(tab.lin * sizeof(t_cell*));
    if (tab.tab == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    tab.tab[0] = malloc(tab.lin * tab.col * sizeof(t_cell));
    if (tab.tab[0] == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    for (i = 1; i < tab.lin; i++)
        tab.tab[i] = tab.tab[0] + i * tab.col;

    for (i = 0; i < tab.lin; i++) {
        for (j = 0; j < tab.col; j++) {
            if (fscanf(tabuleiro, "%d", &tab.tab[i][j].cor) != 1) {
                printf("Invalid file format.\n");
                exit(1);
            }
            tab.tab[i][j].flood = 0;
        }
    }

    tab.tab[0][0].flood = 1;
    
    fclose(tabuleiro);

    return tab;
}


// Verifica se a jogada esta dentro do tabuleiro. Retorna 0 se estiver, 1 caso contrario
int verifica_borda(t_tabuleiro *tab, int *x, int *y) {
    return (*x < 0 || *x > (tab->col - 1) || *y < 0 || *y > (tab->lin - 1));
}


// Pinta as pecas ja conectadas da nova cor da jogada
void pinta_flood(t_tabuleiro *tab, int *new_color) {
    for (int i = 0; i < tab->lin; i++) {
        for (int j = 0; j < tab->col; j++) {
            if (tab->tab[i][j].flood == 1) {
                tab->tab[i][j].cor = *new_color;
                tab->tab[i][j].flood = 0;
            } else {
                tab->tab[i][j].flood = 0;
            }
        }
    }
}


// pinta o tavuleiro com a nova jogada
void pinta_tabuleiro(t_tabuleiro *tab, int x, int y, int *new_color) {
    if (verifica_borda(tab, &x, &y) || tab->tab[x][y].cor != *new_color || tab->tab[x][y].flood == 1) {
        return;
    }

    tab->tab[x][y].flood = 1;

    pinta_tabuleiro(tab, x + 1, y, new_color);
    pinta_tabuleiro(tab, x - 1, y, new_color);
    pinta_tabuleiro(tab, x, y + 1, new_color);
    pinta_tabuleiro(tab, x, y - 1, new_color);
}


// Printa o Tabuleiro
void printa_tabuleiro(t_tabuleiro *tab) {
    for (int i = 0; i < tab->lin; i++) {
        for (int j = 0; j < tab->col; j++) {
            if(tab->tab[i][j].flood == 1)
                printf("\033[1;36m %2d \033[0m", tab->tab[i][j].cor);
            else
                printf("\033[0;31m %2d \033[0m", tab->tab[i][j].cor);
        }
        printf("\n");
    }
}
