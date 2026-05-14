#include <iostream>
#include <string>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

vector<int> computeLPS(const string &P) {
    // LPS = longest prefix that is also a suffix.
    // KMP uses this table to avoid re-checking characters.
    int m = (int)P.size();
    vector<int> lps(m, 0);
    int len = 0, i = 1;
    while (i < m) {
        if (P[i] == P[len]) lps[i++] = ++len;
        else if (len) len = lps[len - 1];
        else lps[i++] = 0;
    }
    return lps;
}

int KMPsearch(const string &T, const string &P) {
    // KMP scans the text once and jumps back using the LPS table.
    int n = (int)T.size(), m = (int)P.size(), count = 0;
    vector<int> lps = computeLPS(P);
    int i = 0, j = 0;
    while (i < n) {
        if (T[i] == P[j]) { i++; j++; }
        else if (j) j = lps[j - 1];
        else i++;
        if (j == m) {
            count++;
            j = lps[j - 1];
        }
    }
    return count;
}

const long long base = 256;
const long long mod = 2;

int RabinKarpsearch(const string &T, const string &P) {
    // Rabin-Karp uses rolling hashes, but hash matches still need a full check.
    int n = (int)T.size(), m = (int)P.size(), count = 0;
    if (m > n) return 0;
    long long pHash = 0, tHash = 0, h = 1;
    for (int i = 0; i < m - 1; i++) h = (h * base) % mod;
    for (int i = 0; i < m; i++) {
        pHash = (pHash * base + P[i]) % mod;
        tHash = (tHash * base + T[i]) % mod;
    }
    for (int i = 0; i <= n - m; i++) {
        if (pHash == tHash) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (T[i + j] != P[j]) {
                    match = false;
                    break;
                }
            }
            if (match) count++;
        }
        if (i < n - m) {
            tHash = (tHash - T[i] * h % mod + mod) % mod;
            tHash = (tHash * base + T[i + m]) % mod;
        }
    }
    return count;
}

int main() {
    // Large deterministic text (500kB) to satisfy assignment size requirement.
    int n = 500000;
    string T;
    T.reserve(n);

    for (int i = 0; i < n; i++) {
        T.push_back((i % 2 == 0) ? 'a' : 'b');
    }

    // Pattern selected so many overlaps occur in alternating "abab..." text.
    string P = "ababa";

    // Force at least one known match near the end for reproducible correctness.
    for (int i = 0; i < (int)P.size(); i++) {
        T[n - (int)P.size() + i] = P[i];
    }

    // Average over many runs to reduce timer noise.
    int runs = 100;
    long long totalKMP = 0, totalRK = 0;
    int resKMP = 0, resRK = 0;

    // Same text and pattern are reused every time so the comparison is fair.
    for (int i = 0; i < runs; i++) {
        auto start = steady_clock::now();
        resKMP = KMPsearch(T, P);
        auto end = steady_clock::now();
        totalKMP += duration_cast<microseconds>(end - start).count();

        start = steady_clock::now();
        resRK = RabinKarpsearch(T, P);
        end = steady_clock::now();
        totalRK += duration_cast<microseconds>(end - start).count();
    }

    cout << "Text size: " << T.size() << ", Pattern size: " << P.size() << "\n";
    cout << "Average KMP time: " << totalKMP / (double)runs << " us\n";
    cout << "Average Rabin-Karp time: " << totalRK / (double)runs << " us\n";
    cout << "KMP found " << resKMP << " occurrences\n";
    cout << "Rabin-Karp found " << resRK << " occurrences\n";
    // If this ratio is >= 2, KMP is at least twice as fast as Rabin-Karp.
    cout << "Speedup (Rabin-Karp / KMP): " << (totalRK / (double)totalKMP) << "x\n";

    return 0;
}
