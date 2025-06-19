#include <SqliteLazy.h>
#include <iostream>
//#include "MariadbLazy.h"
#include "WhereFilter.h"

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




//p1_2.printNestedVectors();


//  LazyOrm::WhereFilter p2 = {LazyOrm::AND, {"AGE","<=",30}};

//  LazyOrm::WhereFilter p3 = {{"AGE","<=",30}};

//  LazyOrm::WhereFilter p4 = {"a wwwww ccc"};

//  // Accessing the nested elements:
//  auto& nested1 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(p1.nested[2]));
//  auto& nested2 = std::get<std::vector<WherePair>>(std::get<std::vector<WherePair>>(nested1[1].nested[1]));
//  auto limit = std::get<int>(nested2[0].nested[1]);
//  std::cout << "Limit: " << limit << std::endl;

//  LazyOrm::MariadbFilteringLazy filters;
//  filters.setWhereFilter(p1);

//  return filters.filter_conditions();
}

std::string where1() {
LazyOrm::WhereFilter whereFilter = {
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

return whereFilter.toString();
}

std::string where2() {
LazyOrm::WhereFilter whereFilter = {
    {
        {
            {"name","like","asqar"},
            {"OR"},
            {"name","like","mamad"}
        },
        {"AND"},
        {
            {"name","like","asqar"},
            {"OR"},
            {"name","like","mamad"}
        },
        {"AND","sss"},
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
        }
    }
};

return whereFilter.toString();
}

std::string where3() {
LazyOrm::WhereFilter whereFilter = {{"AGE","<=",30}};
return whereFilter.toString();
}

std::string where4() {
LazyOrm::WhereFilter whereFilter = {"a wwwww ccc"};
return whereFilter.toString();
}

std::string where5() {
    std::vector<LazyOrm::DbVariant> a1 = {"name","like","asqar"};
    std::vector<LazyOrm::DbVariant> a2 = {"name","like","khar"};

    LazyOrm::WhereFilter likes;
    likes.append(a1);
    likes.append({"OR"});
    likes.append(a2);
    likes.append({"OR"});
    likes.append({"name","like","akbar"});

    return likes.toString();
}

std::string where6() {
    LazyOrm::WhereFilter whereFilter = {
        {
         {"field1","NULL"},
         {"OR"},
         {"field1",""}
        }
    };
    return whereFilter.toString();
}


std::string where7() {

    LazyOrm::SqliteLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair";


    lazyOrm[LazyOrm::WHERE] = {

            {"field1","NULL"},
            {"OR"},
            {"field1",""}

    };

    return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string where8() {
    LazyOrm::HavingFilter havingFilter = {
        {
            {"COUNT(*)",">","2"},
            {"OR"},
            {"hair","blue"}
        }
    };
    return havingFilter.toString();
}

std::string where9() {

    LazyOrm::SqliteLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair";


    lazyOrm[LazyOrm::Having::HAVING] = {

        {"COUNT(*)",">","2"},
        {"OR"},
        {"hair","blue"}

    };

    return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_WHERE]" ) {

    // std::cout << where2() << std::endl;

    REQUIRE( Catch::trim(where1()) == R"(WHERE (((`AGE` in '[4,5,6,7,8]') OR (`AGE` in '[1,2,3,4,5]')) AND ((`AGE` in '[40,50,60,70,80]') OR (`AGE` in '[10,20,30,40,50]'))) AND ((`name` like 'asqar') OR (`name` like 'mamad')) NOT ((`name` like 'asqar') OR (`name` like 'mamad')) AND NOT ((`name` like 'asqar') OR (`name` like 'mamad')) OR NOT ((`name` like 'asqar') OR (`name` like 'mamad')))" );
    REQUIRE( Catch::trim(where2()) == R"(WHERE (((`name` like 'asqar') OR (`name` like 'mamad')) AND ((`name` like 'asqar') OR (`name` like 'mamad')) AND SSS (((`AGE` in '[4,5,6,7,8]') OR (`AGE` in '[1,2,3,4,5]')) AND ((`AGE` in '[40,50,60,70,80]') OR (`AGE` in '[10,20,30,40,50]')))))" );
    // REQUIRE( Catch::trim(where3()) == R"(WHERE AGE <= 30)" );
    REQUIRE( Catch::trim(where4()) == R"(WHERE a wwwww ccc)" );
    // REQUIRE( Catch::trim(where5()) == R"(WHERE ((`name` like 'asqar') OR (`name` like 'khar') OR (`name` like 'akbar')))" );
    REQUIRE( Catch::trim(where6()) == R"(WHERE ((`field1` = NULL) OR (`field1` = '')))" );
    REQUIRE( Catch::trim(where7()) == R"(SELECT `age`,`hair`,`name` FROM student WHERE ((`field1` = NULL) OR (`field1` = '')) ;)" );
    REQUIRE( Catch::trim(where8()) == R"(HAVING ((`COUNT(*)` > '2') OR (`hair` = 'blue')))" );
    REQUIRE( Catch::trim(where9()) == R"(SELECT `age`,`hair`,`name` FROM student HAVING ((`COUNT(*)` > '2') OR (`hair` = 'blue')) ;)" );
}
