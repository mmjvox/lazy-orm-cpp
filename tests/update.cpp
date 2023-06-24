#include <iostream>
#include "MariadbLazy.h"
#include "DbList.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string update1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "11,11";

  return lazyOrm.queryString();
}

TEST_CASE( "Factorials are computed", "[Lazy_UPDATE]" ) {
    REQUIRE( Catch::trim(update1()) == R"(UPDATE student SET `age`='6',`hair`='pink',`name`='anya'  WHERE (`grade` in (1,5,7,9)) GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,11 ;)" );
}
