#include <iostream>
#include "LazyOrm.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string query1()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES (?,?,?)  returning idx;";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB);
}

std::string query2()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO student (`age`,`hair`,`name`) VALUES ($1,$2,$3)  returning idx;";
    lazyOrm<<"56"<<"black"<<"jack";

    return lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB);
}

TEST_CASE( "Factorials are computed", "[Lazy_raw_query]" ) {
    REQUIRE( Catch::trim(query1()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
    REQUIRE( Catch::trim(query2()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
}

TEST_CASE("mariadb raw query substitutes property values, not property keys", "[Lazy_raw_query][mariadb]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id,name) VALUES (?,?);";
    // Keys are labels; the values are the parameters. Map key order is
    // "id" < "name", so the first ? <- 12 and the second <- "raw".
    lazyOrm.setProperties({{"name", "raw"}, {"id", 12}});

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) ==
             R"(INSERT INTO users (id,name) VALUES ('12','raw');)" );
}

TEST_CASE("mariadb raw query rejects missing and extra positional parameters", "[Lazy_raw_query][mariadb]")
{
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id,name) VALUES (?,?);";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB).empty() );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="INSERT INTO users (id) VALUES (?);";
        lazyOrm<<"a"<<"b";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB).empty() );
    }
    {
        // Parameters without any placeholder are silently-discarded extras.
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT 1;";
        lazyOrm<<"a";
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB).empty() );
    }
}

TEST_CASE("mariadb raw query ignores placeholder-like text in literals and comments", "[Lazy_raw_query][mariadb]")
{
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT '?' AS q, ?;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT '?' AS q, 'v';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? /* ? */;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT 'v' /* ? */;)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? -- ?\n;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == "SELECT 'v' -- ?\n;" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? # ?\n;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == "SELECT 'v' # ?\n;" );
    }
    {
        // A backtick-quoted identifier may contain placeholder-like text.
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT `col?`, ?;";
        lazyOrm<<"v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT `col?`, 'v';)" );
    }
}

TEST_CASE("mariadb raw query orders parameters by property key", "[Lazy_raw_query][mariadb]")
{
    // Insertion order is "b" then "a", but mProperties is a std::map, so
    // parameters follow ascending key order.
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?, ?;";
    lazyOrm<<"b"<<"a";

    REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT 'a', 'b';)" );
}

TEST_CASE("mariadb raw query handles empty strings and ordinary vs shorthand star", "[Lazy_raw_query][mariadb]")
{
    using Pair = std::pair<LazyOrm::DbVariant, LazyOrm::DbVariant>;
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?;";
        lazyOrm.setProperties({Pair{"name", ""}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT '';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?;";
        lazyOrm.setProperties({Pair{"pattern", "*"}});
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT '*';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ? FROM users;";
        lazyOrm << "*";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT * FROM users;)" );
    }
}

TEST_CASE("mariadb raw query respects dialect comments, quotes, and rejects mixed/malformed placeholders", "[Lazy_raw_query][mariadb]")
{
    // MariaDB '--' requires whitespace or control char after '--'.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT 1--2, ?;";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT 1--2, 'v';)" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?--?\n;";
        lazyOrm << "a" << "b";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == "SELECT 'a'--'b'\n;" );
    }
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT ?--\t?\n;";
        lazyOrm << "a";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == "SELECT 'a'--\t?\n;" );
    }
    // Non-nested block comments: first '*/' closes the comment.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT /* /* nested-looking */ ?;";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) ==
                 R"(SELECT /* /* nested-looking */ 'v';)" );
    }
    // Single/double quotes honor backslash escapes; backticks do not.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]=R"(SELECT '\\', 'O\'Brien ?', "\\", "a\"?", `col\`, ?;)";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) ==
                 R"(SELECT '\\', 'O\'Brien ?', "\\", "a\"?", `col\`, 'v';)" );
    }
    // Unquoted identifier with '$'.
    {
        LazyOrm::LazyOrm lazyOrm;
        lazyOrm[LazyOrm::RAW_QUERY]="SELECT col$1, ?;";
        lazyOrm << "v";
        REQUIRE( Catch::trim(lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB)) == R"(SELECT col$1, 'v';)" );
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
        REQUIRE( lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB).empty() );
    }
}
