ShortestPathWithVisual.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <climits> // For INT_MAX
#include <cstdlib> // For rand() function
#include <ctime>   // For seeding rand()
#include <chrono> //measuring runtime
#include <stack> //For path tracing
#include <limits>
#include <algorithm>

using namespace std;
using namespace chrono;

const int INF = numeric_limits<int>::max(); // Represents infinity (no direct connection)

// Function to generate a random distance between 0 and 100
int generateRandomDistance() {
    int chance = rand() % 2;

    if(chance == 0){
        return INF;
    }else if(chance == 1){
        return 6 + (rand() % 200); // Generates a random number between 0 and 200
    }

    return INF;

    //return rand() % 101; // Generates a random number between 0 and 100


}

// Function to create a weighted graph with random distances using an adjacency matrix
vector<vector<int>> createWeightedGraph(int n) {
    // Seed the random number generator with current time
    srand(time(0));

    // Initialize the graph with n nodes and INF distances
    vector<vector<int>> graph(n, vector<int>(n, INF));

    // Fill the diagonal with zeros (no self-loops)
    for (int i = 0; i < n; ++i) {
        graph[i][i] = 0;
    }

    // Fill the graph with random distances
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int distance = generateRandomDistance();
            graph[i][j] = distance;
            graph[j][i] = distance; // Since it's an undirected graph
        }
    }

    return graph;
}

// Dijkstra's algorithm to find the shortest distance between two nodes
vector<int> dijkstra(const vector<vector<int>>& graph, int startNode, int endNode, vector<int>& shortestPath) {
    int n = graph.size();
    vector<int> distance(n, INF);
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);

    distance[startNode] = 0;

    auto startTime = high_resolution_clock::now();

    for (int i = 0; i < n - 1; ++i) {
        int minDistance = INF;
        int minIndex = -1;

        // Find the vertex with the minimum distance
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && distance[j] < minDistance) {
                minDistance = distance[j];
                minIndex = j;
            }
        }

        // Mark the selected vertex as visited
        visited[minIndex] = true;

        // Update the distance values of the adjacent vertices
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && graph[minIndex][j] != INF &&
                distance[minIndex] != INF && distance[minIndex] + graph[minIndex][j] < distance[j]) {
                distance[j] = distance[minIndex] + graph[minIndex][j];
                parent[j] = minIndex;
            }
        }
    }

    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);

    // Trace the shortest path
    int current = endNode;
    while (current != -1) {
        shortestPath.push_back(current);
        current = parent[current];
    }

    //reverse(shortestPath.begin(), shortestPath.end());
    cout << "Dijkstra's runtime: " <<duration.count() <<" microseconds" <<endl;


    return distance;
}

// Floyd-Warshall algorithm to find the shortest paths between all pairs of nodes
vector<vector<int>> floydWarshall(const vector<vector<int>>& graph, int startNode, int endNode, vector<int>& shortestPath) {
    int n = graph.size();
    vector<vector<int>> distanceMatrix = graph;
    vector<vector<int>> next(n, vector<int>(n, -1));

        auto startTime = high_resolution_clock::now();

    // Initialize the distance and next matrices
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (graph[i][j] != INF && i != j) {
                next[i][j] = j;
            }
        }
    }

    // Floyd-Warshall algorithm
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (distanceMatrix[i][k] != INF && distanceMatrix[k][j] != INF &&
                    distanceMatrix[i][k] + distanceMatrix[k][j] < distanceMatrix[i][j]) {
                    distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);

    // Trace the shortest path
    if (next[startNode][endNode] != -1) {
        int current = startNode;
        shortestPath.push_back(current);

        while (current != endNode) {
            current = next[current][endNode];
            shortestPath.push_back(current);
        }
    }

 cout << "Floyd-Warshall runtime: " << duration.count() << " microseconds" << endl;


    return distanceMatrix;
}

