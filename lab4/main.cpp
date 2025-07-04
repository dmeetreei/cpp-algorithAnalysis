#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <sstream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

void secondMaxIterative(int &max1, int &max2) {
    int n;
    cin >> n;
    while (n != 0) {
        if (n > max1) {
            max2 = max1;
            max1 = n;
        } else if (n == max1) {
            max2 = max1;
        } else if (n < max1 && n > max2) {
            max2 = n;
        }
        cin >> n;
    }
}

void secondMaxRecursive(int &max1, int &max2) {
    int n;
    cin >> n;
    if (n == 0) {
        return;
    }

    if (n > max1) {
        max2 = max1;
        max1 = n;
    } else if (n == max1) {
        max2 = max1;
    } else if (n > max2) {
        max2 = n;
    }

    secondMaxRecursive(max1, max2);
}

/*
 * анализ алгоритмов
 */
pair<int, long long> runSecondMaxTimed(const vector<int>& data, bool recursive, int repeats = 5) {
    long long total_time = 0;
    int result = -1;

    for (int i = 0; i < repeats; ++i) {
        // Подмена ввода
        streambuf* orig = cin.rdbuf();
        stringstream input;
        for (int num : data) input << num << " ";
        cin.rdbuf(input.rdbuf());

        int max1 = INT_MIN, max2 = INT_MIN;

        auto start = chrono::high_resolution_clock::now();
        if (recursive)
            secondMaxRecursive(max1, max2);
        else
            secondMaxIterative(max1, max2);
        auto end = chrono::high_resolution_clock::now();

        cin.rdbuf(orig);

        long long time_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
        total_time += time_us;
        result = max2; // он должен быть одинаковым каждый раз
    }

    return {result, total_time / repeats};
}

void runAllTests() {
    vector<int> sizes = {3, 30, 100, 200, 500, 1000, 2000, 5000, 10000, 50000,10000};
    ofstream csv("results.csv");
    csv << "N,second_max_recursive,time_recursive_us,second_max_iterative,time_iterative_us\n";
    csv.flush(); // сразу пишем заголовок

    srand(43); // фиксированный сид

    for (int N : sizes) {
        vector<int> testData;
        for (int i = 0; i < N; ++i)
            testData.push_back((rand() << 15) | rand());
        testData.push_back(0); // конец

        pair<int, long long> rec = runSecondMaxTimed(testData, true);

        auto iter = runSecondMaxTimed(testData, false);

        csv << N << "," << rec.first << "," << rec.second << "," << iter.first << "," << iter.second << "\n";
        csv.flush(); 

        cout << "N = " << N << " записано\n";
    }

    csv.close();
    cout << "\nВсе результаты сохранены в results.csv\n";
}


int main() {
    /*
    int max1, max2;
    cin >> max1;
    cin >> max2;

    if (max2 > max1) {
        swap(max1, max2);
    }

    //secondMaxIterative(max1,max2);
    secondMaxRecursive(max1, max2);
    cout << max2 << endl;
*/
    runAllTests();

    return 0;
}
