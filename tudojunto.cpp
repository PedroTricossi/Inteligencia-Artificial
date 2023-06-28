#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <omp.h>

using namespace std;

#define PROFUNDIDADE 0

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
    vector<int> contadores_cores;
    int componentes_restantes;
}t_grafo_tabuleiro;

// Cria um tabuleiro de flood-it usando como base as informacoes do enunciado do trabalho
t_tabuleiro le_tabuleiro(const char* filename) {
    // char filename[1024];
    int i, j;
    t_tabuleiro tab;
    FILE* tabuleiro;

    tabuleiro = fopen(filename, "r");
    if (tabuleiro == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }

    if (fscanf(tabuleiro, "%d%d%d", &tab.lin, &tab.col, &tab.cor) != 3) {
        printf("Invalid file format.\n");
        exit(1);
    }

    tab.tab = (t_cell**)malloc(tab.lin * sizeof(t_cell*));
    if (tab.tab == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

    tab.tab[0] = (t_cell*)malloc(tab.lin * tab.col * sizeof(t_cell));
    if (tab.tab[0] == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }

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
                vert.distancias.push_back(-1);
                vert.distancias.push_back(-1);
                vert.distancias.push_back(-1);
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

    return;
}

int ganhou(t_vertice v, int area)
{
    if (v.area == area)
        return 1;

    return 0;
}

void flood(t_grafo_tabuleiro& grafotab, int cor)
{
    vector<int>& vizinhos = grafotab.grafo[0].vizinhos;

    for (int v = static_cast<int>(vizinhos.size()) - 1; v >= 0; --v)
    {
        t_vertice& vert_vizinho = grafotab.grafo[vizinhos[v]];

        if (vert_vizinho.cor == cor)
        {
            grafotab.componentes_restantes--;
            grafotab.contadores_cores[vert_vizinho.cor - 1]--;
            grafotab.grafo[0].area += vert_vizinho.area;

            for (int i = 0; i < 3; i++)
            {
                if (vert_vizinho.distancias[i] < grafotab.grafo[0].distancias[i])
                    grafotab.grafo[0].distancias[i] = vert_vizinho.distancias[i];
            }

            grafotab.grafo[vizinhos[v]].cor = 0;

            vizinhos.erase(vizinhos.begin() + v);

            for (int viz_viz : vert_vizinho.vizinhos)
            {
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
        {
            flood(novo_grafo, c); // esse grafo é passado por referencia!
        }

        novo_grafo.passos = tentativa;

        grafos_tentativas.push_back(novo_grafo);
    }
}

int avalia_tabuleiro(t_grafo_tabuleiro grafotab, int area, int cores)
{
    int cores_restantes = 0;
    for (int c=0; c<cores; c++)
    {
        if (grafotab.contadores_cores[c] > 0) 
            cores_restantes++;
    }
    
    int avaliacao = 0;
    avaliacao += (area - grafotab.grafo[0].area) * 1;
    avaliacao += grafotab.componentes_restantes * 1;
    avaliacao += cores_restantes * 5;
    avaliacao += grafotab.grafo[0].distancias[0] * 1; 
    avaliacao += grafotab.grafo[0].distancias[1] * 1;
    avaliacao += grafotab.grafo[0].distancias[2] * 2;  

    return avaliacao;
}

void dijkstra(int ind_root, vector<t_vertice>& grafo_real, vector<t_vertice> grafo, int canto)
{

    vector<int> nao_visitados;
    for (int i=0; i < static_cast<int>(grafo.size()); i++)
        nao_visitados.push_back(i);
    

    while(!nao_visitados.empty())
    {
        pair<int, int> menor_dist = make_pair(0, grafo[nao_visitados[0]].distancias[canto]);

        for (int nv=0; nv < static_cast<int>(nao_visitados.size()); nv++)
        {
            if (((grafo[nao_visitados[nv]].distancias[canto] < menor_dist.second) && menor_dist.second != -1 && grafo[nao_visitados[nv]].distancias[canto] != -1)
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
                if ((nova_dist < grafo[viz].distancias[canto]  && grafo[viz].distancias[canto] != -1)
                    || grafo[viz].distancias[canto] == -1)
                {
                    grafo[viz].distancias[canto] = nova_dist;
                    grafo_real[viz].distancias[canto] = nova_dist;
                }
            }
        }
    }
}

t_grafo_tabuleiro grafotab;

vector<int> jogadas;


int main(int argc, char* argv[]){
    int fim = 0;

    t_tabuleiro tab;
 
    tab = le_tabuleiro(argv[1]);

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

    for (int i=0; i<tab.cor; i++)
        grafotab.contadores_cores.push_back(0);

    grafotab.componentes_restantes = 0;
    for (t_vertice v : grafotab.grafo)
    {
        grafotab.contadores_cores[v.cor-1]++;
        grafotab.componentes_restantes++;
    }

    int area = tab.lin * tab.col;


    int numThreads = omp_get_max_threads();
    printf("threads: %d\n", numThreads);
    omp_set_num_threads(numThreads);

    vector<t_grafo_tabuleiro> grafos_tentativas;
    for (int i=1; i <= tab.cor; i++)
    {
        if (grafotab.grafo[0].cor == i)
            continue;

        vector<int> tentativa;

        tentativa.push_back(i);

                gera_grafos1(tentativa, grafotab, grafos_tentativas, tab.cor, PROFUNDIDADE);
    }

    #pragma omp taskwait


    pair<int, int> melhor = make_pair(1, INT_MAX);
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

    if (fim)
    {

        for (int j : jogadas)
            cout << "a " << j << "\n";

        return 0;
    }

    jogadas.push_back(melhor.first);


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





    printf("jogadas ruins descartadas\n");

    // Define a custom reduction for pair<int, int>
    #pragma omp declare reduction(minpair : std::pair<int, int> : \
        omp_out = (omp_in.first < omp_out.first) ? omp_in : omp_out) \
        initializer(omp_priv = std::pair<int, int>(INT_MAX, INT_MAX))

    while (!fim)
    {
        bool found_solution = false;
        int tam_tentativas = static_cast<int>(grafos_tentativas.size());
        vector<t_grafo_tabuleiro> novas_tentativas; // New tentative solutions

        #pragma omp parallel shared(fim, grafos_tentativas, jogadas, found_solution, tam_tentativas, novas_tentativas)
        {
            #pragma omp for schedule(dynamic)
            for (int c = 1; c <= tab.cor; c++)
            {
                if (found_solution)
                    continue;

                for (int t = 0; t < tam_tentativas; t++)
                {
                    if (grafos_tentativas[t].passos.back() == c)
                        continue;

                    t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
                    nova_tentativa.passos.push_back(c);
                    flood(nova_tentativa, c);

                    #pragma omp critical
                    {
                        novas_tentativas.push_back(nova_tentativa); // Store the new tentative solution
                    }

                    if (ganhou(nova_tentativa.grafo[0], area))
                    {
                        #pragma omp critical
                        {
                            fim = true;
                            found_solution = true;
                            jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                        }
                        break;
                    }
                }
            }
        }

        if (fim)
            break;

        grafos_tentativas.insert(grafos_tentativas.end(), novas_tentativas.begin(), novas_tentativas.end()); // Merge new tentative solutions

        grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);

        tam_tentativas = static_cast<int>(grafos_tentativas.size());
        if (tam_tentativas == 0)
            break;

        pair<int, int> melhor = make_pair(1, INT_MAX);

        #pragma omp parallel for reduction(minpair:melhor)
        for (int i = 0; i < tam_tentativas; i++)
        {
            int avaliacao = avalia_tabuleiro(grafos_tentativas[i], area, tab.cor);
            if (avaliacao < melhor.second)
            {
                melhor.first = grafos_tentativas[i].passos[0];
                melhor.second = avaliacao;
            }
        }

        jogadas.push_back(melhor.first);

        grafos_tentativas.erase(
            remove_if(grafos_tentativas.begin(), grafos_tentativas.end(),
                    [&](const t_grafo_tabuleiro& g) {
                        return g.passos[0] != jogadas.back();
                    }),
            grafos_tentativas.end());
    }

    for (int j : jogadas)
        cout << "a " << j << "\n";

    return 0;
}