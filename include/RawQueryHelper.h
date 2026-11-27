#ifndef RAWQUERYHELPER_H
#define RAWQUERYHELPER_H

#include "DbVariant.h"

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace LazyOrm {
namespace RawQuery {

enum class Dialect
{
    PostgreSQL,
    MariaDB,
    SQLite
};

// One placeholder occurrence found in SQL code, i.e. outside string
// literals, quoted identifiers, comments and (for PostgreSQL) dollar-quoted
// strings.
struct PlaceholderToken
{
    size_t position;          // byte offset of the token start
    size_t length;            // token length in bytes
    bool numbered;            // true: $N, false: positional ?
    unsigned long long index; // 1-based placeholder index (numbered tokens only)
};

struct ScanResult
{
    std::vector<PlaceholderToken> tokens;
    // false when the SQL contains a malformed or unsupported $-prefixed token:
    // $0, $1abc, bare $, $abc, numeric overflow, or dollar-quoting outside
    // PostgreSQL. Such a query can never be satisfied.
    bool valid = true;
};

struct ResolvedParameter
{
    DbVariant value;
    bool fromShorthand = false;
};

// Scans `sql` for placeholders using lexical rules for `dialect`:
// - Single-quoted strings: '' doubling in all dialects; backslash escapes
//   in MariaDB default mode and PostgreSQL E'...' / e'...' escape strings,
//   not in PostgreSQL ordinary '...' (standard_conforming_strings=on) or SQLite.
// - Double-quoted tokens: "" doubling in all dialects; backslash escapes
//   only in MariaDB default mode (without ANSI_QUOTES).
// - Backtick-quoted identifiers: `` doubling (no backslash escapes).
// - SQLite bracket-quoted identifiers: [...].
// - Unquoted identifiers ([A-Za-z_][A-Za-z0-9_$]*): skipped whole so `$`
//   inside identifiers such as `col$1` is never treated as a placeholder.
// - Line comments: `--` unconditionally in PostgreSQL/SQLite, and `--`
//   followed by whitespace/control/EOF in MariaDB; `#` in MariaDB only.
// - Block comments: nested /* */ in PostgreSQL; non-nested in MariaDB/SQLite.
// - Dollar-quoted strings ($tag$ ... $tag$): PostgreSQL only.
// - `?`: positional placeholder in MariaDB/SQLite; JSONB operator in PostgreSQL.
ScanResult scan(const std::string &sql, Dialect dialect);

// Resolves the ordered RAW_QUERY parameter list from the property map.
// Parameters follow std::map key order (ascending DbVariant key order,
// lexicographic for string keys), NOT caller insertion order.
// Keys present in `shorthandKeys` (inserted via key-only `<<` or
// `setProperties(vector<string>)`) contribute their KEY as the parameter;
// all other entries (including explicit empty-string values) contribute
// their VALUE.
std::vector<ResolvedParameter> resolveParameters(const std::map<DbVariant, DbVariant> &properties,
                                                 const std::set<DbVariant> &shorthandKeys);

// Numbered contract: every index in 1..parameterCount must be referenced at
// least once, no index may exceed parameterCount, and no positional `?`
// placeholder tokens may be present. Queries without numbered placeholders
// satisfy the contract only with zero parameters.
bool numberedContractSatisfied(const ScanResult &scanResult, size_t parameterCount);

// Positional contract: only `?` placeholders are permitted (any `$N` token
// causes rejection) and their count must equal `parameterCount`.
bool positionalContractSatisfied(const ScanResult &scanResult, size_t parameterCount);

// Renders one resolved parameter for the regular (string-substitution)
// path, preserving DbVariant::setQuote() conversion semantics. Only an
// intentional shorthand "*" (or explicit "[no']" / "[no`]" prefix via
// DbVariant::setQuote()) renders unquoted; an ordinary keyed parameter
// whose value is "*" is quoted as '*'.
std::string renderParameter(const ResolvedParameter &parameter);

}
}

#endif // RAWQUERYHELPER_H
