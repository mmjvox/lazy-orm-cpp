#include <iostream>
#include "MariadbLazy.h"
#include "DbList.h"
#include "ResultRow.h"
#include "Result.h"
#include <catch2/catch_all.hpp>
#include <cstdint>

std::string result1() {

   LazyOrm::Result result;
   for(int i=10; i>0; i--){
        LazyOrm::ResultRow row;
        row.insert("name","kimia");
        row.insert("age",22);
        row.insert("job","developer");
        row.insert("clever",true);
        row.insert("kind",true);
        row.insert("hair","blue");
        result.push_back(row);
   }

   return result.toIndentedString();
}

std::string result2() {

    LazyOrm::ResultRow row;
    row.insert("name","anya");

    return row["name"].toString();
}

TEST_CASE( "Factorials are computed", "[Lazy_Result]" ) {
    std::cout << result2() << std::endl;
    REQUIRE( Catch::trim(result2()) == R"(anya)" );
}
