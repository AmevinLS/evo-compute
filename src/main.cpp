#include <iostream>
#include <optional>
#include "read_data.cpp"

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, vector<T> vec) {
    for (const T& elem : vec) {
        os << elem << " ";
    }
    return os;
}

template <typename T>
ostream& operator<<(ostream& os, vector<vector<T>> matrix) {
    for (auto& row : matrix) {
        os << row << endl;
    }
    return os;
}

int main() {
    TSPInstance tsp = read_tsp("../data/TSPA.csv");
    // cout << tsp.getDistanceMatrix();
    tsp.getDistanceMatrix();
    cout << tsp.getDistanceMatrix();
    return 0;
}