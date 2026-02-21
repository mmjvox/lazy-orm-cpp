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

TEST_CASE( "Factorials are computed", "[Lazy_raw_query]" ) {
    REQUIRE( Catch::trim(query1()) == R"(INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack')  returning idx;)" );
}
