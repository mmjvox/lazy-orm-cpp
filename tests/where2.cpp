#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>


std::string aaaaaa() {

  LazyOrm::WhereFilter p1 = { LazyOrm::AND, {
       std::vector<LazyOrm::DbVariant>{3,4},
      {
        std::vector<LazyOrm::WhereFilter>{
          { LazyOrm::OR, {
              std::vector<LazyOrm::DbVariant>{5,"6"},
              {
                std::vector<LazyOrm::WhereFilter>{
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

  LazyOrm::WhereFilter p2 = {LazyOrm::AND, {"AGE","<=",30}};

  LazyOrm::WhereFilter p3 = {{"AGE","<=",30}};

  LazyOrm::WhereFilter p4 = {{"a wwwww ccc"}};

//  // Accessing the nested elements:
//  auto& nested1 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(p1.nested[2]));
//  auto& nested2 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(nested1[1].nested[1]));
//  auto limit = std::get<int>(nested2[0].nested[1]);
//  std::cout << "Limit: " << limit << std::endl;

  LazyOrm::MariadbFilteringLazy filters;
  filters.setWhereFilter(p1);

  return filters.test_string();
}

TEST_CASE( "Factorials are computed", "[Lazy_WHERE2]" ) {

    std::cout <<"where2 \n " << aaaaaa() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
