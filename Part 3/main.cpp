#include <iostream>
#include <vector>
#include <cstdint>
#include <functional>

using namespace std;

template <typename T>
uint64_t valueHash(const T& value) {
    return static_cast<uint64_t>(hash<T>{}(value));
}

template <typename T>
bool sameBlock(const vector<vector<T>>& picture,
               int row,
               int col,
               int k) {
    int n = (int)picture[0].size();
    int cornerCol = n - k;

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            if (picture[row + i][col + j] != picture[i][cornerCol + j]) {
                return false;
            }
        }
    }

    return true;
}

template <typename T>
bool topRightCornerAppearsAgain(const vector<vector<T>>& picture, int k) {
    const uint64_t MASK = (1ULL << 32) - 1;
    const uint64_t ROW_BASE = 257;
    const uint64_t COL_BASE = 263;

    int rows = (int)picture.size();
    if (rows == 0) return false;

    int cols = (int)picture[0].size();
    if (cols == 0 || k <= 0 || k > rows || k > cols) return false;

    for (const auto& row : picture) {
        if ((int)row.size() != cols) return false;
    }

    uint64_t rowPower = 1;
    for (int i = 1; i < k; i++) {
        rowPower = (rowPower * ROW_BASE) & MASK;
    }

    uint64_t colPower = 1;
    for (int i = 1; i < k; i++) {
        colPower = (colPower * COL_BASE) & MASK;
    }

    int lastStartCol = cols - k;

    vector<vector<uint64_t>> horizontalHash(
        rows,
        vector<uint64_t>(lastStartCol + 1)
    );

    for (int r = 0; r < rows; r++) {
        uint64_t h = 0;

        for (int c = 0; c < k; c++) {
            h = (h * ROW_BASE + valueHash(picture[r][c])) & MASK;
        }

        horizontalHash[r][0] = h;

        for (int c = 1; c <= lastStartCol; c++) {
            uint64_t removed =
                (valueHash(picture[r][c - 1]) * rowPower) & MASK;

            h = (h + MASK + 1 - removed) & MASK;
            h = (h * ROW_BASE + valueHash(picture[r][c + k - 1])) & MASK;

            horizontalHash[r][c] = h;
        }
    }

    int cornerCol = cols - k;

    uint64_t targetHash = 0;
    for (int r = 0; r < k; r++) {
        targetHash =
            (targetHash * COL_BASE + horizontalHash[r][cornerCol]) & MASK;
    }

    for (int c = 0; c <= lastStartCol; c++) {
        uint64_t blockHash = 0;

        for (int r = 0; r < k; r++) {
            blockHash =
                (blockHash * COL_BASE + horizontalHash[r][c]) & MASK;
        }

        for (int r = 0; r <= rows - k; r++) {
            bool originalCorner = (r == 0 && c == cornerCol);

            if (!originalCorner && blockHash == targetHash) {
                if (sameBlock(picture, r, c, k)) {
                    return true;
                }
            }

            if (r < rows - k) {
                uint64_t removed =
                    (horizontalHash[r][c] * colPower) & MASK;

                blockHash = (blockHash + MASK + 1 - removed) & MASK;
                blockHash =
                    (blockHash * COL_BASE + horizontalHash[r + k][c]) & MASK;
            }
        }
    }

    return false;
}

int main() {
    cout << "Part 3 - 2D Rabin-Karp Corner Search\n\n";

    vector<vector<int>> picture1 = {
        {5, 1, 8, 7, 7},
        {3, 4, 6, 2, 2},
        {7, 7, 0, 1, 9},
        {2, 2, 3, 4, 5},
        {9, 8, 7, 6, 5}
    };

    cout << "Test 1, K=2: ";
    cout << (topRightCornerAppearsAgain(picture1, 2) ? "MATCH FOUND" : "NO MATCH") << "\n";

    vector<vector<int>> picture2 = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    cout << "Test 2, K=2: ";
    cout << (topRightCornerAppearsAgain(picture2, 2) ? "MATCH FOUND" : "NO MATCH") << "\n";

    vector<vector<char>> picture3 = {
        {'x', 'y', 'a', 'b'},
        {'z', 'w', 'c', 'd'},
        {'a', 'b', 'q', 'r'},
        {'c', 'd', 's', 't'}
    };

    cout << "Test 3, K=2: ";
    cout << (topRightCornerAppearsAgain(picture3, 2) ? "MATCH FOUND" : "NO MATCH") << "\n";

    vector<vector<int>> picture4 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    cout << "Test 4, K=3: ";
    cout << (topRightCornerAppearsAgain(picture4, 3) ? "MATCH FOUND" : "NO MATCH") << "\n";

    return 0;
}