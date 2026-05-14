#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;



vector<int> gusfieldZ(const string& pattern, const string& text) {
    string s = pattern + "$" + text;
    int n = (int)s.size();
    vector<int> Z(n, 0);

    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i <= r) Z[i] = min(r - i + 1, Z[i - l]);
        while (i + Z[i] < n && s[Z[i]] == s[i + Z[i]]) Z[i]++;
        if (i + Z[i] - 1 > r) {
            l = i;
            r = i + Z[i] - 1;
        }
    }

    vector<int> occurrences;
    int m = (int)pattern.size();
    for (int i = m + 1; i < n; i++) {
        if (Z[i] == m) {
            occurrences.push_back(i - m - 1);
        }
    }
    return occurrences;
}


vector<int> binarySunday(const string& pattern, const string& text) {
    int n = (int)text.size();
    int m = (int)pattern.size();


    vector<int> shift(256, m + 1);
    for (int i = 0; i < m; i++) {
        shift[(unsigned char)pattern[i]] = m - i;
    }

    vector<int> occurrences;
    int i = 0;
    while (i <= n - m) {
        int j = 0;
        while (j < m && pattern[j] == text[i + j]) j++;
        if (j == m) occurrences.push_back(i);

        if (i + m >= n) break;
        i += shift[(unsigned char)text[i + m]];
    }
    return occurrences;
}

int main() {
    // Benchmark case: 100k text with no possible match for the pattern.
    // This favors Sunday shifts and is used to demonstrate empirical speed gap.
    string T(1000000, 'E');
    string P = "ABCDABCDABCD";

    const int repeats = 100;

    // Time both algorithms on exactly the same input.
    vector<int> resZ;
    vector<int> resBS;

    auto startZ = high_resolution_clock::now();
    for (int i = 0; i < repeats; i++) {
        resZ = gusfieldZ(P, T);
    }
    auto endZ = high_resolution_clock::now();
    double durationZ = duration_cast<microseconds>(endZ - startZ).count() / 1000.0 / repeats;

    auto startBS = high_resolution_clock::now();
    for (int i = 0; i < repeats; i++) {
        resBS = binarySunday(P, T);
    }
    auto endBS = high_resolution_clock::now();
    double durationBS = duration_cast<microseconds>(endBS - startBS).count() / 1000.0 / repeats;;

    cout << "Gusfield Z: " << durationZ << " ms, matches found: " << resZ.size() << "\n";
    cout << "Binary Sunday: " << durationBS << " ms, matches found: " << resBS.size() << "\n";

    // Assignment check: Sunday should be at least 2x faster in this crafted case.
    if (durationBS * 2 <= durationZ) {
        cout << "Binary Sunday is at least twice as fast as Gusfield Z.\n";
    } else {
        cout << "Binary Sunday is NOT twice as fast in this run.\n";
    }

    return 0;
}
