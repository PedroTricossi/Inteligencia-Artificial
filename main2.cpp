#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <omp.h>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

int profundidade = 1;

// mudar para hashmap? (no momento, nao pode tirar vertices do vetor para nao quebrar a indexacao dos vizinhos)
// vector<t_vertice> grafo;
t_grafo_tabuleiro grafotab;

vector<int> jogadas;

int main(int argc, char* argv[]){

    int p1 = -1, p2 = -1, p3 = -1, p4 = -1, p5 = -1, p6 = -1;

    // Check if the required command-line arguments are provided
    if (argc < 2) {
        cout << "Usage: program_name filename [profundidade p1 p2 p3 p4 p5 p6]" << endl;
        return 1;
    }

    int fim = 0;

    t_tabuleiro tab;

    if (argc >= 3) profundidade = atoi(argv[2]);
    // Assign the values of p1 to p6 if they are provided as command-line arguments
    if (argc >= 4) p1 = atoi(argv[3]);
    if (argc >= 5) p2 = atoi(argv[4]);
    if (argc >= 6) p3 = atoi(argv[5]);
    if (argc >= 7) p4 = atoi(argv[6]);
    if (argc >= 8) p5 = atoi(argv[7]);
    if (argc >= 9) p6 = atoi(argv[8]);
    
 
    tab = le_tabuleiro(argv[1], p1, p2, p3, p4, p5, p6);
    // cout << "tabuleiro matriz feito!!";

    mat2graph(tab, grafotab.grafo);
    // printf("fez o tabuleiro\n");

    // dijkstra cantos
    // SD = 0
    // IE = 1
    // ID = 2
    for (t_vertice vert : grafotab.grafo)
    {
        if (vert.distancias[0] == 0)
        {
            // cout << "dijk SD" << endl;
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 0);
        }

        if (vert.distancias[1] == 0)
        {
            // cout << "dijk IE" << endl;
            dijkstra(vert.indice, grafotab.grafo, grafotab.grafo, 1);
        }

        if (vert.distancias[2] == 0)
        {
            // cout << "dijk ID" << endl;
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

    // cout << "setou cores" << endl;
    // cout << grafotab.grafo.size() << endl;
    grafotab.componentes_restantes = 0;
    for (t_vertice v : grafotab.grafo)
    {
        // cout << v.indice << endl;
        grafotab.contadores_cores[v.cor-1]++;
        grafotab.componentes_restantes++;
    }

    // area total do tabuleiro
    int area = tab.lin * tab.col;

    // printf("contadores setados\n");
    // printf("contadores setados\n");

    int numThreads = omp_get_max_threads();
    // printf("threads: %d\n", numThreads);
    omp_set_num_threads(numThreads);

    // gera sequencias de jogadas
    vector<t_grafo_tabuleiro> grafos_tentativas; // grafos gerados apos considerar n jogadas, e as jogadas correspondentes
    // #pragma omp parallel for
    for (int i=1; i <= tab.cor; i++)
    {
        // nao faz sentido jogar a mesma cor do vertice principal ja
        if (grafotab.grafo[0].cor == i)
            continue;

        vector<int> tentativa;

        // ja comeca a tentativa de uma cor
        tentativa.push_back(i);
        // cout << tentativa[0] << endl;

        // #pragma omp critical
        // {
        //     #pragma omp task
        //     {
                gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, profundidade);
        //     }
        // }
    }

    #pragma omp taskwait

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

    
    while (!fim)
    {
        int tam_tentativas = static_cast<int>(grafos_tentativas.size());
        bool found_solution = false; // Declare a local flag for each parallel thread

        // #pragma omp parallel shared(fim, grafos_tentativas, jogadas, found_solution)
        {
            // #pragma omp for schedule(dynamic)
            // for (int c = 1; c <= tab.cor; c++)
            for (int t = 0; t < tam_tentativas; t++)
            {
                printf("oi15\n");
                if (found_solution)
                    continue; // Skip iteration if a solution has been found

                // for (int t = 0; t < tam_tentativas; t++)
                for (int v = 0; v < static_cast<int>(grafos_tentativas[t].grafo[0].vizinhos.size()); v++)
                {
                    printf("oi1\n");
                    int id_v = grafos_tentativas[t].grafo[0].vizinhos[v];
                    printf("oi2\n");
                    int cor = grafos_tentativas[t].grafo[id_v].cor;
                    printf("oi3\n");
                    if (grafos_tentativas[t].passos.back() == cor)
                        continue;

                    if (v > 3)
                        break;

                    printf("oi6, %d\n", omp_get_thread_num());
                    t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
                    printf("oi7, %d\n", omp_get_thread_num());
                    nova_tentativa.passos.push_back(cor);
                    flood(nova_tentativa, cor);
                    // #pragma omp critical
                    {
                        printf("oi4, %d\n", omp_get_thread_num());
                        grafos_tentativas.push_back(nova_tentativa);
                        printf("oi5\n");
                    }
                    printf("oi: %d %d\n", nova_tentativa.grafo[0].area, area);
                    // if (ganhou(nova_tentativa.grafo[0], area))
                    if(nova_tentativa.grafo[0].area == area)
                    {
                        printf("oi8\n");
                        // #pragma omp critical
                        {
                            fim = true;
                            found_solution = true; // Set the flag for this thread
                            jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                            printf("oi9\n");
                        }
                        break; // Exit the inner loop
                    }
                    printf("oi10\n");
                }
            }
        }

        if (fim)
            break;

        // eliminar as tentativas que geraram as novas, vindas do passo anterior
        // for (int i=0; i < tam_tentativas; i++)
        printf("oi11\n");
        grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);
        printf("oi12\n");

        printf("descartou os antigos\n");
        tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        printf("tentativas: %d\n", tam_tentativas);
        if (!tam_tentativas) break;

        // avaliar grafos gerados com a heurística
        pair<int, int> melhor = make_pair(1, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
        // pair<int, int> global_melhor = melhor;

        for (int i = 0; i < tam_tentativas; i++)
        {
            printf("avalia %d\n", i);
            int avaliacao = avalia_tabuleiro(grafos_tentativas[i], area, tab.cor);
            // #pragma omp critical
            {
                if (avaliacao < melhor.second)
                {
                    melhor.first = grafos_tentativas[i].passos[0];
                    melhor.second = avaliacao;
                }
            }
        }

        printf("avaliou\n");

        // guarda a melhor jogada
        jogadas.push_back(melhor.first);
        printf("jogou: %d\n", jogadas.back());

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
       
    }

    // printf("\nGANHOU\n");

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}