// Bellman-Ford algorithm to find the shortest paths from a single source node
vector<int> bellmanFord(const vector<vector<int>>& graph, int startNode, int endNode, vector<int>& shortestPath) {
    int n = graph.size();
    vector<int> distance(n, INF);
    vector<int> parent(n, -1);

    distance[startNode] = 0;

    auto startTime = high_resolution_clock::now();

    // Relax edges repeatedly
    for (int i = 0; i < n - 1; ++i) {
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (graph[u][v] != INF && distance[u] != INF && distance[u] + graph[u][v] < distance[v]) {
                    distance[v] = distance[u] + graph[u][v];
                    parent[v] = u;
                }
            }
        }
    }

    // Check for negative-weight cycles
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (graph[u][v] != INF && distance[u] != INF && distance[u] + graph[u][v] < distance[v]) {
                cout << "Graph contains negative-weight cycle. Bellman-Ford cannot be applied." << endl;
                exit(1);
            }
        }
    }

    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);

    // Trace the shortest path
    int current = endNode;
    while (current != -1) {
        shortestPath.push_back(current);
        current = parent[current];
    }
    //reverse(shortestPath.begin(), shortestPath.end());

    cout << "Bellman-Ford runtime: " << duration.count() << " microseconds" << endl;

    return distance;
}

// Function to print the adjacency matrix
void printWeightedGraph(const vector<vector<int>>& graph) {
    for (const auto& row : graph) {
        for (int distance : row) {
            if (distance == INF) {
                cout << "INF ";
            } else {
                cout << distance << " ";
            }
        }
        cout << endl;
    }
}

// Function to print the indices of the shortest path
void printShortestPath(const vector<int>& shortestPath) {
    cout << "Shortest Path: ";
    for (int node : shortestPath) {
        cout << node << " ";
    }
    cout << endl;
}

void printShortestPathRev(const vector<int>& shortestPath) {
    cout << "Shortest Path: ";

    // Iterate over the shortestPath vector in reverse order
    for (auto a = shortestPath.rbegin(); a != shortestPath.rend(); ++a) {
        cout << *a << " ";
    }

    cout << endl;
}

void generateGraphvizDOT(vector<vector<int>> weight, int size, const char* filename, vector<int> &shortest, bool r) {
    const int INF = INT_MAX;
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error opening the file " << filename << std::endl;
        return;
    }

    file << "graph G {\n";

    /*for(int x =0 ; x<shortest.size(); x++) {
        cout << shortest[x] << " ";
    }*/
    /*
    if(r){
        //reverse(shortest.begin(), shortest.end());
        int start = 0;
        int end = shortest.size() - 1;

        while (start < end) {
            // Swap elements at start and end indices
            std::swap(shortest[start], shortest[end]);

            // Move indices towards the center
            start++;
            end--;
        }
    }
*/
    for (int i = 0; i < size; ++i) {
        for (int j = i+1; j < size; ++j) {
            if (weight[i][j] != 0 && weight[i][j] != INF) {
                file << "  " << i << " -- " << j << " [label=" << weight[i][j] << "]";
                for(int k = 0; k < shortest.size()-1; ++k) {
                    if(i == shortest[k] && j == shortest[k+1] || j == shortest[k] && i ==shortest[k+1]){
                        file << " [penwidth = 5] [color = red]";
                        break;
                    }
                }
                file << ";\n";
            }
        }
    }

    file << "}\n";
    file.close();
}

//const int MAX_SIZE = 5;  // Adjust this based on your matrix size
//const int INF = INT_MAX;

