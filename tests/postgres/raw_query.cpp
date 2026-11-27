#include <iostream>
#include "LazyOrm.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string query1()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES ($1,$2,$3)  returning idx;";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Postgres);
}

std::string query2()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES ($1,$2,$3)  returning idx;";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Postgres);
}

TEST_CASE( "Factorials are computed", "[Lazy_raw_query]" ) {
    REQUIRE( Catch::trim(query1()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
    REQUIRE( Catch::trim(query2()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
}

TEST_CASE("postgres raw query substitutes property values, not property keys", "[Lazy_raw_query][postgres]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id,name) VALUES ($1,$2);";
    // Keys are labels; the values are the parameters. Map key order is
    // "id" < "name", so $1 <- 12 and $2 <- "raw".
    lazyOrm.setProperties({{"name", "raw"}, {"id", 12}});

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
             R"(INSERT INTO users (id,name) VALUES ('12','raw');)" );
}

TEST_CASE("postgres raw query resolves out-of-order numbered placeholders", "[Lazy_raw_query][postgres]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT $2, $1;";
    lazyOrm<<"a"<<"b";

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'b', 'a';)" );
}

TEST_CASE("postgres raw query reuses repeated numbered placeholders", "[Lazy_raw_query][postgres]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $1;";
    lazyOrm<<"x";

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'x', 'x';)" );
}

TEST_CASE("postgres raw query rejects missing, extra and invalid placeholders", "[Lazy_raw_query][postgres]")
{
    // Gap: $2 referenced but only one parameter supplied.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $2;";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // Extra: more parameters than the largest referenced index.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm<<"a"<<"b";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // $0 is not a valid PostgreSQL placeholder.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $0;";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // Placeholder index overflow can never be satisfied.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $99999999999999999999999999;";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // Parameters without any placeholder are silently-discarded extras.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT 1;";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
}

TEST_CASE("postgres raw query ignores placeholder-like text in literals and comments", "[Lazy_raw_query][postgres]")
{
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT '$1', $1;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '$1', 'v';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1 /* $2 */;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'v' /* $2 */;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1 -- $2\n;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == "SELECT 'v' -- $2\n;" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $$ $1 $$, $1;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT $$ $1 $$, 'v';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $body$ $1 $body$, $1;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT $body$ $1 $body$, 'v';)" );
    }
    // A quoted identifier may contain placeholder-like text.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT \"col$1\", $1;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT "col$1", 'v';)" );
    }
}

TEST_CASE("postgres raw query orders parameters by property key", "[Lazy_raw_query][postgres]")
{
    // Insertion order is "b" then "a", but mProperties is a std::map, so
    // parameters follow ascending key order: $1 <- "a", $2 <- "b".
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $2;";
    lazyOrm<<"b"<<"a";

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'a', 'b';)" );
}

TEST_CASE("postgres raw query renders value types with DbVariant conversion rules", "[Lazy_raw_query][postgres]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $2, $3, $4;";
    lazyOrm.setProperties({
        {"b", true},
        {"f", 1.5},
        {"n", "null"},
        {"s", "O'Brien"},
    });

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
             R"(SELECT true, '1.5', NULL, 'O\'Brien';)" );
}

TEST_CASE("postgres raw query enforces gap-free numbered placeholders", "[Lazy_raw_query][postgres]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    // Gap: $2 with 2 parameters ($1 never referenced).
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $2;";
        lazyOrm.setProperties({Pair{"a", "first"}, Pair{"b", "second"}});
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // Gap: $1, $3 with 3 parameters ($2 never referenced).
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $3;";
        lazyOrm.setProperties({Pair{"a", "first"}, Pair{"b", "second"}, Pair{"c", "third"}});
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
    // Valid repeated + complete references: $1, $1, $2 with 2 parameters.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $1, $2;";
        lazyOrm.setProperties({Pair{"a", "first"}, Pair{"b", "second"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT 'first', 'first', 'second';)" );
    }
    // Out-of-order complete references: $2, $1 with 2 parameters.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $2, $1;";
        lazyOrm.setProperties({Pair{"a", "first"}, Pair{"b", "second"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT 'second', 'first';)" );
    }
}

TEST_CASE("postgres raw query distinguishes explicit empty string from shorthand", "[Lazy_raw_query][postgres]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm.setProperties({Pair{"name", ""}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm["name"] = "";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm.setProperty("name", "");
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1, $2;";
        lazyOrm << "alpha" << Pair{"beta", ""};
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'alpha', '';)" );
    }
    // Overwriting shorthand with explicit empty string and vice versa.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm << "name";
        lazyOrm["name"] = "";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm["name"] = "";
        lazyOrm << "name";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'name';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm.setProperties({Pair{"*", ""}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm << "";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '';)" );
    }
}

