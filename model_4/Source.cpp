#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <stack>
#include <set>
#include <queue>
#include <Windows.h>
using namespace std;

const double INF = numeric_limits<double>::infinity();

struct Cell {
    double cost; //Стоимость перевозки
    int allocation; //Распределение
};

//Функция для вычисления базисного решения методом СЗУ
void northwestCorner(vector<vector<Cell>>& table, vector<int>& supply, vector<int>& demand) {
    int m = supply.size();
    int n = demand.size();

    for (short i = 0, j = 0; i < m && j < n;) {
        int allocated = min(supply[i], demand[j]);
        table[i][j].allocation = allocated;
        supply[i] -= allocated;
        demand[j] -= allocated;

        if (supply[i] == 0) ++i; //Склад исчерпан
        if (demand[j] == 0) ++j; //Пункт назначения исчерпан
    }
}

//Функция для вычисления целевой функции
double calculateObjective(const vector<vector<Cell>>& table) {
    double totalCost = 0;
    for (const auto& row : table) {
        for (const auto& cell : row) {
            totalCost += cell.allocation * cell.cost;
        }
    }
    return totalCost;
}

//Функция для вывода таблицы дельт
void printDeltaTable(const vector<vector<Cell>>& table, const vector<double>& u, const vector<double>& v) {
    int m = table.size(); //Количество строк
    int n = table[0].size(); //Количество столбцов

    cout << endl << "Таблица дельт (cost - (u[i] + v[j])):" << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            double delta = table[i][j].cost - (u[i] + v[j]);
            cout << fixed << setprecision(2) << delta << " "; //Выводим дельту с точностью до 2 знаков
        }
        cout << endl;
    }
    cout << endl;
}

//Функция для проверки оптимальности методом потенциалов
bool potentialsMethod(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v, bool isDeltaTablePrint) {
    int m = table.size();
    int n = table[0].size();
    u.assign(m, INF);
    v.assign(n, INF);
    u[0] = 0; //Начальный потенциал

    //Найти потенциалы
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

    //Проверить оптимальность
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
    if (isDeltaTablePrint) printDeltaTable(table, u, v);
    return optimal;
}

//Функция для поиска минимальной дельты и построения цикла
pair<int, int> findDeltaAndCycle(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    double minDelta = INF;
    int minI = -1, minJ = -1;

    //Ищем ячейку с минимальной дельтой
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (table[i][j].allocation == 0) {
                double delta = table[i][j].cost - (u[i] + v[j]);
                if (delta < minDelta) {
                    minDelta = delta;
                    minI = i;
                    minJ = j;
                }
            }
        }
    }

    return { minI, minJ };
}

//Удаляет промежуточные точки, оставляя только вершины
vector<pair<int, int>> cleanCycle(vector<pair<int, int>> path) {
    vector<pair<int, int>> cleanedPath;
    cleanedPath.push_back(path[0]); //Добавляем стартовую точку

    for (size_t i = 1; i < path.size() - 1; ++i) {
        auto [prevX, prevY] = cleanedPath.back(); //Последняя добавленная точка
        auto [curX, curY] = path[i];
        auto [nextX, nextY] = path[i + 1];

        //Если направление движения изменилось, точка является вершиной
        if ((curX - prevX != nextX - curX) || (curY - prevY != nextY - curY)) {
            cleanedPath.push_back({ curX, curY });
        }
    }

    //Проверяем, совпадает ли последняя точка с первой
    if (path.back() != path[0]) {
        cleanedPath.push_back(path.back()); //Добавляем последнюю точку, если она не дублирует первую
    }

    return cleanedPath;
}

