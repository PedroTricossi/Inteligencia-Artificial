#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

#define PROFUNDIDADE 3

// mudar para hashmap? (no momento, nao pode tirar vertices do vetor para nao quebrar a indexacao dos vizinhos)
// vector<t_vertice> grafo;
t_grafo_tabuleiro grafotab;

vector<int> jogadas;

int main(){
    int fim = 0;

    t_tabuleiro tab;
 
    tab = le_tabuleiro();

    mat2graph(tab, grafotab.grafo);

    // dijkstra cantos
    // SD = 0
    // IE = 1
    // ID = 2
    for (t_vertice vert : grafotab.grafo)
    {
        vert.distancias.push_back(-1);
        vert.distancias.push_back(-1);
        vert.distancias.push_back(-1);

        vert.visitado = 0;

        if (vert.distancias[0] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, 0);
        }
        if (vert.distancias[1] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, 1);
        }
        if (vert.distancias[2] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, 2);
        }
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


    // gera sequencias de jogadas
    vector<t_grafo_tabuleiro> grafos_tentativas; // grafos gerados apos considerar n jogadas, e as jogadas correspondentes
    for (int i=1; i <= tab.cor; i++)
    {
        // nao faz sentido jogar a mesma cor do vertice principal ja
        if (grafotab.grafo[0].cor == i)
            continue;

        vector<int> tentativa;

        // ja comeca a tentativa de uma cor
        tentativa.push_back(i);

        gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, PROFUNDIDADE);
    }


    // avaliar grafos gerados com a heurística
    // também verificar se algum ganhou ja
    pair<int, int> melhor = make_pair(1, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
    for (t_grafo_tabuleiro t : grafos_tentativas)
    {
        fim = ganhou(t.grafo[0], area);
        if (fim)
        {
            jogadas = t.passos;
            break;
        }

        int avaliacao = avalia_tabuleiro(t, area, tab.cor);

        if (avaliacao < melhor.second)
        {
            melhor.first = t.passos[0];
            melhor.second = avaliacao;
        }
    }

    // se já achou uma solucao vencedora nesses <PROFUNDIDADE> primeiros passos
    if (fim)
    {
        for (int j : jogadas)
            cout << jogadas[j];

        return 0;
    }

    // guarda a melhor jogada
    jogadas.push_back(melhor.first);

    // eliminar os ruins
    vector<int> ruins;
    for (int t=0; t < static_cast<int>(grafos_tentativas.size()); t++)
    {
        if (grafos_tentativas[t].passos[0] != jogadas.back())
            ruins.push_back(t);

        else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
            grafos_tentativas[t].passos.erase(grafos_tentativas[t].passos.begin() + 0);
    }

    for (int r : ruins)
        grafos_tentativas.erase(grafos_tentativas.begin() + r);


    // gerar novo passo para os grafos que restaram
    // avaliar novos passos
    // escolher o melhor
    // eliminar os ruins
    // ate acabar o jogo
    while(!fim)
    {
        int tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        for (int c=1; c <= tab.cor; c++)
        {
            for (int t=0; t < tam_tentativas; t++)
            {
                t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
                nova_tentativa.passos.push_back(c);
                flood(nova_tentativa, c);
                grafos_tentativas.push_back(nova_tentativa);

                fim = ganhou(nova_tentativa.grafo[0], area);
                if (fim)
                {
                    // for (int c : nova_tentativa.tentativa)
                    // {
                    //     jogadas.push_back(c);
                    // }
                    // faz o mesmo que o loop, um append de vectors basicamente
                    jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                    break;
                }
            }
            if (fim)
                break;
        }
        if (fim)
            break;

        // eliminar as tentativas que geraram as novas, vindas do passo anterior
        for (int i=0; i < tam_tentativas; i++)
            grafos_tentativas.erase(grafos_tentativas.begin() + i);


        // avaliar grafos gerados com a heurística
        pair<int, int> melhor = make_pair(1, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
        for (t_grafo_tabuleiro t : grafos_tentativas)
        {
            int avaliacao = avalia_tabuleiro(t, area, tab.cor);

            if (avaliacao < melhor.second)
            {
                melhor.first = t.passos[0];
                melhor.second = avaliacao;
            }
        }

        // guarda a melhor jogada
        jogadas.push_back(melhor.first);

        // eliminar os ruins
        vector<int> ruins;
        for (int t=0; t < static_cast<int>(grafos_tentativas.size()); t++)
        {
            if (grafos_tentativas[t].passos[0] != jogadas.back())
                ruins.push_back(t);

            else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
                grafos_tentativas[t].passos.erase(grafos_tentativas[t].passos.begin() + 0);
        }

        for (int r : ruins)
            grafos_tentativas.erase(grafos_tentativas.begin() + r);
        
    }

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << jogadas[j];

    return 0;
}