#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>


std::string aaaaaa() {

  LazyOrm::WherePair p1 = { LazyOrm::AND, {
       3 ,
       4 ,
      {
        std::vector<LazyOrm::WherePair>{
          { LazyOrm::OR, {
              { 5 },
              { 6 },
              {
                std::vector<LazyOrm::WherePair>{
                  { LazyOrm::AND, {
                      { "foo" },
                      { "bar" }
                    }
                  },
                  { LazyOrm::AND, { "name","like","an%" } }
                }
              }
            }
          },
          { LazyOrm::OR, { "AGE","in","[4,5,6,7,8]" } }
        }
      }
    }
  };

  LazyOrm::WherePair p2 = {LazyOrm::AND, {"AGE","<=",30}};

//  // Accessing the nested elements:
//  auto& nested1 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(p1.nested[2]));
//  auto& nested2 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(nested1[1].nested[1]));
//  auto limit = std::get<int>(nested2[0].nested[1]);
//  std::cout << "Limit: " << limit << std::endl;

  return {};
}

TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {

    std::cout <<"where2 " << aaaaaa() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
