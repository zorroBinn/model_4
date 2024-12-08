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
    double cost; //��������� ���������
    int allocation; //�������������
};

//������� ��� ���������� ��������� ������� ������� ���
void northwestCorner(vector<vector<Cell>>& table, vector<int>& supply, vector<int>& demand) {
    int m = supply.size();
    int n = demand.size();

    for (short i = 0, j = 0; i < m && j < n;) {
        int allocated = min(supply[i], demand[j]);
        table[i][j].allocation = allocated;
        supply[i] -= allocated;
        demand[j] -= allocated;

        if (supply[i] == 0) ++i; //����� ��������
        if (demand[j] == 0) ++j; //����� ���������� ��������
    }
}

//������� ��� ���������� ������� �������
double calculateObjective(const vector<vector<Cell>>& table) {
    double totalCost = 0;
    for (const auto& row : table) {
        for (const auto& cell : row) {
            totalCost += cell.allocation * cell.cost;
        }
    }
    return totalCost;
}

//������� ��� ������ ������� �����
void printDeltaTable(const vector<vector<Cell>>& table, const vector<double>& u, const vector<double>& v) {
    int m = table.size(); //���������� �����
    int n = table[0].size(); //���������� ��������

    cout << endl << "������� ����� (cost - (u[i] + v[j])):" << endl;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            double delta = table[i][j].cost - (u[i] + v[j]);
            cout << fixed << setprecision(2) << delta << " "; //������� ������ � ��������� �� 2 ������
        }
        cout << endl;
    }
    cout << endl;
}

