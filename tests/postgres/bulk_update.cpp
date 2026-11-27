#include <iostream>
#include "DbList.h"
#include "PostgreLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

namespace {

LazyOrm::PostgreLazy fruitsQuery()
{
  LazyOrm::PostgreLazy lazyOrm;
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

    // PostgreSQL-native UPDATE ... SET ... FROM ... WHERE: the batch is
    // inserted into the temporary staging table (never into `fruits`), the
    // UPDATE matches target rows through the staging table on the primary
    // key, the primary key is not part of the SET assignments, all updated
    // columns share one SET clause, and the WHERE filter qualifies target rows.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_fruits"; )"
                                           "\n" R"(CREATE TEMPORARY TABLE "lazyormtemp_fruits" (LIKE "fruits"); )"
                                           "\n" R"(INSERT INTO "lazyormtemp_fruits" ("age","cute","family","hair","id","name") VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE "fruits" t SET "age" = u."age","cute" = u."cute","family" = u."family","hair" = u."hair","name" = u."name" FROM "lazyormtemp_fruits" u WHERE t."id" = u."id" AND (t."userid" = '20') ; )"
                                           "\n" R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_fruits";)"
            );
}

TEST_CASE( "Bulk UPDATE without WHERE updates every staged row", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();

    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_fruits"; )"
                                           "\n" R"(CREATE TEMPORARY TABLE "lazyormtemp_fruits" (LIKE "fruits"); )"
                                           "\n" R"(INSERT INTO "lazyormtemp_fruits" ("age","cute","family","hair","id","name") VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE "fruits" t SET "age" = u."age","cute" = u."cute","family" = u."family","hair" = u."hair","name" = u."name" FROM "lazyormtemp_fruits" u WHERE t."id" = u."id" ; )"
                                           "\n" R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_fruits";)"
            );
}

TEST_CASE( "Bulk UPDATE filters target rows without restricting staging inserts", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();
    lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7})}};

    const auto query = lazyOrm.query_with_trim_consecutive_spaces();

    // The WHERE clause is ANDed with the primary-key join on the UPDATE only.
    REQUIRE( query.find(R"(WHERE t."id" = u."id" AND (t."grade" in (1,5,7)))") != std::string::npos );
    // The staging INSERT stays unfiltered.
    REQUIRE( query.find(R"(INSERT INTO "lazyormtemp_fruits" ("age","cute","family","hair","id","name") VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )") != std::string::npos );
}

TEST_CASE( "Bulk UPDATE associates values with their columns and escapes data", "[bulk_update]" ) {

    LazyOrm::PostgreLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "items";
    lazyOrm.setProperties({
        {{"id",1},{"title","O'Brien"},{"price",9.5},{"stock",std::monostate{}},{"flag",false}},
        {{"id",2},{"title","plain"},{"price",3},{"stock",7},{"flag",true}}
    });

    // std::monostate renders as NULL, booleans, numbers and escaped quotes
    // are rendered per row, and each value keeps the column of its own row.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_items"; )"
                                           "\n" R"(CREATE TEMPORARY TABLE "lazyormtemp_items" (LIKE "items"); )"
                                           "\n" R"(INSERT INTO "lazyormtemp_items" ("flag","id","price","stock","title") VALUES (false,'1','9.5',NULL,'O\'Brien'),(true,'2','3','7','plain') ; )"
                                           "\n" R"(UPDATE "items" t SET "flag" = u."flag","price" = u."price","stock" = u."stock","title" = u."title" FROM "lazyormtemp_items" u WHERE t."id" = u."id" ; )"
                                           "\n" R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_items";)"
            );
}

