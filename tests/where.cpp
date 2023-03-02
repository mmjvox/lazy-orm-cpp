#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>





std::string where1() {

    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter({"AGE",12});
    filters.setFilter(LazyOrm::LIMIT , {10,10});
    filters.setFilter(LazyOrm::OR, {"AGE","25"});
    filters.setFilter(LazyOrm::OR, {"AGE","<=",30});
    filters.setFilter(LazyOrm::AND,  {"SALARY",65000});
    filters.setFilter(LazyOrm::OR,  {"hair","pink"});
    filters.setFilter(LazyOrm::AND, {"height","between", "99,198"});
    filters.setFilter(LazyOrm::OR , {"grade","in", "[1,5,7,9]"});
    filters.setFilter(LazyOrm::LIMIT , {11,23});
    filters.setFilter(LazyOrm::LIMIT , {"10,10"});
    filters.setFilter(LazyOrm::ORDERBY , "aa");



//    LazyOrm::filterTypes fff=std::vector{
//        LazyOrm::filterPair{LazyOrm::AND, {"aaa",1}},
//        LazyOrm::filterPair{LazyOrm::OR, {"bbb",2.2}}
//    };

//    filters.setFilter(LazyOrm::AND, fff);

//    filters.test_init({"aaa",12,{"ac","54"}});

//    {
//        {"AND",{"AGE","25"}},

//        {"OR",{"SALARY","65000"}},

//        {"AND",{"hair","pink"}},

////        {"AND",
////                {
////                    {"OR",{"aaa","1"}},

////                {"AND",{"bbb","2"}},

////                {"OR",{"ccc","between",{"1","9"}}},

////                {"OR",{"ddd","in",{"1","2","3"}}}
////                }
////        }
//    };

return filters.testString();
}


TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {

    std::cout << where1() << std::endl;

//    REQUIRE( select1() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
//    REQUIRE( select2() == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
}
