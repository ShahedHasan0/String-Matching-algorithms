#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>



int rabin_karp(const std::string& T, const std::string& P, long long q = 1000003) {
    // Rolling hash lets Rabin-Karp skip full comparisons most of the time.
    // The larger modulus keeps hash collisions lower in this benchmark.
    long long d = 256;
    int M = P.size();
    int N = T.size();
    long long h = 1;
    for (int i = 0; i < M - 1; i++)
        h = (h * d) % q;

    long long p = 0, t = 0;
    for (int i = 0; i < M; i++) {
        p = (d * p + P[i]) % q;
        t = (d * t + T[i]) % q;
    }

    for (int s = 0; s <= N - M; s++) {
        if (p == t && T.substr(s, M) == P)
            return s;
        if (s < N - M) {
            t = (d * (t - T[s] * h) + T[s + M]) % q;
            if (t < 0) t += q;
        }
    }
    return -1;
}


int sunday(const std::string& T, const std::string& P) {
    // Sunday looks at the next character after the current window to decide the jump.
    // In this case, that still forces many repeated character checks.
    int N = T.size(), M = P.size();
    std::unordered_map<char, int> shift;
    for (int i = 0; i < M; ++i) {
        shift[P[i]] = M - i;
    }

    int i = 0;
    while (i <= N - M) {
        if (T.substr(i, M) == P)
            return i;
        if (i + M >= N)
            return -1;
        char next_char = T[i + M];
        i += shift.count(next_char) ? shift[next_char] : M + 1;
    }
    return -1;
}

int main() {
    // Pattern is long and almost all 'a' so Sunday will compare many characters per shift.
    // The last 'b' prevents a match, so both algorithms must scan the whole text.
    std::string P(255, 'a');
    P.push_back('b');

    // Text is also deterministic, so the benchmark is fully reproducible.
    // This setup is chosen to make Rabin-Karp win in practice.
    std::string T(500000, 'a');

    std::cout << "Running Rabin-Karp...\n";
    // Run both algorithms multiple times and average the timing.
    int runs = 20;
    double rk_time = 0.0;
    double s_time = 0.0;
    int rk_result = -1;
    int s_result = -1;

    for (int i = 0; i < runs; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        rk_result = rabin_karp(T, P);
        auto end = std::chrono::high_resolution_clock::now();
        rk_time += std::chrono::duration<double>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        s_result = sunday(T, P);
        end = std::chrono::high_resolution_clock::now();
        s_time += std::chrono::duration<double>(end - start).count();
    }

    rk_time /= runs;
    s_time /= runs;

    std::cout << "\n--- Results ---\n";
    std::cout << "Rabin-Karp: " << rk_time << " seconds\n";
    std::cout << "Sunday:     " << s_time << " seconds\n";
    std::cout << "Rabin-Karp result: " << rk_result << "\n";
    std::cout << "Sunday result:     " << s_result << "\n";
    // If Sunday / Rabin-Karp >= 2, then Rabin-Karp is at least 2x faster.
    // This is the line to mention during defense when proving the assignment condition.
    // If this ratio is above 2, the assignment condition is satisfied.
    std::cout << "Ratio (Sunday / Rabin-Karp): " << (s_time / rk_time) << "x faster\n";

    return 0;
}
