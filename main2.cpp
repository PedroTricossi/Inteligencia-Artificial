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
    for (auto v = grafotab.grafo[0].vizinhos.begin(); v != grafotab.grafo[0].vizinhos.end(); v++)
    {
        printf("%d %d\n", grafotab.grafo[*v].indice, grafotab.grafo[*v].cor);
    }

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
    
    // Checkpoint

    vector<int> cores_usadas;
    for (int cor=1; cor <= tab.cor; cor++)
        cores_usadas.push_back(0);

    // // gera sequencias de jogadas
    vector<t_grafo_tabuleiro> grafos_tentativas; // grafos gerados apos considerar n jogadas, e as jogadas correspondentes
    // // #pragma omp parallel for
    // // for (int i=1; i <= tab.cor; i++)
    // for (auto viz = grafotab.grafo[0].vizinhos.begin(); viz != grafotab.grafo[0].vizinhos.end(); viz++)
    // {

    //     if (cores_usadas[grafotab.grafo[*viz].cor - 1] == 1)
    //         continue;

    //     cores_usadas[grafotab.grafo[*viz].cor - 1] = 1;

    //     printf("%d %d\n", grafotab.grafo[*viz].indice, grafotab.grafo[*viz].cor);

    //     vector<int> tentativa;

    //     // ja comeca a tentativa de uma cor
    //     tentativa.push_back(grafotab.grafo[*viz].cor);

    //     gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, profundidade);
    // }

    flood(grafotab, 3);
    // flood(grafotab, 2);
    printf("area %d\n", grafotab.grafo[0].area);
    while(1);

    // avaliar grafos gerados com a heurística
    // também verificar se algum ganhou ja ganhou
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
        printf("area: %d\n", t.grafo[0].area);
        printf("passo1: %d\n", t.passos[0]);
        printf("passo2: %d\n", t.passos[1]);

        if (avaliacao < melhor.second)
        {
            melhor.first = t.passos[0];
            melhor.second = avaliacao;
            printf("melhor: %d\n", melhor.first);
        }
    }

    // se já achou uma solucao vencedora nesses <PROFUNDIDADE> primeiros passos
    if (fim)
    {

        for (int j : jogadas)
            cout << "a " << j << "\n";

        return 0;
    }

    // guarda a melhor jogada
    jogadas.push_back(melhor.first);


    auto teste = grafos_tentativas.begin();
    while (teste != grafos_tentativas.end())
    {
        printf("primeira jogada: %d\n", teste->passos[0]);
        printf("passos: %ld\n", teste->passos.size());
        teste++;
    }

    // eliminar os ruins
    auto it = grafos_tentativas.begin();
    while (it != grafos_tentativas.end())
    {
        if (it->passos[0] != jogadas.back())
        {
            printf("apagou\n");
            it = grafos_tentativas.erase(it);
        }

        else
        {
            it->passos.erase(it->passos.begin() + 0);
            ++it;
        }
    }

    // printf("back: %d\n", jogadas.back());

    flood(grafotab, jogadas.back());

    // int id1 = grafotab.grafo[0].vizinhos[0];
    // int id2 = grafotab.grafo[0].vizinhos[1];

    // printf("cor: %d\n", grafotab.grafo[id1].cor); //grafotab.grafo[0].vizinhos[0];
    // printf("cor: %d\n", grafotab.grafo[id2].cor);


    printf("------------------ sobraram:\n");
    auto teste2 = grafos_tentativas.begin();
    while (teste2 != grafos_tentativas.end())
    {
        // teste2->grafo = grafotab.grafo;
        // teste2->contadores_cores = grafotab.contadores_cores;
        // teste2->componentes_restantes = grafotab.componentes_restantes;
        printf("primeira jogada: %d\n", teste2->passos[0]);
        printf("passos: %ld\n", teste2->passos.size());
        teste2++;
    }
    printf("oi\n");
    while(1){int oi = 0;}

    
    while (!fim)
    {
        int tam_tentativas = static_cast<int>(grafos_tentativas.size());
        int found_solution = 0; // Declare a local flag for each parallel thread

        vector<t_grafo_tabuleiro> vec_tentativas = grafos_tentativas; // faz uma copia pra nao fazero erase que da segfault
        // #pragma omp parallel shared(fim, vec_tentativas, jogadas, found_solution)
        {
            // #pragma omp for schedule(dynamic)
            // for (int c = 1; c <= tab.cor; c++)
            // for (int t = 0; t < tam_tentativas; t++)
            for (auto tenta = grafos_tentativas.begin(); tenta != grafos_tentativas.end(); tenta++)
            {
                printf("oi15\n");
                if (found_solution)
                    continue; // Skip iteration if a solution has been found
                

                int conta_vizinhos = 0;
                // for (int t = 0; t < tam_tentativas; t++)
                // for (int v = 0; v < static_cast<int>(grafos_tentativas[t].grafo[0].vizinhos.size()); v++)
                for (auto v = tenta->grafo[0].vizinhos.begin(); v != tenta->grafo[0].vizinhos.end(); v++)
                {
                    // printf("oi1\n");
                    // int id_v = grafos_tentativas[t].grafo[0].vizinhos[v];
                    // printf("oi2\n");
                    // int cor = grafos_tentativas[t].grafo[id_v].cor;
                    // printf("\toi3\n");

                    printf("\tcor: %d\n", tenta->grafo[*v].cor);
                    printf("\tid: %d\n", tenta->grafo[*v].indice);
                    printf("\tarea: %d\n", tenta->grafo[0].area);
                    printf("---------\n");

                    // if (grafos_tentativas[t].passos.back() == cor)
                    //     continue;

                    if (conta_vizinhos > 2)
                        break;
                    conta_vizinhos++;

                    // printf("oi6, %d\n", omp_get_thread_num());
                    // t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
                    // printf("oi7, %d\n", omp_get_thread_num());
                    // nova_tentativa.passos.push_back(cor);
                    // flood(nova_tentativa, cor);
                    // // #pragma omp critical
                    // {
                    //     printf("oi4, %d\n", omp_get_thread_num());
                    //     vec_tentativas.push_back(nova_tentativa);
                    //     printf("oi5\n");
                    // }
                    // printf("oi: %d %d\n", nova_tentativa.grafo[0].area, area);
                    // // if (ganhou(nova_tentativa.grafo[0], area))
                    // if(nova_tentativa.grafo[0].area == area)
                    // {
                    //     printf("oi8\n");
                    //     // #pragma omp critical
                    //     {
                    //         fim = true;
                    //         found_solution = true; // Set the flag for this thread
                    //         jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                    //         printf("oi9\n");
                    //     }
                    //     break; // Exit the inner loop
                    // }
                    // printf("oi10\n");
                }
            }
        }

        if (fim)
            break;

        // eliminar as tentativas que geraram as novas, vindas do passo anterior
        // for (int i=0; i < tam_tentativas; i++)
        printf("oi11\n");
        // grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);
        grafos_tentativas = vec_tentativas;
        printf("------------------ sobraram:\n");
        auto teste3 = grafos_tentativas.begin();
        while (teste3 != grafos_tentativas.end())
        {
            printf("primeira jogada: %d\n", teste3->passos[0]);
            printf("passos: %ld\n", teste3->passos.size());
            teste3++;
        }
        printf("oi\n");
        while(1){int oi = 1;}
        // printf("oi12\n");

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

        // grafos_tentativas.erase(remove_if(grafos_tentativas.begin(), grafos_tentativas.end(), [](const t_grafo_tabuleiro& grafo_tentativa) {
        //     return grafo_tentativa.passos[0] != jogadas.back();
        // }), grafos_tentativas.end());

        // eliminar os ruins
        int apagados = 0;
        auto it = grafos_tentativas.end();
        while (it != grafos_tentativas.begin())
        {
            if (it->passos[0] != jogadas.back())
            {
                it = grafos_tentativas.erase(it);
                apagados++;
                printf("apagar %d\n", apagados);
            }

            else
            {
                it->passos.erase(it->passos.begin() + 0);
                --it;
            }
        }
       
    }

    // printf("\nGANHOU\n");

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}