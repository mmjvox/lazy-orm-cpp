#include <iostream>
#include "LazyOrm.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string select1()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair";

    return lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB);
}

std::string select2()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Postgres);
}

std::string select3()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Sqlite3);
}


std::string select4()
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::INSERT]="student";
    lazyOrm["name"] = "jack";
    lazyOrm["age"] = "56";
    lazyOrm["hair"] = "black";
    lazyOrm[LazyOrm::PrimaryKey] = "idx";

    return lazyOrm.queryString(LazyOrm::LazyOrm::Postgres);
}

TEST_CASE( "Factorials are computed", "[Lazy_class_LazyOrm]" ) {
    // std::cout << select4() << std::endl;
    REQUIRE( Catch::trim(select1()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select2()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select3()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select4()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
}