//Функция для поиска пути и построения цикла
vector<pair<int, int>> findCycle(const vector<vector<Cell>>& table, pair<int, int> start) {
    int m = table.size();
    int n = table[0].size();

    //Направления для перемещения: вверх, вправо, вниз, влево
    int dx[] = { 0, -1, 1, 0 };
    int dy[] = { 1, 0, 0, -1 };

    vector<pair<int, int>> path; //Путь, который составит цикл
    set<pair<int, int>> visited; //Набор для отслеживания посещённых ячеек
    path.push_back(start); //Добавляем стартовую точку в путь
    visited.insert(start); //Помечаем её как посещённую

    while (true) {
        bool foundNext = false;

        //Пробуем двигаться в четырёх направлениях
        for (int dir = 0; dir < 4; ++dir) {
            auto [x, y] = path.back();   //Текущая точка
            int nx = x, ny = y;

            //Ищем наиболее удалённую ячейку в данном направлении
            while (true) {
                nx += dx[dir];
                ny += dy[dir];

                if (nx < 0 || nx >= m || ny < 0 || ny >= n) break; //Вышли за пределы
                if (table[nx][ny].allocation > 0 || nx == start.first && ny == start.second) {
                    if (path.size() > 2 && nx == start.first && ny == start.second) {
                        //Если вернулись в начальную точку, завершаем цикл
                        path.push_back({ nx, ny });
                        return cleanCycle(path);
                    }
                    if (visited.find({ nx, ny }) == visited.end()) {
                        //Нашли подходящую ячейку
                        path.push_back({ nx, ny });
                        visited.insert({ nx, ny });
                        foundNext = true;
                        break;
                    }
                }
            }

            if (foundNext) break; //Переходим к следующей итерации, если нашли ячейку
        }

        //Если не нашли следующую ячейку, удаляем последний элемент
        if (!foundNext) {
            visited.erase(path.back());
            path.pop_back();

            //Если путь стал пустым, цикла нет
            if (path.empty()) {
                return {};
            }
        }
    }
}

//Функция для корректировки значений в ячейках цикла
void improveSolution(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    //Ищем минимальную дельту и соответствующую ячейку
    auto [minI, minJ] = findDeltaAndCycle(table, u, v);
    if (minI == -1 || minJ == -1) {
        cout << "Нет отрицательных дельт, решение оптимально." << endl;
        return;
    }

    cout << "Цель: улучшить решение по клетке (" << minI + 1 << ", " << minJ + 1 << ")" << endl;

    //Строим цикл
    auto cycle = findCycle(table, { minI, minJ });
    if (cycle.empty()) {
        cout << "Не удалось найти цикл." << endl;
        return;
    }

    //Выводим найденный цикл
    cout << "Цикл: ";
    for (const auto& [x, y] : cycle) {
        cout << "(" << x + 1 << ", " << y + 1 << ") ";
    }
    cout << endl;

    //Найдём минимальное значение X на нечётных позициях цикла
    double X = INF;
    for (size_t i = 1; i < cycle.size(); i += 2) {
        auto [x, y] = cycle[i];
        X = min(X, table[x][y].allocation);
    }

    //Корректируем значения в ячейках цикла
    for (size_t i = 0; i < cycle.size(); ++i) {
        auto [x, y] = cycle[i];
        if (i % 2 == 0) { //Чётные позиции уменьшаем
            table[x][y].allocation += X;
        }
        else { //Нечётные позиции увеличиваем
            table[x][y].allocation -= X;
        }
    }

    cout << "Решение улучшено" << endl;
}

//Функция для вывода таблицы и потенциалов
void printTableAndPotentials(const vector<vector<Cell>>& table, const vector<double>& u, const vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            cout << setw(6) << table[i][j].allocation;
        }
        cout << " | " << fixed << setprecision(2) << (u[i] == INF ? 0 : u[i]) << endl; //Потенциалы u справа
    }

    cout << string(6 * n + 4, '-') << endl;
    for (short j = 0; j < n; ++j) {
        cout << setw(6) << fixed << setprecision(2) << (v[j] == INF ? 0 : v[j]);
    }
    cout << "  <^ Потенциалы V и U" << endl;
}

//Основная функция
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int m = 4, n = 5; //Размерность таблицы
    vector<int> supply = { 20, 35, 40, 15 }; //Запасы
    vector<int> demand = { 30, 20, 25, 15, 20 }; //Потребности

    //Матрица затрат
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

    vector<double> u, v;
    potentialsMethod(table, u, v, false);
    cout << "Первоначальный план (Северо-Западный угол):" << endl;
    printTableAndPotentials(table, u, v);

    double totalCost = calculateObjective(table);
    cout << endl << "Значение целевой функции: F = " << fixed << setprecision(2) << totalCost << endl << endl;

    while (!potentialsMethod(table, u, v, true)) {
        improveSolution(table, u, v); //Улучшаем решение
        potentialsMethod(table, u, v, false); //Пересчитываем потенциалы после улучшения
        printTableAndPotentials(table, u, v);
        totalCost = calculateObjective(table);
        cout << "Текущая стоимость: F = " << fixed << setprecision(2) << totalCost << endl;
    }

    cout << "Нет отрицательных дельт, оптимальное решение достигнуто." << endl;
    return 0;
}
