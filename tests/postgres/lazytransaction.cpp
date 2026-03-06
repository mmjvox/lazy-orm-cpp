#include "LazyOrm.h"
#include "LazyTransaction.h"
#include <iostream>


#include <catch2/catch_all.hpp>
#include <cstdint>

std::string transaction1() {

  LazyOrm::LazyOrm lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::LazyOrm lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::LazyTransaction transaction;
  transaction.append(lazyOrm);
  transaction.append(lazyOrm2);

  return transaction.query_with_trim_consecutive_spaces(LazyOrm::LazyTransaction::Postgres);
}

std::string transaction2() {

  LazyOrm::LazyOrm lazyOrm;
  lazyOrm[LazyOrm::UPDATE]="student";
  lazyOrm["name"]="anya";
  lazyOrm["age"]=6;
  lazyOrm["hair"]="pink";

  LazyOrm::LazyOrm lazyOrm2;
  lazyOrm2[LazyOrm::SELECT]="student";
  lazyOrm2<<"name"<<"age"<<"hair";

  LazyOrm::LazyTransaction transaction({lazyOrm,lazyOrm2});

  return transaction.query_with_trim_consecutive_spaces(LazyOrm::LazyTransaction::Postgres);
}

TEST_CASE( "Factorials are computed", "[Lazy_Transaction]" ) {

  REQUIRE( Catch::trim(transaction1()) == R"(BEGIN TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT TRANSACTION;)" );
  REQUIRE( Catch::trim(transaction2()) == R"(BEGIN TRANSACTION; UPDATE student SET "age"='6',"hair"='pink',"name"='anya' ; SELECT "age","hair","name" FROM student; COMMIT TRANSACTION;)" );
}