TEST_CASE("postgres raw query quotes ordinary star values and preserves explicit wildcard", "[Lazy_raw_query][postgres]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm.setProperties({Pair{"pattern", "*"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '*';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1;";
        lazyOrm["pattern"] = "*";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '*';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1 FROM users;";
        lazyOrm << "*";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT * FROM users;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1 FROM users;";
        lazyOrm.setProperties({Pair{"cols", "[no']*"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT * FROM users;)" );
    }
}

TEST_CASE("postgres raw query supports JSONB operators and rejects stray question marks", "[Lazy_raw_query][postgres]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT data #> $1, data #>> $2 FROM users;";
        lazyOrm.setProperties({Pair{"a", "{a,b}"}, Pair{"b", "{c}"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT data #> '{a,b}', data #>> '{c}' FROM users;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT data ? 'key', data ?| $1, data ?& $2 FROM users;";
        lazyOrm.setProperties({Pair{"a", "x"}, Pair{"b", "y"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT data ? 'key', data ?| 'x', data ?& 'y' FROM users;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT data ? 'key' FROM users;";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT data ? 'key' FROM users;)" );
    }
    {
        // '?' is a JSONB operator in PostgreSQL, not a placeholder, so a
        // supplied parameter has no $N placeholder to bind to.
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? FROM users;";
        lazyOrm << "v";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
}

TEST_CASE("postgres raw query uses dialect-specific comments, strings, identifiers, and rejects malformed dollars", "[Lazy_raw_query][postgres]")
{
    // '#' is not a comment in PostgreSQL.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1 # $2;";
        lazyOrm << "a" << "b";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT 'a' # 'b';)" );
    }
    // '--' starts a comment even without trailing whitespace.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $1--$2\n;";
        lazyOrm << "a";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == "SELECT 'a'--$2\n;" );
    }
    // Nested block comments.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT /* outer /* inner $2 */ $3 */ $1;";
        lazyOrm << "a";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 "SELECT /* outer /* inner $2 */ $3 */ 'a';" );
    }
    // Ordinary single-quoted strings do NOT treat backslash as escape; E'...' / e'...' do.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]=R"(SELECT '\', $1, 'a''b\', $2;)";
        lazyOrm << "x" << "y";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT '\', 'x', 'a''b\', 'y';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]=R"(SELECT E'O\'Brien $2', e'\\', $1;)";
        lazyOrm << "x";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT E'O\'Brien $2', e'\\', 'x';)" );
    }
    // Double-quoted identifier does not treat backslash as escape.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]=R"(SELECT "col\", $1;)";
        lazyOrm << "x";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT "col\", 'x';)" );
    }
    // Dollar-quoted strings hide malformed dollar forms too.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT $$ $0 $1abc $ $abc ? $$, $tag$ $2 $tag$, $1;";
        lazyOrm << "x";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT $$ $0 $1abc $ $abc ? $$, $tag$ $2 $tag$, 'x';)" );
    }
    // Unquoted SQL identifiers containing '$' stay intact.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT col$1, v$session, $1;";
        lazyOrm << "x";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Postgres)) ==
                 R"(SELECT col$1, v$session, 'x';)" );
    }
    // Malformed dollar tokens are rejected.
    for (const std::string &badSql : {
             std::string("SELECT $1abc;"),
             std::string("SELECT $1_foo;"),
             std::string("SELECT $1$;"),
             std::string("SELECT $;"),
             std::string("SELECT $abc;"),
             std::string("SELECT $1tag$;"),
         }) {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY] = badSql;
        lazyOrm << "x";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Postgres).empty() );
    }
}
