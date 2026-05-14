// Task 2: Wildcard Pattern Matching (Brute-force and Sunday algorithms)
// ? = single char, * = zero or more chars, \ = escape special chars

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

enum TokenType {
    LITERAL,
    WILDCARD_ONE,
    WILDCARD_ANY
};

struct Token {
    TokenType type;
    char value;
};


// Parse pattern into tokens: handle escaping (\), wildcards (?, *), and literals
std::vector<Token> tokenizePattern(const std::string& pattern) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '\\') {
            // Escape char: next char is literal (not wildcard)
            if (i + 1 < pattern.size()) {
                tokens.push_back({LITERAL, pattern[i + 1]});
                ++i;
            } else {
                tokens.push_back({LITERAL, '\\'});
            }
        } else if (pattern[i] == '?') {
            tokens.push_back({WILDCARD_ONE, '?'});  // ? = match exactly one char
        } else if (pattern[i] == '*') {
            tokens.push_back({WILDCARD_ANY, '*'});  // * = match zero or more chars
        } else {
            tokens.push_back({LITERAL, pattern[i]});  // Regular character
        }
    }
    return tokens;
}

bool hasStar(const std::vector<Token>& tokens) {
    for (const auto& tok : tokens) {
        if (tok.type == WILDCARD_ANY) return true;
    }
    return false;
}


// Core matching: check if pattern tokens match text starting at position t_idx
bool matchAt(const std::string& text, const std::vector<Token>& tokens, size_t t_idx, size_t p_idx) {
    size_t text_len = text.size();
    size_t pattern_len = tokens.size();

    while (p_idx < pattern_len) {
        if (t_idx > text_len) return false;

        Token tok = tokens[p_idx];
        if (tok.type == LITERAL) {
            // Literal: must match exact char
            if (t_idx >= text_len || text[t_idx] != tok.value)
                return false;
            ++t_idx;
            ++p_idx;
        } else if (tok.type == WILDCARD_ONE) {
            // ?: consume exactly one char
            if (t_idx >= text_len)
                return false;
            ++t_idx;
            ++p_idx;
        } else if (tok.type == WILDCARD_ANY) {
            // *: match zero or more chars (backtracking approach)
            if (p_idx + 1 == pattern_len)
                return true;  // * at end matches everything remaining

            // Try all skip lengths: 0, 1, 2, ... until match found
            for (size_t skip = 0; t_idx + skip <= text_len; ++skip) {
                if (matchAt(text, tokens, t_idx + skip, p_idx + 1)) {
                    return true;
                }
            }
            return false;
        }
    }

    return true;  // All pattern tokens matched successfully
}


bool matchLengthAt(const std::string& text, const std::vector<Token>& tokens, size_t start) {
    return matchAt(text, tokens, start, 0);
}


// Brute-force: try matching at every position in text
bool bruteForceMatch(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) return true;
    auto tokens = tokenizePattern(pattern);
    for (size_t i = 0; i <= text.size(); ++i) {
        // Check if pattern matches starting at position i
        if (matchLengthAt(text, tokens, i)) {
            return true;  // Found match
        }
    }
    return false;  // No match found
}


// Build shift table for Sunday: store skip distances for literal chars
std::unordered_map<char, size_t> buildShiftTable(const std::vector<Token>& tokens) {
    std::unordered_map<char, size_t> table;
    size_t len = tokens.size();
    for (size_t i = 0; i < len; ++i) {
        if (tokens[i].type == LITERAL) {
            table[tokens[i].value] = len - i;  // Distance to shift
        }
    }
    return table;
}


// Sunday: optimized search using skip table to avoid unnecessary checks
bool sundayMatch(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) return true;
    auto tokens = tokenizePattern(pattern);
    if (hasStar(tokens)) {
        return bruteForceMatch(text, pattern);
    }
    auto shiftTable = buildShiftTable(tokens);
    size_t i = 0;
    while (i + tokens.size() <= text.size()) {
        if (matchLengthAt(text, tokens, i)) {
            return true;  // Found match
        }
        // Look at next char and shift accordingly
        size_t nextCharIdx = i + tokens.size();
        if (nextCharIdx < text.size()) {
            char nextChar = text[nextCharIdx];
            if (shiftTable.count(nextChar))
                i += shiftTable[nextChar];  // Shift by table value
            else
                i += tokens.size() + 1;  // Not in table, skip whole pattern
        } else {
            break;  // No more text to check
        }
    }
    return false;  // No match found
}

int main() {
    std::string text, pattern;
    std::cout << "Enter text: ";
    std::getline(std::cin, text);
    std::cout << "Enter pattern (with *, ?, \\): ";
    std::getline(std::cin, pattern);

    // Run Brute-force algorithm
    bool bf_result = bruteForceMatch(text, pattern);
    std::cout << "Brute-force: " << (bf_result ? "true" : "false") << "\n";

    // Run Sunday algorithm
    bool s_result = sundayMatch(text, pattern);
    std::cout << "Sunday: " << (s_result ? "true" : "false") << "\n";

    return 0;
}