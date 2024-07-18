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
        row["name"]="kimia";
        row["age"]=22;
        row["job"]="developer";
        row["clever"]=true;
        row["kind"]=true;
        row["hair"]="blue";
        result.push_back(row);
   }

   return result.toIndentedString();
}

TEST_CASE( "Factorials are computed", "[Lazy_Result]" ) {
    std::cout << result1() << std::endl;
    REQUIRE( Catch::trim(result1()) == R"(UPDATE student SET `age`='6',`hair`='pink',`name`='anya'  WHERE (`grade` in (1,5,7,9)) GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,11 ;)" );
}
