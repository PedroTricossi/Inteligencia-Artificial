#ifndef __FLOOD_IT__
#define __FLOOD_IT__

#include <vector>
using namespace std;

typedef struct 
{
    int flood;
    int cor;
}t_cell;

typedef struct
{
    int visitado;
    int indice;
    int cor;
    int area;
    vector<int> distancias;
    vector<int> vizinhos; // indices dos vizinhos

    // t_vertice() : distancias(3, -1), visitado(0){}
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

typedef struct 
{
    vector<int> passos;
    vector<t_vertice> grafo;
    vector<int> contadores_cores;
    int componentes_restantes;
}t_grafo_tabuleiro;


t_tabuleiro le_tabuleiro();

int verifica_borda(t_tabuleiro *tab, int *x, int *y);

void liga_componente(int i, int j, t_tabuleiro& tab, t_vertice& vert, vector<t_vertice>& grafo);

void mat2graph(t_tabuleiro& tab, vector<t_vertice>& grafo);

int ganhou(t_vertice v, int area);

void flood(t_grafo_tabuleiro& grafotab, int cor);

void gera_grafos1(vector<int> tentativa, t_grafo_tabuleiro grafo_original, vector<t_grafo_tabuleiro>& grafos_tentativas, int cores, int profundidade);

int avalia_tabuleiro(t_grafo_tabuleiro grafotab, int area, int cores);

void dijkstra(int ind_root, vector<t_vertice>& grafo_real, vector<t_vertice> grafo, int canto);

#endif