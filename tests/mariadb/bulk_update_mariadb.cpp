#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string bulk_update1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::BULK_UPDATE]="fruits";
  //
  lazyOrm.setProperties({
      {{"id","1"},{"name","anya"},{"family","forger"},{"age",6},{"hair","pink"},{"cute",true}},
      {{"id","2"},{"name","damian"},{"family","Desmond"},{"age",8},{"hair","black"},{"cute",false}},
      {{"id","3"},{"name","loid"},{"family","forger"},{"age",30},{"hair","blond"},{"cute",false}}
  });
  //
  lazyOrm[LazyOrm::WHERE] = {{"userid", "20"}};

  return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_UPDATE]" ) {

    // std::cout << bulk_update1() << std::endl;

   REQUIRE( Catch::trim(bulk_update1()) == R"(DROP TEMPORARY TABLE IF EXISTS lazyormtempfruits;
CREATE TEMPORARY TABLE lazyormtempfruits LIKE fruits;
INSERT INTO fruits (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ;
UPDATE fruits t
JOIN updates u ON t.id = u.id
SET t.age = u.age;
SET t.cute = u.cute;
SET t.family = u.family;
SET t.hair = u.hair;
SET t.id = u.id;
SET t.name = u.name;
SET t.age = u.age;
SET t.cute = u.cute;
SET t.family = u.family;
SET t.hair = u.hair;
SET t.id = u.id;
SET t.name = u.name;
SET t.age = u.age;
SET t.cute = u.cute;
SET t.family = u.family;
SET t.hair = u.hair;
SET t.id = u.id;
SET t.name = u.name;
DROP TEMPORARY TABLE IF EXISTS lazyormtempfruits;)" );
}
