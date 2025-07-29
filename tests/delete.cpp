#include <iostream>
#include "DbList.h"
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
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  //
  LazyOrm::MariadbFilteringLazy filters;
  filters[LazyOrm::GROUPBY] = {{"group1","group2"}};
  filters[LazyOrm::ORDERBY] = {{"num1","num2"}};
  filters[LazyOrm::LIMIT] = "11,11";

  lazyOrm.setFilter(filters);

  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string delete2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::DELETE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "12,12";

  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string delete3() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::DELETE]="student";
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "13,13";

  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string delete4() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::DELETE]="student";
    lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({"1","5","7","9"})}};
    lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
    lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
    lazyOrm[LazyOrm::LIMIT] = "13,13";

    return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_DELETE]" ) {

//    std::cout << delete2() << std::endl;

    REQUIRE( Catch::trim(delete1()) == R"(DELETE FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY 'group1','group2' ORDER BY 'num1','num2' LIMIT 11,11 ;)" );
    REQUIRE( Catch::trim(delete2()) == R"(DELETE FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY 'group1','group2' ORDER BY 'num1','num2' LIMIT 12,12 ;)" );
    REQUIRE( Catch::trim(delete3()) == R"(DELETE FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY 'group1','group2' ORDER BY 'num1','num2' LIMIT 13,13 ;)" );
    REQUIRE( Catch::trim(delete4()) == R"(DELETE FROM student WHERE (`grade` in ('1','5','7','9')) GROUP BY 'group1','group2' ORDER BY 'num1','num2' LIMIT 13,13 ;)" );
}
