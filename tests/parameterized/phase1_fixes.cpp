#include "DbList.h"
#include "JoinMariadb.h"
#include "LazyOrm.h"
#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"

#include <catch2/catch_all.hpp>

TEST_CASE("copies transfer count properties", "[copy][counts]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source.setCountType({"age"});

    LazyOrm::MariadbLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name`,COUNT(`age`) FROM student;)");
}

TEST_CASE("copies transfer unique keys", "[copy][unique_keys]")
{
    LazyOrm::PostgreLazy source;
    source[LazyOrm::INSERT_UPDATE] = "users";
    source[LazyOrm::UniqueKeys] = {"id"};
    source["id"] = 7;
    source["name"] = "inserted";
    source["[update]name"] = "updated";

    LazyOrm::PostgreLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(INSERT INTO users ("id","name") VALUES ('7','inserted') ON CONFLICT ("id") DO UPDATE SET "name"='updated';)");
}

TEST_CASE("copies transfer descending order type", "[copy][orderby]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::ORDERBY_DESC] = {{"num1"}};

    LazyOrm::MariadbLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name` FROM student ORDER BY `num1` DESC ;)");
}

TEST_CASE("LazyOrm builds parameterized SQLite queries", "[parameterized][sqlite][lazyorm]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::INSERT] = "users";
    lazyOrm.setProperties({{"id", 7}, {"name", "jack"}});

    LazyOrm::SqliteLazy direct;
    direct[LazyOrm::INSERT] = "users";
    direct.setProperties({{"id", 7}, {"name", "jack"}});

    const auto viaLazyOrm = lazyOrm.buildParameterized(LazyOrm::LazyOrm::Sqlite3);
    const auto expected = direct.buildParameterized();
    REQUIRE_FALSE(viaLazyOrm.sql.empty());
    REQUIRE(Catch::trim(viaLazyOrm.sql) == Catch::trim(expected.sql));
    REQUIRE(viaLazyOrm.params.size() == expected.params.size());
}

TEST_CASE("parameterized LIMIT accepts comma vector syntax", "[parameterized][limit]")
{
    LazyOrm::PostgreLazy postgreQuery;
    postgreQuery[LazyOrm::SELECT] = "users";
    postgreQuery << "id";
    postgreQuery[LazyOrm::LIMIT] = {{11, ",", 23}};
    const auto postgre = postgreQuery.buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(SELECT "id" FROM users LIMIT $1 OFFSET $2 ;)");
    REQUIRE(postgre.params.size() == 2);
    REQUIRE(postgre.params[0].toLongLong() == 11);
    REQUIRE(postgre.params[1].toLongLong() == 23);

    LazyOrm::MariadbLazy mariaQuery;
    mariaQuery[LazyOrm::SELECT] = "users";
    mariaQuery << "id";
    mariaQuery[LazyOrm::LIMIT] = {{11, ",", 23}};
    const auto maria = mariaQuery.buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(SELECT `id` FROM users LIMIT ? OFFSET ? ;)");
    REQUIRE(maria.params.size() == 2);
    REQUIRE(maria.params[0].toLongLong() == 23);
    REQUIRE(maria.params[1].toLongLong() == 11);
}

TEST_CASE("parameterized PostgreSQL INSERT IGNORE places RETURNING after ON CONFLICT", "[parameterized][insert_ignore]")
{
    LazyOrm::PostgreLazy query;
    query[LazyOrm::INSERT_IGNORE] = "users";
    query.setProperties({{"id", 7}, {"name", "ignored"}});
    query.setPrimaryKey("id");

    const auto parameterized = query.buildParameterized();
    REQUIRE(Catch::trim(parameterized.sql) ==
            R"(INSERT INTO users ("id","name") VALUES ($1,$2) ON CONFLICT DO NOTHING returning id;)");
    REQUIRE(parameterized.params.size() == 2);
}

TEST_CASE("PostgreSQL CONTAINS and COUNT quote identifiers with double quotes", "[postgres][contains][count]")
{
    LazyOrm::PostgreLazy containsQuery;
    containsQuery[LazyOrm::CONTAINS] = "users";
    containsQuery[LazyOrm::WHERE] = {{"id", 7}};
    REQUIRE(Catch::trim(containsQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS "contains" FROM users WHERE ("id" = '7') ;)");

    LazyOrm::PostgreLazy countQuery;
    countQuery[LazyOrm::COUNT] = "users";
    countQuery[LazyOrm::WHERE] = {{"id", 7}};
    REQUIRE(Catch::trim(countQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT COUNT(*) AS "count" FROM users WHERE ("id" = '7') ;)");
}

TEST_CASE("join separates select lists of joined tables with commas", "[join][mariadb]")
{
    LazyOrm::MariadbLazy student;
    student[LazyOrm::SELECT] = "student";
    student.setProperties({"name", "age", "hair"});

    LazyOrm::MariadbLazy grade;
    grade[LazyOrm::SELECT] = "grade";
    grade << "grade" << "passed";

    LazyOrm::JoinMariadb joinMariadb;
    joinMariadb[LazyOrm::Primary] = student;
    joinMariadb[LazyOrm::Joins] = {
        {LazyOrm::JoinRole::LeftJoin, grade, "student_id", "id"}
    };

    REQUIRE(Catch::trim(joinMariadb.query_with_trim_consecutive_spaces()) ==
            "SELECT \n"
            "`student`.`age`, \n"
            "`student`.`hair`, \n"
            "`student`.`name`, \n"
            "`grade`.`grade`, \n"
            "`grade`.`passed` \n"
            "FROM student \n"
            "LEFT JOIN grade ON `student`.`id` = `grade`.`student_id` \n"
            ";");
}
