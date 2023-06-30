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

int profundidade = 2;
int poda = 5;

// vector<t_vertice> grafo;
t_grafo_tabuleiro grafotab;

vector<int> jogadas;

int main(int argc, char* argv[]){

    int p1 = -1, p2 = -1, p3 = -1, p4 = -1, p5 = -1, p6 = -1;

    if (argc < 2) {
        cout << "Uso: programa arquivo_entrada [profundidade p1 p2 p3 p4 p5 p6]" << endl;
        return 1;
    }

    int fim = 0;

    t_tabuleiro tab;

    if (argc >= 3) profundidade = atoi(argv[2]);
    if (argc >= 4) p1 = atoi(argv[3]);
    if (argc >= 5) p2 = atoi(argv[4]);
    if (argc >= 6) p3 = atoi(argv[5]);
    if (argc >= 7) p4 = atoi(argv[6]);
    if (argc >= 8) p5 = atoi(argv[7]);
    if (argc >= 9) p6 = atoi(argv[8]);
    
 
    tab = le_tabuleiro(argv[1], p1, p2, p3, p4, p5, p6);
    // cout << "tabuleiro matriz feito!!";

    mat2graph(tab, grafotab.grafo);
 

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

    for (auto v = grafotab.grafo[0].vizinhos.begin(); v != grafotab.grafo[0].vizinhos.end(); v++)
    {
        if (grafotab.grafo[*v].distancias[0] <= grafotab.grafo[0].distancias[0])
            grafotab.grafo[0].distancias[0] = grafotab.grafo[*v].distancias[0] + 1;

        if (grafotab.grafo[*v].distancias[1] <= grafotab.grafo[0].distancias[1])
            grafotab.grafo[0].distancias[1] = grafotab.grafo[*v].distancias[1] + 1;

        if (grafotab.grafo[*v].distancias[2] <= grafotab.grafo[0].distancias[2])
            grafotab.grafo[0].distancias[2] = grafotab.grafo[*v].distancias[2] + 1;
    }

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
    for (auto viz : grafotab.grafo[0].vizinhos)
    {

        if (cores_usadas[grafotab.grafo[viz].cor - 1] == 1)
            continue;

        cores_usadas[grafotab.grafo[viz].cor - 1] = 1;

        // printf("%d %d\n", grafotab.grafo[viz].indice, grafotab.grafo[viz].cor);

        vector<int> tentativa;

        // ja comeca a tentativa de uma cor
        tentativa.push_back(grafotab.grafo[viz].cor);

        gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, profundidade);
    }


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
        // printf("area: %d\n", t.grafo[0].area);
        // printf("passo1: %d\n", t.passos[0]);
        // printf("passo2: %d\n", t.passos[1]);

        if (avaliacao < melhor.second)
        {
            melhor.first = t.passos[0];
            melhor.second = avaliacao;
            // printf("\tmelhor passo: %d\n", melhor.first);
            // printf("\tmelhor avaliacao: %d\n", melhor.second);
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
    // printf("melhor jogada: %d\n", jogadas.back());

    // eliminar os ruins
    auto it = grafos_tentativas.begin();
    while (it != grafos_tentativas.end())
    {
        if (it->passos[0] != jogadas.back())
        {
            // printf("apagou\n");
            it = grafos_tentativas.erase(it);
        }

        else
        {
            it->passos.erase(it->passos.begin() + 0);
            ++it;
        }
    }


    while (!fim)
    {
        int tam_tentativas = static_cast<int>(grafos_tentativas.size());
        // printf("tentativas: %d\n", tam_tentativas);
        int found_solution = 0; // Declare a local flag for each parallel thread

        // so pushar pra esse, nao precisa do erase mais
        vector<t_grafo_tabuleiro> vec_tentativas;// = grafos_tentativas; // faz uma copia pra nao fazero erase que da segfault
        // #pragma omp parallel shared(fim, vec_tentativas, jogadas, found_solution)
        {
            // #pragma omp for schedule(dynamic)
            for (auto tenta : grafos_tentativas) // para cada tentativa
            {

                if (found_solution)
                    break; // Skip iteration if a solution has been found

                vector<int> cores_usadas;
                for (int cor=1; cor <= tab.cor; cor++)
                    cores_usadas.push_back(0);
                
                int conta_vizinhos = 0;
                // for (int t = 0; t < tam_tentativas; t++)
                // for (int v = 0; v < static_cast<int>(grafos_tentativas[t].grafo[0].vizinhos.size()); v++)
                for (auto v : tenta.grafo[0].vizinhos) // para os vizinhos do principal na tentativa
                {
                    if (cores_usadas[tenta.grafo[v].cor - 1] == 1)
                        continue;
                    cores_usadas[tenta.grafo[v].cor - 1] = 1;


                    if (conta_vizinhos >= poda)
                    {
                        // printf("limite vizinhos\n");
                        break;
                    }
                    conta_vizinhos++;

                    t_grafo_tabuleiro nova_tentativa = tenta;
                    nova_tentativa.passos.push_back(tenta.grafo[v].cor);
                    flood(nova_tentativa, tenta.grafo[v].cor);
                    // #pragma omp critical
                    {
                        vec_tentativas.push_back(nova_tentativa);
                    }

                    if (ganhou(nova_tentativa.grafo[0], area))
                    {
                        // #pragma omp critical
                        {
                            fim = 1;
                            found_solution = true; // Set the flag for this thread
                            jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                        }
                        break; // Exit the inner loop
                    }
                }

                if (fim)
                    break;
            }

            // if (fim)
            //     break;
        }

        if (fim)
            break;


        grafos_tentativas = vec_tentativas;


        tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
        // printf("tentativas: %d\n", tam_tentativas);

        if (!tam_tentativas) break;

        if (static_cast<int>(grafos_tentativas[0].passos.size()) >= profundidade)
        {
            vector<int> tmp;
            tmp.push_back(1);
            // avaliar grafos gerados com a heurística
            pair<vector<int>, int> melhor = make_pair(tmp, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
            int melhor_tent = 0;
            for (int i = 0; i < tam_tentativas; i++)
            {
                int avaliacao = avalia_tabuleiro(grafos_tentativas[i], area, tab.cor);
                {
                    if (avaliacao < melhor.second)
                    {
                        melhor.first = grafos_tentativas[i].passos;
                        melhor.second = avaliacao;
                        melhor_tent = i;
                    }
                }
            }

            // guarda as melhores jogadas
            for (int c : melhor.first)
                jogadas.push_back(c);


            vector<t_grafo_tabuleiro> apaga;
            apaga.push_back(grafos_tentativas[melhor_tent]);
            grafos_tentativas = apaga;

            vector<int> apaga_passos;
            apaga_passos.push_back(grafos_tentativas[0].passos.back());
            grafos_tentativas[0].passos = apaga_passos;
            grafos_tentativas[0].passos.pop_back();
        }
    }

    // printf("\nGANHOU\n");

    // saiu do loop = achou sequencia vencedora
    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}