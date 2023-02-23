#include <iostream>
//#include "MariadbLazy.h"
#include "MariadbFilteringLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>





std::string where1() {

    LazyOrm::MariadbFilteringLazy filters;
    filters.setFilter(LazyOrm::AND, {"AGE","25"});
    filters.setFilter(LazyOrm::OR, {"SALARY","65000"});
    filters.setFilter(LazyOrm::AND, {"AGE","<=","30"});
    filters.setFilter(LazyOrm::AND, {"hair","pink"});

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
