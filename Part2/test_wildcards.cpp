// Randomized cross-check: wildcard Sunday must agree with wildcard brute force.
// Build (from the Part2 folder):
//   g++ -std=c++17 test_wildcards.cpp Sunday.cpp bruteForce.cpp -o test_wildcards
// Run:
//   ./test_wildcards      (Windows Command Prompt: test_wildcards)
#include <iostream>
#include <random>
#include <string>

bool bruteForceMatch(const std::string& text, const std::string& pattern);
bool sundayMatch(const std::string& text, const std::string& pattern);

int main() {
    std::mt19937 rng(12345);              // fixed seed, so results are reproducible
    const char patternAlphabet[] = {'a', 'b', '?', '*'};
    const int trials = 20000;
    int mismatches = 0;

    for (int t = 0; t < trials; t++) {
        std::string text, pattern;
        int textLen = rng() % 10;
        int patLen = 1 + rng() % 5;

        for (int i = 0; i < textLen; i++) text += "ab"[rng() % 2];
        for (int i = 0; i < patLen; i++) pattern += patternAlphabet[rng() % 4];

        if (bruteForceMatch(text, pattern) != sundayMatch(text, pattern)) {
            if (mismatches < 5)
                std::cout << "MISMATCH: text='" << text << "' pattern='" << pattern << "'\n";
            mismatches++;
        }
    }

    std::cout << mismatches << " mismatches out of " << trials << " random tests\n";
    return mismatches == 0 ? 0 : 1;
}
