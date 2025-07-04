#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <tuple>

using namespace std;
using namespace std::chrono;

// Функция умножения матриц — обычный алгоритм
vector<vector<int>> multiplyMatrixDefault(const vector<vector<int>> &a, const vector<vector<int>> &b) {
    int l = a.size();
    int m = a[0].size();
    int n = b[0].size();

    vector<vector<int>> c(l, vector<int>(n, 0));

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < n; j++) {
            for (int r = 0; r < m; r++) {
                c[i][j] += a[i][r] * b[r][j];
            }
        }
    }

    return c;
}

// Умножение матриц по алгоритму Винограда
vector<vector<int>> multiplyMatrixVinograd(const vector<vector<int>> &a, const vector<vector<int>> &b) {
    int n = a.size();
    int m = a[0].size();
    int k = b[0].size();

    vector<int> rowFactor(n, 0);
    vector<int> colFactor(k, 0);
    vector<vector<int>> c(n, vector<int>(k, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m / 2; ++j) {
            rowFactor[i] += a[i][2 * j] * a[i][2 * j + 1];
        }
    }

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < m / 2; ++j) {
            colFactor[i] += b[2 * j][i] * b[2 * j + 1][i];
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            c[i][j] = -(rowFactor[i] + colFactor[j]);
            for (int l = 0; l < m / 2; ++l) {
                c[i][j] += (a[i][2 * l] + b[2 * l + 1][j]) * (a[i][2 * l + 1] + b[2 * l][j]);
            }
        }
    }

    if (m % 2 == 1) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) {
                c[i][j] += a[i][m - 1] * b[m - 1][j];
            }
        }
    }

    return c;
}

// Оптимизированный алгоритм Винограда
vector<vector<int>> multiplyMatrixVinogradOptimized(const vector<vector<int>> &a, const vector<vector<int>> &b) {
    int n = a.size();
    int m = a[0].size();       // столбцы A = строки B
    int k = b[0].size();

    vector<int> rowFactor(n, 0);
    vector<int> colFactor(k, 0);
    vector<vector<int>> c(n, vector<int>(k, 0));

    // Предвычисление rowFactor
    for (int i = 0; i < n; ++i)
        for (int j = 1; j < m; j += 2)
            rowFactor[i] += a[i][j] * a[i][j - 1];

    // Предвычисление colFactor
    for (int j = 0; j < k; ++j)
        for (int i = 1; i < m; i += 2)
            colFactor[j] += b[i][j] * b[i - 1][j];

    bool isOdd = (m & 1);
    int last = m - 1;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            c[i][j] = -(rowFactor[i] + colFactor[j]);
            for (int u = 1; u < m; u += 2) {
                c[i][j] += (a[i][u - 1] + b[u][j]) * (a[i][u] + b[u - 1][j]);
            }
            if (isOdd)
                c[i][j] += a[i][last] * b[last][j];
        }
    }

    return c;
}

vector<vector<int>> generateMatrix(int rows, int cols) {
    vector<vector<int>> matrix(rows, vector<int>(cols));
    for (auto &row: matrix) {
        for (auto &elem: row) {
            elem = rand() % 21 - 10; // Генерация от -10 до 10
        }
    }
    return matrix;
}

template<typename Func>
double measureTime(Func func, const vector<vector<int>> &a, const vector<vector<int>> &b) {
    auto start = high_resolution_clock::now();
    func(a, b);
    auto stop = high_resolution_clock::now();
    return duration<double>(stop - start).count();
}

void saveToCSV(const string &filename, const vector<tuple<int, string, double, double, double>> &data) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return;
    }
    file << "Size,Type,Default,Winograd,Optimized_Winograd\n";
    for (const auto &[size, type, defaultTime, winogradTime, optimizedTime]: data) {
        file << size << "," << type << "," << defaultTime << "," << winogradTime << "," << optimizedTime << "\n";
    }
    file.close();
}

int main() {
    vector<tuple<int, string, double, double, double>> results;

    srand(static_cast<unsigned int>(time(0)));

    vector<int> best_sizes = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
    vector<int> worst_sizes = {51, 101, 151, 201, 251, 301, 351, 401, 451, 501};

    cout << "Начало замеров лучших случаев (чётные размеры)" << endl;
    for (int size: best_sizes) {
        cout << "\nРазмер: " << size << " x " << size << " (лучший случай)" << endl;

        auto a = generateMatrix(size, size);
        auto b = generateMatrix(size, size);

        double tDefault = measureTime(multiplyMatrixDefault, a, b);
        double tVinograd = measureTime(multiplyMatrixVinograd, a, b);
        double tOptimized = measureTime(multiplyMatrixVinogradOptimized, a, b);

        results.emplace_back(size, "best", tDefault, tVinograd, tOptimized);
    }

    cout << "\nНачало замеров худших случаев (нечётные размеры)" << endl;
    for (int size: worst_sizes) {
        cout << "\nРазмер: " << size << " x " << size << " (худший случай)" << endl;

        auto a = generateMatrix(size, size);
        auto b = generateMatrix(size, size);

        double tDefault = measureTime(multiplyMatrixDefault, a, b);
        double tVinograd = measureTime(multiplyMatrixVinograd, a, b);
        double tOptimized = measureTime(multiplyMatrixVinogradOptimized, a, b);

        results.emplace_back(size, "worst", tDefault, tVinograd, tOptimized);
    }

    saveToCSV("measurements.csv", results);

    cout << "\nЗамеры завершены! Результаты сохранены в measurements.csv ===" << endl;

    return 0;
}
