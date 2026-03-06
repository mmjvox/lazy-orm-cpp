#include <iostream>
#include "PostgreLazy.h"
#include "TransactionPostgre.h"


#include <catch2/catch_all.hpp>
#include <cstdint>

std::string transaction1() {

  LazyOrm::PostgreLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::PostgreLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::TransactionPostgre transaction;
  transaction.append(lazyOrm);
  transaction.append(lazyOrm2);

  return transaction.query_with_trim_consecutive_spaces();
}

std::string transaction2() {

  LazyOrm::PostgreLazy lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::PostgreLazy lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::TransactionPostgre transaction({lazyOrm,lazyOrm2});

  return transaction.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_Transaction]" ) {

  REQUIRE( Catch::trim(transaction1()) == R"(BEGIN TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT TRANSACTION;)" );
  REQUIRE( Catch::trim(transaction2()) == R"(BEGIN TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT TRANSACTION;)" );
}
