/*
 * "Jewish-style Carp" Algorithm
 * ==============================
 * A 2D Rabin-Karp variant that detects if the top-right K×K corner of an M×N picture
 * appears elsewhere in the picture.
 * 
 * Key features:
 * - Uses bitwise AND masks instead of modulo operations for O(1) hashing
 * - Rolling hash technique for O(1) window updates
 * - Linear time complexity O(M*N) with potential O(K²) verification for hash collisions
 * - Template-based design for arbitrary data types
 */

#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
bool rabin_karp_corner_match(const vector<vector<T>>& image, int K) {
    // Bitwise MASK replaces modulo operations for faster computation
    const uint64_t MASK = (1ULL << 32) - 1ULL;
    
    // Base values for row and column rolling hashes
    const uint64_t BASE_ROW = 911382323ULL;
    const uint64_t BASE_COL = 972663749ULL;

    const int M = static_cast<int>(image.size());
    if (M == 0) return false;

    const int N = static_cast<int>(image[0].size());
    if (N == 0 || K <= 0 || K > M || K > N) return false;

    for (const auto& row : image) {
        if (static_cast<int>(row.size()) != N) return false;
    }

    auto token = [&](const T& value) -> uint64_t {
        return static_cast<uint64_t>(std::hash<T>{}(value)) & MASK;
    };

    uint64_t row_pow = 1;
    for (int i = 1; i < K; ++i) {
        row_pow = (row_pow * BASE_ROW) & MASK;
    }

    uint64_t col_pow = 1;
    for (int i = 1; i < K; ++i) {
        col_pow = (col_pow * BASE_COL) & MASK;
    }

    const int max_col_start = N - K;
    vector<vector<uint64_t>> row_hash(M, vector<uint64_t>(max_col_start + 1, 0));

    // Phase 1: Compute row hashes (left-to-right rolling hash for each row)
    // Time: O(M*N) - processes each pixel exactly once
    // Uses bitwise & for O(1) hash updates instead of modulo division
    for (int r = 0; r < M; ++r) {
        uint64_t h = 0;
        for (int c = 0; c < K; ++c) {
            h = ((h * BASE_ROW) + token(image[r][c])) & MASK;
        }
        row_hash[r][0] = h;

        for (int c = 1; c <= max_col_start; ++c) {
            const uint64_t out = (token(image[r][c - 1]) * row_pow) & MASK;
            h = (h + MASK + 1ULL - out) & MASK;
            h = ((h * BASE_ROW) + token(image[r][c + K - 1])) & MASK;
            row_hash[r][c] = h;
        }
    }

    // Phase 2: Compute corner hash
    // Time: O(K) - combines K row hashes
    const int corner_col = N - K;
    uint64_t corner_hash = 0;
    for (int r = 0; r < K; ++r) {
        corner_hash = ((corner_hash * BASE_COL) + row_hash[r][corner_col]) & MASK;
    }

    // Phase 3: Search for corner duplicates using rolling hash on column axis
    // Time: O(M*N) - iterates through all block positions
    //       + O(K²) per hash collision for verification
    // Uses bitwise & for O(1) hash updates as blocks slide vertically
    for (int c = 0; c <= max_col_start; ++c) {
        uint64_t block_hash = 0;
        for (int r = 0; r < K; ++r) {
            block_hash = ((block_hash * BASE_COL) + row_hash[r][c]) & MASK;
        }

        for (int r = 0; r <= M - K; ++r) {
            if (!(r == 0 && c == corner_col) && block_hash == corner_hash) {
                bool match = true;
                for (int i = 0; i < K && match; ++i) {
                    for (int j = 0; j < K; ++j) {
                        if (image[r + i][c + j] != image[i][corner_col + j]) {
                            match = false;
                            break;
                        }
                    }
                }
                if (match) return true;
            }

            if (r < M - K) {
                const uint64_t out = (row_hash[r][c] * col_pow) & MASK;
                block_hash = (block_hash + MASK + 1ULL - out) & MASK;
                block_hash = ((block_hash * BASE_COL) + row_hash[r + K][c]) & MASK;
            }
        }
    }

    return false;
}

int main() {
    cout << "\n=== Jewish-style Carp Algorithm Tests ===" << endl;

    // Test 1: Corner match found (top-right 2×2 duplicated)
    cout << "\nTest 1: Basic corner duplication detection" << endl;
    vector<vector<int>> image1 = {
        {1, 2, 3, 9, 0},
        {4, 5, 6, 7, 8},
        {9, 0, 3, 9, 0},
        {6, 7, 8, 7, 8},
        {9, 0, 1, 2, 3}
    };
    bool found1 = rabin_karp_corner_match(image1, 2);
    cout << "Image 5x5, K=2. Top-right corner: [[9,0],[7,8]]" << endl;
    cout << "Result: " << (found1 ? "MATCH FOUND" : "NO MATCH ") << endl;

    // Test 2: No match - unique corner
    cout << "\nTest 2: No corner duplication (unique corner)" << endl;
    vector<vector<int>> image2 = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };
    bool found2 = rabin_karp_corner_match(image2, 2);
    cout << "Image 5x5, K=2. Top-right corner: [[4,5],[9,10]]" << endl;
    cout << "Result: " << (found2 ? "MATCH FOUND" : "NO MATCH ") << endl;

    // Test 3: Exact duplication (all same values)
    cout << "\nTest 3: All same values" << endl;
    vector<vector<char>> image3 = {
        {'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'}
    };
    bool found3 = rabin_karp_corner_match(image3, 2);
    cout << "Image 5x5 (all 'A'), K=2. Top-right corner: [[A,A],[A,A]]" << endl;
    cout << "Result: " << (found3 ? "MATCH FOUND" : "NO MATCH ") << endl;

    // Test 4: K equals image dimensions
    cout << "\nTest 4: K equals image dimensions (K=M=N=3)" << endl;
    vector<vector<int>> image4 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    bool found4 = rabin_karp_corner_match(image4, 3);
    cout << "Image 3x3, K=3. Top-right corner: entire image" << endl;
    cout << "Result: " << (found4 ? "MATCH FOUND (self)" : "NO MATCH") << endl;

    // Test 5: Larger pattern with match
    cout << "\nTest 5: Larger K value (K=3)" << endl;
    vector<vector<int>> image5 = {
        {1, 2, 3, 1, 2, 3},
        {4, 5, 6, 4, 5, 6},
        {7, 8, 9, 7, 8, 9},
        {0, 0, 0, 0, 0, 0}
    };
    bool found5 = rabin_karp_corner_match(image5, 3);
    cout << "Image 4x6, K=3. Top-right corner: [[1,2,3],[4,5,6],[7,8,9]]" << endl;
    cout << "Result: " << (found5 ? "MATCH FOUND" : "NO MATCH ") << endl;

    return 0;
}
