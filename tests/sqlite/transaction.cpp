#include <iostream>
#include "SqliteLazy.h"
#include "Transaction.h"


#include <catch2/catch_all.hpp>
#include <cstdint>

std::string transaction1() {

  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::SqliteLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::Transaction<LazyOrm::SqliteLazy> transaction;
  transaction.append(lazyOrm);
  transaction.append(lazyOrm2);

  return transaction.query_with_trim_consecutive_spaces();
}

std::string transaction2() {

  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::SqliteLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::Transaction<LazyOrm::SqliteLazy> transaction({lazyOrm,lazyOrm2});

  return transaction.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_Transaction]" ) {

  REQUIRE( Catch::trim(transaction1()) == R"(START TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT;)" );
  REQUIRE( Catch::trim(transaction2()) == R"(START TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT;)" );
}
