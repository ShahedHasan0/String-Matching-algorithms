#include <string>
#include <vector>

enum TokenType {
    LITERAL,
    WILDCARD_ONE,
    WILDCARD_ANY
};

struct Token {
    TokenType type;
    char value;
};

static std::vector<Token> tokenizePattern(const std::string& pattern) {
    std::vector<Token> tokens;

    for (size_t i = 0; i < pattern.size(); i++) {
        if (pattern[i] == '\\') {
            if (i + 1 < pattern.size()) {
                tokens.push_back({LITERAL, pattern[i + 1]});
                i++;
            } else {
                tokens.push_back({LITERAL, '\\'});
            }
        } else if (pattern[i] == '?') {
            tokens.push_back({WILDCARD_ONE, '?'});
        } else if (pattern[i] == '*') {
            tokens.push_back({WILDCARD_ANY, '*'});
        } else {
            tokens.push_back({LITERAL, pattern[i]});
        }
    }

    return tokens;
}

static bool matchAt(const std::string& text,
                    const std::vector<Token>& tokens,
                    size_t ti,
                    size_t pi) {
    while (pi < tokens.size()) {
        if (tokens[pi].type == LITERAL) {
            if (ti >= text.size() || text[ti] != tokens[pi].value)
                return false;
            ti++;
            pi++;
        }
        else if (tokens[pi].type == WILDCARD_ONE) {
            if (ti >= text.size())
                return false;
            ti++;
            pi++;
        }
        else if (tokens[pi].type == WILDCARD_ANY) {
            if (pi + 1 == tokens.size())
                return true;

            for (size_t skip = 0; ti + skip <= text.size(); skip++) {
                if (matchAt(text, tokens, ti + skip, pi + 1))
                    return true;
            }

            return false;
        }
    }

    return true;
}

bool bruteForceMatch(const std::string& text, const std::string& pattern) {
    if (pattern.empty())
        return true;

    std::vector<Token> tokens = tokenizePattern(pattern);

    for (size_t i = 0; i <= text.size(); i++) {
        if (matchAt(text, tokens, i, 0))
            return true;
    }

    return false;
}