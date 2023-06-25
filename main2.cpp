#include <stdio.h>
#include <stdlib.h>
#include "flood_it2.h"
#include "ia.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>

using namespace std;

#define PROFUNDIDADE 3

// mudar para hashmap? (no momento, nao pode tirar vertices do vetor para nao quebrar a indexacao dos vizinhos)
vector<t_vertice> grafo;

vector<int> jogadas;

int main(){
    int ganhou = 0;

    t_tabuleiro tab;
 
    tab = le_tabuleiro();

    t_grafo grafo;
    mat2graph(tab, grafo);

    // dijkstra cantos
    // SD = 0
    // IE = 1
    // ID = 2
    for (t_vertice vert : grafo)
    {
        if (vert.distancias[0] == 0)
        {
            dijkstra(vert.indice, grafo, 0);
        }
        if (vert.distancias[1] == 0)
        {
            dijkstra(vert.indice, grafo, 1);
        }
        if (vert.distancias[2] == 0)
        {
            dijkstra(vert.indice, grafo, 2);
        }
    }


    // guarda quantos componentes de cada cor ainda restam
    // e também quantos componentes restam
    vector<int> contadores_cores(tab.cor, 0); // ESSAS COISAS NAO TAO FUNCIONANDO AINDA AAAAAAAAAA TEM QUE TER UM PRA CADA GRAFO
    int componentes_restantes = 0;
    for (t_vertice v : grafo)
    {
        contadores_cores[v.cor]++;
        componentes_restantes++;
    }

    int area = tab.lin * tab.col;


    // gera sequencias de jogadas
    vector<t_tentativa> grafos_tentativas; // grafos gerados apos considerar n jogadas, e as jogadas correspondentes
    for (int i=1; i <= tab.cor; i++)
    {
        // nao faz sentido jogar a mesma cor do vertice principal ja
        if (grafo[0].cor == i)
            continue;

        vector<int> tentativa;

        // ja comeca a tentativa de uma cor
        tentativa.push_back(i);

        gera_grafos1(tentativa, grafo, grafos_tentativas, tab.cor, PROFUNDIDADE);
    }


    // avaliar grafos gerados com a heurística
    // também verificar se algum ganhou ja
    pair<int, int> melhor = (1, INT_MAX) // cor, pontuacao. Quanto menor a pontuacao, melhor
    for (t_tentativa t : grafos_tentativas)
    {
        ganhou = ganhou(grafos_tentativas[t].grafo[0], area);
        if (ganhou)
        {
            jogadas = grafos_tentativas[t].passos
            break;
        }

        int avaliacao = avalia_tabuleiro(t.grafo[0]);

        if (avaliacao < melhor.second)
        {
            melhor.first = t.passos[0];
            melhor.second = avaliacao;
        }
    }

    // se já achou uma solucao vencedora nesses <PROFUNDIDADE> primeiros passos
    if (ganhou)
    {
        for (int j : jogadas)
            cout << jogadas[j];

        return 0;
    }

    // guarda a melhor jogada
    jogadas.push_back(melhor.first);

    // eliminar os ruins
    vector<int> ruins;
    for (int t=0; t < static_cast<int>(grafos_tentativas); t++)
    {
        if (grafos_tentativas[t].tentativa[0] != jogadas.back())
            ruins.push_back(t);

        else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
            grafos_tentativas[t].tentativas.erase(grafos_tentativas[t].tentativas.begin() + 0);
    }

    for (int r : ruins)
        grafos_tentativas.erase(grafos_tentativas.begin() + r);


    // gerar novo passo para os grafos que restaram
    // avaliar novos passos
    // escolher o melhor
    // eliminar os ruins
    // ate acabar o jogo
    while(!ganhou)
    {
        int tam_tentativas = static_cast<int>(grafos_tentativas); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        for (int c=1; c <= tab.cor; c++)
        {
            for (int t=0; t < tam_tentativas; t++)
            {
                t_tentativa nova_tentativa = grafos_tentativas[t];
                nova_tentativa.tentativa.push_back(c);
                flood(nova_tentativa.grafo, c);
                grafos_tentativas.push_back(nova_tentativa);

                ganhou = ganhou(nova_tentativa.grafo[0], area);
                if (ganhou)
                {
                    // for (int c : nova_tentativa.tentativa)
                    // {
                    //     jogadas.push_back(c);
                    // }
                    // faz o mesmo que o loop, um append de vectors basicamente
                    jogadas.insert(jogadas.end(), nova_tentativa.tentativa.begin(), nova_tentativa.tentativa.end());
                    break;
                }
            }
            if (ganhou)
                break;
        }
        if (ganhou)
            break;

        // eliminar as tentativas que geraram as novas, vindas do passo anterior
        for (int i=0; i < tam_tentativas; i++)
            grafos_tentativas.erase(grafos_tentativas.begin() + i);


        // avaliar grafos gerados com a heurística
        pair<int, int> melhor = (1, INT_MAX) // cor, pontuacao. Quanto menor a pontuacao, melhor
        for (t_tentativa t : grafos_tentativas)
        {
            int avaliacao = avalia_tabuleiro(t.grafo[0]);

            if (avaliacao < melhor.second)
            {
                melhor.first = t.tentativa[0];
                melhor.second = avaliacao;
            }
        }

        // guarda a melhor jogada
        jogadas.push_back(melhor.first);

        // eliminar os ruins
        vector<int> ruins;
        for (int t=0; t < static_cast<int>(grafos_tentativas); t++)
        {
            if (grafos_tentativas[t].tentativa[0] != jogadas.back())
                ruins.push_back(t);

            else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
                grafos_tentativas[t].tentativas.erase(grafos_tentativas[t].tentativas.begin() + 0);
        }

        for (int r : ruins)
            grafos_tentativas.erase(grafos_tentativas.begin() + r);
        
    }

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << jogadas[j];

    return 0;
}