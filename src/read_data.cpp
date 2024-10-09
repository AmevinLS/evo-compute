#include <cmath>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

typedef vector<vector<int>> DistanceMatrix;

class TSPInstance {
   private:
    mutable optional<DistanceMatrix> mDistanceMatrix;

    DistanceMatrix calcDistanceMatrix() const {
        DistanceMatrix matrix(n, vector<int>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    matrix[i][j] = 0;
                    continue;
                } else if (i > j) {
                    matrix[i][j] = matrix[j][i];
                    continue;
                }
                matrix[i][j] = int(
                    round(sqrt(pow(xs[i] - xs[j], 2) + pow(ys[i] - ys[j], 2))));
                // NOTE: hopefully pow() floating point operation doesn't
                // cause any errors
            }
        }
        return matrix;
    }

   public:
    int n;
    vector<int> xs;
    vector<int> ys;
    vector<int> vals;

    TSPInstance(vector<int> xs, vector<int> ys, vector<int> vals)
        : n(xs.size()), xs(xs), ys(ys), vals(vals) {}

    DistanceMatrix getDistanceMatrix() {
        if (!mDistanceMatrix) {
            mDistanceMatrix = calcDistanceMatrix();
        }
        return *mDistanceMatrix;
    }
};

TSPInstance read_tsp(const string& filepath) {
    ifstream fin(filepath);

    int n = 0;
    vector<int> xs, ys, vals;

    string line;
    while (getline(fin, line)) {
        stringstream ss(line);

        string value;
        getline(ss, value, ';');
        xs.push_back(stoi(value));

        getline(ss, value, ';');
        ys.push_back(stoi(value));

        getline(ss, value, ';');
        vals.push_back(stoi(value));

        n++;
    }

    return TSPInstance(xs, ys, vals);
}