TEST_CASE( "Bulk UPDATE rejects invalid input with an empty query", "[bulk_update][validation]" ) {

    // Empty batch.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty table name.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Missing primary-key configuration.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Primary key missing from a row.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Inconsistent column sets.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",2},{"age",8}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Duplicate primary-key values are ambiguous.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",1},{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Only the primary key: nothing to update.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1}},{{"id",2}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty row inside the batch.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Invalid input also fails the parameterized path.
    {
      LazyOrm::PostgreLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE binds every value as a parameter", "[bulk_update][parameterized]" ) {

    LazyOrm::PostgreLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({
        {{"id", 1}, {"name", "Alice"}, {"age", 25}},
        {{"id", 2}, {"name", "Bob"}, {"age", 30}}
    });
    lazyOrm[LazyOrm::WHERE] = {{"active", true}};

    const auto query = lazyOrm.buildParameterized();

    // PostgreSQL placeholders are numbered across all statements. Batch
    // values are bound row-major; the WHERE value is bound last. The staging
    // INSERT targets the temporary table, and the primary key is only used
    // for the join, not for a SET assignment.
    REQUIRE( Catch::trim(query.sql) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ; CREATE TEMPORARY TABLE "lazyormtemp_users" (LIKE "users") ; INSERT INTO "lazyormtemp_users" ("age","id","name") VALUES ($1,$2,$3),($4,$5,$6) ; UPDATE "users" t SET "age" = u."age","name" = u."name" FROM "lazyormtemp_users" u WHERE t."id" = u."id" AND (t."active" = $7) ; DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ;)" );

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

TEST_CASE( "Bulk UPDATE restricts pg_temp cleanup and handles schema-qualified and malformed identifiers", "[bulk_update][identifiers]" ) {

    {
        LazyOrm::PostgreLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "public.users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        const std::string expectedDrop = R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_public_users";)";
        REQUIRE( sql.starts_with(expectedDrop) );
        REQUIRE( sql.ends_with(expectedDrop) );
        REQUIRE( sql.find(R"(CREATE TEMPORARY TABLE "lazyormtemp_public_users" (LIKE "public"."users");)") != std::string::npos );
        REQUIRE( sql.find(R"(UPDATE "public"."users" t SET "name" = u."name" FROM "lazyormtemp_public_users" u WHERE t."id" = u."id" ;)") != std::string::npos );
    }

    for (const std::string &badTable : {std::string(".users"), std::string("users."), std::string("a..b"), std::string("users; DROP TABLE users;--")}) {
        LazyOrm::PostgreLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = badTable;
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
    }
}

TEST_CASE( "Bulk UPDATE validates primary-key types, NULLs and duplicates", "[bulk_update][validation]" ) {

    {
        LazyOrm::PostgreLazy lazyOrm;
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
        LazyOrm::PostgreLazy lazyOrm;
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
        LazyOrm::PostgreLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", std::monostate{}}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE places AND after primary-key match and preserves nested filters", "[bulk_update][where]" ) {

    LazyOrm::PostgreLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
    lazyOrm[LazyOrm::WHERE] = {
        {
            {"status", "active"},
            {"OR"},
            {"role", "admin"}
        },
        {"OR NOT"},
        {
            {"deleted", true},
            {"AND"},
            {"locked", false}
        }
    };

    const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
    REQUIRE( sql.find(R"(WHERE t."id" = u."id" AND (((t."status" = 'active') OR (t."role" = 'admin')) OR NOT ((t."deleted" = true) AND (t."locked" = false))) ;)") != std::string::npos );

    const auto param = lazyOrm.buildParameterized();
    REQUIRE( param.sql.find(R"(WHERE t."id" = u."id" AND (((t."status" = $3) OR (t."role" = $4)) OR NOT ((t."deleted" = $5) AND (t."locked" = $6))) ;)") != std::string::npos );
    REQUIRE( param.params.size() == 6 );
}

TEST_CASE( "Bulk UPDATE rejects invalid columns, raw markers, and keeps NULL vs string NULL consistent", "[bulk_update][validation]" ) {

    for (const std::string &badCol : {std::string(""), std::string("a.b"), std::string("[no']name"), std::string("[func]name")}) {
        LazyOrm::PostgreLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {badCol, "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    LazyOrm::PostgreLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "users";
    lazyOrm.setProperties({
        {{"id", 1}, {"label", "NULL"}, {"note", std::monostate{}}}
    });
    const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
    REQUIRE( sql.find(R"(VALUES ('1','NULL',NULL) ;)") != std::string::npos );

    const auto param = lazyOrm.buildParameterized();
    REQUIRE( param.sql.find(R"(VALUES ($1,$2,$3) ;)") != std::string::npos );
    REQUIRE( param.params.size() == 3 );
    REQUIRE( param.params[1].typeName() == "string" );
    REQUIRE( param.params[1].toString() == "NULL" );
    REQUIRE( param.params[2].typeName().empty() );
}
