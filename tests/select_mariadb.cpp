#include <iostream>
#include "DbList.h"
#include "MariadbLazy.h"
#include "SqliteLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

std::string select1() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm.setProperties({"name","age","hair","*"});
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select2() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair"<<"*";
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select3() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select4() {
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
//  lazyOrm[LazyOrm::HAVING] = {{"height",">","160"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = "11,11";

//  lazyOrm.setFilter({LazyOrm::HAVING, {{"height",">","160"}}});
  //
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select5() {
  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = {{"5",",","6"}};
  //
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select6() {
  LazyOrm::SqliteLazy lazyOrm;
  lazyOrm[LazyOrm::SELECT]="student";
  lazyOrm<<"name"<<"age"<<"hair";
  //
  lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7,9})}};
  lazyOrm[LazyOrm::GROUPBY] = {{"group1","group2"}};
  lazyOrm[LazyOrm::ORDERBY] = {{"num1","num2"}};
  lazyOrm[LazyOrm::LIMIT] = {{"5","6"}};
  //
  return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select7() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair"<<"*";
    lazyOrm[LazyOrm::COUNT_PROPS] = {{"name","age as age2","hair as 'hair2'","*"}};
    return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select8() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm<<"name"<<"age"<<"hair"<<"count (*)"<<"count (name)"<<"count (age as age2)"<<"count (hair as 'hair2')";
    return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select9() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT_DISTINCT]="student";
    lazyOrm.setProperties({"name","age","hair","*"});
    return lazyOrm.query_with_trim_consecutive_spaces();
}

std::string select10() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::SELECT]="student";
    lazyOrm.setProperties({"name","age","hair","*"});
    lazyOrm.enableDistinctSelect(true);
    return lazyOrm.query_with_trim_consecutive_spaces();
}

TEST_CASE( "Factorials are computed", "[Lazy_SELECT]" ) {
    REQUIRE( Catch::trim(select1()) == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select2()) == R"(SELECT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select3()) == R"(SELECT `age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select4()) == R"(SELECT `age`,`hair`,`name` FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY `group1`,`group2` ORDER BY `num1`,`num2` LIMIT 11,11 ;)" );

    REQUIRE( Catch::trim(select7()) == R"(SELECT *,`age`,`hair`,`name`,COUNT(`name`),COUNT(`age` as 'age2'),COUNT(`hair` as 'hair2'),COUNT(*) FROM student;)" );
    REQUIRE( Catch::trim(select8()) == R"(SELECT `age`,`hair`,`name`,COUNT(*),COUNT(`name`),COUNT(`age` as 'age2'),COUNT(`hair` as 'hair2') FROM student;)" );

    REQUIRE( Catch::trim(select9()) == R"(SELECT DISTINCT *,`age`,`hair`,`name` FROM student;)" );
    REQUIRE( Catch::trim(select10()) == R"(SELECT DISTINCT *,`age`,`hair`,`name` FROM student;)" );
}
