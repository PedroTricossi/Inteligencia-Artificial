#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>

using namespace std;

#define PROFUNDIDADE 0

struct Cell {
    int flood;
    int cor;
};

struct Vertex {
    int visitado;
    int indice;
    int cor;
    int area;
    vector<int> distancias;
    vector<int> vizinhos;
};

struct Board {
    int lin;
    int col;
    int cor;
    vector<vector<Cell>> tab;
};

struct GraphBoard {
    vector<int> passos;
    vector<Vertex> grafo;
    vector<int> contadores_cores;
    int componentes_restantes;
};

Board le_tabuleiro(const char* filename) {

    int i, j;
    Board tab;
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

    tab.tab.resize(tab.lin, vector<Cell>(tab.col));

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

int verifica_borda(const Board& tab, int x, int y) {
    return (x < 0 || x > (tab.col - 1) || y < 0 || y > (tab.lin - 1));
}

void liga_componente(int i, int j, Board& tab, Vertex& vert, vector<Vertex>& grafo) {
    printf("entrou liga\n");
    if (i < 0 || i >= tab.lin || j < 0 || j >= tab.col)
        return;

    if (tab.tab[i][j].cor != vert.cor) {
        if (tab.tab[i][j].cor < 0 && tab.tab[i][j].cor != ((vert.indice + 1) * -1)) {
            if (vert.vizinhos.empty()) {
                vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
            } else {
                if ((find(vert.vizinhos.begin(), vert.vizinhos.end(), (tab.tab[i][j].cor + 1) * -1)) == vert.vizinhos.end()) {
                    vert.vizinhos.push_back((tab.tab[i][j].cor + 1) * -1);
                    grafo[(tab.tab[i][j].cor + 1) * -1].vizinhos.push_back(vert.indice);
                }
            }
        }
        return;
    }

    vert.area++;

    if (i == 0 && j == tab.col-1) {
        vert.distancias[0] = 0;
    } else if (i == tab.lin-1 && j == tab.col-1) {
        vert.distancias[1] = 0;
    } else if (i == tab.lin-1 && j == 0) {
        vert.distancias[2] = 0;
    } else if (i == 0 && j == 0) {
        vert.distancias[3] = 0;
    }

    if (tab.tab[i][j].flood == 0) {
        tab.tab[i][j].flood = 1;
        vert.visitado = 1;

        liga_componente(i + 1, j, tab, vert, grafo);
        liga_componente(i - 1, j, tab, vert, grafo);
        liga_componente(i, j + 1, tab, vert, grafo);
        liga_componente(i, j - 1, tab, vert, grafo);
    }
}

GraphBoard mat2graph(Board& tab) {
    printf("entrou mat2graph\n");
    int i, j, cor;
    vector<Vertex> grafo(tab.cor + 1);
    GraphBoard graphBoard;
    graphBoard.componentes_restantes = tab.cor;

    for (cor = 1; cor <= tab.cor; cor++) {
        grafo[cor].cor = cor;
        grafo[cor].indice = cor;
        grafo[cor].visitado = 0;
        grafo[cor].area = 0;
        grafo[cor].distancias.resize(4, INT_MAX);
        grafo[cor].vizinhos.clear();
    }

    for (i = 0; i < tab.lin; i++) {
        for (j = 0; j < tab.col; j++) {
            if (tab.tab[i][j].cor > 0) {
                int cor = tab.tab[i][j].cor;
                if (grafo[cor].visitado == 0) {
                    liga_componente(i, j, tab, grafo[cor], grafo);
                    graphBoard.contadores_cores.push_back(cor);
                }
            }
        }
    }

    graphBoard.grafo = move(grafo);
    graphBoard.passos.resize(tab.cor + 1);

    return graphBoard;
}

void dijkstra(GraphBoard& graphBoard) {
    printf("entrou dijkstra\n");
    vector<int> distancias(graphBoard.grafo.size(), INT_MAX);
    vector<int> visitados(graphBoard.grafo.size(), 0);
    int no_atual = 1;

    distancias[no_atual] = 0;

    while (no_atual != 0) {
        visitados[no_atual] = 1;
        for (int viz : graphBoard.grafo[no_atual].vizinhos) {
            if (visitados[viz] == 0) {
                int dist = graphBoard.grafo[no_atual].distancias[viz-1];
                if (distancias[viz] > distancias[no_atual] + dist) {
                    distancias[viz] = distancias[no_atual] + dist;
                    graphBoard.passos[viz] = no_atual;
                }
            }
        }

        int min_dist = INT_MAX;
        no_atual = 0;
        for (vector<Vertex>::size_type i = 1; i < graphBoard.grafo.size(); i++) {
            if (visitados[i] == 0 && distancias[i] < min_dist) {
                min_dist = distancias[i];
                no_atual = i;
            }
        }
    }
}

void pinta_tabuleiro(Board& tab, GraphBoard& graphBoard, int cor_atual, int cor_alvo) {
    vector<int> pilha;
    int i, j;

    for (i = 0; i < tab.lin; i++) {
        for (j = 0; j < tab.col; j++) {
            if (tab.tab[i][j].cor == cor_atual) {
                tab.tab[i][j].cor = cor_alvo;
                pilha.push_back(i);
                pilha.push_back(j);
            }
        }
    }

    while (!pilha.empty()) {
        int col = pilha.back();
        pilha.pop_back();
        int lin = pilha.back();
        pilha.pop_back();

        if (!verifica_borda(tab, lin, col - 1) && tab.tab[lin][col - 1].cor == cor_atual) {
            tab.tab[lin][col - 1].cor = cor_alvo;
            pilha.push_back(lin);
            pilha.push_back(col - 1);
        }

        if (!verifica_borda(tab, lin, col + 1) && tab.tab[lin][col + 1].cor == cor_atual) {
            tab.tab[lin][col + 1].cor = cor_alvo;
            pilha.push_back(lin);
            pilha.push_back(col + 1);
        }

        if (!verifica_borda(tab, lin - 1, col) && tab.tab[lin - 1][col].cor == cor_atual) {
            tab.tab[lin - 1][col].cor = cor_alvo;
            pilha.push_back(lin - 1);
            pilha.push_back(col);
        }

        if (!verifica_borda(tab, lin + 1, col) && tab.tab[lin + 1][col].cor == cor_atual) {
            tab.tab[lin + 1][col].cor = cor_alvo;
            pilha.push_back(lin + 1);
            pilha.push_back(col);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    Board tab = le_tabuleiro(input_file);

    GraphBoard graphBoard = mat2graph(tab);

    dijkstra(graphBoard);

    int cor_atual = 1;

    while (graphBoard.componentes_restantes > 1) {
        int cor_alvo = graphBoard.passos[cor_atual];
        pinta_tabuleiro(tab, graphBoard, cor_atual, cor_alvo);
        cor_atual = cor_alvo;
        graphBoard.componentes_restantes--;
    }

    FILE* output = fopen(output_file, "w");
    if (output == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    fprintf(output, "%d %d\n", tab.lin, tab.col);

    for (int i = 0; i < tab.lin; i++) {
        for (int j = 0; j < tab.col; j++) {
            fprintf(output, "%d ", tab.tab[i][j].cor);
        }
        fprintf(output, "\n");
    }

    fclose(output);

    return 0;
}