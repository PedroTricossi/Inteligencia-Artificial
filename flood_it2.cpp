#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

int p1 = 1;
int p2 = 2;
int p3 = 100;
int p4 = 1;
int p5 = 1;
int p6 = 3;

// Cria um tabuleiro de flood-it usando como base as informacoes do enunciado do trabalho
t_tabuleiro le_tabuleiro(const char* filename, int p1, int p2, int p3, int p4, int p5, int p6) {
    // char filename[1024];
    int i, j;
    t_tabuleiro tab;
    FILE* tabuleiro;

    // pesos parametros
    if (p1 != -1)
        ::p1 = p1;
    if (p2 != -1)
        ::p2 = p2;
    if (p3 != -1)
        ::p3 = p3;
    if (p4 != -1)
        ::p4 = p4;
    if (p5 != -1)
        ::p5 = p5;
    if (p6 != -1)
        ::p6 = p6;

    // cout << "oiiiiii";

    // printf("Digite o nome do arquivo: ");
    // scanf("%s", filename);
    // fgets(filename, 1024, stdin);

    // cout << "oiiiiii";

    tabuleiro = fopen(filename, "r");
    if (tabuleiro == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    // cout << "oiiiiii";

    if (fscanf(tabuleiro, "%d%d%d", &tab.lin, &tab.col, &tab.cor) != 3) {
        printf("Formato inválido.\n");
        exit(1);
    }

    // cout << "reconheceu o arquivo ao menos";

    // tab.tab = malloc(tab.lin * sizeof(t_cell*));
    tab.tab = (t_cell**)malloc(tab.lin * sizeof(t_cell*));
    // tab.tab = new t_cell*[tab.lin];
    if (tab.tab == NULL) {
        printf("Erro malloc.\n");
        exit(1);
    }

    // cout << "alocou parcialmente";

    // tab.tab[0] = malloc(tab.lin * tab.col * sizeof(t_cell));
    tab.tab[0] = (t_cell*)malloc(tab.lin * tab.col * sizeof(t_cell));
    // tab.tab[0] = new t_cell[tab.lin * tab.col];
    if (tab.tab[0] == NULL) {
        printf("Erro malloc.\n");
        exit(1);
    }

    // cout << "alocou a memoria da matriz";

    for (i = 1; i < tab.lin; i++)
        tab.tab[i] = tab.tab[0] + i * tab.col;

    for (i = 0; i < tab.lin; i++) {
        for (j = 0; j < tab.col; j++) {
            if (fscanf(tabuleiro, "%d", &tab.tab[i][j].cor) != 1) {
                printf("Formato inválido.\n");
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

void liga_componente(int i, int j, t_tabuleiro& tab, t_vertice& vert, vector<t_vertice>& grafo)
{
    if (i < 0 || i >= tab.lin || j < 0 || j >= tab.col)
        return;

    if (tab.tab[i][j].cor != vert.cor)
    {
        // se célula pertence a um compontente diferente
        if (tab.tab[i][j].cor < 0 && tab.tab[i][j].cor != ((vert.indice + 1) * -1))
        {
            // se não tem nenhum vizinho
            if (vert.vizinhos.empty())
            {
                if (tab.tab[i][j].cor != -1)
                {
                    vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                    if (vert.indice != 0) // O vértice 0 não deve estar na vizinhança de ninguém!
                        grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
                     // os dois vértices se têm como vizinhos
                }
                else // caso seja do vértice 0. O vértice 0 não deve estar na vizinhança de ninguém!
                {
                    int ja_tem = 0;
                    ja_tem = (find(grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.begin(), 
                        grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.end(), 
                        vert.indice) == grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.end());
                    
                    if (ja_tem)
                        grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
                }
            }
            else
            {
                // ou se não tiver esse componente como vizinho ainda
                if ((find(vert.vizinhos.begin(), vert.vizinhos.end(), (tab.tab[i][j].cor + 1) * -1)) == vert.vizinhos.end())
                {
                    if (tab.tab[i][j].cor != -1)
                    {
                        vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                        if (vert.indice != 0) // O vértice 0 não deve estar na vizinhança de ninguém!
                            grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
                        // os dois vértices se têm como vizinhos
                    }
                    else // caso seja o vértice 0. O vértice 0 não deve estar na vizinhança de ninguém!
                    {
                        int ja_tem = 0;
                        ja_tem = (find(grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.begin(), 
                            grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.end(), 
                            vert.indice) == grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.end());
                        
                        if (ja_tem)
                            grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
                    }
                }
            }
        }

        return;
    }

    // se passou dos dois testes, mesma cor
    // liga a célula no novo vértice, aumentando sua área
    vert.area++;

    // anota se o vértice contém algum dos cantos
    if (i == 0 && j == tab.col-1)
    {
        vert.distancias[0] = 0;
        // cout << "SD" << endl;
    } 
    else if (i == tab.lin-1 && j == 0)
    {
        vert.distancias[1] = 0;
        // cout << "IE" << endl;
    } 
    else if (i == tab.lin-1 && j == tab.col-1)
    {
        vert.distancias[2] = 0;
        // cout << "ID" << endl;
    }

    // pode marcar a célula como visitada já
    // e parte do componente em questao
    tab.tab[i][j].cor = (vert.indice + 1) * -1;

    liga_componente(i-1, j, tab, vert, grafo);
    liga_componente(i, j+1, tab, vert, grafo);
    liga_componente(i+1, j, tab, vert, grafo);
    liga_componente(i, j-1, tab, vert, grafo);

    return;
}

void mat2graph(t_tabuleiro& tab, vector<t_vertice>& grafo)
{
    for (int i=0; i < tab.lin; i++)
    {
        for (int j=0; j < tab.col; j++)
        {
            if (tab.tab[i][j].cor >= 0) // a célula ainda não foi visitada
            {
                t_vertice vert;
                vert.indice = static_cast<int>(grafo.size());
                vert.cor = tab.tab[i][j].cor;
                vert.area = 1;
                vert.distancias.push_back(INT_MAX);
                vert.distancias.push_back(INT_MAX);
                vert.distancias.push_back(INT_MAX);
                vert.visitado = 0;

                // anota se o vértice contém algum dos cantos
                if (i == 0 && j == tab.col-1)
                {
                    vert.distancias[0] = 0;
                    // cout << "SD" << endl;
                } 
                else if (i == tab.lin-1 && j == 0)
                {
                    vert.distancias[1] = 0;
                    // cout << "IE" << endl;
                } 
                else if (i == tab.lin-1 && j == tab.col-1)
                {
                    vert.distancias[2] = 0;
                    // cout << "ID" << endl;
                }

                // esquema para indicar a que componente os ja visitados pertencem
                tab.tab[i][j].cor = (vert.indice + 1) * -1; //(vert.indice + 1) * -1; // considerar que as cores comecam em 1, indices em 0

                // vai tentar se ligar às células nas quatro direções
                liga_componente(i-1, j, tab, vert, grafo);
                liga_componente(i, j+1, tab, vert, grafo);
                liga_componente(i+1, j, tab, vert, grafo);
                liga_componente(i, j-1, tab, vert, grafo);


                grafo.push_back(vert);
            }
        }
    }
    // grafo[0].flood = 1

    return;
}

//verifica se já ganhou o jogo
int ganhou(t_vertice v, int area)
{
    if (v.area == area)
        return 1;

    return 0;
}

void flood(t_grafo_tabuleiro& grafotab, int cor)
{
    // printf("entrou flood\n");
    // para poder manipular o vetor sem quebrar o loop
    vector<int> vizinhos = grafotab.grafo[0].vizinhos;
    // for (int v=0; v < static_cast<int>(vizinhos.size()); v++)
    //     printf("vizinho: %d\n",vizinhos[v]);


    for (auto v = vizinhos.begin(); v != vizinhos.end(); v++)
    {
        // grafotab.grafo[vizinhos[v]] é um vértice vizinho do vértice principal
        t_vertice vert_vizinho = grafotab.grafo[*v];
        if (vert_vizinho.cor == cor)
        {
            // printf("floodar vertice: %d %d\n", vert_vizinho.area, cor);
            grafotab.componentes_restantes--;
            grafotab.contadores_cores[vert_vizinho.cor-1]--;
            grafotab.grafo[0].area += vert_vizinho.area;

            // atualiza distancias para os cantos
            // printf("atualiza cantos\n");
            for (int i=0; i<3; i++)
            {
                if (vert_vizinho.distancias[i] < grafotab.grafo[0].distancias[i])
                    grafotab.grafo[0].distancias[i] = vert_vizinho.distancias[i];
            }

            // evitar de adicionar esse vertice novamente à vizinhança
            // no caso de ele ser vizinho de outro vizinho futuro
            // printf("muda cor %d\n", grafotab.grafo[*v].indice);
            grafotab.grafo[*v].cor = 0;

            // printf("rouba vizinhanca dele\n");
            // adicionar vizinhanca do vertice floodado
            for (int viz_viz : vert_vizinho.vizinhos)
            {
                // verificar se já nao tem o vizinho
                // printf("viz_viz %d\n", viz_viz);
                int ja_tem = 0;
                if (find(grafotab.grafo[0].vizinhos.begin(), grafotab.grafo[0].vizinhos.end(), viz_viz) != grafotab.grafo[0].vizinhos.end())
                    ja_tem = 1;

                // printf("ja tem: %d\n", ja_tem);   
                if (ja_tem == 0)//grafotab.grafo[viz_viz].cor != 0)
                {
                    grafotab.grafo[0].vizinhos.push_back(viz_viz);
                }

            }
        }
    }
    vector<int>novos_vizinhos;
    for (int v=0; v < static_cast<int>(grafotab.grafo[0].vizinhos.size()); v++)
    {
        // printf("\tapagar? id: %d cor: %d\n", grafotab.grafo[grafotab.grafo[0].vizinhos[v]].indice, grafotab.grafo[grafotab.grafo[0].vizinhos[v]].cor);
        if (grafotab.grafo[grafotab.grafo[0].vizinhos[v]].cor != 0)
            novos_vizinhos.push_back(grafotab.grafo[0].vizinhos[v]);
    }
    grafotab.grafo[0].vizinhos = novos_vizinhos;


    // vector<int> vizinhos2 = grafotab.grafo[0].vizinhos;
    // for (int v=0; v < static_cast<int>(vizinhos2.size()); v++)
    //     printf("vizinho2: %d\n",vizinhos2[v]);

}


void gera_grafos1(vector<int> tentativa, t_grafo_tabuleiro grafo_original, vector<t_grafo_tabuleiro>& grafos_tentativas, int cores, int profundidade)
{
    if (profundidade > 0)
    {
        for(int c=1; c <= cores; c++)
        {
            if (c == tentativa.back()) // NAO PASSAR VAZIA: loop que chama para cada cor na main
                continue; // nao pode duas da mesma cor na sequencia

            tentativa.push_back(c);
            gera_grafos1(tentativa, grafo_original, grafos_tentativas, cores, profundidade-1);
            tentativa.pop_back();
        }
    }
    else
    {
        t_grafo_tabuleiro novo_grafo = grafo_original;
        // cout << novo_grafo.componentes_restantes << endl;
        for (int c : tentativa)
        {
            // cout << c;
            flood(novo_grafo, c); // esse grafo é passado por referencia!
        }

        novo_grafo.passos = tentativa;

        grafos_tentativas.push_back(novo_grafo);
    }
}

// pontuação do tabuleiro de acordo com as heurísticas definidas
int avalia_tabuleiro(t_grafo_tabuleiro grafotab, int area, int cores)
{
    int cores_restantes = 0;
    for (int c=0; c<cores; c++)
    {
        if (grafotab.contadores_cores[c] > 0) 
            cores_restantes++;
    }
    
    int avaliacao = 0;
    avaliacao += (area - grafotab.grafo[0].area) * p1;
    avaliacao += grafotab.componentes_restantes * p2;
    avaliacao += cores_restantes * p3;
    avaliacao += grafotab.grafo[0].distancias[0] * p4; 
    avaliacao += grafotab.grafo[0].distancias[1] * p5;
    avaliacao += grafotab.grafo[0].distancias[2] * p6;  

    return avaliacao;
}

void dijkstra(int ind_root, vector<t_vertice>& grafo_real, vector<t_vertice> grafo, int canto)
{
    // printf("entrou dijkstra\n");

    vector<int> nao_visitados;
    for (int i=0; i < static_cast<int>(grafo.size()); i++)
        nao_visitados.push_back(i);
    
    // printf("colocou nao visitados\n");

    while(!nao_visitados.empty())
    {
        // indice nao_visitados, distancia
        pair<int, int> menor_dist = make_pair(0, grafo[nao_visitados[0]].distancias[canto]);
        // printf("dist par %d\n", grafo[nao_visitados[0]].distancias[canto]);

        // procura vertice com menor distancia entre os nao visitados
        for (int nv=0; nv < static_cast<int>(nao_visitados.size()); nv++)
        {
            if (grafo[nao_visitados[nv]].distancias[canto] < menor_dist.second)
            {
                // printf("mudou!\n");
                menor_dist.first = nv;
                menor_dist.second = grafo[nao_visitados[nv]].distancias[canto];
            }
        }

        // printf("achou menor vertice: %d %d\n", menor_dist.first, menor_dist.second);

        int atual = nao_visitados[menor_dist.first];
        // printf("\tdist atual: %d\n", grafo[atual].distancias[canto]);

        // printf("apagar %d\n", menor_dist.second);
        nao_visitados.erase(nao_visitados.begin() + menor_dist.first);
        // printf("\tapagou %d\n", menor_dist.second);
        grafo[atual].visitado = 1;

        for (int viz : grafo[atual].vizinhos)
        {
            if (grafo[viz].visitado == 0)
            {
                int nova_dist = grafo[atual].distancias[canto] + 1;
                // printf("\t\tnova_dist: %d\n", grafo[atual].distancias[canto]);
                if (nova_dist < grafo[viz].distancias[canto])
                {
                    grafo[viz].distancias[canto] = nova_dist;
                    grafo_real[viz].distancias[canto] = nova_dist;
                }
            }
        }
    }
}

// void dijkstra(int ind_root, vector<t_vertice>& grafo_real, vector<t_vertice> grafo, int canto)
// {
//     // Store indices of unvisited vertices
//     vector<int> nao_visitados;
//     nao_visitados.reserve(grafo.size());

//     for (int i = 0; i < static_cast<int>(grafo.size()); i++)
//         nao_visitados.push_back(i);

//     while (!nao_visitados.empty())
//     {
//         // Find vertex with the smallest distance among the unvisited vertices
//         int menor_dist_index = 0;
//         int menor_dist_value = grafo[nao_visitados[0]].distancias[canto];

//         for (int nv = 1; nv < static_cast<int>(nao_visitados.size()); nv++)
//         {
//             int dist_value = grafo[nao_visitados[nv]].distancias[canto];
//             if ((dist_value < menor_dist_value || menor_dist_value == -1) && dist_value != -1)
//             {
//                 menor_dist_index = nv;
//                 menor_dist_value = dist_value;
//             }
//         }

//         // Process the vertex with the smallest distance
//         int atual = nao_visitados[menor_dist_index];
//         nao_visitados.erase(nao_visitados.begin() + menor_dist_index);
//         grafo[atual].visitado = 1;

//         // Update distances of neighboring vertices
//         for (int viz : grafo[atual].vizinhos)
//         {
//             if (grafo[viz].visitado == 0)
//             {
//                 int nova_dist = grafo[atual].distancias[canto] + 1;
//                 if ((nova_dist < grafo[viz].distancias[canto] && grafo[viz].distancias[canto] != -1) || grafo[viz].distancias[canto] == -1)
//                 {
//                     grafo[viz].distancias[canto] = nova_dist;
//                     grafo_real[viz].distancias[canto] = nova_dist;
//                 }
//             }
//         }
//     }
// }