#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "flood_it2.h"
// #include "ia.h"

using namespace std;

// Cria um tabuleiro de flood-it usando como base as informacoes do enunciado do trabalho
t_tabuleiro le_tabuleiro() {
    char filename[1024];
    int i, j;
    t_tabuleiro tab;
    FILE* tabuleiro;

    cout << "oiiiiii";

    // printf("Enter filename: ");
    // scanf("%s", filename);
    fgets(filename, 1024, stdin);

    cout << "oiiiiii";

    tabuleiro = fopen(filename, "r");
    if (tabuleiro == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    cout << "oiiiiii";

    if (fscanf(tabuleiro, "%d%d%d", &tab.lin, &tab.col, &tab.cor) != 3) {
        printf("Invalid file format.\n");
        exit(1);
    }

    cout << "reconheceu o arquivo ao menos";

    // tab.tab = malloc(tab.lin * sizeof(t_cell*));
    tab.tab = (t_cell**)malloc(tab.lin * sizeof(t_cell*));
    // tab.tab = new t_cell*[tab.lin];
    if (tab.tab == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    cout << "alocou parcialmente";

    // tab.tab[0] = malloc(tab.lin * tab.col * sizeof(t_cell));
    tab.tab[0] = (t_cell*)malloc(tab.lin * tab.col * sizeof(t_cell));
    // tab.tab[0] = new t_cell[tab.lin * tab.col];
    if (tab.tab[0] == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    cout << "alocou a memoria da matriz";

    for (i = 1; i < tab.lin; i++)
        tab.tab[i] = tab.tab[0] + i * tab.col;

    for (i = 0; i < tab.lin; i++) {
        for (j = 0; j < tab.col; j++) {
            if (fscanf(tabuleiro, "%d", &tab.tab[i][j].cor) != 1) {
                printf("Invalid file format.\n");
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
                vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice); // os dois vértices se têm como vizinhos
            }
            else
            {
                // ou se não tiver esse componente como vizinho ainda
                if ((find(vert.vizinhos.begin(), vert.vizinhos.end(), (tab.tab[i][j].cor + 1) * -1)) == vert.vizinhos.end())
                {
                    vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                    grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);  // os dois vértices se têm como vizinhos
                }
            }
        }

        return;
    }

    // se passou dos dois testes, mesma cor
    // liga a célula no novo vértice, aumentando sua área
    vert.area++;

    // anota se o vértice contém algum dos cantos
    if (i == 0 && j == tab.col-1) vert.distancias[0] = 0;
    else if (i == tab.lin-1 && j == 0) vert.distancias[1] = 0;
    else if (i == tab.lin-1 && j == tab.col-1) vert.distancias[2] = 0;

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
                vert.distancias.push_back(-1);
                vert.distancias.push_back(-1);
                vert.distancias.push_back(-1);

                // anota se o vértice contém algum dos cantos
                if (i == 0 && j == tab.col-1) vert.distancias[0] = 0;
                else if (i == tab.lin-1 && j == 0) vert.distancias[1] = 0;
                else if (i == tab.lin-1 && j == tab.col-1) vert.distancias[2] = 0;

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
    // para poder manipular o vetor sem quebrar o loop
    vector<int> vizinhos = grafotab.grafo[0].vizinhos;

    // for (t_vertice viz : vizinhos)
    for (int v=0; v < static_cast<int>(vizinhos.size()); v++)
    {
        // grafotab.grafo[vizinhos[v]] é um vértice vizinho do vértice principal
        t_vertice vert_vizinho = grafotab.grafo[vizinhos[v]];
        if (vert_vizinho.cor == cor)
        {
            grafotab.componentes_restantes--;
            grafotab.contadores_cores[vert_vizinho.cor-1]--;
            grafotab.grafo[0].area += vert_vizinho.area;

            // atualiza distancias para os cantos
            for (int i=0; i<3; i++)
            {
                if (vert_vizinho.distancias[i] < grafotab.grafo[0].distancias[i])
                    grafotab.grafo[0].distancias[i] = vert_vizinho.distancias[i];
            }

            // evitar de adicionar esse vertice novamente à vizinhança
            // no caso de ele ser vizinho de outro vizinho futuro
            grafotab.grafo[vizinhos[v]].cor = 0;

            grafotab.grafo[0].vizinhos.erase(grafotab.grafo[0].vizinhos.begin() + v);

            // adicionar vizinhanca do vertice floodado
            for (int viz_viz : vert_vizinho.vizinhos)
            {
                // verificar se já nao tem o vizinho
                if (viz_viz != 0 && grafotab.grafo[viz_viz].cor != 0)
                    grafotab.grafo[0].vizinhos.push_back(viz_viz);
            }
        }
    }
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
        
        for (int c : tentativa)
            flood(novo_grafo, c); // esse grafo é passado por referencia!

        // grafos_tentativas.tentativa.passos.push_back(tentativa);
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
    avaliacao += (area - grafotab.grafo[0].area);
    avaliacao += grafotab.componentes_restantes;
    avaliacao += cores_restantes;
    avaliacao += grafotab.grafo[0].distancias[0]; 
    avaliacao += grafotab.grafo[0].distancias[1];
    avaliacao += grafotab.grafo[0].distancias[2];  

    return avaliacao;
}

void dijkstra(int ind_root, vector<t_vertice>& grafo, int canto)
{
    vector<int> nao_visitados;
    for (int i=0; i < static_cast<int>(grafo.size()); i++)
        nao_visitados.push_back(i);
    
    while(!nao_visitados.empty())
    {
        // indice nao_visitados, distancia
        pair<int, int> menor_dist = make_pair(0, grafo[nao_visitados[0]].distancias[canto]);
        // pair<int, int> menor_dist = ()

        // procura vertice com menor distancia entre os nao visitados
        for (int nv : nao_visitados)
        {
            if (grafo[nao_visitados[nv]].distancias[canto] < menor_dist.second
                || menor_dist.second == -1)
            {
                menor_dist.first = nv;
                menor_dist.second = grafo[nao_visitados[nv]].distancias[canto];
            }
        }

        int atual = nao_visitados[menor_dist.first];

        nao_visitados.erase(nao_visitados.begin() + menor_dist.first);
        grafo[atual].visitado = 1;

        for (int viz : grafo[atual].vizinhos)
        {
            if (grafo[viz].visitado == 0)
            {
                int nova_dist = grafo[atual].distancias[canto] + 1;
                if (nova_dist < grafo[viz].distancias[canto]
                    || grafo[viz].distancias[canto] == -1)
                    grafo[viz].distancias[canto] = nova_dist;
            }
        }
    }
}