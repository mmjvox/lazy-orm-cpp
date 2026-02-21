#include <iostream>
#include "SqliteLazy.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string update1() {
  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = {{"5",",","6"}};

  return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_UPDATE]" ) {
    REQUIRE( Catch::trim(update1()) == R"(UPDATE student SET "age"='6',"hair"='pink',"name"='anya' WHERE ("grade" in (1,5,7,9)) GROUP BY "group1","group2" ORDER BY "num1","num2" LIMIT 5 , 6 ;)" );
}
