#include "RawQueryHelper.h"

#include <algorithm>
#include <cctype>
#include <limits>

namespace LazyOrm {
namespace RawQuery {
namespace {

bool isIdentStart(char ch)
{
    const unsigned char u = static_cast<unsigned char>(ch);
    return std::isalpha(u) || ch == '_' || u >= 0x80;
}

bool isIdentCont(char ch)
{
    const unsigned char u = static_cast<unsigned char>(ch);
    return std::isalnum(u) || ch == '_' || ch == '$' || u >= 0x80;
}

bool isDollarTagCont(char ch)
{
    const unsigned char u = static_cast<unsigned char>(ch);
    return std::isalnum(u) || ch == '_' || u >= 0x80;
}

size_t skipQuoted(const std::string &sql, size_t start, char quote, bool backslashEscapes)
{
    const size_t size = sql.size();
    size_t i = start;
    while (i < size) {
        if (backslashEscapes && sql[i] == '\\' && i + 1 < size) {
            i += 2;
        } else if (sql[i] == quote && i + 1 < size && sql[i + 1] == quote) {
            i += 2;
        } else if (sql[i] == quote) {
            ++i;
            break;
        } else {
            ++i;
        }
    }
    return i;
}

size_t skipLineComment(const std::string &sql, size_t start)
{
    const size_t size = sql.size();
    size_t i = start;
    while (i < size && sql[i] != '\n' && sql[i] != '\r') {
        ++i;
    }
    return i;
}

} // namespace

ScanResult scan(const std::string &sql, Dialect dialect)
{
    ScanResult result;
    const size_t size = sql.size();
    size_t i = 0;

    while (i < size) {
        const char c = sql[i];

        if (dialect == Dialect::PostgreSQL && (c == 'E' || c == 'e') &&
            i + 1 < size && sql[i + 1] == '\'') {
            // PostgreSQL escape string literal E'...' / e'...'.
            i = skipQuoted(sql, i + 2, '\'', true);
            continue;
        }

        if (c == '\'') {
            // Single-quoted string literal. Backslash escapes apply in
            // MariaDB default mode, not in SQLite or PostgreSQL ordinary
            // strings under standard_conforming_strings=on.
            const bool backslashEscapes = (dialect == Dialect::MariaDB);
            i = skipQuoted(sql, i + 1, '\'', backslashEscapes);
            continue;
        }

        if (c == '"') {
            // In MariaDB default mode (without ANSI_QUOTES), "..." is a
            // string literal that honors backslash escapes.
            const bool backslashEscapes = (dialect == Dialect::MariaDB);
            i = skipQuoted(sql, i + 1, '"', backslashEscapes);
            continue;
        }

        if (c == '`') {
            i = skipQuoted(sql, i + 1, '`', false);
            continue;
        }

        if (dialect == Dialect::SQLite && c == '[') {
            ++i;
            while (i < size && sql[i] != ']') {
                ++i;
            }
            if (i < size) {
                ++i;
            }
            continue;
        }

        if (c == '-' && i + 1 < size && sql[i + 1] == '-') {
            if (dialect == Dialect::MariaDB) {
                // MariaDB/MySQL requires whitespace, a control character, or
                // EOF immediately after `--` to form a comment.
                if (i + 2 < size) {
                    const unsigned char next = static_cast<unsigned char>(sql[i + 2]);
                    if (!std::isspace(next) && !std::iscntrl(next)) {
                        i += 2;
                        continue;
                    }
                }
            }
            i = skipLineComment(sql, i + 2);
            continue;
        }

        if (c == '#' && dialect == Dialect::MariaDB) {
            i = skipLineComment(sql, i + 1);
            continue;
        }

        if (c == '/' && i + 1 < size && sql[i + 1] == '*') {
            const bool nested = (dialect == Dialect::PostgreSQL);
            size_t depth = 1;
            i += 2;
            while (i < size && depth > 0) {
                if (nested && sql[i] == '/' && i + 1 < size && sql[i + 1] == '*') {
                    ++depth;
                    i += 2;
                } else if (sql[i] == '*' && i + 1 < size && sql[i + 1] == '/') {
                    --depth;
                    i += 2;
                } else {
                    ++i;
                }
            }
            continue;
        }

        if (isIdentStart(c)) {
            ++i;
            while (i < size && isIdentCont(sql[i])) {
                ++i;
            }
            continue;
        }

        if (c == '?') {
            if (dialect != Dialect::PostgreSQL) {
                result.tokens.push_back({i, 1, false, 0});
            }
            ++i;
            continue;
        }

        if (c == '$') {
            if (i + 1 < size && std::isdigit(static_cast<unsigned char>(sql[i + 1]))) {
                size_t j = i + 1;
                unsigned long long index = 0;
                bool overflow = false;
                while (j < size && std::isdigit(static_cast<unsigned char>(sql[j]))) {
                    const unsigned digit = static_cast<unsigned>(sql[j] - '0');
                    if (index > (std::numeric_limits<unsigned long long>::max() - digit) / 10) {
                        overflow = true;
                    } else {
                        index = index * 10 + digit;
                    }
                    ++j;
                }
                // Reject trailing identifier characters like $1abc, $1_x, or $1$.
                if (j < size && isIdentCont(sql[j])) {
                    while (j < size && isIdentCont(sql[j])) {
                        ++j;
                    }
                    result.valid = false;
                    i = j;
                    continue;
                }
                if (overflow || index == 0) {
                    result.valid = false;
                    i = j;
                    continue;
                }
                result.tokens.push_back({i, j - i, true, index});
                i = j;
                continue;
            }

            if (dialect == Dialect::PostgreSQL) {
                // Dollar-quoted string: $$...$$ or $tag$...$tag$ where tag
                // starts with an identifier start character (never a digit).
                bool isDollarQuote = false;
                size_t j = i + 1;
                if (j < size && sql[j] == '$') {
                    isDollarQuote = true;
                } else if (j < size && isIdentStart(sql[j])) {
                    ++j;
                    while (j < size && isDollarTagCont(sql[j])) {
                        ++j;
                    }
                    if (j < size && sql[j] == '$') {
                        isDollarQuote = true;
                    }
                }
                if (isDollarQuote) {
                    const std::string delimiter = sql.substr(i, j - i + 1);
                    const size_t close = sql.find(delimiter, j + 1);
                    i = (close == std::string::npos) ? size : (close + delimiter.size());
                    continue;
                }
            }

            // Bare `$`, `$abc` without closing `$`, or non-PostgreSQL dollar syntax.
            result.valid = false;
            ++i;
            while (i < size && isIdentCont(sql[i])) {
                ++i;
            }
            continue;
        }

        ++i;
    }
    return result;
}

std::vector<ResolvedParameter> resolveParameters(const std::map<DbVariant, DbVariant> &properties,
                                                 const std::set<DbVariant> &shorthandKeys)
{
    std::vector<ResolvedParameter> values;
    values.reserve(properties.size());
    for (const auto &[key, value] : properties) {
        const bool fromShorthand = shorthandKeys.find(key) != shorthandKeys.end();
        values.push_back({fromShorthand ? key : value, fromShorthand});
    }
    return values;
}

bool numberedContractSatisfied(const ScanResult &scanResult, size_t parameterCount)
{
    if (!scanResult.valid) {
        return false;
    }
    std::vector<bool> seen(parameterCount, false);
    for (const auto &token : scanResult.tokens) {
        if (!token.numbered) {
            return false;
        }
        if (token.index == 0 || token.index > parameterCount) {
            return false;
        }
        seen[static_cast<size_t>(token.index - 1)] = true;
    }
    return std::all_of(seen.begin(), seen.end(), [](bool present) { return present; });
}

bool positionalContractSatisfied(const ScanResult &scanResult, size_t parameterCount)
{
    if (!scanResult.valid) {
        return false;
    }
    size_t positionalCount = 0;
    for (const auto &token : scanResult.tokens) {
        if (token.numbered) {
            return false;
        }
        ++positionalCount;
    }
    return positionalCount == parameterCount;
}

std::string renderParameter(const ResolvedParameter &parameter)
{
    if (parameter.fromShorthand &&
        parameter.value.typeName() == "string" &&
        parameter.value.toString() == "*") {
        return "*";
    }
    return parameter.value.setQuote();
}

}
}
