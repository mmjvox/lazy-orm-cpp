#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string delete1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::DELETE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";
  return lazyOrm.queryString();
}


TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {

    std::cout << delete1() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
