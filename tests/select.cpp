#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string select1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm.setProperties({"name","age","hair","*"});
  return lazyOrm.queryString();
}
std::string select2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair"<<"*";
  return lazyOrm.queryString();
}

std::string select3() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  return lazyOrm.queryString();
}

std::string select4() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", "[1,5,7,9]"}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
//  lazyOrm[LazyOrm::HAVING] = {{"height",">","160"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "11,11";

//  lazyOrm.setFilter({LazyOrm::HAVING, {{"height",">","160"}}});
  //
  return lazyOrm.queryString();
}

TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {

    std::cout << select4() << std::endl;

    REQUIRE( Catch::trim(select1()) == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select2()) == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select3()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
//    REQUIRE( Catch::trim(select4()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
}
