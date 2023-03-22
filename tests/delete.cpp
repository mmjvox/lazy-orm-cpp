#include <iostream>
#include "MariadbFilteringLazy.h"
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string delete1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::DELETE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::MariadbFilteringLazy filters;
  filters[LazyOrm::WHERE] = {{"grade","in", "[1,5,7,9]"}};
  filters[LazyOrm::GROUPBY] = {{"group1","group2"}};
  filters[LazyOrm::ORDERBY] = {{"num1","num2"}};
  filters[LazyOrm::LIMIT] = "10,10";

  lazyOrm.setFilter(filters);

  return lazyOrm.queryString();
}

std::string delete2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::DELETE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  //
//  lazyOrm[LazyOrm::WHERE] = {{"grade","in", "[1,5,7,9]"}};
//  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "10,10";

  return lazyOrm.queryString();
}


TEST_CASE( "Factorials are computed", "[Lazy_DELETE]" ) {

    std::cout << delete2() << std::endl;

    REQUIRE( Catch::trim(delete1()) == R"(DELETE FROM studentWHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 10,10  ;)" );
//    REQUIRE( Catch::trim(delete2()) == R"(DELETE FROM studentWHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 10,10  ;)" );
}
