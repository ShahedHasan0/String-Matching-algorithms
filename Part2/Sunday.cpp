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

static bool segmentMatchesAt(
    const std::string& text,
    const std::vector<Token>& segment,
    size_t pos
) {
    if (pos + segment.size() > text.size())
        return false;

    for (size_t i = 0; i < segment.size(); i++) {
        if (segment[i].type == LITERAL && text[pos + i] != segment[i].value)
            return false;

        if (segment[i].type == WILDCARD_ONE)
            continue;
    }

    return true;
}

static size_t sundayFindSegment(
    const std::string& text,
    const std::vector<Token>& segment,
    size_t start
) {
    size_t n = text.size();
    size_t m = segment.size();

    if (m == 0)
        return start;

    if (start + m > n)
        return std::string::npos;

    std::unordered_map<char, size_t> shift;

    for (size_t i = 0; i < m; i++) {
        if (segment[i].type == LITERAL) {
            shift[segment[i].value] = m - i;
        }
    }

    size_t questionShift = m + 1;

    for (size_t i = 0; i < m; i++) {
        if (segment[i].type == WILDCARD_ONE) {
            questionShift = m - i;
        }
    }

    size_t pos = start;

    while (pos + m <= n) {
        if (segmentMatchesAt(text, segment, pos))
            return pos;

        size_t nextIndex = pos + m;

        if (nextIndex >= n)
            break;

        char nextChar = text[nextIndex];

        size_t shiftValue = m + 1;

        if (shift.count(nextChar)) {
            shiftValue = shift[nextChar];
        }

        if (questionShift < shiftValue) {
            shiftValue = questionShift;
        }

        pos += shiftValue;
    }

    return std::string::npos;
}

static std::vector<std::vector<Token>> splitByStar(const std::vector<Token>& tokens) {
    std::vector<std::vector<Token>> segments;
    std::vector<Token> current;

    for (const Token& token : tokens) {
        if (token.type == WILDCARD_ANY) {
            segments.push_back(current);
            current.clear();
        } else {
            current.push_back(token);
        }
    }

    segments.push_back(current);
    return segments;
}

bool sundayMatch(const std::string& text, const std::string& pattern) {
    if (pattern.empty())
        return true;

    std::vector<Token> tokens = tokenizePattern(pattern);
    std::vector<std::vector<Token>> segments = splitByStar(tokens);

    bool startsWithStar = !tokens.empty() && tokens.front().type == WILDCARD_ANY;
    bool endsWithStar = !tokens.empty() && tokens.back().type == WILDCARD_ANY;

    size_t currentPos = 0;

    for (size_t s = 0; s < segments.size(); s++) {
        const std::vector<Token>& segment = segments[s];

        if (segment.empty())
            continue;

        size_t found = sundayFindSegment(text, segment, currentPos);

        if (found == std::string::npos)
            return false;

        if (s == 0 && !startsWithStar && found != 0)
            return false;

        currentPos = found + segment.size();
    }

    return true;
}