//������� ��� �������� ������������� ������� �����������
bool potentialsMethod(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v, bool isDeltaTablePrint) {
    int m = table.size();
    int n = table[0].size();
    u.assign(m, INF);
    v.assign(n, INF);
    u[0] = 0; //��������� ���������

    //����� ����������
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

    //��������� �������������
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

//������� ��� ������ ����������� ������ � ���������� �����
pair<int, int> findDeltaAndCycle(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    double minDelta = INF;
    int minI = -1, minJ = -1;

    //���� ������ � ����������� �������
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

//������� ������������� �����, �������� ������ �������
vector<pair<int, int>> cleanCycle(vector<pair<int, int>> path) {
    vector<pair<int, int>> cleanedPath;
    cleanedPath.push_back(path[0]); //��������� ��������� �����

    for (size_t i = 1; i < path.size() - 1; ++i) {
        auto [prevX, prevY] = cleanedPath.back(); //��������� ����������� �����
        auto [curX, curY] = path[i];
        auto [nextX, nextY] = path[i + 1];

        //���� ����������� �������� ����������, ����� �������� ��������
        if ((curX - prevX != nextX - curX) || (curY - prevY != nextY - curY)) {
            cleanedPath.push_back({ curX, curY });
        }
    }

    //���������, ��������� �� ��������� ����� � ������
    if (path.back() != path[0]) {
        cleanedPath.push_back(path.back()); //��������� ��������� �����, ���� ��� �� ��������� ������
    }

    return cleanedPath;
}

//������� ��� ������ ���� � ���������� �����
vector<pair<int, int>> findCycle(const vector<vector<Cell>>& table, pair<int, int> start) {
    int m = table.size();
    int n = table[0].size();

    //����������� ��� �����������: �����, ������, ����, �����
    int dx[] = { 0, -1, 1, 0 };
    int dy[] = { 1, 0, 0, -1 };

    vector<pair<int, int>> path; //����, ������� �������� ����
    set<pair<int, int>> visited; //����� ��� ������������ ���������� �����
    path.push_back(start); //��������� ��������� ����� � ����
    visited.insert(start); //�������� � ��� ����������

    while (true) {
        bool foundNext = false;

        //������� ��������� � ������ ������������
        for (int dir = 0; dir < 4; ++dir) {
            auto [x, y] = path.back();   //������� �����
            int nx = x, ny = y;

            //���� �������� �������� ������ � ������ �����������
            while (true) {
                nx += dx[dir];
                ny += dy[dir];

                if (nx < 0 || nx >= m || ny < 0 || ny >= n) break; //����� �� �������
                if (table[nx][ny].allocation > 0 || nx == start.first && ny == start.second) {
                    if (path.size() > 2 && nx == start.first && ny == start.second) {
                        //���� ��������� � ��������� �����, ��������� ����
                        path.push_back({ nx, ny });
                        return cleanCycle(path);
                    }
                    if (visited.find({ nx, ny }) == visited.end()) {
                        //����� ���������� ������
                        path.push_back({ nx, ny });
                        visited.insert({ nx, ny });
                        foundNext = true;
                        break;
                    }
                }
            }

            if (foundNext) break; //��������� � ��������� ��������, ���� ����� ������
        }

        //���� �� ����� ��������� ������, ������� ��������� �������
        if (!foundNext) {
            visited.erase(path.back());
            path.pop_back();

            //���� ���� ���� ������, ����� ���
            if (path.empty()) {
                return {};
            }
        }
    }
}

//������� ��� ������������� �������� � ������� �����
void improveSolution(vector<vector<Cell>>& table, vector<double>& u, vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    //���� ����������� ������ � ��������������� ������
    auto [minI, minJ] = findDeltaAndCycle(table, u, v);
    if (minI == -1 || minJ == -1) {
        cout << "��� ������������� �����, ������� ����������." << endl;
        return;
    }

    cout << "����: �������� ������� �� ������ (" << minI + 1 << ", " << minJ + 1 << ")" << endl;

    //������ ����
    auto cycle = findCycle(table, { minI, minJ });
    if (cycle.empty()) {
        cout << "�� ������� ����� ����." << endl;
        return;
    }

    //������� ��������� ����
    cout << "����: ";
    for (const auto& [x, y] : cycle) {
        cout << "(" << x + 1 << ", " << y + 1 << ") ";
    }
    cout << endl;

    //����� ����������� �������� X �� �������� �������� �����
    double X = INF;
    for (size_t i = 1; i < cycle.size(); i += 2) {
        auto [x, y] = cycle[i];
        X = min(X, table[x][y].allocation);
    }

    //������������ �������� � ������� �����
    for (size_t i = 0; i < cycle.size(); ++i) {
        auto [x, y] = cycle[i];
        if (i % 2 == 0) { //׸���� ������� ���������
            table[x][y].allocation += X;
        }
        else { //�������� ������� �����������
            table[x][y].allocation -= X;
        }
    }

    cout << "������� ��������" << endl;
}

//������� ��� ������ ������� � �����������
void printTableAndPotentials(const vector<vector<Cell>>& table, const vector<double>& u, const vector<double>& v) {
    int m = table.size();
    int n = table[0].size();

    for (short i = 0; i < m; ++i) {
        for (short j = 0; j < n; ++j) {
            cout << setw(6) << table[i][j].allocation;
        }
        cout << " | " << fixed << setprecision(2) << (u[i] == INF ? 0 : u[i]) << endl; //���������� u ������
    }

    cout << string(6 * n + 4, '-') << endl;
    for (short j = 0; j < n; ++j) {
        cout << setw(6) << fixed << setprecision(2) << (v[j] == INF ? 0 : v[j]);
    }
    cout << "  <^ ���������� V � U" << endl;
}

//�������� �������
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int m = 4, n = 5; //����������� �������
    vector<int> supply = { 20, 35, 40, 15 }; //������
    vector<int> demand = { 30, 20, 25, 15, 20 }; //�����������

    //������� ������
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
    cout << "�������������� ���� (������-�������� ����):" << endl;
    printTableAndPotentials(table, u, v);

    double totalCost = calculateObjective(table);
    cout << endl << "�������� ������� �������: F = " << fixed << setprecision(2) << totalCost << endl << endl;

    while (!potentialsMethod(table, u, v, true)) {
        improveSolution(table, u, v); //�������� �������
        potentialsMethod(table, u, v, false); //������������� ���������� ����� ���������
        printTableAndPotentials(table, u, v);
        totalCost = calculateObjective(table);
        cout << "������� ���������: F = " << fixed << setprecision(2) << totalCost << endl;
    }

    cout << "��� ������������� �����, ����������� ������� ����������." << endl;
    return 0;
}
