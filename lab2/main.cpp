#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <climits>
#include <fstream>

#ifdef __GNUC__
#include <x86intrin.h>
#endif

using namespace std;

uint64_t get_cycles() {
    return __rdtsc();
}

// Итеративный Левенштейн
int iterativeLevenshtein(const string& s1, const string& s2) {
    size_t n = s1.size(), m = s2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for (size_t i = 0; i <= n; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= m; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = min({
                                   dp[i-1][j] + 1,
                                   dp[i][j-1] + 1,
                                   dp[i-1][j-1] + cost
                           });
        }
    }
    return dp[n][m];
}

// Итеративный Дамерау-Левенштейн
int iterativeDamerauLevenshtein(const string& s1, const string& s2) {
    size_t n = s1.size(), m = s2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for (size_t i = 0; i <= n; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= m; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = min({
                                   dp[i-1][j] + 1,
                                   dp[i][j-1] + 1,
                                   dp[i-1][j-1] + cost
                           });

            if (i > 1 && j > 1 && s1[i-2] == s2[j-1] && s1[i-1] == s2[j-2]) {
                dp[i][j] = min(dp[i][j], dp[i-2][j-2] + cost);
            }
        }
    }
    return dp[n][m];
}

// Рекурсивный Дамерау-Левенштейн (без кэша)
int recursiveDamerau(const string& s1, const string& s2, int i, int j) {
    if (i == 0) return j;
    if (j == 0) return i;

    int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
    int ins = recursiveDamerau(s1, s2, i, j-1) + 1;
    int del = recursiveDamerau(s1, s2, i-1, j) + 1;
    int subst = recursiveDamerau(s1, s2, i-1, j-1) + cost;
    int res = min({ins, del, subst});

    if (i > 1 && j > 1 && s1[i-2] == s2[j-1] && s1[i-1] == s2[j-2]) {
        int trans = recursiveDamerau(s1, s2, i-2, j-2) + cost;
        res = min(res, trans);
    }
    return res;
}

// Рекурсивный Дамерау-Левенштейн с кэшем
int recursiveDamerauCache(const string& s1, const string& s2, int i, int j, vector<vector<int>>& cache) {
    if (cache[i][j] != INT_MAX) return cache[i][j];

    if (i == 0) return cache[i][j] = j;
    if (j == 0) return cache[i][j] = i;

    int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
    int ins = recursiveDamerauCache(s1, s2, i, j-1, cache) + 1;
    int del = recursiveDamerauCache(s1, s2, i-1, j, cache) + 1;
    int subst = recursiveDamerauCache(s1, s2, i-1, j-1, cache) + cost;
    int res = min({ins, del, subst});

    if (i > 1 && j > 1 && s1[i-2] == s2[j-1] && s1[i-1] == s2[j-2]) {
        int trans = recursiveDamerauCache(s1, s2, i-2, j-2, cache) + cost;
        res = min(res, trans);
    }
    return cache[i][j] = res;
}

// Генератор случайных строк
string randomString(size_t length) {
    static mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(0, 25);
    string s;
    for (size_t i = 0; i < length; ++i)
        s += static_cast<char>('a' + dist(gen));
    return s;
}

void runTests() {
    ofstream csv("results.csv");
    csv << "Длина;"
        << "Левенштейн (такты);Левенштейн (память);"
        << "Дамерау Итер (такты);Дамерау Итер (память);"
        << "Дамерау Кэш (такты);Дамерау Кэш (память);"
        << "Дамерау Рекурс (такты)\n";

    vector<int> lengths = {1, 2, 3, 4, 5, 10};

    for (int len : lengths) {
        double t_lev = 0, t_dam_iter = 0, t_dam_cache = 0, t_dam_recur = 0;
        size_t mem_lev = 0, mem_dam_iter = 0, mem_dam_cache = 0;

        for (int i = 0; i < 100; ++i) {
            string s1 = randomString(len);
            string s2 = randomString(len);

            // Итеративный Левенштейн
            uint64_t start = get_cycles();
            iterativeLevenshtein(s1, s2);
            t_lev += (get_cycles() - start);
            mem_lev = (len+1)*(len+1)*sizeof(int);

            // Итеративный Дамерау
            start = get_cycles();
            iterativeDamerauLevenshtein(s1, s2);
            t_dam_iter += (get_cycles() - start);
            mem_dam_iter = (len+1)*(len+1)*sizeof(int);

            // Рекурсивный с кэшем
            vector<vector<int>> cache(len+1, vector<int>(len+1, INT_MAX));
            start = get_cycles();
            recursiveDamerauCache(s1, s2, len, len, cache);
            t_dam_cache += (get_cycles() - start);
            mem_dam_cache = (len+1)*(len+1)*sizeof(int);

            // Рекурсивный без кэша
            if (len <= 5) {
                start = get_cycles();
                recursiveDamerau(s1, s2, len, len);
                t_dam_recur += (get_cycles() - start);
            }
        }

        csv << len << ";"
            << t_lev/100 << ";" << mem_lev << ";"
            << t_dam_iter/100 << ";" << mem_dam_iter << ";"
            << t_dam_cache/100 << ";" << mem_dam_cache << ";"
            << (len <=5 ? to_string(t_dam_recur/100) : "N/A")
            << "\n";
    }
    csv.close();
}

int main() {
    //setlocale(LC_ALL, "Russian");

    cout << "Выберите режим:\n1 - Ручной ввод\n2 - Тесты\n";
    int mode;
    cin >> mode;

    if (mode == 1) {
        string s1, s2;
        cout << "Строка 1: ";
        cin >> s1;
        cout << "Строка 2: ";
        cin >> s2;

        cout << "\nРезультаты:\n";
        cout << "Итеративный Левенштейн: "
             << iterativeLevenshtein(s1, s2) << endl;

        cout << "Итеративный Дамерау: "
             << iterativeDamerauLevenshtein(s1, s2) << endl;

        int max_size = max(static_cast<int>(s1.size()), static_cast<int>(s2.size()));
        vector<vector<int>> cache(max_size+1, vector<int>(max_size+1, INT_MAX));
        cout << "Рекурсивный Дамерау (кэш): "
             << recursiveDamerauCache(s1, s2, s1.size(), s2.size(), cache) << endl;

        if (s1.size() <= 5 && s2.size() <= 5) {
            cout << "Рекурсивный Дамерау: "
                 << recursiveDamerau(s1, s2, s1.size(), s2.size()) << endl;
        } else {
            cout << "Рекурсивный Дамерау: недоступен для длин >5\n";
        }
    }
    else if (mode == 2) {
        runTests();
    }

    return 0;
}