int main() {
    /*int adjacencyMatrix[MAX_SIZE][MAX_SIZE] = {
            {0,   66,  INF, 191, INF},
            {66,  0,   68,  INF, 179},
            {INF, 68,  0,   136, 50},
            {191, INF, 136, 0,   INF},
            {INF, 179, 50,  INF, 0}
    };*/

    int n;

    // Get the number of nodes from the user
    cout << "Enter the number of nodes: ";
    cin >> n;

    // Create the weighted graph using an adjacency matrix
    vector<vector<int>> weightedGraph = createWeightedGraph(n);

    //Adjacency matrix
    cout << "Adjacency Matrix Created" << endl;
    //printWeightedGraph(weightedGraph);

    bool end = true;
    while (end == true) {

        string option;
        cout << "What would you like to do?: Print : Dijkstra : Floyd : Bellman : End : ";
        cin >> option;

        if (option == "Dijkstra") {

            // Get the start and end nodes for Dijkstra's algorithm
            int startValueDijkstra, endValueDijkstra;
            cout << "Enter the start node value: ";
            cin >> startValueDijkstra;
            cout << "Enter the end node value: ";
            cin >> endValueDijkstra;

            // Convert node values to indices
            int startIndexDijkstra = startValueDijkstra;
            int endIndexDijkstra = endValueDijkstra;

            // Run Dijkstra's algorithm
            vector<int> shortestPathDijkstra;
            vector<int> shortestDistancesDijkstra = dijkstra(weightedGraph, startIndexDijkstra, endIndexDijkstra,
                                                             shortestPathDijkstra);

            // Print the shortest distance and path from the start node to the end node using Dijkstra's
            cout << "Shortest distance from node " << startValueDijkstra << " to node " << endValueDijkstra
                 << " (Dijkstra's): ";
            if (shortestDistancesDijkstra[endIndexDijkstra] == INF) {
                cout << "No path exists" << endl;
            } else {
                cout << shortestDistancesDijkstra[endIndexDijkstra] << endl;
                printShortestPathRev(shortestPathDijkstra);
                generateGraphvizDOT(weightedGraph, n, "graph.dot", shortestPathDijkstra, true);
            }

        } else if (option == "Floyd") {
            //Floyd
            // Get the start and end nodes for Floyd-Warshall algorithm
            int startValueFloydWarshall, endValueFloydWarshall;
            cout << "Enter the start node value for Floyd-Warshall algorithm: ";
            cin >> startValueFloydWarshall;
            cout << "Enter the end node value for Floyd-Warshall algorithm: ";
            cin >> endValueFloydWarshall;

            // Convert node values to indices
            int startIndexFloydWarshall = startValueFloydWarshall;
            int endIndexFloydWarshall = endValueFloydWarshall;

            // Run Floyd-Warshall algorithm
            vector<int> shortestPathFloydWarshall;
            vector<vector<int>> allPairsShortestPaths = floydWarshall(weightedGraph, startIndexFloydWarshall,
                                                                      endIndexFloydWarshall, shortestPathFloydWarshall);

            // Print the shortest distance and path from the start node to the end node using Floyd-Warshall
            cout << "Shortest distance from node " << startValueFloydWarshall << " to node " << endValueFloydWarshall
                 << " (Floyd-Warshall): ";
            if (allPairsShortestPaths[startIndexFloydWarshall][endIndexFloydWarshall] == INF) {
                cout << "No path exists" << endl;
            } else {
                cout << allPairsShortestPaths[startIndexFloydWarshall][endIndexFloydWarshall] << endl;
                printShortestPath(shortestPathFloydWarshall);
                generateGraphvizDOT(weightedGraph, n, "graph.dot", shortestPathFloydWarshall, false);
            }

        } else if (option == "Bellman") {

            // Get the start and end nodes for Bellman-Ford algorithm
            int startValueBellmanFord, endValueBellmanFord;
            cout << "Enter the start node value for Bellman-Ford algorithm: ";
            cin >> startValueBellmanFord;
            cout << "Enter the end node value for Bellman-Ford algorithm: ";
            cin >> endValueBellmanFord;

            // Convert node values to indices
            int startIndexBellmanFord = startValueBellmanFord;
            int endIndexBellmanFord = endValueBellmanFord;

            // Run Bellman-Ford algorithm
            vector<int> shortestPathBellmanFord;
            vector<int> shortestDistancesBellmanFord = bellmanFord(weightedGraph, startIndexBellmanFord,
                                                                   endIndexBellmanFord, shortestPathBellmanFord);

            // Print the shortest distance and path from the start node to the end node using Bellman-Ford
            cout << "Shortest distance from node " << startValueBellmanFord << " to node " << endValueBellmanFord
                 << " (Bellman-Ford): ";
            if (shortestDistancesBellmanFord[endIndexBellmanFord] == INF) {
                cout << "No path exists" << endl;
            } else {
                cout << shortestDistancesBellmanFord[endIndexBellmanFord] << endl;
                printShortestPathRev(shortestPathBellmanFord);
                generateGraphvizDOT(weightedGraph, n, "graph.dot", shortestPathBellmanFord, true);
            }

        } else if(option == "Print"){
            //Print the adjacency matrix
            cout << "Adjacency Matrix:" << endl;
            printWeightedGraph(weightedGraph);

        }else if (option == "End") {
           end = false;
        }

        //generateGraphvizDOT(weightedGraph, n, "graph.dot");

    }
    return 0;
}
