#include <iostream>
#include <string>

bool bruteForceMatch(const std::string& text, const std::string& pattern);
bool sundayMatch(const std::string& text, const std::string& pattern);

int main() {
    std::string text;
    std::string pattern;

    std::cout << "Enter text: ";
    std::getline(std::cin, text);

    std::cout << "Enter pattern (with *, ?, \\): ";
    std::getline(std::cin, pattern);

    bool bruteResult = bruteForceMatch(text, pattern);
    bool sundayResult = sundayMatch(text, pattern);

    std::cout << std::boolalpha;
    std::cout << "\n=== Results ===\n";
    std::cout << "Brute-force: " << bruteResult << "\n";
    std::cout << "Sunday:      " << sundayResult << "\n";

    if (bruteResult == sundayResult) {
        std::cout << "Result: CONSISTENT\n";
    } else {
        std::cout << "Result: MISMATCH\n";
    }

    return 0;
}