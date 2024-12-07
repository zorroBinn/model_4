#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <Windows.h>
using namespace std;

const int INF = int(numeric_limits<int>::max);

struct Cell {
    int cost;       // ��������� ���������
    int allocation; // �������������
};

// ������� ��� ���������� ��������� ������� ������� ���
void northwestCorner(vector<vector<Cell>>& table, vector<int>& supply, vector<int>& demand) {
    int m = supply.size();
    int n = demand.size();

    for (short i = 0, j = 0; i < m && j < n; ) {
        int allocated = min(supply[i], demand[j]);
        table[i][j].allocation = allocated;
        supply[i] -= allocated;
        demand[j] -= allocated;

        if (supply[i] == 0) ++i; // ����� ��������
        if (demand[j] == 0) ++j; // ����� ���������� ��������
    }
}

// ������� ��� ���������� ������� �������
int calculateObjective(const vector<vector<Cell>>& table) {
    int totalCost = 0;
    for (const auto& row : table) {
        for (const auto& cell : row) {
            totalCost += cell.allocation * cell.cost;
        }
    }
    return totalCost;
}

// ������� ��� �������� ������������� ������� �����������
bool potentialsMethod(vector<vector<Cell>>& table, vector<int>& u, vector<int>& v) {
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
                int delta = table[i][j].cost - (u[i] + v[j]);
                if (delta < 0) {
                    optimal = false;
                }
            }
        }
    }
    return optimal;
}

// �������� �������
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int m = 4, n = 5; // ����������� �������
    vector<int> supply = { 20, 35, 40, 15 }; // ������
    vector<int> demand = { 30, 20, 25, 15, 20 }; // �����������
    // ������� ������
    vector<vector<int>> cost = {
        {1, 5, 3, 2, 2},
        {2, 0, 0, 5, 1},
        {4, 2, 3, 0, 3},
        {3, 5, 1, 5, 0}
    };

    vector<vector<Cell>> table(m, vector<Cell>(n));
    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            table[i][j].cost = cost[i][j];
            table[i][j].allocation = 0;
        }
    }

    northwestCorner(table, supply, demand);

    cout << "�������������� ���� (������-�������� ����):\n";
    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            cout << setw(4) << table[i][j].allocation;
        }
        cout << endl;
    }

    cout << "�������� ������� �������: " << calculateObjective(table) << endl;

    vector<int> u, v;
    while (!potentialsMethod(table, u, v)) {
        cout << "������� �� ����������, ����������� ���������...\n";
        // (���� ���������).
        break;
    }

    cout << "����������� ������� ����������.\n";
    return 0;
}