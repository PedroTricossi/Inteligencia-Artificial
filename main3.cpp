#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

#define PROFUNDIDADE 0

t_grafo_tabuleiro grafotab;

vector<int> jogadas;

int main(int argc, char* argv[]){
    int fim = 0;

    t_tabuleiro tab;
 
    tab = le_tabuleiro(argv[1]);
    // cout << "tabuleiro matriz feito!!";

    mat2graph(tab, grafotab.grafo);
    // cout << "tabuleiro grafo feito";

    for (t_vertice vert : grafotab.grafo)
    {
        // printf("INDICE: %d: %d %d %d\n\n", vert.indice, vert.distancias[0], vert.distancias[1], vert.distancias[2]);
        if (vert.distancias[0] == 0)
        {
            // cout << "dijk SD" << endl;
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 0);
        }
        // cout << "dijkstra" << endl;
        // printf("INDICE: %d: %d %d %d\n\n", vert.indice, vert.distancias[0], vert.distancias[1], vert.distancias[2]);
        if (vert.distancias[1] == 0)
        {
            // cout << "dijk IE" << endl;
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 1);
        }
        // cout << "dijkstra" << endl;
        // printf("INDICE: %d: %d %d %d\n\n", vert.indice, vert.distancias[0], vert.distancias[1], vert.distancias[2]);
        if (vert.distancias[2] == 0)
        {
            // cout << " dijk ID" << endl;
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 2);
        }
        // cout << "dijkstra" << endl;
    }

    // guarda quantos componentes de cada cor ainda restam
    // e também quantos componentes restam
    for (int i=0; i<tab.cor; i++)
        grafotab.contadores_cores.push_back(0);

    grafotab.componentes_restantes = 0;
    for (t_vertice v : grafotab.grafo)
    {
        grafotab.contadores_cores[v.cor-1]++;
        grafotab.componentes_restantes++;
    }

    // area total do tabuleiro
    int area = tab.lin * tab.col;

    // avaliar novos passos
    // escolher o melhor
    // eliminar os ruins
    // ate acabar o jogo
    // for (t_vertice v : grafotab.grafo)
    // {
    //     // printf("componente cor: %d\n", v.cor);
    //     // printf("componente area: %d\n\n", v.area);
    // }
    while(!fim)
    {
        // cout << "entrou no loop";
        // printf("entrou no loop\n");

        t_grafo_tabuleiro melhor_grafo = grafotab;
        int score_melhor = avalia_tabuleiro(melhor_grafo, area, tab.cor);

        int melhor = INT_MAX;
        int melhor_cor = 1; 

        // int tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas:\n");
        for (int c=1; c <= tab.cor; c++)
        {   
            if ((!jogadas.empty() && jogadas.back() == c))// || grafotab.grafo[0].cor == c)
                continue;

            // printf("passou if\n");

            t_grafo_tabuleiro tentativa = grafotab;

            flood(tentativa, c);

            int score_tentativa = avalia_tabuleiro(tentativa, area, tab.cor);

            // printf("%d, %d\n\n", score_tentativa, c);

            if (score_tentativa < score_melhor)
            {
                score_melhor = score_tentativa;
                melhor_grafo = tentativa;
                melhor_cor = c; 
            }
        }
        // printf("JOGOU: %d\n", melhor_cor);
        jogadas.push_back(melhor_cor);

        flood(grafotab, melhor_cor);

        // for (t_vertice v : grafotab.grafo)
        // {
        //     printf("componente cor: %d\n", v.cor);
        //     printf("componente area: %d\n\n", v.area);
        // }

        if (ganhou(grafotab.grafo[0], area))
            break;

        // if (jogadas.size() > 3) break;
    }

    // printf("\nGANHOU\n");

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}