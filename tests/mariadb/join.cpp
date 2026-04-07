#include <iostream>
#include "DbList.h"
#include "JoinMariadb.h"
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string join1() {

  LazyOrm::MariadbLazy student;
  student[LazyOrm::SELECT]="student";
  student.setProperties({"name","age","hair"});

  LazyOrm::MariadbLazy grade;
  grade[LazyOrm::SELECT]="grade";
  grade << "grade" << "passed";


  LazyOrm::JoinMariadb joinMariadb;
  joinMariadb[LazyOrm::Primary] = student;
  joinMariadb[LazyOrm::Joins] = {
    {LazyOrm::JoinRole::LeftJoin, grade, "student_id", "id"}
  };

  return joinMariadb.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_JOIN]" ) {
    REQUIRE( Catch::trim(join1()) == R"()" );
}
