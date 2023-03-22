#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string update1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";
  return lazyOrm.queryString();
}


TEST_CASE( "Factorials are computed", "[Lazy_UPDATE]" ) {

    std::cout << update1() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
