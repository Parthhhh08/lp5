#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

class Graph {
private:
    int numVertices;
    vector<vector<int>> adj;

public:
    Graph(int vertices) : numVertices(vertices), adj(vertices) {}

    void addEdge(int src, int dest) {
        adj[src].push_back(dest);
        adj[dest].push_back(src);
    }

    void viewGraph() {
        cout << "Graph:\n";
        for (int i = 0; i < numVertices; i++) {
            cout << "Vertex " << i << " -> ";
            for (int neighbor : adj[i]) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
    }

    void bfs(int startVertex) {
        vector<bool> visited(numVertices, false);
        queue<int> q;

        visited[startVertex] = true;
        q.push(startVertex);

        while (!q.empty()) {
            int currentVertex;

            // Critical to safely pop from shared queue
            #pragma omp critical
            {
                if (!q.empty()) {
                    currentVertex = q.front();
                    q.pop();
                    cout << currentVertex << " ";
                } else {
                    return;
                }
            }

            // Parallel loop to check neighbors
            #pragma omp parallel for
            for (int i = 0; i < adj[currentVertex].size(); i++) {
                int neighbor = adj[currentVertex][i];
                bool doVisit = false;

                // Lock to safely check and update visited
                #pragma omp critical
                {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        doVisit = true;
                    }
                }

                if (doVisit) {
                    #pragma omp critical
                    {
                        q.push(neighbor);
                    }
                }
            }
        }
    }

    void dfs(int startVertex) {
        vector<bool> visited(numVertices, false);
        stack<int> s;

        visited[startVertex] = true;
        s.push(startVertex);

        while (!s.empty()) {
            int currentVertex;

            // Critical to safely pop from shared stack
            #pragma omp critical
            {
                if (!s.empty()) {
                    currentVertex = s.top();
                    s.pop();
                    cout << currentVertex << " ";
                } else {
                    return;
                }
            }

            // Parallel loop to check neighbors
            #pragma omp parallel for
            for (int i = 0; i < adj[currentVertex].size(); i++) {
                int neighbor = adj[currentVertex][i];
                bool doVisit = false;

                // Critical section for visited update
                #pragma omp critical
                {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        doVisit = true;
                    }
                }

                if (doVisit) {
                    #pragma omp critical
                    {
                        s.push(neighbor);
                    }
                }
            }
        }
    }
};

int main() {
    int numVertices;
    cout << "Enter the number of vertices in the graph: ";
    cin >> numVertices;

    Graph graph(numVertices);

    int numEdges;
    cout << "Enter the number of edges in the graph: ";
    cin >> numEdges;

    cout << "Enter the edges (source destination):\n";
    for (int i = 0; i < numEdges; i++) {
        int src, dest;
        cin >> src >> dest;
        graph.addEdge(src, dest);
    }

    graph.viewGraph();

    int startVertex;
    cout << "Enter the starting vertex for BFS and DFS: ";
    cin >> startVertex;

    cout << "Breadth First Search (BFS): ";
    graph.bfs(startVertex);
    cout << endl;

    cout << "Depth First Search (DFS): ";
    graph.dfs(startVertex);
    cout << endl;

    return 0;
}
