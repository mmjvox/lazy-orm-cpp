#include <iostream>
#include "MariadbLazy.h"

#include <catch_amalgamated.hpp>
#include <catch_amalgamated.cpp>
#include <cstdint>

std::string insert1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]= 6;
  lazyOrm["hair"]="pink";
  lazyOrm["cute"]=true;
  return lazyOrm.queryString();
}
std::string insert2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm.setProperties({{"name","ahmad"},{"age",27},{"cute",false}});
  return lazyOrm.queryString();
}

std::string insert3() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::INSERT]="student";
  lazyOrm<<LazyOrm::pair{"name","ahmad"};
  lazyOrm<<LazyOrm::pair{"age",23}<<LazyOrm::pair{"hair","black"};
  lazyOrm<<LazyOrm::pair{"cute",false};
  return lazyOrm.queryString();
}

std::string insert4() {
  LazyOrm::MariadbLazy lazyOrm("student", LazyOrm::INSERT);
  lazyOrm["first_name"]="ali";
  lazyOrm["last_name"]= "yavari";
  lazyOrm["gender"]="male";
  lazyOrm["name_father"]="reza";
  return lazyOrm.queryString();
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {

    std::cout << "Hello Lazy!" << std::endl;

    REQUIRE( insert1() == R"(INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ("6","true","pink","anya") ;)" );
    REQUIRE( insert2() == R"(INSERT INTO student (`age`,`cute`,`name`) VALUES ("27","false","ahmad") ;)" );
    REQUIRE( insert3() == R"(INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ("23","false","black","ahmad") ;)" );
    REQUIRE( insert4() == R"(INSERT INTO student (`first_name`,`gender`,`last_name`,`name_father`) VALUES ("ali","male","yavari","reza") ;)" );
}


//int main()
//{
//





//  LazyOrm::MariadbLazy lazyOrm2;
//  lazyOrm2[LazyOrm::SELECT]="student";
//  lazyOrm2.setProperties({"name","age","hair","*"});
//  std::cout << lazyOrm2.queryString() << std::endl;

//  LazyOrm::MariadbLazy lazyOrm3;
//  lazyOrm3[LazyOrm::SELECT]="student";
//  lazyOrm3<<"name"<<"age"<<"hair"<<"*";
//  std::cout << lazyOrm3.queryString() << std::endl;





//  return 0;
//}
