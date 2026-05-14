#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>

using namespace std;
using namespace chrono;

//  BruteForce: check every possible position in text.
int bruteForce(const string &text, const string &pattern) {
    // If pattern is empty or longer than text, it cannot be found.
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    // Try every possible start position in text.
    for (size_t i = 0; i <= text.size() - pattern.size(); ++i) {
        size_t j = 0;
        // Compare characters one by one
        while (j < pattern.size() && text[i + j] == pattern[j])
            ++j;
        //If full match is found, return starting index.
        if (j == pattern.size())
            return i;
    }
    return -1;
}

//  Sunday: uses skip table to jump ahead in text.
int sunday(const string &text, const string &pattern) {
    // Quick checks for empty pattern or pattern longer than text.
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    int n = text.size(), m = pattern.size();
    unordered_map<char, int> shift;
    // Build shift table
    for (int i = 0; i < m; ++i)
        shift[pattern[i]] = m - i;
    int i = 0;
    while (i <= n - m) {
        int j = 0;
        // Compare pattern with current text window
        while (j < m && text[i + j] == pattern[j])
            ++j;
        if (j == m)
            return i;
        // Jump based on character right after next character.
        if (i + m >= n) break;
        i += shift.count(text[i + m]) ? shift[text[i + m]] : m + 1;
    }
    return -1;
}

//  Build prefix table for KMP
vector<int> computePrefix(const string &pattern) {
    int m = pattern.length();
    vector<int> prefix(m, 0);
    int k = 0;
    // Compute longest prefix=suffix
    for (int q = 1; q < m; ++q) {
        while (k > 0 && pattern[k] != pattern[q])
            k = prefix[k - 1];
        if (pattern[k] == pattern[q])
            k++;
        prefix[q] = k;
    }
    return prefix;
}

//KMP: avoiding re-checking characters by using prefix table.
int kmp(const string &text, const string &pattern) {
    // Quick checks for empty pattern or pattern longer than text.
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    int n = text.length(), m = pattern.length();
    vector<int> prefix = computePrefix(pattern);
    int q = 0;
    // Scan text once
    for (int i = 0; i < n; ++i) {
        while (q > 0 && pattern[q] != text[i])
            q = prefix[q - 1];
        if (pattern[q] == text[i])
            q++;
        if (q == m)
            return i - m + 1;
    }
    return -1;
}

//FSM: build full transition table for pattern matching.
int fsm(const string &text, const string &pattern) {
    // Quick checks for empty pattern or pattern longer than text.
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    int n = text.length(), m = pattern.length();
    // dp[state][char] is the next state.
    vector<unordered_map<char, int>> dp(m + 1);
    // Build transitions
    for (int i = 0; i <= m; ++i) {
        for (char c = ' '; c <= '~'; ++c) {
            int k = min(m, i + 1);
            while (k > 0 && pattern.substr(0, k) != (pattern.substr(0, i) + c).substr(i + 1 - k))
                --k;
            dp[i][c] = k;
        }
    }
    int state = 0;
    // Run FSM
    for (int i = 0; i < n; ++i) {
        state = dp[state][text[i]];
        if (state == m)
            return i - m + 1;
    }
    return -1;
}

//  Rabin-Karp: uses rolling hash to quickly compare pattern with text windows.
int rabinKarp(const string &text, const string &pattern) {
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    const int d = 256, q = 101;
    int n = text.length(), m = pattern.length();
    int h = 1, p = 0, t = 0; //p → hash of pattern t → hash of current text window h → helper value (for removing old char)

    // Precompute multiplier
    for (int i = 0; i < m - 1; i++) h = (h * d) % q;

    // Initial hash values for pattern and first text window.
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }
    
    // Slide window
    for (int i = 0; i <= n - m; i++) {
        // If hashes match, verify actual substring to avoid collision.
        if (p == t && text.substr(i, m) == pattern)
            return i;
        if (i < n - m) {
            // Roll hash: remove left char, add next right char.
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t += q;
        }
    }
    return -1;
}

//  Z-algorithm: computes prefix matches.
vector<int> zFunction(const string &s) {
    int n = s.length();
    vector<int> z(n);
    int l = 0, r = 0;
    // Build Z array where z[i] is prefix match length at i.
    for (int i = 1; i < n; i++) {
        if (i <= r)
            z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]])
            ++z[i];
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

