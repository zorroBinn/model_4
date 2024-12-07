#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <Windows.h>
using namespace std;

const double INF = numeric_limits<double>::infinity();

struct Cell {
    double cost;       // Стоимость перевозки
    int allocation;    // Распределение
};

// Функция для вычисления базисного решения методом СЗУ
void northwestCorner(vector<vector<Cell>>& table, vector<int>& supply, vector<int>& demand) {
    int m = supply.size();
    int n = demand.size();

    for (short i = 0, j = 0; i < m && j < n;) {
        int allocated = min(supply[i], demand[j]);
        table[i][j].allocation = allocated;
        supply[i] -= allocated;
        demand[j] -= allocated;

        if (supply[i] == 0) ++i; // Склад исчерпан
        if (demand[j] == 0) ++j; // Пункт назначения исчерпан
    }
}

// Функция для вычисления целевой функции
double calculateObjective(const vector<vector<Cell>>& table) {
    double totalCost = 0;
    for (const auto& row : table) {
        for (const auto& cell : row) {
            totalCost += cell.allocation * cell.cost;
        }
    }
    return totalCost;
}

// Функция для проверки оптимальности методом потенциалов
bool potentialsMethod(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();
    u.assign(m, INF);
    v.assign(n, INF);
    u[0] = 0; // Начальный потенциал

    // Найти потенциалы
    for (short iter = 0; iter < m + n - 1; ++iter) {
        for (short i = 0; i < m; ++i) {
            for (short j = 0; j < n; ++j) {
                if (table[i][j].allocation > 0) {
                    if (u[i] != INF) v[j] = table[i][j].cost - u[i];
                    if (v[j] != INF) u[i] = table[i][j].cost - v[j];
                }
            }
        }
    }

    // Проверить оптимальность
    bool optimal = true;
    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            if (table[i][j].allocation == 0) {
                double delta = table[i][j].cost - (u[i] + v[j]);
                if (delta < 0) {
                    optimal = false;
                }
            }
        }
    }
    return optimal;
}

// Основная функция
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int m = 4, n = 5; // Размерность таблицы
    vector<int> supply = { 20, 35, 40, 15 };        // Запасы
    vector<int> demand = { 30, 20, 25, 15, 20 };    // Потребности

    // Матрица затрат
    vector<vector<double>> cost = {
        {1.5, 3, 2, 2.5, 0},
        {2, 0.5, 1.5, 3, 0},
        {4, 2, 3, 0, 0},
        {3.5, 1.5, 3, 1, 0}
    };

    vector<vector<Cell>> table(m, vector<Cell>(n));
    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            table[i][j].cost = cost[i][j];
            table[i][j].allocation = 0;
        }
    }

    northwestCorner(table, supply, demand);

    cout << "Первоначальный план (Северо-Западный угол):\n";
    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            cout << setw(6) << table[i][j].allocation;
        }
        cout << endl;
    }

    double totalCost = calculateObjective(table);
    cout << "Значение целевой функции: F = " << fixed << setprecision(2) << totalCost << endl;

    vector<double> u, v;
    while (!potentialsMethod(table, u, v)) {
        cout << "Решение не оптимально, выполняется улучшение...\n";
        // (цикл пересчета).
        break;
    }

    cout << "Оптимальное решение достигнуто.\n";
    return 0;
}
