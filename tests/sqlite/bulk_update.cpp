#include <iostream>
#include "DbList.h"
#include "SqliteLazy.h"

#include <catch2/catch_all.hpp>
#include <cstdint>

namespace {

LazyOrm::SqliteLazy fruitsQuery()
{
  LazyOrm::SqliteLazy lazyOrm;
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

    // SQLite has no UPDATE ... JOIN (and UPDATE ... FROM needs 3.33+), so
    // each updated column gets a correlated subquery against the temporary
    // staging table. The batch is inserted into the staging table (never
    // into `fruits`), target rows match by primary key, the primary key is
    // not updated, all columns share one SET clause, and the WHERE filter
    // qualifies target rows.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS temp.`lazyormtemp_fruits`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`); )"
                                           "\n" R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE `fruits` SET `age` = (SELECT u.`age` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`cute` = (SELECT u.`cute` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`family` = (SELECT u.`family` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`hair` = (SELECT u.`hair` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`name` = (SELECT u.`name` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`) AND (`userid` = '20') ; )"
                                           "\n" R"(DROP TABLE IF EXISTS temp.`lazyormtemp_fruits`;)"
            );
}

TEST_CASE( "Bulk UPDATE without WHERE updates every staged row", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();

    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS temp.`lazyormtemp_fruits`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`); )"
                                           "\n" R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )"
                                           "\n" R"(UPDATE `fruits` SET `age` = (SELECT u.`age` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`cute` = (SELECT u.`cute` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`family` = (SELECT u.`family` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`hair` = (SELECT u.`hair` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`),`name` = (SELECT u.`name` FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`) ; )"
                                           "\n" R"(DROP TABLE IF EXISTS temp.`lazyormtemp_fruits`;)"
            );
}

TEST_CASE( "Bulk UPDATE filters target rows without restricting staging inserts", "[bulk_update]" ) {

    auto lazyOrm = fruitsQuery();
    lazyOrm[LazyOrm::WHERE] = {{"grade","in", LazyOrm::DbList({1,5,7})}};

    const auto query = lazyOrm.query_with_trim_consecutive_spaces();

    // The EXISTS guard and user WHERE clause appear on the outer UPDATE only.
    REQUIRE( query.find("WHERE EXISTS (SELECT 1 FROM `lazyormtemp_fruits` u WHERE u.`id` = `fruits`.`id`) AND (`grade` in (1,5,7))") != std::string::npos );
    // The staging INSERT stays unfiltered.
    REQUIRE( query.find(R"(INSERT INTO `lazyormtemp_fruits` (`age`,`cute`,`family`,`hair`,`id`,`name`) VALUES ('6',true,'forger','pink','1','anya'),('8',false,'Desmond','black','2','damian'),('30',false,'forger','blond','3','loid') ; )") != std::string::npos );
}

TEST_CASE( "Bulk UPDATE associates values with their columns and escapes data", "[bulk_update]" ) {

    LazyOrm::SqliteLazy lazyOrm;
    lazyOrm[LazyOrm::PrimaryKey] = "id";
    lazyOrm[LazyOrm::BULK_UPDATE] = "items";
    lazyOrm.setProperties({
        {{"id",1},{"title","O'Brien"},{"price",9.5},{"stock",std::monostate{}},{"flag",false}},
        {{"id",2},{"title","plain"},{"price",3},{"stock",7},{"flag",true}}
    });

    // std::monostate renders as NULL, booleans, numbers and escaped quotes
    // are rendered per row, and each value keeps the column of its own row.
    REQUIRE( Catch::trim(lazyOrm.query_with_trim_consecutive_spaces()) == R"(DROP TABLE IF EXISTS temp.`lazyormtemp_items`; )"
                                           "\n" R"(CREATE TEMPORARY TABLE `lazyormtemp_items` (`flag`,`id`,`price`,`stock`,`title`); )"
                                           "\n" R"(INSERT INTO `lazyormtemp_items` (`flag`,`id`,`price`,`stock`,`title`) VALUES (false,'1','9.5',NULL,'O\'Brien'),(true,'2','3','7','plain') ; )"
                                           "\n" R"(UPDATE `items` SET `flag` = (SELECT u.`flag` FROM `lazyormtemp_items` u WHERE u.`id` = `items`.`id`),`price` = (SELECT u.`price` FROM `lazyormtemp_items` u WHERE u.`id` = `items`.`id`),`stock` = (SELECT u.`stock` FROM `lazyormtemp_items` u WHERE u.`id` = `items`.`id`),`title` = (SELECT u.`title` FROM `lazyormtemp_items` u WHERE u.`id` = `items`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_items` u WHERE u.`id` = `items`.`id`) ; )"
                                           "\n" R"(DROP TABLE IF EXISTS temp.`lazyormtemp_items`;)"
            );
}

TEST_CASE( "Bulk UPDATE rejects invalid input with an empty query", "[bulk_update][validation]" ) {

    // Empty batch.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty table name.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Missing primary-key configuration.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Primary key missing from a row.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Inconsistent column sets.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",2},{"age",8}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Duplicate primary-key values are ambiguous.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{{"id",1},{"name","damian"}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Only the primary key: nothing to update.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1}},{{"id",2}}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Empty row inside the batch.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::PrimaryKey] = "id";
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}},{}});
      REQUIRE( lazyOrm.queryString().empty() );
    }
    // Invalid input also fails the parameterized path.
    {
      LazyOrm::SqliteLazy lazyOrm;
      lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
      lazyOrm.setProperties({{{"id",1},{"name","anya"}}});
      REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE binds every value as a parameter", "[bulk_update][parameterized]" ) {

    LazyOrm::SqliteLazy lazyOrm;
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
    // only used for the subquery correlation, not for a SET assignment.
    REQUIRE( Catch::trim(query.sql) == R"(DROP TABLE IF EXISTS temp.`lazyormtemp_users` ; CREATE TEMPORARY TABLE `lazyormtemp_users` (`age`,`id`,`name`) ; INSERT INTO `lazyormtemp_users` (`age`,`id`,`name`) VALUES (?,?,?),(?,?,?) ; UPDATE `users` SET `age` = (SELECT u.`age` FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`),`name` = (SELECT u.`name` FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND (`active` = ?) ; DROP TABLE IF EXISTS temp.`lazyormtemp_users` ;)" );

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

TEST_CASE( "Bulk UPDATE SQLite EXISTS guard protects unmatched and filtered rows", "[bulk_update][exists]" ) {

    // Without user WHERE: EXISTS restricts UPDATE to rows present in staging,
    // so rows absent from the batch are never overwritten with NULL.
    {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        REQUIRE( sql.find("WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) ;") != std::string::npos );
    }

    // With user WHERE containing OR: EXISTS must remain ANDed with the
    // parenthesized user filter so rows excluded by WHERE or absent from
    // staging remain unchanged.
    {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        lazyOrm[LazyOrm::WHERE] = {
            {"role", "admin"},
            {"OR"},
            {"active", true}
        };

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        REQUIRE( sql.find("WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND ((`role` = 'admin') OR (`active` = true)) ;") != std::string::npos );

        const auto param = lazyOrm.buildParameterized();
        REQUIRE( param.sql.find("WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND ((`role` = ?) OR (`active` = ?)) ;") != std::string::npos );
        REQUIRE( param.params.size() == 4 );
    }
}

TEST_CASE( "Bulk UPDATE restricts temp cleanup and handles schema-qualified and malformed identifiers", "[bulk_update][identifiers]" ) {

    // Temporary-table DROP explicitly targets the temp schema on both ends.
    {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "main.users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        const std::string expectedDrop = "DROP TABLE IF EXISTS temp.`lazyormtemp_main_users`;";
        REQUIRE( sql.starts_with(expectedDrop) );
        REQUIRE( sql.ends_with(expectedDrop) );
        REQUIRE( sql.find("UPDATE `main`.`users` SET `name` = (SELECT u.`name` FROM `lazyormtemp_main_users` u WHERE u.`id` = `users`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_main_users` u WHERE u.`id` = `users`.`id`) ;") != std::string::npos );
    }

    // Malformed table and primary-key identifiers are rejected.
    for (const std::string &badTable : {std::string(".users"), std::string("users."), std::string("a..b"), std::string("a.b.c"), std::string("users; DROP TABLE users;--"), std::string("users name")}) {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = badTable;
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    for (const std::string &badPk : {std::string(""), std::string("   "), std::string("a.b"), std::string("id; DROP TABLE users")}) {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = badPk;
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }
}

TEST_CASE( "Bulk UPDATE validates primary-key types, NULLs and duplicates", "[bulk_update][validation]" ) {

    // Mixed primary-key type categories (e.g. int 1 vs string "1", int 1 vs
    // double 1.0, bool vs int) are rejected conservatively to prevent
    // database column coercion from turning them into duplicate keys.
    {
        LazyOrm::SqliteLazy lazyOrm;
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
        LazyOrm::SqliteLazy lazyOrm;
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
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({
            {{"id", true}, {"name", "bool_one"}},
            {{"id", 1}, {"name", "int_one"}}
        });
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    // Duplicate values with the same type or integer family (e.g. int 1,
    // long long 1, unsigned int 1) are rejected.
    {
        LazyOrm::SqliteLazy signedDup;
        signedDup[LazyOrm::PrimaryKey] = "id";
        signedDup[LazyOrm::BULK_UPDATE] = "users";
        signedDup.setProperties({
            {{"id", 1}, {"name", "first"}},
            {{"id", 1LL}, {"name", "second"}}
        });
        REQUIRE( signedDup.queryString().empty() );
        REQUIRE( signedDup.buildParameterized().sql.empty() );

        LazyOrm::SqliteLazy mixedWidthDup;
        mixedWidthDup[LazyOrm::PrimaryKey] = "id";
        mixedWidthDup[LazyOrm::BULK_UPDATE] = "users";
        mixedWidthDup.setProperties({
            {{"id", 1}, {"name", "first"}},
            {{"id", 1U}, {"name", "second"}}
        });
        REQUIRE( mixedWidthDup.queryString().empty() );
        REQUIRE( mixedWidthDup.buildParameterized().sql.empty() );
    }

    // NULL primary-key values (monostate or SQL NULL string) are rejected.
    {
        LazyOrm::SqliteLazy monoPk;
        monoPk[LazyOrm::PrimaryKey] = "id";
        monoPk[LazyOrm::BULK_UPDATE] = "users";
        monoPk.setProperties({{{"id", std::monostate{}}, {"name", "Alice"}}});
        REQUIRE( monoPk.queryString().empty() );
        REQUIRE( monoPk.buildParameterized().sql.empty() );

        LazyOrm::SqliteLazy nullStrPk;
        nullStrPk[LazyOrm::PrimaryKey] = "id";
        nullStrPk[LazyOrm::BULK_UPDATE] = "users";
        nullStrPk.setProperties({{{"id", "NULL"}, {"name", "Alice"}}});
        REQUIRE( nullStrPk.queryString().empty() );
        REQUIRE( nullStrPk.buildParameterized().sql.empty() );
    }

    // Homogeneous batches for each supported primary-key category are accepted.
    {
        LazyOrm::SqliteLazy intBatch;
        intBatch[LazyOrm::PrimaryKey] = "id";
        intBatch[LazyOrm::BULK_UPDATE] = "users";
        intBatch.setProperties({
            {{"id", 1}, {"name", "signed"}},
            {{"id", 2U}, {"name", "unsigned"}}
        });
        REQUIRE_FALSE( intBatch.queryString().empty() );
        REQUIRE_FALSE( intBatch.buildParameterized().sql.empty() );

        LazyOrm::SqliteLazy floatBatch;
        floatBatch[LazyOrm::PrimaryKey] = "id";
        floatBatch[LazyOrm::BULK_UPDATE] = "users";
        floatBatch.setProperties({{{"id", 3.5}, {"name", "float"}}});
        REQUIRE_FALSE( floatBatch.queryString().empty() );

        LazyOrm::SqliteLazy strBatch;
        strBatch[LazyOrm::PrimaryKey] = "id";
        strBatch[LazyOrm::BULK_UPDATE] = "users";
        strBatch.setProperties({
            {{"id", std::string("1")}, {"name", "a"}},
            {{"id", std::string("01")}, {"name", "b"}}
        });
        REQUIRE_FALSE( strBatch.queryString().empty() );

        LazyOrm::SqliteLazy boolBatch;
        boolBatch[LazyOrm::PrimaryKey] = "id";
        boolBatch[LazyOrm::BULK_UPDATE] = "users";
        boolBatch.setProperties({{{"id", true}, {"name", "bool"}}});
        REQUIRE_FALSE( boolBatch.queryString().empty() );

        LazyOrm::SqliteLazy blobBatch;
        blobBatch[LazyOrm::PrimaryKey] = "id";
        blobBatch[LazyOrm::BULK_UPDATE] = "users";
        blobBatch.setProperties({{{"id", LazyOrm::BlobType{std::byte{0x0a}}}, {"name", "blob"}}});
        REQUIRE_FALSE( blobBatch.queryString().empty() );
    }
}

TEST_CASE( "Bulk UPDATE places AND after EXISTS and preserves nested filters", "[bulk_update][where]" ) {

    // Deeply nested groups with AND, OR, AND NOT, OR NOT and sibling conditions.
    {
        LazyOrm::SqliteLazy lazyOrm;
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
            },
            {"OR NOT"},
            {"locked", true}
        };

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        REQUIRE( sql.find(R"(WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND (((`status` = 'active') AND ((`age` > '18') OR (`role` = 'admin'))) AND NOT ((`deleted` = true) OR ((`archived` = true) AND (`hidden` = true))) OR NOT (`locked` = true)) ;)") != std::string::npos );

        const auto param = lazyOrm.buildParameterized();
        REQUIRE( param.sql.find(R"(WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND (((`status` = ?) AND ((`age` > ?) OR (`role` = ?))) AND NOT ((`deleted` = ?) OR ((`archived` = ?) AND (`hidden` = ?))) OR NOT (`locked` = ?)) ;)") != std::string::npos );
        REQUIRE( param.params.size() == 9 );
        REQUIRE( param.params[2].toString() == "active" );
        REQUIRE( param.params[3].toLongLong() == 18 );
        REQUIRE( param.params[4].toString() == "admin" );
        REQUIRE( param.params[8].toBool() == true );
    }

    // Sibling conditions after a leaf vector<DbVariant> logical operator
    // (non-returning traversal check).
    {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {"name", "Alice"}}});
        lazyOrm[LazyOrm::WHERE] = LazyOrm::WhereFilter{
            LazyOrm::WhereFilter(std::vector<LazyOrm::DbVariant>{"a", 1}),
            LazyOrm::WhereFilter(std::vector<LazyOrm::DbVariant>{"OR"}),
            LazyOrm::WhereFilter(std::vector<LazyOrm::DbVariant>{"b", 2}),
            LazyOrm::WhereFilter(std::vector<LazyOrm::DbVariant>{"AND"}),
            LazyOrm::WhereFilter(std::vector<LazyOrm::DbVariant>{"c", 3})
        };

        const auto sql = lazyOrm.query_with_trim_consecutive_spaces();
        REQUIRE( sql.find(R"(WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND ((`a` = '1') OR (`b` = '2') AND (`c` = '3')) ;)") != std::string::npos );

        const auto param = lazyOrm.buildParameterized();
        REQUIRE( param.sql.find(R"(WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND ((`a` = ?) OR (`b` = ?) AND (`c` = ?)) ;)") != std::string::npos );
        REQUIRE( param.params.size() == 5 );
    }
}

