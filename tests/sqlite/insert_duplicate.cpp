#include <iostream>
#include "SqliteLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string insert1() {
  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT_UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]= 6;
  lazyOrm["hair"]="pink";
  lazyOrm["[update]hair"]="black";
  lazyOrm["cute"]=true;
  return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[ON DUPLICATE KEY UPDATE]" ) {

    REQUIRE( Catch::trim(insert1()) == R"()" );
}
