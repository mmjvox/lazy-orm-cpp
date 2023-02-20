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
  std::cout << lazyOrm.queryString() << std::endl;
  return lazyOrm.queryString();
}

TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {

    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
