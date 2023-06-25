#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>  // Add this line

using namespace std;

class Graph {
private:
    struct Vertex {
        int color;
        int area;
        vector<int> neighbors;

        Vertex(int c = 0, int a = 0) : color(c), area(a) {}
    };

    vector<Vertex> vertices;
    int numColors;

public:
    Graph(int n, int colors) : vertices(n), numColors(colors) {}

    void addEdge(int u, int v) {
        vertices[u].neighbors.push_back(v);
        vertices[v].neighbors.push_back(u);
    }

    void setColor(int u, int color) {
        vertices[u].color = color;
    }

    int getColor(int u) {
        return vertices[u].color;
    }

    int getArea(int u) {
        return vertices[u].area;
    }

    void updateArea(int u, int area) {
        vertices[u].area = area;
    }

    void floodFill(int u, int targetColor, int newColor) {
        if (vertices[u].color != targetColor) {
            return;
        }

        queue<int> q;
        q.push(u);
        vector<bool> visited(vertices.size(), false);

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            visited[current] = true;
            vertices[current].color = newColor;

            // acho que nao faz sentido isso daqui
            for (int neighbor : vertices[current].neighbors) {
                if (!visited[neighbor] && vertices[neighbor].color == targetColor) {
                    q.push(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
    }

    void calculateAreas() {
        vector<bool> visited(vertices.size(), false);

        for (int i = 0; i < vertices.size(); i++) {
            if (!visited[i]) {
                int color = vertices[i].color;
                int area = dfs(i, color, visited);
                updateArea(i, area);
            }
        }
    }

    int dfs(int u, int targetColor, vector<bool>& visited) {
        visited[u] = true;
        int area = 1;

        for (int neighbor : vertices[u].neighbors) {
            if (!visited[neighbor] && vertices[neighbor].color == targetColor) {
                area += dfs(neighbor, targetColor, visited);
            }
        }

        return area;
    }

    bool isAllSameColor() {
        int color = vertices[0].color;

        for (int i = 1; i < vertices.size(); i++) {
            if (vertices[i].color != color) {
                return false;
            }
        }

        return true;
    }

    void printBoard() {
        int sqrtN = sqrt(vertices.size());

        for (int i = 0; i < vertices.size(); i++) {
            if (i % sqrtN == 0 && i != 0) {
                cout << endl;
            }

            cout << vertices[i].color << " ";
        }

        cout << endl;
    }
};

int main() {
    int N = 16; // Board size (N x N)
    int numColors = 4; // Number of colors

    Graph graph(N, numColors);

    // Create the graph connections
    for (int i = 0; i < N; i++) {
        if (i + 1 < N && (i + 1) % static_cast<int>(sqrt(N)) != 0) {
            graph.addEdge(i, i + 1);
        }

        if (i + static_cast<int>(sqrt(N)) < N) {
            graph.addEdge(i, i + static_cast<int>(sqrt(N)));
        }
    }

    // Set initial colors
    for (int i = 0; i < N; i++) {
        int color = rand() % numColors;
        graph.setColor(i, color);
    }

    // Play the game
    int moves = 0;

    while (!graph.isAllSameColor()) {
        graph.calculateAreas();
        graph.printBoard();

        int targetColor = graph.getColor(0);
        int newColor;

        do {
            cout << "Enter a color (0-" << numColors - 1 << "): ";
            cin >> newColor;
        } while (newColor < 0 || newColor >= numColors);

        graph.floodFill(0, targetColor, newColor);
        moves++;
    }

    cout << "Congratulations! You won in " << moves << " moves." << endl;

    return 0;
}