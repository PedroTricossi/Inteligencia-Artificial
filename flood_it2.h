#ifndef __FLOOD_IT__
#define __FLOOD_IT__

typedef struct 
{
    int flood;
    int cor;
}t_cell;

typedef struct
{
    // adicionar variaveis referentes a heuristicas
    // int flood // se é o vértice que flooda os outros
    int visitado;
    int indice;
    // int componente;
    int cor;
    int area;
    // int dist_SD;
    // int dist_IE;
    // int dist_ID;
    vector<int> distancias;
    vector<int> vizinhos; // indices dos vizinhos

    t_vertice() : distancias(3, -1), visitado(0){}
}t_vertice;

typedef struct 
{
    int lin;
    int col;
    int cor;
    t_cell **tab;
}t_tabuleiro;

typedef struct
{
    vector<int> passos;
    vector<t_vertice> grafo;
}t_tentativa;

// typedef struct
// {
//     vector<int> contadores_cores;
//     vector<t_vertice> vertices;
// }t_grafo


t_tabuleiro le_tabuleiro();

int verifica_borda(t_tabuleiro *tab, int *x, int *y);

void mat2graph(t_tabuleiro& tab, vector<t_vertice>& grafo);

int ganhou(t_vertice v, int lin, int col);

#endif