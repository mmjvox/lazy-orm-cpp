#include <iostream>
#include "MariadbLazy.h"
#include "Transaction.h"


#include <catch2/catch_all.hpp>
#include <cstdint>

std::string transaction1() {

  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::MariadbLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction;
  transaction.append(lazyOrm);
  transaction.append(lazyOrm2);

  return transaction.queryString();
}

std::string transaction2() {

  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::MariadbLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({lazyOrm,lazyOrm2});

  return transaction.queryString();
}

TEST_CASE( "Factorials are computed", "[Lazy_Transaction]" ) {

  REQUIRE( Catch::trim(transaction1()) == R"(START TRANSACTION; UPDATE student SET `age`='6',`hair`='pink',`name`='anya' ; SELECT `age`,`hair`,`name` FROM student; COMMIT;)" );
  REQUIRE( Catch::trim(transaction2()) == R"(START TRANSACTION; UPDATE student SET `age`='6',`hair`='pink',`name`='anya' ; SELECT `age`,`hair`,`name` FROM student; COMMIT;)" );
}
