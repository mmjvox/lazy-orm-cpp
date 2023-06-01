#include <iostream>
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string batch_insert1() {

  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::BATCH_INSERT]="student";
  lazyOrm.setProperties({
                          {{"name","anya"},{"family","forger"},{"age",6},{"hair","pink"},{"cute",true}},
                          {{"name","damian"},{"family","Desmond"},{"age",8},{"hair","black"},{"cute",false}},
                          {{"name","loid"},{"family","forger"},{"age",30},{"hair","blond"},{"cute",false}}
                        });
  return lazyOrm.queryString();
}

std::string batch_insert2() {

  std::list<std::map<std::string, LazyOrm::DbVariant>> batchProperties;
  batchProperties.push_back({{"name","anya"},{"family","forger"},{"age",6},{"hair","pink"},{"cute",true}});
  batchProperties.push_back({{"name","damian"},{"family","Desmond"},{"age",8},{"hair","black"},{"cute",false}});
  batchProperties.push_back({{"name","loid"},{"family","forger"},{"age",30},{"hair","blond"},{"cute",false}});

  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::BATCH_INSERT]="student";
  lazyOrm.setProperties(batchProperties);
  return lazyOrm.queryString();
}

TEST_CASE( "Factorials are computed", "[Lazy_BATCH_INSERT]" ) {

    REQUIRE( Catch::trim(batch_insert1()) == R"(INSERT INTO student (`age`,`cute`,`family`,`hair`,`name`) VALUES ('6',true,'forger','pink','anya'),('8',false,'Desmond','black','damian'),('30',false,'forger','blond','loid') ;)" );
    REQUIRE( Catch::trim(batch_insert2()) == R"(INSERT INTO student (`age`,`cute`,`family`,`hair`,`name`) VALUES ('6',true,'forger','pink','anya'),('8',false,'Desmond','black','damian'),('30',false,'forger','blond','loid') ;)" );
}
