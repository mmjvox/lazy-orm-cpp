#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>


std::string aaaaaa() {

//    LazyOrm::WhereFilter p0 = { LazyOrm::AND, {
//                          std::vector<LazyOrm::DbVariant>{3,4},
//                          {
//                              std::vector<LazyOrm::WhereFilter>{
//                                  { LazyOrm::OR, {
//                                                       std::vector<LazyOrm::DbVariant>{5,"6"},
//                                                       {
//                                                           std::vector<LazyOrm::WhereFilter>{
//                                                               { LazyOrm::AND, std::vector<LazyOrm::DbVariant>{
//                                                                                     { "foo" },{ "bar" }
//                                                                                 }
//                                                               },
//                                                               { LazyOrm::AND, std::vector<LazyOrm::DbVariant>{ "name","like","an%" } }
//                                                           }
//                                                       }
//                                                   }
//                                  },
//                                  { LazyOrm::OR, std::vector<LazyOrm::DbVariant>{ "AGE","in","[4,5,6,7,8]" } }
//                              }
//                          }
//                      }
//    };

//  LazyOrm::WhereFilter p1 = {
//   LazyOrm::AND,
//    {
//      {3,4},

//        {
//          {
//             LazyOrm::OR, {
//              {5,"6"},
//              {
//                  { LazyOrm::AND, {
//                      { "foo" },
//                      { "bar" }
//                    }
//                  },
//                  { LazyOrm::AND, { "name","like","an%" } }
//              }
//            }
//          },
//          { LazyOrm::OR, { "AGE","in","[4,5,6,7,8]" } }
//        }

//    }
//  };

LazyOrm::WhereFilter p1_1 = {
    {
        {
         { "AGE","in","[4,5,6,7,8]" },
         {"or"},
         { "AGE","in","[1,2,3,4,5]" },
         },
        {"and"},
        {
         { "AGE","in","[40,50,60,70,80]" },
         {"or"},
         { "AGE","in","[10,20,30,40,50]" },
         }
    },
    {"AND"},
    {
        {"name","like","asqar"},
        {"OR"},
        {"name","like","mamad"}
    },
    {"NOT"},
    {
        {"name","like","asqar"},
        {"OR"},
        {"name","like","mamad"}
    },
    {"AND NOT"},
    {
        {"name","like","asqar"},
        {"OR"},
        {"name","like","mamad"}
    },
    {"OR","NOT"},
    {
        {"name","like","asqar"},
        {"OR"},
        {"name","like","mamad"}
    }
};

return p1_1.toString();


//LazyOrm::WhereFilter p1_2 = {
//    {
//        {
//            {"name","like","asqar"},
//            {"OR"},
//            {"name","like","mamad"}
//        },
//        {"AND"},
//        {
//            {"name","like","asqar"},
//            {"OR"},
//            {"name","like","mamad"}
//        },
//        {"AND","sss"},
//        {
//            {
//             { "AGE","in","[4,5,6,7,8]" },
//             {"or"},
//             { "AGE","in","[1,2,3,4,5]" },
//             },
//            {"and"},
//            {
//             { "AGE","in","[40,50,60,70,80]" },
//             {"or"},
//             { "AGE","in","[10,20,30,40,50]" },
//             }
//        }
//    }
//};

//p1_2.printNestedVectors();


//  LazyOrm::WhereFilter p2 = {LazyOrm::AND, {"AGE","<=",30}};

//  LazyOrm::WhereFilter p3 = {{"AGE","<=",30}};

//  LazyOrm::WhereFilter p4 = {{"a wwwww ccc"}};

//  // Accessing the nested elements:
//  auto& nested1 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(p1.nested[2]));
//  auto& nested2 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(nested1[1].nested[1]));
//  auto limit = std::get<int>(nested2[0].nested[1]);
//  std::cout << "Limit: " << limit << std::endl;

//  LazyOrm::MariadbFilteringLazy filters;
//  filters.setWhereFilter(p1);

//  return filters.where_conditions();
}

TEST_CASE( "Factorials are computed", "[Lazy_WHERE2]" ) {

    std::cout <<"where2 " << aaaaaa() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