TEST_CASE( "Bulk UPDATE rejects invalid columns, raw markers, and keeps NULL vs string NULL consistent", "[bulk_update][validation]" ) {

    for (const std::string &badCol : {std::string(""), std::string("bad col"), std::string("a.b"), std::string("[no']name"), std::string("[no`]name"), std::string("[func]name"), std::string("[update]name"), std::string("name; DROP TABLE users")}) {
        LazyOrm::SqliteLazy lazyOrm;
        lazyOrm[LazyOrm::PrimaryKey] = "id";
        lazyOrm[LazyOrm::BULK_UPDATE] = "users";
        lazyOrm.setProperties({{{"id", 1}, {badCol, "Alice"}}});
        REQUIRE( lazyOrm.queryString().empty() );
        REQUIRE( lazyOrm.buildParameterized().sql.empty() );
    }

    // Raw markers in batch data values and WHERE operands are rejected.
    {
        LazyOrm::SqliteLazy rawBatch;
        rawBatch[LazyOrm::PrimaryKey] = "id";
        rawBatch[LazyOrm::BULK_UPDATE] = "users";
        rawBatch.setProperties({{{"id", 1}, {"name", "[no']NOW()"}}});
        REQUIRE( rawBatch.queryString().empty() );
        REQUIRE( rawBatch.buildParameterized().sql.empty() );
    }
    {
        LazyOrm::SqliteLazy rawWhere;
        rawWhere[LazyOrm::PrimaryKey] = "id";
        rawWhere[LazyOrm::BULK_UPDATE] = "users";
        rawWhere.setProperties({{{"id", 1}, {"name", "Alice"}}});
        rawWhere[LazyOrm::WHERE] = {{"name", "[no']1 OR 1=1"}};
        REQUIRE( rawWhere.queryString().empty() );
        REQUIRE( rawWhere.buildParameterized().sql.empty() );
    }

    // Identical inputs produce equivalent NULL vs string "NULL" semantics.
    LazyOrm::SqliteLazy lazyOrm;
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
