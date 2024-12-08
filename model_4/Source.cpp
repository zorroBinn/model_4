#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <Windows.h>
using namespace std;

const double INF = numeric_limits<double>::infinity();

struct Cell {
    double cost;       // ��������� ���������
    int allocation;    // �������������
};

// ������� ��� ���������� ��������� ������� ������� ���
void northwestCorner(vector<vector<Cell>>& table, vector<int>& supply, vector<int>& demand) {
    int m = supply.size();
    int n = demand.size();

    for (short i = 0, j = 0; i < m && j < n;) {
        int allocated = min(supply[i], demand[j]);
        table[i][j].allocation = allocated;
        supply[i] -= allocated;
        demand[j] -= allocated;

        if (supply[i] == 0) ++i; // ����� ��������
        if (demand[j] == 0) ++j; // ����� ���������� ��������
    }
}

// ������� ��� ���������� ������� �������
double calculateObjective(const vector<vector<Cell>>& table) {
    double totalCost = 0;
    for (const auto& row : table) {
        for (const auto& cell : row) {
            totalCost += cell.allocation * cell.cost;
        }
    }
    return totalCost;
}

// ������� ��� �������� ������������� ������� �����������
bool potentialsMethod(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();
    u.assign(m, INF);
    v.assign(n, INF);
    u[0] = 0; // ��������� ���������

    // ����� ����������
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

    // ��������� �������������
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

// ������� ��� ������ ������� � �����������
void printTableAndPotentials(const vector<vector<Cell>>& table, const vector<double>& u, const vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            cout << setw(6) << table[i][j].allocation;
        }
        cout << " | " << fixed << setprecision(2) << (u[i] == INF ? 0 : u[i]) << endl; // ���������� u ������
    }

    cout << string(6 * n + 4, '-') << endl; // �����������
    for (short j = 0; j < n; ++j) {
        cout << setw(6) << fixed << setprecision(2) << (v[j] == INF ? 0 : v[j]);
    }
    cout << "  <^ ���������� V � U" << endl;
}

// �������� �������
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int m = 4, n = 5; // ����������� �������
    vector<int> supply = { 20, 35, 40, 15 };        // ������
    vector<int> demand = { 30, 20, 25, 15, 20 };    // �����������

    // ������� ������
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
    potentialsMethod(table, u, v);
    cout << "�������������� ���� (������-�������� ����):\n";
    printTableAndPotentials(table, u, v);

    double totalCost = calculateObjective(table);
    cout << endl << "�������� ������� �������: F = " << fixed << setprecision(2) << totalCost << endl << endl;

    while (!potentialsMethod(table, u, v)) {
        cout << "������� �� ����������, ����������� ���������...\n";
        
        break; // �������� ����������� ��� ������� �������
    }

    cout << "����������� ������� ����������.\n";
    return 0;
}
