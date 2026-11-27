#include <iostream>
#include "DbList.h"
#include "MariadbLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

namespace {

LazyOrm::MariadbLazy fruitsQuery()
{
  LazyOrm::MariadbLazy lazyOrm;
  lazyOrm[LazyOrm::PrimaryKey] = "id";
  lazyOrm[LazyOrm::BULK_UPDATE]="fruits";
  //
  lazyOrm.setProperties({
      {{"id","1"},{"name","anya"},{"family","forger"},{"age",6},{"hair","pink"},{"cute",true}},
      {{"id","2"},{"name","damian"},{"family","Desmond"},{"age",8},{"hair","black"},{"cute",false}},
      {{"id","3"},{"name","loid"},{"family","forger"},{"age",30},{"hair","blond"},{"cute",false}}
  });
  return lazyOrm;
}

}

TEST_CASE( "Bulk UPDATE stages rows and updates through the staging table", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();
    lazyOrm[LazyOrm::WHERE] = {{"userid", "20"}};

    // The batch is inserted into the temporary staging table (never into
    // `fruits`), the UPDATE joins against that staging table on the primary
    // key, the primary key is not part of the SET assignments, all updated
    // columns share one SET clause, and the WHERE filter qualifies target rows.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_fruits` LIKE `fruits`; )"
                                           "\n" R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE `fruits` t JOIN `lazyormtemp_fruits` u ON t.`id` = u.`id` SET t.`age` = u.`age`,t.`cute` = u.`cute`,t.`family` = u.`family`,t.`hair` = u.`hair`,t.`name` = u.`name` WHERE (t.`userid` = '20') ; )"
                                           "\n" R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`;)"
            );
}

TEST_CASE( "Bulk UPDATE without WHERE updates every staged row", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();

    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_fruits` LIKE `fruits`; )"
                                           "\n" R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE `fruits` t JOIN `lazyormtemp_fruits` u ON t.`id` = u.`id` SET t.`age` = u.`age`,t.`cute` = u.`cute`,t.`family` = u.`family`,t.`hair` = u.`hair`,t.`name` = u.`name` ; )"
                                           "\n" R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`;)"
            );
}

TEST_CASE( "Bulk UPDATE filters target rows without restricting staging inserts", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();
    lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7})}};

    const auto query = lazyOrm.query_with_trim_consecutive_spaces();

    // The WHERE clause appears only once, on the UPDATE statement.
    REQUIRE( query.find("WHERE (t.`grade` in (1,5,7))") != std::string::npos );
    // The staging INSERT stays unfiltered.
    REQUIRE( query.find(R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )") != std::string::npos );
}

TEST_CASE( "Bulk UPDATE associates values with their columns and escapes data", "[bulk_update]" ) {

    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "items";
    lazyOrm.setProperties({
        {{"id",1},{"title","O'Brien"},{"price",9.5},{"stock",std::monostate{}},{"flag",false}},
        {{"id",2},{"title","plain"},{"price",3},{"stock",7},{"flag",true}}
    });

    // std::monostate renders as NULL, booleans, numbers and escaped quotes
    // are rendered per row, and each value keeps the column of its own row.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_items`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_items` LIKE `items`; )"
                                           "\n" R"(INSERT INTO `lazyormtemp_items` (`flag`,`id`,`price`,`stock`,`title`) VALUES (false,'1','9.5',NULL,'O\'Brien'),(true,'2','3','7','plain') ; )"
                                           "\n" R"(UPDATE `items` t JOIN `lazyormtemp_items` u ON t.`id` = u.`id` SET t.`flag` = u.`flag`,t.`price` = u.`price`,t.`stock` = u.`stock`,t.`title` = u.`title` ; )"
                                           "\n" R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_items`;)"
            );
}

