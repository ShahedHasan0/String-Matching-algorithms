#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <functional>

using namespace std;
using namespace std::chrono;

int rabinKarp(const string& T, const string& P) {
    if (P.empty() || P.size() > T.size()) return -1;

    long long d = 256;
    long long q = 1000003;

    int N = (int)T.size();
    int M = (int)P.size();

    long long h = 1;
    for (int i = 0; i < M - 1; i++)
        h = (h * d) % q;

    long long pHash = 0;
    long long tHash = 0;

    for (int i = 0; i < M; i++) {
        pHash = (d * pHash + P[i]) % q;
        tHash = (d * tHash + T[i]) % q;
    }

    for (int s = 0; s <= N - M; s++) {
        if (pHash == tHash && T.substr(s, M) == P)
            return s;

        if (s < N - M) {
            tHash = (d * (tHash - T[s] * h) + T[s + M]) % q;
            if (tHash < 0) tHash += q;
        }
    }

    return -1;
}

int sunday(const string& T, const string& P) {
    if (P.empty() || P.size() > T.size()) return -1;

    int N = (int)T.size();
    int M = (int)P.size();

    unordered_map<char, int> shift;

    for (int i = 0; i < M; i++) {
        shift[P[i]] = M - i;
    }

    int i = 0;

    while (i <= N - M) {
        if (T.substr(i, M) == P)
            return i;

        if (i + M >= N) return -1;

        char nextChar = T[i + M];

        if (shift.count(nextChar))
            i += shift[nextChar];
        else
            i += M + 1;
    }

    return -1;
}

double measure_time(function<int(const string&, const string&)> func,
                    const string& T,
                    const string& P,
                    int runs) {
    auto start = high_resolution_clock::now();

    int result = -1;
    for (int i = 0; i < runs; i++) {
        result = func(T, P);
    }

    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start).count() / (double)runs;
}

int main() {
    ofstream file("RabinKarp_vs_Sunday.txt");

    int n = 500000;

    string T(n, 'a');

    string P(255, 'a');
    P.push_back('b');

    int runs = 20;

    double rk_time = measure_time(rabinKarp, T, P, runs);
    double sunday_time = measure_time(sunday, T, P, runs);

    double ratio = sunday_time / rk_time;

    file << "RabinKarp_time(us): " << rk_time << "\n";
    file << "Sunday_time(us): " << sunday_time << "\n";
    file << "Speedup(Sunday/RabinKarp): " << ratio << "x\n";

    file.close();

    cout << "Saved to RabinKarp_vs_Sunday.txt\n";
    cout << "RabinKarp_time(us): " << rk_time << "\n";
    cout << "Sunday_time(us): " << sunday_time << "\n";
    cout << "Speedup(Sunday/RabinKarp): " << ratio << "x\n";

    return 0;
}