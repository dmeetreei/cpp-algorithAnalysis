#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

using namespace std;

const int NUM_CITIES = 10;
const int NUM_ANTS = 3;
const int NUM_DAYS = 2;

const double ALPHA = 0.01;
const double BETA = 1.0;
const double RO = 0.5;
const double MIN_PHER = 0.001;

// создаём матрицу расстояний
vector<vector<int>> generateGraph() {
    vector<vector<int>> graph;
    for (int i = 0; i < NUM_CITIES; i++) {
        vector<int> row;
        for (int j = 0; j < NUM_CITIES; j++) {
            row.push_back(0);
        }
        graph.push_back(row);
    }

    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = i + 1; j < NUM_CITIES; j++) {
            int distance = rand() % 15 + 1;
            graph[i][j] = distance;
            graph[j][i] = distance;
        }
    }
    return graph;
}

// создаём матрицу феромонов
vector<vector<double>> initializePheromones() {
    vector<vector<double>> pheromones;
    for (int i = 0; i < NUM_CITIES; i++) {
        vector<double> row;
        for (int j = 0; j < NUM_CITIES; j++) {
            row.push_back(1.0);
        }
        pheromones.push_back(row);
    }
    return pheromones;
}

// считаем Q как среднее расстояние между городами
double calculateQ(const vector<vector<int>>& graph) {
    double sum_distance = 0;
    int num_roads = 0;
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = i + 1; j < NUM_CITIES; j++) {
            sum_distance += graph[i][j];
            num_roads++;
        }
    }
    return sum_distance / num_roads;
}

// выбираем следующий город для муравья
int chooseNextCity(int current_city, const vector<bool>& visited, const vector<vector<double>>& pher, const vector<vector<int>>& graph) {
    vector<pair<int, double>> probs;
    double total = 0;

    for (int i = 0; i < NUM_CITIES; i++) {
        if (!visited[i]) {
            double tau = pow(pher[current_city][i], ALPHA);
            double eta = pow(1.0 / graph[current_city][i], BETA);
            double prob = tau * eta;
            probs.push_back(make_pair(i, prob));
            total += prob;
        }
    }

    if (total == 0) {
        vector<int> candidates;
        for (int i = 0; i < NUM_CITIES; i++) {
            if (!visited[i]) {
                candidates.push_back(i);
            }
        }
        return candidates[rand() % candidates.size()];
    }

    double rand_val = (double)rand() / RAND_MAX;
    double cumulative = 0;

    for (int i = 0; i < probs.size(); i++) {
        cumulative += probs[i].second / total;
        if (rand_val <= cumulative) {
            return probs[i].first;
        }
    }

    return probs[probs.size() - 1].first;
}

int main() {
    srand(time(0));

    vector<vector<int>> graph = generateGraph();
    vector<vector<double>> pher = initializePheromones();
    double Q = calculateQ(graph);

    cout << "Матрица смежности:\n";
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = 0; j < NUM_CITIES; j++) {
            cout << graph[i][j] << " ";
        }
        cout << endl;
    }

    vector<int> best_path;
    double best_length = numeric_limits<double>::max();

    for (int day = 0; day < NUM_DAYS; day++) {
        for (int ant = 0; ant < NUM_ANTS; ant++) {
            int start_city = 0;
            int current_city = start_city;

            vector<bool> visited(NUM_CITIES, false);
            visited[current_city] = true;
            vector<int> path;
            path.push_back(current_city);
            double path_length = 0;

            for (int step = 1; step < NUM_CITIES; step++) {
                int next_city = chooseNextCity(current_city, visited, pher, graph);
                path_length += graph[current_city][next_city];
                visited[next_city] = true;
                path.push_back(next_city);
                current_city = next_city;
            }

            path_length += graph[current_city][start_city];
            path.push_back(start_city);

            double d_pher = Q / path_length;
            for (int i = 0; i < path.size() - 1; i++) {
                int from = path[i];
                int to = path[i + 1];
                pher[from][to] += d_pher;
                pher[to][from] += d_pher;
            }

            if (path_length < best_length) {
                best_length = path_length;
                best_path = path;
            }

            cout << "\nОбщая длина: " << path_length << endl;
            cout << "Пройденный путь: ";
            for (int i = 0; i < path.size(); i++) {
                cout << path[i] << " ";
            }
            cout << endl;
        }

        for (int i = 0; i < NUM_CITIES; i++) {
            for (int j = 0; j < NUM_CITIES; j++) {
                pher[i][j] *= (1.0 - RO);
                if (pher[i][j] < MIN_PHER) {
                    pher[i][j] = MIN_PHER;
                }
            }
        }
    }

    cout << "\nЛучший путь: ";
    for (int i = 0; i < best_path.size(); i++) {
        cout << best_path[i] << " ";
    }
    cout << "\nЕго длина: " << best_length << endl;

    return 0;
}
