#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

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

double measure_time(function<vector<int>(const string&, const string&)> func,
                    const string& T, const string& P, int runs) {

    auto start = high_resolution_clock::now();

    vector<int> res;
    for (int i = 0; i < runs; i++) {
        res = func(P, T);   // ⚠️ note order (your functions use pattern, text)
    }

    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start).count() / (double)runs;
}

int main() {
    ofstream file("Sunday_vs_Z.txt");

    int n = 1000000;
    string T(n, 'E');   // no match case

    string P = "ABCDABCDABCD";

    int runs = 50;

    double z_time = measure_time(gusfieldZ, T, P, runs);
    double s_time = measure_time(binarySunday, T, P, runs);

    double ratio = z_time / s_time;

    file << "GusfieldZ_time(us): " << z_time << "\n";
    file << "Sunday_time(us): " << s_time << "\n";
    file << "Speedup(Z/Sunday): " << ratio << "x\n";

    file.close();

    cout << "Saved to Sunday_vs_Z.txt\n";

    return 0;
}