// Use Z-array to find pattern in text.
int gusfieldZ(const string &text, const string &pattern) {
    // Quick checks for empty pattern or pattern longer than text.
    if (pattern.empty() || pattern.size() > text.size())
        return -1;
    // Use "pattern$text" so full pattern matches appear in Z values.
    string s = pattern + "$" + text;
    vector<int> z = zFunction(s);
    int m = pattern.length();
    for (int i = m + 1; i < z.size(); ++i) {
        if (z[i] == m)
            return i - m - 1;
    }
    return -1;
}

//   Saving results to CSV file
void logToCSV(
    ofstream &csv,
    const string &algorithm,
    int textLength,
    const string &patternType,
    int patternLength,
    double timeSeconds,
    int foundIndex
) {
    csv << algorithm << ","
        << textLength << ","
        << patternType << ","
        << patternLength << ","
        << timeSeconds << ","
        << foundIndex << "\n";
}

// ---------- MAIN ----------
int main() {
    //Open book file for reading.
    ifstream chapterFile("book.txt");
    if (!chapterFile.is_open()) {
        cerr << "Error can't open book.txt\n";
        return 1;
    }
    //Creating CSV output file for results.
    ofstream csv("results.csv");
    if (!csv.is_open()) {
        cerr << "Error can't create results.csv\n";
        return 1;
    }

    csv << "Algorithm,TextLength,PatternType,PatternLength,TimeSeconds,FoundIndex\n";

    // Read entire book into a string.
    string text((istreambuf_iterator<char>(chapterFile)), istreambuf_iterator<char>());

    // Replace newlines with spacing.
    for (auto& ch : text) {
        if (ch == '\n' || ch == '\r') ch = ' ';
    }

    // Remove extra spaces
    string cleanedText;
    bool lastWasSpace = false;
    for (char ch : text) {
        if (ch == ' ') {
            if (!lastWasSpace) {
                cleanedText += ch;
                lastWasSpace = true;
            }
        } else {
            cleanedText += ch;
            lastWasSpace = false;
        }
    }

    // Choose patterns from end of text
    int smallStart = cleanedText.size() - 1000;
    int largeStart = cleanedText.size() - 2000;

    string smallPattern = cleanedText.substr(smallStart, 70);
    string largePattern = cleanedText.substr(largeStart, 300);

    vector<pair<string, string>> patterns = {
        {"Small", smallPattern},
        {"Large", largePattern}
    };

    // Different text lengths to test algorithms on.
    vector<int> textLengths = {
        1000,
        5000,
        10000,
        25000,
        50000,
        100000,
        200000,
    };

    // List of algorithms to benchmark.
    vector<pair<string, function<int(const string&, const string&)>>> algorithms = {
        {"BruteForce", bruteForce},
        {"Sunday", sunday},
        {"KMP", kmp},
        {"FSM", fsm},
        {"RabinKarp", rabinKarp},
        {"ZAlgorithm", gusfieldZ}
    };

    // Run experiments for each combination of text length, pattern type, and algorithm.
    for (int len : textLengths) {
        if (len > (int)cleanedText.size()) continue;

        string currentText = cleanedText.substr(0, len);

        for (const auto& [patternType, pattern] : patterns) {
            int patternLength = (int)pattern.length();

            for (auto& [name, func] : algorithms) {
                int foundIndex = -1;
                const int repeats = 30;

                auto start = high_resolution_clock::now();

                // Repeat to get stable timing.
                for (int r = 0; r < repeats; r++) {
                    foundIndex = func(currentText, pattern);
            }

            auto end = high_resolution_clock::now();

                double durationSeconds =
                (duration_cast<microseconds>(end - start).count() / 1000000.0) / repeats;
                logToCSV(
                    csv,
                    name,
                    len,
                    patternType,
                    patternLength,
                    durationSeconds,
                    foundIndex
                );

                cout<< name
                    << ", text length: " << len
                    << ", pattern type: "<< patternType
                    << ", pattern length: " << patternLength
                    << ", time: " << durationSeconds << " s"
                    << ", found at: " << foundIndex << "\n";
            }
        }
    }


    cout<< "Ready results.csv\n";
    return 0;
}