#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <functional>
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

int RabinKarpsearch(const string &T, const string &P) {
    int n = (int)T.size(), m = (int)P.size(), count = 0;
    if (m == 0 || m > n) return 0;

    long long d = 256;
    long long q = 2;   // intentionally small to create many collisions
    long long h = 1;
    long long p_hash = 0;
    long long t_hash = 0;

    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    for (int i = 0; i < m; i++) {
        p_hash = (d * p_hash + P[i]) % q;
        t_hash = (d * t_hash + T[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p_hash == t_hash) {
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
            t_hash = (d * (t_hash - T[i] * h) + T[i + m]) % q;
            if (t_hash < 0) t_hash += q;
        }
    }

    return count;
}

double measure_time(function<int(const string&, const string&)> func,
                    const string& T, const string& P, int runs) {
    auto start = high_resolution_clock::now();

    int res;
    for (int i = 0; i < runs; i++) {
        res = func(T, P);
    }

    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start).count() / (double)runs;
}

int main() {
    ofstream file("KMP_vs_RabinKarp.txt");

    int n = 500000;
    string T;
    T.reserve(n);
    for (int i = 0; i < n; i++) {
        T.push_back((i % 2 == 0) ? 'a' : 'b');
    }

    string P="ababa";

    int runs = 50;

    double kmp_time = measure_time(KMPsearch, T, P, runs);
    double rk_time  = measure_time(RabinKarpsearch, T, P, runs);

    double ratio = rk_time / kmp_time;

    file << "KMP_time(us): " << kmp_time << "\n";
    file << "RabinKarp_time(us): " << rk_time << "\n";
    file << "Speedup(RK/KMP): " << ratio << "x\n";

    file.close();

    cout << "Saved to KMP_vs_RabinKarp.txt\n";

    return 0;
}