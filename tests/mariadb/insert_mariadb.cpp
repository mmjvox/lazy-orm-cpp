#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string insert1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]= 6;
  lazyOrm["hair"]="pink";
  lazyOrm["cute"]=true;
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string insert2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm.setProperties({{"name","ahmad"},{"age",27},{"cute",false}});
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string insert3() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm<<LazyOrm::pair{"name","ahmad"};
  lazyOrm<<LazyOrm::pair{"age",23}<<LazyOrm::pair{"hair","black"};
  lazyOrm<<LazyOrm::pair{"cute",false};
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string insert4() {
  LazyOrm::MariadbLazy lazyOrm("student", LazyOrm::INSERT);
  lazyOrm["first_name"]="ali";
  lazyOrm["last_name"]= "yavari";
  lazyOrm["gender"]="male";
  lazyOrm["name_father"]="reza";
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string insert5() {
    LazyOrm::MariadbLazy * lazyOrm = new LazyOrm::MariadbLazy();
    lazyOrm->setQueryType(LazyOrm::INSERT);
    lazyOrm->setTabeName("student");
    lazyOrm->setProperty("name", "anya");
    lazyOrm->setProperty("age", "6");
    lazyOrm->setProperty("hair", "pink");
    lazyOrm->setProperty("cute", true);
    return lazyOrm->query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_INSERT]" ) {

    REQUIRE( Catch::trim(insert1()) == R"(INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ('6',true,'pink','anya') ;)" );
    REQUIRE( Catch::trim(insert2()) == R"(INSERT INTO student (`age`,`cute`,`name`) VALUES ('27',false,'ahmad') ;)" );
    REQUIRE( Catch::trim(insert3()) == R"(INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ('23',false,'black','ahmad') ;)" );
    REQUIRE( Catch::trim(insert4()) == R"(INSERT INTO student (`first_name`,`gender`,`last_name`,`name_father`) VALUES ('ali','male','yavari','reza') ;)" );
    REQUIRE( Catch::trim(insert5()) == R"(INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ('6',true,'pink','anya') ;)" );
}
