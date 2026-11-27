#include <iostream>
#include "LazyOrm.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string query1()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES (?,?,?);";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB);
}

std::string query2()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES ($1,$2,$3);";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3);
}

TEST_CASE( "Factorials are computed", "[Lazy_raw_query]" ) {
    REQUIRE( Catch::trim(query1()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack');)" );
    REQUIRE( Catch::trim(query2()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack');)" );
}

TEST_CASE("sqlite raw query substitutes property values, not property keys", "[Lazy_raw_query][sqlite]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id,name) VALUES (?,?);";
    // Keys are labels; the values are the parameters. Map key order is
    // "id" < "name", so the first ? <- 12 and the second <- "raw".
    lazyOrm.setProperties({{"name", "raw"}, {"id", 12}});

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) ==
             R"(INSERT INTO users (id,name) VALUES ('12','raw');)" );
}

TEST_CASE("sqlite raw query rejects missing and extra positional parameters", "[Lazy_raw_query][sqlite]")
{
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id,name) VALUES (?,?);";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3).empty() );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id) VALUES (?);";
        lazyOrm<<"a"<<"b";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3).empty() );
    }
}

TEST_CASE("sqlite raw query ignores placeholder-like text in literals and comments", "[Lazy_raw_query][sqlite]")
{
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT '?' AS q, ?;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT '?' AS q, 'v';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? /* ? */;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT 'v' /* ? */;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? -- ?\n;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == "SELECT 'v' -- ?\n;" );
    }
}

TEST_CASE("sqlite raw query resolves numbered placeholders in the regular path", "[Lazy_raw_query][sqlite]")
{
    // The regular (string-substitution) path supports $N placeholders for
    // every dialect; out-of-order and repeated indices resolve correctly.
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT $2, $1, $1;";
    lazyOrm<<"a"<<"b";

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT 'b', 'a', 'a';)" );
}

TEST_CASE("sqlite raw query handles empty strings and ordinary vs shorthand star", "[Lazy_raw_query][sqlite]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?;";
        lazyOrm.setProperties({Pair{"name", ""}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?;";
        lazyOrm.setProperties({Pair{"pattern", "*"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT '*';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? FROM users;";
        lazyOrm << "*";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT * FROM users;)" );
    }
}

TEST_CASE("sqlite raw query respects dialect comments, quotes, and rejects mixed/malformed placeholders", "[Lazy_raw_query][sqlite]")
{
    // '#' is NOT a comment in SQLite; '--' IS a comment without whitespace.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? # ?\n;";
        lazyOrm << "a" << "b";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == "SELECT 'a' # 'b'\n;" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?--?\n;";
        lazyOrm << "a";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == "SELECT 'a'--?\n;" );
    }
    // Non-nested block comments: first '*/' closes the comment.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT /* /* nested-looking */ ?;";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) ==
                 R"(SELECT /* /* nested-looking */ 'v';)" );
    }
    // No backslash escapes in SQLite literals or quoted identifiers; bracket identifiers supported.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]=R"(SELECT '\', 'a''b\', "col\", `col\`, [col?], ?;)";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) ==
                 R"(SELECT '\', 'a''b\', "col\", `col\`, [col?], 'v';)" );
    }
    // Unquoted identifier with '$'.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT col$1, ?;";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3)) == R"(SELECT col$1, 'v';)" );
    }
    // Reject mixed $N/?, dollar-quoted strings (PostgreSQL-only), and malformed $ tokens.
    for (const std::string &badSql : {
             std::string("SELECT $1, ?;"),
             std::string("SELECT $$ ? $$;"),
             std::string("SELECT $tag$ ? $tag$;"),
             std::string("SELECT $0;"),
             std::string("SELECT $1abc;"),
             std::string("SELECT $;"),
             std::string("SELECT $abc;"),
         }) {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY] = badSql;
        lazyOrm << "v";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3).empty() );
    }
}
