#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

#define PROFUNDIDADE 6

// mudar para hashmap? (no momento, nao pode tirar vertices do vetor para nao quebrar a indexacao dos vizinhos)
// vector<t_vertice> grafo;
t_grafo_tabuleiro grafotab;

vector<int> jogadas;

int main(int argc, char* argv[]){
    int fim = 0;

    t_tabuleiro tab;
 
    tab = le_tabuleiro(argv[1]);
    // cout << "tabuleiro matriz feito!!";

    mat2graph(tab, grafotab.grafo);
    // cout << "tabuleiro grafo feito";

    // dijkstra cantos
    // SD = 0
    // IE = 1
    // ID = 2
    for (t_vertice vert : grafotab.grafo)
    {
        if (vert.distancias[0] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 0);
        }

        if (vert.distancias[1] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 1);
        }

        if (vert.distancias[2] == 0)
        {
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 2);
        }
    }
    // cout << "passou dos dijkstra";
    // printf("passou dijkstra\n");
    // printf("%d %d %d\n", grafotab.grafo[0].distancias[0], grafotab.grafo[0].distancias[1], grafotab.grafo[0].distancias[2]);

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

    // cout << "contadores setados";
    // printf("contadores setados\n");

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
        // cout << tentativa[0] << endl;

        gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, PROFUNDIDADE);
    }

    // cout << "sequencias iniciais geradas";
    // printf("sequencias iniciais geradas\n");
    // cout << grafos_tentativas.size() << endl;

    // avaliar grafos gerados com a heurística
    // também verificar se algum ganhou ja ganhou
    pair<int, int> melhor = make_pair(1, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
    for (t_grafo_tabuleiro t : grafos_tentativas)
    {
        fim = ganhou(t.grafo[0], area);
        if (fim)
        {
            // printf("%d\n", t.componentes_restantes);
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
        // printf("\nGANHOU\n");

        for (int j : jogadas)
            cout << "a " << j << "\n";

        return 0;
    }

    // guarda a melhor jogada
    jogadas.push_back(melhor.first);

    // cout << "melhor jogada avaliada";
    // printf("melhor jogada avaliada\n");
    // int tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
    // printf("tentativas: %d\n", tam_tentativas);

    // eliminar os ruins
    auto it = grafos_tentativas.begin();
    while (it != grafos_tentativas.end())
    {
        if (it->passos[0] != jogadas.back())
            it = grafos_tentativas.erase(it);

        else
        {
            it->passos.erase(it->passos.begin() + 0);
            ++it;
        }
    }

    // tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
    // printf("tentativas: %d\n", tam_tentativas);

    // vector<int> ruins;
    // for (int t=0; t < static_cast<int>(grafos_tentativas.size()); t++)
    // {
    //     if (grafos_tentativas[t].passos[0] != jogadas.back())
    //         ruins.push_back(t);

    //     else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
    //         grafos_tentativas[t].passos.erase(grafos_tentativas[t].passos.begin() + 0);
    // }

    // for (int r : ruins)
    //     grafos_tentativas.erase(grafos_tentativas.begin() + r);

    // cout << "jogadas ruins descartadas";
    // printf("jogadas ruins descartadas\n");

    // gerar novo passo para os grafos que restaram
    // avaliar novos passos
    // escolher o melhor
    // eliminar os ruins
    // ate acabar o jogo
    while(!fim)
    {
        // cout << "entrou no loop";
        // printf("entrou no loop\n");

        int tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas: %d\n", tam_tentativas);
        for (int c=1; c <= tab.cor; c++)
        {
            for (int t=0; t < tam_tentativas; t++)
            {
                // printf("checa ultima jogada\n");
                if (grafos_tentativas[t].passos.back() == c)
                    continue;

                t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
                nova_tentativa.passos.push_back(c);
                // printf("vai floodar\n");
                flood(nova_tentativa, c);
                // printf("floodou\n");
                grafos_tentativas.push_back(nova_tentativa);
                // printf("pushou np vetor\n");

                fim = ganhou(nova_tentativa.grafo[0], area);
                // printf("viu se ganhou\n");
                if (fim)
                {
                    // printf("\nGANHOU\n");
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

        // cout << "definiu as proximas jogadas a serem avaliadas";
        // printf("definiu as proximas jogadas a serem avaliadas\n");
        // tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas: %d\n", tam_tentativas);

        // eliminar as tentativas que geraram as novas, vindas do passo anterior
        // for (int i=0; i < tam_tentativas; i++)
        grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);

        // printf("descartou os antigos\n");
        tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas: %d\n", tam_tentativas);
        if (!tam_tentativas) break;

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

        // cout << "avaliadas dentro do loop";
        // printf("avaliadas dentro do loop\n");

        // // guarda a melhor jogada
        // jogadas.pop_back();
        jogadas.push_back(melhor.first);

        // eliminar os ruins
        auto it = grafos_tentativas.begin();
        while (it != grafos_tentativas.end())
        {
            if (it->passos[0] != jogadas.back())
                it = grafos_tentativas.erase(it);

            else
            {
                it->passos.erase(it->passos.begin() + 0);
                ++it;
            }
        }

        // vector<int> ruins;
        // for (int t=0; t < static_cast<int>(grafos_tentativas.size()); t++)
        // {
        //     if (grafos_tentativas[t].passos[0] != jogadas.back())
        //         ruins.push_back(t);

        //     else // ja tira a jogada da sequencia de cada tentativa, ja que é a posicao 0 que é analisada sempre
        //         grafos_tentativas[t].passos.erase(grafos_tentativas[t].passos.begin() + 0);
        // }

        // for (int r : ruins)
        //     grafos_tentativas.erase(grafos_tentativas.begin() + r);
        
        // cout << "ruins descartadas no loop";
        // printf("ruins descartadas no loop\n");
        tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas: %d\n", tam_tentativas);

        // for (int j : jogadas)
        //     cout << j << "\n";
    }

    // printf("\nGANHOU\n");

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}