TEST_CASE( "Bulk UPDATE rejects invalid input with an empty query", "[bulk_update][validation]" ) {

    // Empty batch.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty table name.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Missing primary-key configuration.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Primary key missing from a row.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Inconsistent column sets.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",2},{"age",8}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Duplicate primary-key values are ambiguous.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",1},{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Only the primary key: nothing to update.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1}},{{"id",2}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty row inside the batch.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Invalid input also fails the parameterized path.
    {
      LazyOrm::MariadbLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE binds every value as a parameter", "[bulk_update][parameterized]" ) {

    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({
        {{"id", 1}, {"name", "Alice"}, {"age", 25}},
        {{"id", 2}, {"name", "Bob"}, {"age", 30}}
    });
    lazyOrm[LazyOrm::WHERE] = {{"active", true}};

    const auto query = lazyOrm.buildParameterized();

    // Batch values are bound row-major; the WHERE value is bound last. The
    // staging INSERT targets the temporary table, and the primary key is
    // only used for the join, not for a SET assignment.
    REQUIRE( Catch::trim(query.sql) == R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_users` ; CREATE TEMPORARY TABLE `lazyormtemp_users` LIKE `users` ; INSERT INTO `lazyormtemp_users` (`age`,`id`,`name`) VALUES (?,?,?),(?,?,?) ; UPDATE `users` t JOIN `lazyormtemp_users` u ON t.`id` = u.`id` SET t.`age` = u.`age`,t.`name` = u.`name` WHERE (t.`active` = ?) ; DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_users` ;)" );

    REQUIRE( query.params.size() == 7 );
    REQUIRE( query.params[0].typeName() == "int" );
    REQUIRE( query.params[0].toLongLong() == 25 );
    REQUIRE( query.params[1].typeName() == "int" );
    REQUIRE( query.params[1].toLongLong() == 1 );
    REQUIRE( query.params[2].typeName() == "string" );
    REQUIRE( query.params[2].toString() == "Alice" );
    REQUIRE( query.params[3].typeName() == "int" );
    REQUIRE( query.params[3].toLongLong() == 30 );
    REQUIRE( query.params[4].typeName() == "int" );
    REQUIRE( query.params[4].toLongLong() == 2 );
    REQUIRE( query.params[5].typeName() == "string" );
    REQUIRE( query.params[5].toString() == "Bob" );
    REQUIRE( query.params[6].typeName() == "boolean" );
    REQUIRE( query.params[6].toBool() );
}

TEST_CASE( "Bulk UPDATE handles schema-qualified and malformed identifiers", "[bulk_update][identifiers]" ) {

    {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "mydb.users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        const std::string expectedDrop = R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_mydb_users`;)";
        REQUIRE( sql.starts_with(expectedDrop) );
        REQUIRE( sql.ends_with(expectedDrop) );
        REQUIRE( sql.find(R"(CREATE TEMPORARY TABLE `lazyormtemp_mydb_users` LIKE `mydb`.`users`;)") != std::string::npos );
        REQUIRE( sql.find(R"(UPDATE `mydb`.`users` t JOIN `lazyormtemp_mydb_users` u ON t.`id` = u.`id` SET t.`name` = u.`name` ;)") != std::string::npos );
    }

    for (const std::string &badTable : {std::string(".users"), std::string("users."), std::string("a..b"), std::string("users; DROP TABLE users;--")}) {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = badTable;
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
    }
}

TEST_CASE( "Bulk UPDATE validates primary-key types, NULLs and duplicates", "[bulk_update][validation]" ) {

    // Mixed primary-key type categories (e.g. int 1 vs string "1", int 1 vs
    // double 1.0, bool vs int) are rejected conservatively to prevent
    // database column coercion from turning them into duplicate keys.
    {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({
            {{"id", 1}, {"name", "int_one"}},
            {{"id", std::string("1")}, {"name", "str_one"}}
        });
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({
            {{"id", 1}, {"name", "int_one"}},
            {{"id", 1.0}, {"name", "float_one"}}
        });
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({
            {{"id", 1}, {"name", "first"}},
            {{"id", 1U}, {"name", "second"}}
        });
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", std::monostate{}}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE places WHERE cleanly and preserves nested AND/OR/NOT filters", "[bulk_update][where]" ) {

    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
    lazyOrm[LazyOrm::WHERE] = {
        {
            {"status", "active"},
            {"AND"},
            {
                {"age", ">", 18},
                {"OR"},
                {"role", "admin"}
            }
        },
        {"AND NOT"},
        {
            {"deleted", true},
            {"OR"},
            {
                {"archived", true},
                {"AND"},
                {"hidden", true}
            }
        }
    };

    const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
    REQUIRE( sql.find(R"(SET t.`name` = u.`name` WHERE (((t.`status` = 'active') AND ((t.`age` > '18') OR (t.`role` = 'admin'))) AND NOT ((t.`deleted` = true) OR ((t.`archived` = true) AND (t.`hidden` = true)))) ;)") != std::string::npos );

    const auto param = lazyOrm.buildParameterized();
    REQUIRE( param.sql.find(R"(SET t.`name` = u.`name` WHERE (((t.`status` = ?) AND ((t.`age` > ?) OR (t.`role` = ?))) AND NOT ((t.`deleted` = ?) OR ((t.`archived` = ?) AND (t.`hidden` = ?)))) ;)") != std::string::npos );
    REQUIRE( param.params.size() == 8 );
}

TEST_CASE( "Bulk UPDATE rejects invalid batch columns, raw markers, and keeps NULL vs string NULL consistent", "[bulk_update][validation]" ) {

    // Invalid batch column identifiers and raw markers are rejected.
    for (const std::string &badCol : {std::string(""), std::string("bad col"), std::string("a.b"), std::string("[no']name"), std::string("[no`]name"), std::string("[func]name"), std::string("name); DROP TABLE users;--")}) {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {badCol, "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    // Raw-expression markers in batch values are rejected.
    for (const std::string &rawVal : {std::string("[no']NOW()"), std::string("[no`]1")}) {
        LazyOrm::MariadbLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {"name", rawVal}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    // Identical input with std::monostate (SQL NULL) and "NULL" (ordinary string).
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({
        {{"id", 1}, {"label", "NULL"}, {"note", std::monostate{}}}
    });
    const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
    REQUIRE( sql.find(R"(VALUES ('1','NULL',NULL) ;)") != std::string::npos );

    const auto param = lazyOrm.buildParameterized();
    REQUIRE( param.sql.find(R"(VALUES (?,?,?) ;)") != std::string::npos );
    REQUIRE( param.params.size() == 3 );
    REQUIRE( param.params[1].typeName() == "string" );
    REQUIRE( param.params[1].toString() == "NULL" );
    REQUIRE( param.params[2].typeName().empty() );
}
