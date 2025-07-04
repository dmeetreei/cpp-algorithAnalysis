#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>


// Генерация лучшего случая (уже отсортированный массив)
int *generateBestCase(int size) {
    int *arr = (int *) malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    return arr;
}

// Генерация худшего случая (обратный порядок)
int *generateWorstCase(int size) {
    int *arr = (int *) malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        arr[i] = size - i - 1;
    }
    return arr;
}

// Генерация случайного массива
int *generateRandomCase(int size) {
    int *arr = (int *) malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % (size * 10);
    }
    return arr;
}

// Копия массива (для переиспользования данных)
int *copyArray(int *src, int size) {
    int *dest = (int *) malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
    return dest;
}

void bubbleSort(int arr[], int n) {
    int swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        int temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
    }
}


double measureTime(int* arr, int size, void (*sortFunc)(int*, int)) {
    int* data = copyArray(arr, size);

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq); // Получаем частоту таймера
    QueryPerformanceCounter(&start);  // Начало замера

    sortFunc(data, size);

    QueryPerformanceCounter(&end); // Конец замера
    free(data);

    // Вычисление времени в миллисекундах
    return (double)(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
}

void measureAndExport(const char* filename, int* arr, int size, void (*sortFunc)(int*, int), const char* sortName, const char* caseType) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        printf("Ошибка открытия файла!\n");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        double time = measureTime(arr, size, sortFunc);

        // если >= 0.001 то до 3 знаков, иначе сколько значащих цифр есть
        if (time >= 0.001)
            fprintf(file, "%s,%s,%d,%d,%.3f\n", sortName, caseType, size, i + 1, time);
        else
            fprintf(file, "%s,%s,%d,%d,%.10f\n", sortName, caseType, size, i + 1, time);
    }

    fclose(file);
}


int main() {
    srand(time(NULL));
    const int sizes[] = {10, 100, 500, 1000, 2000, 5000, 10000, 25000, 50000};
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const char* cases[] = {"best", "worst", "random"};

    // Создание файла с заголовком
    FILE* file = fopen("raw_results.csv", "w");
    fprintf(file, "Algorithm,Case,Size,Run,Time\n");
    fclose(file);

    for (int i = 0; i < num_sizes; i++) {
        int size = sizes[i];
        printf("Processing size: %d\n", size);

        for (int j = 0; j < 3; j++) {
            int* arr;
            switch(j) {
                case 0: arr = generateBestCase(size); break;
                case 1: arr = generateWorstCase(size); break;
                case 2: arr = generateRandomCase(size); break;
            }

            measureAndExport("raw_results.csv", arr, size, bubbleSort, "Bubble", cases[j]);
            measureAndExport("raw_results.csv", arr, size, insertionSort, "Insertion", cases[j]);
            measureAndExport("raw_results.csv", arr, size, selectionSort, "Selection", cases[j]);

            free(arr);
        }
    }

    return 0;
}
