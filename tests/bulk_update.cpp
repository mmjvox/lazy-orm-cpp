#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string bulk_update1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::BULK_UPDATE]="fruits";
  lazyOrm["name.apple"]=3;
  lazyOrm["name.banana"]=2;
  lazyOrm["name.orange"]=5;
  lazyOrm["name.lemon"]=8;
  //
  lazyOrm.setProperties({
      {{"name","anya"},{"family","forger"},{"age",6},{"hair","pink"},{"cute",true}},
      {{"name","damian"},{"family","Desmond"},{"age",8},{"hair","black"},{"cute",false}},
      {{"name","loid"},{"family","forger"},{"age",30},{"hair","blond"},{"cute",false}}
  });
  //
  lazyOrm[LazyOrm::WHERE] = {{"userid", "20"}};

  return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_UPDATE]" ) {

    std::cout << bulk_update1() << std::endl;

//    REQUIRE( Catch::trim(bulk_update1()) == R"(UPDATE student SET `age`='6',`hair`='pink',`name`='anya'  WHERE `grade` in '[1,5,7,9]' GROUP BY group1,group2 ORDER BY num1,num2 LIMIT 11,11 ;)" );
}
