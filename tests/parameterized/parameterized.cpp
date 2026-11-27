#include "DbList.h"
#include "JoinMariadb.h"
#include "LazyOrm.h"
#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"

#include <catch2/catch_all.hpp>

namespace {

using LazyOrm::DbVariant;
using LazyOrm::ParameterizedQuery;

void requireInteger(const DbVariant &value, long long expected)
{
    REQUIRE(value.typeName() == "int");
    REQUIRE(value.toLongLong() == expected);
}

void requireDouble(const DbVariant &value, long double expected)
{
    REQUIRE(value.typeName() == "float");
    REQUIRE(value.toLongDouble() == expected);
}

void requireString(const DbVariant &value, const std::string &expected)
{
    REQUIRE(value.typeName() == "string");
    REQUIRE(value.toString() == expected);
}

// Type-aware DbVariant equality: two variants are equivalent only when they
// hold the same underlying type and the same value of that type. Comparing
// toString() alone would treat e.g. int 7 and string "7" as equal.
bool sameVariantValue(const DbVariant &actual, const DbVariant &expected)
{
    if (actual.typeName() != expected.typeName()) {
        return false;
    }
    const std::string type = actual.typeName();
    if (type == "string") {
        return actual.toString() == expected.toString();
    }
    if (type == "int") {
        return actual.toLongLong() == expected.toLongLong();
    }
    if (type == "uint") {
        return actual.toULongLong() == expected.toULongLong();
    }
    if (type == "float") {
        return actual.toLongDouble() == expected.toLongDouble();
    }
    if (type == "boolean") {
        return actual.toBool() == expected.toBool();
    }
    if (type == "blob") {
        return actual.toBlob() == expected.toBlob();
    }
    if (type == "list") {
        const auto actualList = actual.toList();
        const auto expectedList = expected.toList();
        if (actualList.size() != expectedList.size()) {
            return false;
        }
        for (size_t i = 0; i < actualList.size(); ++i) {
            if (!sameVariantValue(actualList.at(i), expectedList.at(i))) {
                return false;
            }
        }
        return true;
    }
    // Only monostate (empty typeName) carries no value. Unknown types must not
    // silently compare equal.
    return type.empty();
}

void requireSameVariant(const DbVariant &actual, const DbVariant &expected)
{
    REQUIRE(actual.typeName() == expected.typeName());
    REQUIRE(sameVariantValue(actual, expected));
}

void requireSameParameterized(const LazyOrm::ParameterizedQuery &actual,
                              const LazyOrm::ParameterizedQuery &expected)
{
    REQUIRE(actual.sql == expected.sql);
    REQUIRE(actual.params.size() == expected.params.size());
    for (size_t i = 0; i < expected.params.size(); ++i) {
        requireSameVariant(actual.params[i], expected.params[i]);
    }
}

template<typename Lazy>
Lazy insertQuery()
{
    Lazy query;
    query[LazyOrm::INSERT] = "users";
    query.setProperties({
        {"id", 7},
        {"score", 9.5},
        {"active", true},
        {"name", "O'Reilly"},
        {"deleted_at", std::monostate{}}
    });
    return query;
}

template<typename Lazy>
Lazy selectQuery()
{
    Lazy query;
    query[LazyOrm::SELECT] = "users";
    query << "id" << "name";
    query[LazyOrm::WHERE] = {
        {"deleted_at", "IS", "NULL"},
        {"AND"},
        {"id", "in", LazyOrm::DbList({2, 4, 6})},
        {"AND"},
        {"score", "BETWEEN", 1.5, "AND", 9.5},
        {"AND"},
        {"active", true}
    };
    query[LazyOrm::LIMIT] = {{10, 20}};
    return query;
}

template<typename Lazy>
Lazy updateQuery()
{
    Lazy query;
    query[LazyOrm::UPDATE] = "users";
    query["name"] = "updated";
    query["age"] = 31;
    query[LazyOrm::WHERE] = {{"id", 42}};
    return query;
}

template<typename Lazy>
Lazy deleteQuery()
{
    Lazy query;
    query[LazyOrm::DELETE] = "users";
    query[LazyOrm::WHERE] = {{"id", "in", LazyOrm::DbList({7, 9})}};
    return query;
}

template<typename Lazy>
Lazy upsertQuery()
{
    Lazy query;
    query[LazyOrm::INSERT_UPDATE] = "users";
    query[LazyOrm::UniqueKeys] = {"id"};
    query["id"] = 7;
    query["name"] = "inserted";
    query["[update]name"] = "updated";
    return query;
}

template<typename Lazy>
Lazy bulkInsertQuery()
{
    Lazy query;
    query[LazyOrm::BULK_INSERT] = "users";
    query.setProperties({
        {{"id", 1}, {"name", "first"}},
        {{"id", 2}, {"name", "second"}}
    });
    return query;
}

}

TEST_CASE("parameterized INSERT preserves native values", "[parameterized][insert]")
{
    const auto postgre = insertQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(INSERT INTO users ("active","deleted_at","id","name","score") VALUES ($1,$2,$3,$4,$5) ;)");
    REQUIRE(postgre.params.size() == 5);
    REQUIRE(postgre.params[0].typeName() == "boolean");
    REQUIRE(postgre.params[0].toBool());
    REQUIRE(postgre.params[1].typeName().empty());
    requireInteger(postgre.params[2], 7);
    requireString(postgre.params[3], "O'Reilly");
    requireDouble(postgre.params[4], 9.5);

    const auto maria = insertQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(INSERT INTO users (`active`,`deleted_at`,`id`,`name`,`score`) VALUES (?,?,?,?,?) ;)");
    REQUIRE(maria.params.size() == 5);
    REQUIRE(maria.params[0].typeName() == "boolean");
    REQUIRE(maria.params[1].typeName().empty());
    requireInteger(maria.params[2], 7);
    requireString(maria.params[3], "O'Reilly");
    requireDouble(maria.params[4], 9.5);

    const auto sqlite = insertQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(INSERT INTO users (`active`,`deleted_at`,`id`,`name`,`score`) VALUES (?,?,?,?,?) ;)");
    REQUIRE(sqlite.params.size() == 5);
    REQUIRE(sqlite.params[0].typeName() == "boolean");
    REQUIRE(sqlite.params[1].typeName().empty());
    requireInteger(sqlite.params[2], 7);
    requireString(sqlite.params[3], "O'Reilly");
    requireDouble(sqlite.params[4], 9.5);
}

TEST_CASE("parameterized SELECT handles null IN BETWEEN and pagination", "[parameterized][select]")
{
    const auto postgre = selectQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(SELECT "id","name" FROM users WHERE (("deleted_at" IS NULL) AND ("id" in ($1,$2,$3)) AND ("score" BETWEEN $4 AND $5) AND ("active" = $6)) LIMIT $7 OFFSET $8 ;)");
    REQUIRE(postgre.params.size() == 8);
    requireInteger(postgre.params[0], 2);
    requireInteger(postgre.params[1], 4);
    requireInteger(postgre.params[2], 6);
    requireDouble(postgre.params[3], 1.5);
    requireDouble(postgre.params[4], 9.5);
    REQUIRE(postgre.params[5].typeName() == "boolean");
    requireInteger(postgre.params[6], 10);
    requireInteger(postgre.params[7], 20);

    const auto maria = selectQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(SELECT `id`,`name` FROM users WHERE ((`deleted_at` IS NULL) AND (`id` in (?,?,?)) AND (`score` BETWEEN ? AND ?) AND (`active` = ?)) LIMIT ? OFFSET ? ;)");
    REQUIRE(maria.params.size() == 8);
    requireInteger(maria.params[0], 2);
    requireInteger(maria.params[1], 4);
    requireInteger(maria.params[2], 6);
    requireDouble(maria.params[3], 1.5);
    requireDouble(maria.params[4], 9.5);
    REQUIRE(maria.params[5].typeName() == "boolean");
    requireInteger(maria.params[6], 10);
    requireInteger(maria.params[7], 20);

    const auto sqlite = selectQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(SELECT `id`,`name` FROM users WHERE ((`deleted_at` IS NULL) AND (`id` in (?,?,?)) AND (`score` BETWEEN ? AND ?) AND (`active` = ?)) LIMIT ? OFFSET ? ;)");
    REQUIRE(sqlite.params.size() == 8);
    requireInteger(sqlite.params[0], 2);
    requireInteger(sqlite.params[1], 4);
    requireInteger(sqlite.params[2], 6);
    requireDouble(sqlite.params[3], 1.5);
    requireDouble(sqlite.params[4], 9.5);
    REQUIRE(sqlite.params[5].typeName() == "boolean");
    requireInteger(sqlite.params[6], 10);
    requireInteger(sqlite.params[7], 20);
}

TEST_CASE("parameterized UPDATE numbers SET before WHERE", "[parameterized][update]")
{
    const auto postgre = updateQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(UPDATE users SET "age"=$1,"name"=$2 WHERE ("id" = $3) ;)");
    REQUIRE(postgre.params.size() == 3);
    requireInteger(postgre.params[0], 31);
    requireString(postgre.params[1], "updated");
    requireInteger(postgre.params[2], 42);

    const auto maria = updateQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(UPDATE users SET `age`=?,`name`=? WHERE (`id` = ?) ;)");
    REQUIRE(maria.params.size() == 3);
    requireInteger(maria.params[0], 31);
    requireString(maria.params[1], "updated");
    requireInteger(maria.params[2], 42);

    const auto sqlite = updateQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(UPDATE users SET `age`=?,`name`=? WHERE (`id` = ?) ;)");
    REQUIRE(sqlite.params.size() == 3);
    requireInteger(sqlite.params[0], 31);
    requireString(sqlite.params[1], "updated");
    requireInteger(sqlite.params[2], 42);
}

TEST_CASE("parameterized DELETE expands typed IN lists", "[parameterized][delete]")
{
    const auto postgre = deleteQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(DELETE FROM users WHERE ("id" in ($1,$2)) ;)");
    REQUIRE(postgre.params.size() == 2);
    requireInteger(postgre.params[0], 7);
    requireInteger(postgre.params[1], 9);

    const auto maria = deleteQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(DELETE FROM users WHERE (`id` in (?,?)) ;)");
    REQUIRE(maria.params.size() == 2);
    requireInteger(maria.params[0], 7);
    requireInteger(maria.params[1], 9);

    const auto sqlite = deleteQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(DELETE FROM users WHERE (`id` in (?,?)) ;)");
    REQUIRE(sqlite.params.size() == 2);
    requireInteger(sqlite.params[0], 7);
    requireInteger(sqlite.params[1], 9);
}

TEST_CASE("parameterized UPSERT uses dialect conflict syntax", "[parameterized][upsert]")
{
    const auto postgre = upsertQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(INSERT INTO users ("id","name") VALUES ($1,$2) ON CONFLICT ("id") DO UPDATE SET "name"=$3;)");
    REQUIRE(postgre.params.size() == 3);
    requireInteger(postgre.params[0], 7);
    requireString(postgre.params[1], "inserted");
    requireString(postgre.params[2], "updated");

    const auto maria = upsertQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(INSERT INTO users (`id`,`name`) VALUES (?,?) ON DUPLICATE KEY UPDATE `name`=?;)");
    REQUIRE(maria.params.size() == 3);
    requireInteger(maria.params[0], 7);
    requireString(maria.params[1], "inserted");
    requireString(maria.params[2], "updated");

    const auto sqlite = upsertQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(INSERT INTO users (`id`,`name`) VALUES (?,?) ON CONFLICT (`id`) DO UPDATE SET `name`=?;)");
    REQUIRE(sqlite.params.size() == 3);
    requireInteger(sqlite.params[0], 7);
    requireString(sqlite.params[1], "inserted");
    requireString(sqlite.params[2], "updated");
}

TEST_CASE("parameterized BULK INSERT numbers every row", "[parameterized][bulk_insert]")
{
    const auto postgre = bulkInsertQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(INSERT INTO users ("id","name") VALUES ($1,$2),($3,$4) ;)");
    REQUIRE(postgre.params.size() == 4);
    requireInteger(postgre.params[0], 1);
    requireString(postgre.params[1], "first");
    requireInteger(postgre.params[2], 2);
    requireString(postgre.params[3], "second");

    const auto maria = bulkInsertQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(INSERT INTO users (`id`,`name`) VALUES (?,?),(?,?) ;)");
    REQUIRE(maria.params.size() == 4);
    requireInteger(maria.params[0], 1);
    requireString(maria.params[1], "first");
    requireInteger(maria.params[2], 2);
    requireString(maria.params[3], "second");

    const auto sqlite = bulkInsertQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(INSERT INTO users (`id`,`name`) VALUES (?,?),(?,?) ;)");
    REQUIRE(sqlite.params.size() == 4);
    requireInteger(sqlite.params[0], 1);
    requireString(sqlite.params[1], "first");
    requireInteger(sqlite.params[2], 2);
    requireString(sqlite.params[3], "second");
}

template<typename Lazy>
Lazy bulkUpdateQuery()
{
    Lazy query;
    query[LazyOrm::PrimaryKey] = "id";
    query[LazyOrm::BULK_UPDATE] = "users";
    query.setProperties({
        {{"id", 1}, {"name", "first"}, {"score", 1.5}},
        {{"id", 2}, {"name", "second"}, {"score", 2.5}}
    });
    query[LazyOrm::WHERE] = {
        {"deleted_at", "IS", "NULL"},
        {"AND"},
        {"id", "in", LazyOrm::DbList({2, 4, 6})}
    };
    return query;
}

TEST_CASE("parameterized BULK UPDATE stages, joins and filters per dialect", "[parameterized][bulk_update]")
{
    // Batch values are bound row-major first, the WHERE filter values are
    // bound last, and every statement references the same staging table.
    const auto postgre = bulkUpdateQuery<LazyOrm::PostgreLazy>().buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ; CREATE TEMPORARY TABLE "lazyormtemp_users" (LIKE "users") ; INSERT INTO "lazyormtemp_users" ("id","name","score") VALUES ($1,$2,$3),($4,$5,$6) ; UPDATE "users" t SET "name" = u."name","score" = u."score" FROM "lazyormtemp_users" u WHERE t."id" = u."id" AND ((t."deleted_at" IS NULL) AND (t."id" in ($7,$8,$9))) ; DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ;)");
    REQUIRE(postgre.params.size() == 9);
    requireInteger(postgre.params[0], 1);
    requireString(postgre.params[1], "first");
    requireDouble(postgre.params[2], 1.5);
    requireInteger(postgre.params[3], 2);
    requireString(postgre.params[4], "second");
    requireDouble(postgre.params[5], 2.5);
    // WHERE parameters follow all batch parameters.
    requireInteger(postgre.params[6], 2);
    requireInteger(postgre.params[7], 4);
    requireInteger(postgre.params[8], 6);

    const auto maria = bulkUpdateQuery<LazyOrm::MariadbLazy>().buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_users` ; CREATE TEMPORARY TABLE `lazyormtemp_users` LIKE `users` ; INSERT INTO `lazyormtemp_users` (`id`,`name`,`score`) VALUES (?,?,?),(?,?,?) ; UPDATE `users` t JOIN `lazyormtemp_users` u ON t.`id` = u.`id` SET t.`name` = u.`name`,t.`score` = u.`score` WHERE ((t.`deleted_at` IS NULL) AND (t.`id` in (?,?,?))) ; DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_users` ;)");
    REQUIRE(maria.params.size() == 9);
    requireInteger(maria.params[0], 1);
    requireString(maria.params[1], "first");
    requireDouble(maria.params[2], 1.5);
    requireInteger(maria.params[3], 2);
    requireString(maria.params[4], "second");
    requireDouble(maria.params[5], 2.5);
    requireInteger(maria.params[6], 2);
    requireInteger(maria.params[7], 4);
    requireInteger(maria.params[8], 6);

    const auto sqlite = bulkUpdateQuery<LazyOrm::SqliteLazy>().buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(DROP TABLE IF EXISTS temp.`lazyormtemp_users` ; CREATE TEMPORARY TABLE `lazyormtemp_users` (`id`,`name`,`score`) ; INSERT INTO `lazyormtemp_users` (`id`,`name`,`score`) VALUES (?,?,?),(?,?,?) ; UPDATE `users` SET `name` = (SELECT u.`name` FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`),`score` = (SELECT u.`score` FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) WHERE EXISTS (SELECT 1 FROM `lazyormtemp_users` u WHERE u.`id` = `users`.`id`) AND ((`deleted_at` IS NULL) AND (`id` in (?,?,?))) ; DROP TABLE IF EXISTS temp.`lazyormtemp_users` ;)");
    REQUIRE(sqlite.params.size() == 9);
    requireInteger(sqlite.params[0], 1);
    requireString(sqlite.params[1], "first");
    requireDouble(sqlite.params[2], 1.5);
    requireInteger(sqlite.params[3], 2);
    requireString(sqlite.params[4], "second");
    requireDouble(sqlite.params[5], 2.5);
    requireInteger(sqlite.params[6], 2);
    requireInteger(sqlite.params[7], 4);
    requireInteger(sqlite.params[8], 6);
}

TEST_CASE("parameterized BULK UPDATE binds blobs, NULLs and quoted strings", "[parameterized][bulk_update]")
{
    LazyOrm::PostgreLazy query;
    query[LazyOrm::PrimaryKey] = "id";
    query[LazyOrm::BULK_UPDATE] = "users";
    query.setProperties({
        {{"id", 1}, {"name", "O'Brien"}, {"avatar", LazyOrm::BlobType{std::byte{0x01}, std::byte{0x02}}}, {"deleted_at", std::monostate{}}}
    });
    query[LazyOrm::WHERE] = {{"active", true}};

    const auto parameterized = query.buildParameterized();
    REQUIRE(Catch::trim(parameterized.sql) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ; CREATE TEMPORARY TABLE "lazyormtemp_users" (LIKE "users") ; INSERT INTO "lazyormtemp_users" ("avatar","deleted_at","id","name") VALUES ($1,$2,$3,$4) ; UPDATE "users" t SET "avatar" = u."avatar","deleted_at" = u."deleted_at","name" = u."name" FROM "lazyormtemp_users" u WHERE t."id" = u."id" AND (t."active" = $5) ; DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ;)");

    REQUIRE(parameterized.params.size() == 5);
    // Blob values are bound, never interpolated.
    REQUIRE(parameterized.params[0].typeName() == "blob");
    REQUIRE(parameterized.params[0].toBlob() == LazyOrm::BlobType{std::byte{0x01}, std::byte{0x02}});
    // NULL is bound as a parameter on the staging INSERT.
    REQUIRE(parameterized.params[1].typeName().empty());
    requireInteger(parameterized.params[2], 1);
    requireString(parameterized.params[3], "O'Brien");
    // The WHERE value is bound after all batch values.
    REQUIRE(parameterized.params[4].typeName() == "boolean");
    REQUIRE(parameterized.params[4].toBool());
}

TEST_CASE("parameterized BULK UPDATE rejects invalid input", "[parameterized][bulk_update]")
{
    LazyOrm::PostgreLazy noPrimaryKey;
    noPrimaryKey[LazyOrm::BULK_UPDATE] = "users";
    noPrimaryKey.setProperties({
        {{"id", 1}, {"name", "first"}}
    });
    REQUIRE(noPrimaryKey.buildParameterized().sql.empty());

    LazyOrm::MariadbLazy duplicateKeys;
    duplicateKeys[LazyOrm::PrimaryKey] = "id";
    duplicateKeys[LazyOrm::BULK_UPDATE] = "users";
    duplicateKeys.setProperties({
        {{"id", 1}, {"name", "first"}},
        {{"id", 1}, {"name", "second"}}
    });
    REQUIRE(duplicateKeys.buildParameterized().sql.empty());

    LazyOrm::SqliteLazy primaryKeyOnly;
    primaryKeyOnly[LazyOrm::PrimaryKey] = "id";
    primaryKeyOnly[LazyOrm::BULK_UPDATE] = "users";
    primaryKeyOnly.setProperties({
        {{"id", 1}},
        {{"id", 2}}
    });
    REQUIRE(primaryKeyOnly.buildParameterized().sql.empty());

    LazyOrm::PostgreLazy badTable;
    badTable[LazyOrm::PrimaryKey] = "id";
    badTable[LazyOrm::BULK_UPDATE] = "public..users";
    badTable.setProperties({{{"id", 1}, {"name", "first"}}});
    REQUIRE(badTable.buildParameterized().sql.empty());

    LazyOrm::SqliteLazy nullPk;
    nullPk[LazyOrm::PrimaryKey] = "id";
    nullPk[LazyOrm::BULK_UPDATE] = "users";
    nullPk.setProperties({{{"id", std::monostate{}}, {"name", "first"}}});
    REQUIRE(nullPk.buildParameterized().sql.empty());

    LazyOrm::PostgreLazy mixedTypes;
    mixedTypes[LazyOrm::PrimaryKey] = "id";
    mixedTypes[LazyOrm::BULK_UPDATE] = "public.users";
    mixedTypes.setProperties({
        {{"id", 1}, {"name", "int_one"}},
        {{"id", std::string("1")}, {"name", "str_one"}}
    });
    REQUIRE(mixedTypes.buildParameterized().sql.empty());

    LazyOrm::PostgreLazy schemaQualified;
    schemaQualified[LazyOrm::PrimaryKey] = "id";
    schemaQualified[LazyOrm::BULK_UPDATE] = "public.users";
    schemaQualified.setProperties({
        {{"id", 1}, {"name", "NULL"}, {"note", std::monostate{}}},
        {{"id", 2}, {"name", "second"}, {"note", "ok"}}
    });
    schemaQualified[LazyOrm::WHERE] = {
        {
            {"status", "active"},
            {"OR"},
            {"role", "admin"}
        },
        {"AND NOT"},
        {"deleted", true}
    };
    const auto schemaResult = schemaQualified.buildParameterized();
    REQUIRE(Catch::trim(schemaResult.sql) == R"(DROP TABLE IF EXISTS pg_temp."lazyormtemp_public_users" ; CREATE TEMPORARY TABLE "lazyormtemp_public_users" (LIKE "public"."users") ; INSERT INTO "lazyormtemp_public_users" ("id","name","note") VALUES ($1,$2,$3),($4,$5,$6) ; UPDATE "public"."users" t SET "name" = u."name","note" = u."note" FROM "lazyormtemp_public_users" u WHERE t."id" = u."id" AND (((t."status" = $7) OR (t."role" = $8)) AND NOT (t."deleted" = $9)) ; DROP TABLE IF EXISTS pg_temp."lazyormtemp_public_users" ;)");
    REQUIRE(schemaResult.params.size() == 9);
    requireInteger(schemaResult.params[0], 1);
    requireString(schemaResult.params[1], "NULL");
    REQUIRE(schemaResult.params[2].typeName().empty());
    requireInteger(schemaResult.params[3], 2);
    requireString(schemaResult.params[4], "second");
    requireString(schemaResult.params[5], "ok");
    requireString(schemaResult.params[6], "active");
    requireString(schemaResult.params[7], "admin");
    REQUIRE(schemaResult.params[8].typeName() == "boolean");
    REQUIRE(schemaResult.params[8].toBool());
}

TEST_CASE("PostgreSQL converts comma pagination in both APIs", "[parameterized][limit]")
{
    LazyOrm::PostgreLazy query;
    query[LazyOrm::SELECT] = "users";
    query << "id";
    query[LazyOrm::LIMIT] = "5,10";

    REQUIRE(Catch::trim(query.query_with_trim_consecutive_spaces()) == R"(SELECT "id" FROM users LIMIT 5 OFFSET 10 ;)");
    const auto parameterized = query.buildParameterized();
    REQUIRE(Catch::trim(parameterized.sql) == R"(SELECT "id" FROM users LIMIT $1 OFFSET $2 ;)");
    REQUIRE(parameterized.params.size() == 2);
    requireInteger(parameterized.params[0], 5);
    requireInteger(parameterized.params[1], 10);
}

TEST_CASE("parameterized raw queries retain placeholders and native parameters", "[parameterized][raw]")
{
    LazyOrm::PostgreLazy postgreQuery;
    postgreQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES ($1,$2)";
    postgreQuery.setProperties({{"name", "raw"}, {"id", 12}});
    const auto postgre = postgreQuery.buildParameterized();
    REQUIRE(postgre.sql == "INSERT INTO users (id,name) VALUES ($1,$2)");
    REQUIRE(postgre.params.size() == 2);
    requireInteger(postgre.params[0], 12);
    requireString(postgre.params[1], "raw");

    LazyOrm::MariadbLazy mariaQuery;
    mariaQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES (?,?)";
    mariaQuery.setProperties({{"name", "raw"}, {"id", 12}});
    const auto maria = mariaQuery.buildParameterized();
    REQUIRE(maria.sql == "INSERT INTO users (id,name) VALUES (?,?)");
    REQUIRE(maria.params.size() == 2);
    requireInteger(maria.params[0], 12);
    requireString(maria.params[1], "raw");

    LazyOrm::SqliteLazy sqliteQuery;
    sqliteQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES (?,?)";
    sqliteQuery.setProperties({{"name", "raw"}, {"id", 12}});
    const auto sqlite = sqliteQuery.buildParameterized();
    REQUIRE(sqlite.sql == "INSERT INTO users (id,name) VALUES (?,?)");
    REQUIRE(sqlite.params.size() == 2);
    requireInteger(sqlite.params[0], 12);
    requireString(sqlite.params[1], "raw");
}

TEST_CASE("parameterized postgres raw queries resolve numbered placeholders by index", "[parameterized][raw]")
{
    // Out-of-order placeholders: the parameter vector stays in property key
    // order; $2 still binds the second parameter.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $2, $1";
        query["b"] = "second";
        query["a"] = "first";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT $2, $1");
        REQUIRE(parameterized.params.size() == 2);
        requireString(parameterized.params[0], "first");
        requireString(parameterized.params[1], "second");
    }
    // Repeated placeholders bind a single parameter; it is not consumed twice.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $1, $1";
        query["a"] = "same";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT $1, $1");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "same");
    }
}

TEST_CASE("parameterized postgres raw queries reject missing, extra and invalid placeholders", "[parameterized][raw]")
{
    const auto requireRejected = [](LazyOrm::PostgreLazy &query) {
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql.empty());
        REQUIRE(parameterized.params.empty());
    };
    // Gap: $2 referenced but only one parameter supplied.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $2";
        query["a"] = "only";
        requireRejected(query);
    }
    // Gap: $2 referenced with two parameters supplied ($1 never referenced).
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $2";
        query.setProperties({{"a", 1}, {"b", 2}});
        requireRejected(query);
    }
    // Gap: $1, $3 referenced with three parameters supplied ($2 never referenced).
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $1, $3";
        query.setProperties({{"a", 1}, {"b", 2}, {"c", 3}});
        requireRejected(query);
    }
    // Extra: more parameters than the largest referenced index.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $1";
        query.setProperties({{"a", 1}, {"b", 2}});
        requireRejected(query);
    }
    // $0 is not a valid PostgreSQL placeholder.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $0";
        query.setProperties({{"a", 1}});
        requireRejected(query);
    }
    // Malformed dollar tokens ($1abc, $1_x, $1$, bare $, $abc).
    for (const std::string &badSql : {
             std::string("SELECT $1abc"),
             std::string("SELECT $1_x"),
             std::string("SELECT $1$"),
             std::string("SELECT $"),
             std::string("SELECT $abc"),
             std::string("SELECT $1tag$"),
         }) {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = badSql;
        query.setProperties({{"a", 1}});
        requireRejected(query);
    }
    // Placeholder index overflow can never be satisfied.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $99999999999999999999999999";
        requireRejected(query);
    }
    // Parameters without any placeholder are silently-discarded extras.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT 1";
        query.setProperties({{"a", 1}});
        requireRejected(query);
    }
}

TEST_CASE("parameterized postgres raw queries ignore placeholder-like text in literals and comments", "[parameterized][raw]")
{
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT '$1', $1";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT '$1', $1");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "v");
    }
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $1 /* $2 */ -- $3\n";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT $1 /* $2 */ -- $3\n");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "v");
    }
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $$ $1 $$, $tag$ $2 $tag$, $1";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT $$ $1 $$, $tag$ $2 $tag$, $1");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "v");
    }
}

TEST_CASE("parameterized raw queries reject positional placeholder count mismatches", "[parameterized][raw]")
{
    // Missing parameter.
    {
        LazyOrm::MariadbLazy mariaQuery;
        mariaQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES (?,?)";
        mariaQuery.setProperties({{"id", 12}});
        const auto maria = mariaQuery.buildParameterized();
        REQUIRE(maria.sql.empty());
        REQUIRE(maria.params.empty());

        LazyOrm::SqliteLazy sqliteQuery;
        sqliteQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES (?,?)";
        sqliteQuery.setProperties({{"id", 12}});
        const auto sqlite = sqliteQuery.buildParameterized();
        REQUIRE(sqlite.sql.empty());
        REQUIRE(sqlite.params.empty());
    }
    // Extra parameter.
    {
        LazyOrm::MariadbLazy mariaQuery;
        mariaQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id) VALUES (?)";
        mariaQuery.setProperties({{"id", 12}, {"name", "raw"}});
        const auto maria = mariaQuery.buildParameterized();
        REQUIRE(maria.sql.empty());
        REQUIRE(maria.params.empty());

        LazyOrm::SqliteLazy sqliteQuery;
        sqliteQuery[LazyOrm::RAW_QUERY] = "INSERT INTO users (id) VALUES (?)";
        sqliteQuery.setProperties({{"id", 12}, {"name", "raw"}});
        const auto sqlite = sqliteQuery.buildParameterized();
        REQUIRE(sqlite.sql.empty());
        REQUIRE(sqlite.params.empty());
    }
}

TEST_CASE("parameterized raw queries ignore question marks in literals and comments", "[parameterized][raw]")
{
    {
        LazyOrm::MariadbLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT '?' AS q, ? /* ? */ -- ?\n";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT '?' AS q, ? /* ? */ -- ?\n");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "v");
    }
    {
        LazyOrm::SqliteLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT `col?`, ?";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT `col?`, ?");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "v");
    }
}

TEST_CASE("parameterized raw queries preserve native parameter types", "[parameterized][raw]")
{
    const LazyOrm::BlobType blob{LazyOrm::ByteType{0x01}, LazyOrm::ByteType{0x02}};
    LazyOrm::PostgreLazy query;
    query[LazyOrm::RAW_QUERY] = "SELECT $1, $2, $3, $4, $5";
    query.setProperties({
        {"v_bool", true},
        {"v_float", 1.5},
        {"v_int", 42},
        {"v_null", std::monostate{}},
        {"v_text", "O'Brien"},
    });
    const auto parameterized = query.buildParameterized();
    REQUIRE(parameterized.sql == "SELECT $1, $2, $3, $4, $5");
    REQUIRE(parameterized.params.size() == 5);
    // Property key order: v_bool, v_float, v_int, v_null, v_text.
    REQUIRE(parameterized.params[0].typeName() == "boolean");
    REQUIRE(parameterized.params[0].toBool() == true);
    requireDouble(parameterized.params[1], 1.5);
    requireInteger(parameterized.params[2], 42);
    REQUIRE(parameterized.params[3].typeName().empty()); // monostate binds as NULL
    requireString(parameterized.params[4], "O'Brien");

    LazyOrm::PostgreLazy blobQuery;
    blobQuery[LazyOrm::RAW_QUERY] = "SELECT $1";
    blobQuery.setProperties({{"v_blob", blob}});
    const auto blobParameterized = blobQuery.buildParameterized();
    REQUIRE(blobParameterized.params.size() == 1);
    REQUIRE(blobParameterized.params[0].toBlob() == blob);
}

TEST_CASE("parameterized and regular raw queries agree on parameter values and order", "[parameterized][raw]")
{
    // Keys deliberately out of lexicographic insertion order: both paths
    // must resolve parameters in property key order.
    LazyOrm::PostgreLazy query;
    query[LazyOrm::RAW_QUERY] = "SELECT $2, $1, $3";
    query["c"] = "third";
    query["a"] = "first";
    query["b"] = "second";
    const auto parameterized = query.buildParameterized();
    REQUIRE(parameterized.params.size() == 3);
    requireString(parameterized.params[0], "first");
    requireString(parameterized.params[1], "second");
    requireString(parameterized.params[2], "third");

    LazyOrm::LazyOrm regular = query;
    REQUIRE(Catch::trim(regular.queryString(LazyOrm::LazyOrm::Postgres)) ==
            R"(SELECT 'second', 'first', 'third')");
}

TEST_CASE("parameterized postgres raw queries accept repeated and out-of-order complete sets", "[parameterized][raw]")
{
    using Pair = std::pair<DbVariant, DbVariant>;
    LazyOrm::PostgreLazy query;
    query[LazyOrm::RAW_QUERY] = "SELECT $1, $1, $2";
    query.setProperties({Pair{"a", "first"}, Pair{"b", "second"}});
    const auto parameterized = query.buildParameterized();
    REQUIRE(parameterized.sql == "SELECT $1, $1, $2");
    REQUIRE(parameterized.params.size() == 2);
    requireString(parameterized.params[0], "first");
    requireString(parameterized.params[1], "second");
}

TEST_CASE("parameterized raw queries distinguish explicit empty strings and star from shorthand", "[parameterized][raw]")
{
    using Pair = std::pair<DbVariant, DbVariant>;
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT $1, $2";
        query.setProperties({Pair{"a", ""}, Pair{"b", "*"}});
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT $1, $2");
        REQUIRE(parameterized.params.size() == 2);
        requireString(parameterized.params[0], "");
        requireString(parameterized.params[1], "*");

        LazyOrm::LazyOrm regular = query;
        REQUIRE(Catch::trim(regular.queryString(LazyOrm::LazyOrm::Postgres)) == R"(SELECT '', '*')");
    }
    {
        LazyOrm::MariadbLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT ?, ?";
        query << "shorthand" << LazyOrm::pair{"explicit", ""};
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT ?, ?");
        REQUIRE(parameterized.params.size() == 2);
        requireString(parameterized.params[0], "");
        requireString(parameterized.params[1], "shorthand");
    }
    {
        LazyOrm::SqliteLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT ?";
        query["name"] = "";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT ?");
        REQUIRE(parameterized.params.size() == 1);
        requireString(parameterized.params[0], "");
    }
}

TEST_CASE("parameterized MariaDB and SQLite raw queries reject numbered and malformed dollar placeholders", "[parameterized][raw]")
{
    const auto requireMariaRejected = [](const std::string &sql,
                                         std::initializer_list<std::pair<DbVariant, DbVariant>> props) {
        LazyOrm::MariadbLazy query;
        query[LazyOrm::RAW_QUERY] = sql;
        query.setProperties(props);
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql.empty());
        REQUIRE(parameterized.params.empty());
    };
    const auto requireSqliteRejected = [](const std::string &sql,
                                          std::initializer_list<std::pair<DbVariant, DbVariant>> props) {
        LazyOrm::SqliteLazy query;
        query[LazyOrm::RAW_QUERY] = sql;
        query.setProperties(props);
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql.empty());
        REQUIRE(parameterized.params.empty());
    };

    for (const auto &sql : {
             std::string("SELECT $1"),
             std::string("SELECT $1, $2"),
             std::string("SELECT ?, $1"),
             std::string("SELECT $1, ?"),
             std::string("SELECT $0"),
             std::string("SELECT $1abc"),
             std::string("SELECT $"),
             std::string("SELECT $abc"),
             std::string("SELECT $$ ? $$"),
             std::string("SELECT $tag$ ? $tag$"),
         }) {
        requireMariaRejected(sql, {});
        requireMariaRejected(sql, {{"a", 1}});
        requireMariaRejected(sql, {{"a", 1}, {"b", 2}});
        requireSqliteRejected(sql, {});
        requireSqliteRejected(sql, {{"a", 1}});
        requireSqliteRejected(sql, {{"a", 1}, {"b", 2}});
    }

    // Valid quoted/identifier uses of '$' still work with '?'.
    {
        LazyOrm::MariadbLazy maria;
        maria[LazyOrm::RAW_QUERY] = "SELECT '$1', col$1, ?";
        maria["a"] = "ok";
        const auto m = maria.buildParameterized();
        REQUIRE(m.sql == "SELECT '$1', col$1, ?");
        REQUIRE(m.params.size() == 1);
        requireString(m.params[0], "ok");

        LazyOrm::SqliteLazy sqlite;
        sqlite[LazyOrm::RAW_QUERY] = "SELECT '$1', col$1, ?";
        sqlite["a"] = "ok";
        const auto s = sqlite.buildParameterized();
        REQUIRE(s.sql == "SELECT '$1', col$1, ?");
        REQUIRE(s.params.size() == 1);
        requireString(s.params[0], "ok");
    }
}

TEST_CASE("parameterized postgres raw queries handle JSONB operators and dialect-aware scanning", "[parameterized][raw]")
{
    using Pair = std::pair<DbVariant, DbVariant>;
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] =
            "SELECT data #> $1, data #>> $2, data ? 'k', data ?| $3, data ?& $4 FROM users";
        query.setProperties({Pair{"a", "{a}"}, Pair{"b", "{b}"}, Pair{"c", "x"}, Pair{"d", "y"}});
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql ==
                "SELECT data #> $1, data #>> $2, data ? 'k', data ?| $3, data ?& $4 FROM users");
        REQUIRE(parameterized.params.size() == 4);
    }
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT data ? 'k' FROM users";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql == "SELECT data ? 'k' FROM users");
        REQUIRE(parameterized.params.empty());
    }
    {
        // Stray '?' with parameter in PostgreSQL is rejected.
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] = "SELECT ? FROM users";
        query["a"] = "v";
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.sql.empty());
        REQUIRE(parameterized.params.empty());
    }
    // Dialect-specific comments, backslash escapes, and identifiers.
    {
        LazyOrm::PostgreLazy query;
        query[LazyOrm::RAW_QUERY] =
            R"(SELECT '\', E'O\'Brien $2', /* /* $2 */ $3 */ $1--$2)" "\n, $2 # $3";
        query.setProperties({{"a", 1}, {"b", 2}, {"c", 3}});
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.params.size() == 3);
    }
    {
        LazyOrm::MariadbLazy query;
        query[LazyOrm::RAW_QUERY] = R"(SELECT 1--2, /* /* c */ ?, '\\', `col\`, ?)";
        query.setProperties({{"a", 1}, {"b", 2}});
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.params.size() == 2);
    }
    {
        LazyOrm::SqliteLazy query;
        query[LazyOrm::RAW_QUERY] = R"(SELECT '\', /* /* c */ ?, [col?], ? # ?)" "\n--?\n";
        query.setProperties({{"a", 1}, {"b", 2}, {"c", 3}});
        const auto parameterized = query.buildParameterized();
        REQUIRE(parameterized.params.size() == 3);
    }
}

TEST_CASE("parameterized INSERT IGNORE uses dialect-specific syntax", "[parameterized][insert_ignore]")
{
    LazyOrm::PostgreLazy postgreQuery;
    postgreQuery[LazyOrm::INSERT_IGNORE] = "users";
    postgreQuery.setProperties({{"id", 7}, {"name", "ignored"}});
    const auto postgre = postgreQuery.buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(INSERT INTO users ("id","name") VALUES ($1,$2) ON CONFLICT DO NOTHING;)");
    REQUIRE(postgre.params.size() == 2);

    LazyOrm::MariadbLazy mariaQuery;
    mariaQuery[LazyOrm::INSERT_IGNORE] = "users";
    mariaQuery.setProperties({{"id", 7}, {"name", "ignored"}});
    const auto maria = mariaQuery.buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(INSERT IGNORE INTO users (`id`,`name`) VALUES (?,?) ;)");
    REQUIRE(maria.params.size() == 2);

    LazyOrm::SqliteLazy sqliteQuery;
    sqliteQuery[LazyOrm::INSERT_IGNORE] = "users";
    sqliteQuery.setProperties({{"id", 7}, {"name", "ignored"}});
    const auto sqlite = sqliteQuery.buildParameterized();
    REQUIRE(Catch::trim(sqlite.sql) == R"(INSERT OR IGNORE INTO users (`id`,`name`) VALUES (?,?) ;)");
    REQUIRE(sqlite.params.size() == 2);
}

TEST_CASE("SQLite converts comma pagination to OFFSET form", "[parameterized][limit][sqlite]")
{
    LazyOrm::SqliteLazy query;
    query[LazyOrm::SELECT] = "users";
    query << "id";
    query[LazyOrm::LIMIT] = "5,10";

    const auto parameterized = query.buildParameterized();
    REQUIRE(Catch::trim(parameterized.sql) == R"(SELECT `id` FROM users LIMIT ? OFFSET ? ;)");
    REQUIRE(parameterized.params.size() == 2);
    requireInteger(parameterized.params[0], 5);
    requireInteger(parameterized.params[1], 10);
}

TEST_CASE("copies transfer count properties", "[copy][counts]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source.setCountType({"age"});

    LazyOrm::MariadbLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name`,COUNT(`age`) FROM student;)");
}

TEST_CASE("copies transfer unique keys", "[copy][unique_keys]")
{
    LazyOrm::PostgreLazy source;
    source[LazyOrm::INSERT_UPDATE] = "users";
    source[LazyOrm::UniqueKeys] = {"id"};
    source["id"] = 7;
    source["name"] = "inserted";
    source["[update]name"] = "updated";

    LazyOrm::PostgreLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(INSERT INTO users ("id","name") VALUES ('7','inserted') ON CONFLICT ("id") DO UPDATE SET "name"='updated';)");
}

TEST_CASE("copies transfer descending order type", "[copy][orderby]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::ORDERBY_DESC] = {{"num1"}};

    LazyOrm::MariadbLazy copy(source);
    REQUIRE(Catch::trim(copy.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name` FROM student ORDER BY `num1` DESC ;)");
}

TEST_CASE("LazyOrm builds parameterized SQLite queries", "[parameterized][sqlite][lazyorm]")
{
    LazyOrm::LazyOrm lazyOrm;
    lazyOrm[LazyOrm::INSERT] = "users";
    lazyOrm.setProperties({{"id", 7}, {"name", "jack"}, {"score", 9.5}});

    LazyOrm::SqliteLazy direct;
    direct[LazyOrm::INSERT] = "users";
    direct.setProperties({{"id", 7}, {"name", "jack"}, {"score", 9.5}});

    const auto viaLazyOrm = lazyOrm.buildParameterized(LazyOrm::LazyOrm::Sqlite3);
    const auto expected = direct.buildParameterized();
    REQUIRE_FALSE(viaLazyOrm.sql.empty());
    REQUIRE(Catch::trim(viaLazyOrm.sql) == Catch::trim(expected.sql));
    REQUIRE(viaLazyOrm.params.size() == expected.params.size());
    for (size_t i = 0; i < expected.params.size(); ++i) {
        requireSameVariant(viaLazyOrm.params[i], expected.params[i]);
    }
}

TEST_CASE("parameterized LIMIT accepts comma vector syntax", "[parameterized][limit]")
{
    LazyOrm::PostgreLazy postgreQuery;
    postgreQuery[LazyOrm::SELECT] = "users";
    postgreQuery << "id";
    postgreQuery[LazyOrm::LIMIT] = {{11, ",", 23}};
    const auto postgre = postgreQuery.buildParameterized();
    REQUIRE(Catch::trim(postgre.sql) == R"(SELECT "id" FROM users LIMIT $1 OFFSET $2 ;)");
    REQUIRE(postgre.params.size() == 2);
    REQUIRE(postgre.params[0].toLongLong() == 11);
    REQUIRE(postgre.params[1].toLongLong() == 23);

    LazyOrm::MariadbLazy mariaQuery;
    mariaQuery[LazyOrm::SELECT] = "users";
    mariaQuery << "id";
    mariaQuery[LazyOrm::LIMIT] = {{11, ",", 23}};
    const auto maria = mariaQuery.buildParameterized();
    REQUIRE(Catch::trim(maria.sql) == R"(SELECT `id` FROM users LIMIT ? OFFSET ? ;)");
    REQUIRE(maria.params.size() == 2);
    REQUIRE(maria.params[0].toLongLong() == 23);
    REQUIRE(maria.params[1].toLongLong() == 11);
}

TEST_CASE("parameterized PostgreSQL INSERT IGNORE places RETURNING after ON CONFLICT", "[parameterized][insert_ignore]")
{
    LazyOrm::PostgreLazy query;
    query[LazyOrm::INSERT_IGNORE] = "users";
    query.setProperties({{"id", 7}, {"name", "ignored"}});
    query.setPrimaryKey("id");

    const auto parameterized = query.buildParameterized();
    REQUIRE(Catch::trim(parameterized.sql) ==
            R"(INSERT INTO users ("id","name") VALUES ($1,$2) ON CONFLICT DO NOTHING returning id;)");
    REQUIRE(parameterized.params.size() == 2);
}

TEST_CASE("PostgreSQL CONTAINS and COUNT quote identifiers with double quotes", "[postgres][contains][count]")
{
    LazyOrm::PostgreLazy containsQuery;
    containsQuery[LazyOrm::CONTAINS] = "users";
    containsQuery[LazyOrm::WHERE] = {{"id", 7}};
    REQUIRE(Catch::trim(containsQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS "contains" FROM users WHERE ("id" = '7') ;)");

    LazyOrm::PostgreLazy countQuery;
    countQuery[LazyOrm::COUNT] = "users";
    countQuery[LazyOrm::WHERE] = {{"id", 7}};
    REQUIRE(Catch::trim(countQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT COUNT(*) AS "count" FROM users WHERE ("id" = '7') ;)");
}

TEST_CASE("join separates select lists of joined tables with commas", "[join][mariadb]")
{
    LazyOrm::MariadbLazy student;
    student[LazyOrm::SELECT] = "student";
    student.setProperties({"name", "age", "hair"});

    LazyOrm::MariadbLazy grade;
    grade[LazyOrm::SELECT] = "grade";
    grade << "grade" << "passed";

    LazyOrm::JoinMariadb joinMariadb;
    joinMariadb[LazyOrm::Primary] = student;
    joinMariadb[LazyOrm::Joins] = {
        {LazyOrm::JoinRole::LeftJoin, grade, "student_id", "id"}
    };

    REQUIRE(Catch::trim(joinMariadb.query_with_trim_consecutive_spaces()) ==
            "SELECT \n"
            "`student`.`age`, \n"
            "`student`.`hair`, \n"
            "`student`.`name`, \n"
            "`grade`.`grade`, \n"
            "`grade`.`passed` \n"
            "FROM student \n"
            "LEFT JOIN grade ON `student`.`id` = `grade`.`student_id` \n"
            ";");
}

TEST_CASE("copy assignment transfers full query state", "[copy][assign]")
{
    LazyOrm::PostgreLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source.setCountType({"age"});
    source[LazyOrm::WHERE] = {{"grade", "in", LazyOrm::DbList({1, 5})}};
    source[LazyOrm::GROUPBY] = {{"group1", "group2"}};
    source[LazyOrm::ORDERBY_DESC] = {{"num1"}};
    source[LazyOrm::LIMIT] = "5,6";
    source.setPrimaryKey("id");

    LazyOrm::PostgreLazy assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            Catch::trim(source.query_with_trim_consecutive_spaces()));

    const std::string sql = assigned.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("COUNT(\"age\")") != std::string::npos);
    REQUIRE(sql.find("\"grade\" in (1,5)") != std::string::npos);
    REQUIRE(sql.find("GROUP BY \"group1\",\"group2\"") != std::string::npos);
    REQUIRE(sql.find("ORDER BY \"num1\" DESC") != std::string::npos);
    REQUIRE(sql.find("LIMIT 5 OFFSET 6") != std::string::npos);
}

TEST_CASE("copy assignment transfers WHERE and HAVING state", "[copy][assign][where][having]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::WHERE] = {{"name", "like", "a%"}};
    source[LazyOrm::Having::HAVING] = {{"COUNT(*)", ">", "2"}};

    LazyOrm::MariadbLazy assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            Catch::trim(source.query_with_trim_consecutive_spaces()));

    const std::string sql = assigned.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("WHERE (`name` like 'a%')") != std::string::npos);
    REQUIRE(sql.find("HAVING (`COUNT(*)` > '2')") != std::string::npos);
}

namespace {

LazyOrm::WhereFilter deepWhereFilter()
{
    return {
        {
            {"status", "=", "active"},
            {"AND"},
            {
                {"age", ">", 18},
                {"OR"},
                {"role", "=", "admin"}
            }
        },
        {"AND NOT"},
        {
            {"deleted", "=", true},
            {"OR"},
            {
                {"archived", "=", true},
                {"AND"},
                {"hidden", "=", true}
            }
        }
    };
}

LazyOrm::HavingFilter deepHavingFilter()
{
    return {
        {
            {"COUNT(*)", ">", 2},
            {"AND"},
            {
                {"SUM(score)", ">=", 10},
                {"OR"},
                {"AVG(score)", "<", 5}
            }
        },
        {"OR NOT"},
        {
            {"MIN(age)", "<", 18},
            {"AND"},
            {
                {"MAX(age)", ">", 65},
                {"OR"},
                {"COUNT(DISTINCT role)", ">", 3}
            }
        }
    };
}

LazyOrm::WhereFilter staleWhereFilter()
{
    return {
        {
            {"stale", "=", 1},
            {"OR"},
            {"old", "=", 2}
        },
        {"AND"},
        {
            {"legacy", "=", 3},
            {"AND NOT"},
            {"removed", "=", 4}
        }
    };
}

LazyOrm::HavingFilter staleHavingFilter()
{
    return {
        {
            {"stale_count", ">", 9},
            {"OR"},
            {"old_count", "<", 1}
        },
        {"AND NOT"},
        {
            {"legacy_count", "=", 3},
            {"AND"},
            {"removed_count", "=", 4}
        }
    };
}

}

TEST_CASE("copy assignment preserves deeply nested WHERE filters", "[copy][assign][where][nested]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::WHERE] = deepWhereFilter();

    LazyOrm::MariadbLazy assigned;
    assigned[LazyOrm::SELECT] = "old_table";
    assigned << "stale_column";
    assigned[LazyOrm::WHERE] = staleWhereFilter();
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.whereFilter() == source.whereFilter());
    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name` FROM student WHERE (((`status` = 'active') AND ((`age` > '18') OR (`role` = 'admin'))) AND NOT ((`deleted` = true) OR ((`archived` = true) AND (`hidden` = true)))) ;)");
}

TEST_CASE("copy assignment preserves deeply nested HAVING filters", "[copy][assign][having][nested]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::GROUPBY] = {{"name"}};
    source[LazyOrm::Having::HAVING] = deepHavingFilter();

    LazyOrm::MariadbLazy assigned;
    assigned[LazyOrm::SELECT] = "old_table";
    assigned << "stale_column";
    assigned[LazyOrm::GROUPBY] = {{"stale_group"}};
    assigned[LazyOrm::Having::HAVING] = staleHavingFilter();
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.havingFilter() == source.havingFilter());
    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            R"(SELECT `name` FROM student GROUP BY `name` HAVING (((`COUNT(*)` > '2') AND ((`SUM(score)` >= '10') OR (`AVG(score)` < '5'))) OR NOT ((`MIN(age)` < '18') AND ((`MAX(age)` > '65') OR (`COUNT(DISTINCT role)` > '3')))) ;)");
}

TEST_CASE("copy assignment preserves nested WHERE and HAVING together", "[copy][assign][where][having][nested]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::WHERE] = deepWhereFilter();
    source[LazyOrm::GROUPBY] = {{"name"}};
    source[LazyOrm::Having::HAVING] = deepHavingFilter();

    LazyOrm::MariadbLazy assigned;
    assigned[LazyOrm::WHERE] = staleWhereFilter();
    assigned[LazyOrm::Having::HAVING] = staleHavingFilter();
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.whereFilter() == source.whereFilter());
    REQUIRE(assigned.havingFilter() == source.havingFilter());
    const std::string sql = Catch::trim(assigned.query_with_trim_consecutive_spaces());
    REQUIRE(sql ==
            R"(SELECT `name` FROM student WHERE (((`status` = 'active') AND ((`age` > '18') OR (`role` = 'admin'))) AND NOT ((`deleted` = true) OR ((`archived` = true) AND (`hidden` = true)))) GROUP BY `name` HAVING (((`COUNT(*)` > '2') AND ((`SUM(score)` >= '10') OR (`AVG(score)` < '5'))) OR NOT ((`MIN(age)` < '18') AND ((`MAX(age)` > '65') OR (`COUNT(DISTINCT role)` > '3')))) ;)");
    REQUIRE(sql.find("stale") == std::string::npos);
}

TEST_CASE("join separates select lists for multiple joined tables", "[join][mariadb]")
{
    LazyOrm::MariadbLazy student;
    student[LazyOrm::SELECT] = "student";
    student.setProperties({"name"});

    LazyOrm::MariadbLazy grade;
    grade[LazyOrm::SELECT] = "grade";
    grade << "grade" << "passed";

    LazyOrm::MariadbLazy classes;
    classes[LazyOrm::SELECT] = "classes";
    classes << "title";

    LazyOrm::JoinMariadb joinMariadb;
    joinMariadb[LazyOrm::Primary] = student;
    joinMariadb[LazyOrm::Joins] = {
        {LazyOrm::JoinRole::LeftJoin, grade, "student_id", "id"},
        {LazyOrm::JoinRole::InnerJoin, classes, "student_id", "id"}
    };

    const std::string sql = joinMariadb.query_with_trim_consecutive_spaces();
    REQUIRE(Catch::trim(sql) ==
            "SELECT \n"
            "`student`.`name`, \n"
            "`grade`.`grade`, \n"
            "`grade`.`passed`, \n"
            "`classes`.`title` \n"
            "FROM student \n"
            "LEFT JOIN grade ON `student`.`id` = `grade`.`student_id` \n"
            "INNER JOIN classes ON `student`.`id` = `classes`.`student_id` \n"
            ";");
    REQUIRE(sql.find(",,") == std::string::npos);
    REQUIRE(sql.find("SELECT \n,") == std::string::npos);
    REQUIRE(sql.find(", \nFROM") == std::string::npos);
}

TEST_CASE("PostgreSQL COUNT and CONTAINS quote nested WHERE conditions", "[postgres][contains][count][nested]")
{
    const auto nestedWhere = [] {
        return LazyOrm::WhereFilter{
            {
                {"name", "like", "a%"},
                {"OR"},
                {"name", "like", "%z"}
            },
            {"AND"},
            {
                {"age", ">", "18"},
                {"AND"},
                {"grade", "in", LazyOrm::DbList({1, 5})}
            }
        };
    };

    LazyOrm::PostgreLazy countQuery;
    countQuery[LazyOrm::COUNT] = "users";
    countQuery[LazyOrm::WHERE] = nestedWhere();
    REQUIRE(Catch::trim(countQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT COUNT(*) AS "count" FROM users WHERE ((("name" like 'a%') OR ("name" like '%z')) AND (("age" > '18') AND ("grade" in (1,5)))) ;)");

    LazyOrm::PostgreLazy containsQuery;
    containsQuery[LazyOrm::CONTAINS] = "users";
    containsQuery[LazyOrm::WHERE] = nestedWhere();
    REQUIRE(Catch::trim(containsQuery.query_with_trim_consecutive_spaces()) ==
            R"(SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS "contains" FROM users WHERE ((("name" like 'a%') OR ("name" like '%z')) AND (("age" > '18') AND ("grade" in (1,5)))) ;)");
}

TEST_CASE("copy assignment replaces stale destination state", "[copy][assign][stale]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source[LazyOrm::WHERE] = {{"name", "like", "a%"}};
    source[LazyOrm::Having::HAVING] = {{"COUNT(*)", ">", "2"}};

    LazyOrm::MariadbLazy destination;
    destination[LazyOrm::UPDATE] = "old_table";
    destination["stale_column"] = "stale_value";
    destination[LazyOrm::WHERE] = {{"stale", "=", 1}};
    destination[LazyOrm::Having::HAVING] = {{"stale_count", ">", "9"}};

    destination = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(Catch::trim(destination.query_with_trim_consecutive_spaces()) ==
            Catch::trim(source.query_with_trim_consecutive_spaces()));

    const std::string sql = destination.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("stale") == std::string::npos);
    REQUIRE(sql.find("old_table") == std::string::npos);
}

TEST_CASE("copy assignment transfers batch properties", "[copy][assign][bulk_insert]")
{
    LazyOrm::SqliteLazy source;
    source[LazyOrm::BULK_INSERT] = "users";
    source.setProperties({
        {{"id", 1}, {"name", "first"}},
        {{"id", 2}, {"name", "second"}}
    });

    LazyOrm::SqliteLazy assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            Catch::trim(source.query_with_trim_consecutive_spaces()));
    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            R"(INSERT INTO users (`id`,`name`) VALUES ('1','first'),('2','second') ;)");
}

TEST_CASE("copy assignment transfers state into SqliteLazy", "[copy][assign][sqlite]")
{
    LazyOrm::SqliteLazy source;
    source[LazyOrm::UPDATE] = "student";
    source["name"] = "anya";
    source["age"] = 6;
    source[LazyOrm::WHERE] = {{"id", 3}};
    source[LazyOrm::ORDERBY_ASC] = {{"num1"}};
    source[LazyOrm::LIMIT] = {{4, 5}};

    LazyOrm::SqliteLazy assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
            Catch::trim(source.query_with_trim_consecutive_spaces()));

    const std::string sql = assigned.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("UPDATE student SET") != std::string::npos);
    REQUIRE(sql.find("WHERE (`id` = '3')") != std::string::npos);
    REQUIRE(sql.find("ORDER BY `num1` ASC") != std::string::npos);
    REQUIRE(sql.find("LIMIT 4 OFFSET 5") != std::string::npos);
}

TEST_CASE("copy assignment transfers state into LazyOrm for MariaDB", "[copy][assign][lazyorm]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source.setCountType({"age"});
    source[LazyOrm::WHERE] = {
        {"grade", "in", LazyOrm::DbList({1, 5})},
        {"OR"},
        {"grade", "in", LazyOrm::DbList({7, 9})},
        {"AND"},
        {"name", "like", "a%"}
    };
    source[LazyOrm::GROUPBY] = {{"group1"}};
    source[LazyOrm::ORDERBY_DESC] = {{"num1"}};
    source[LazyOrm::LIMIT] = {{10, 20}};

    LazyOrm::LazyOrm assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.queryString(LazyOrm::LazyOrm::MariaDB) == source.queryString());

    const std::string sql = assigned.queryString(LazyOrm::LazyOrm::MariaDB);
    REQUIRE(sql.find("COUNT(`age`)") != std::string::npos);
    REQUIRE(sql.find("`grade` in (1,5)") != std::string::npos);
    REQUIRE(sql.find("`grade` in (7,9)") != std::string::npos);
    REQUIRE(sql.find("GROUP BY `group1`") != std::string::npos);
    REQUIRE(sql.find("ORDER BY `num1` DESC") != std::string::npos);
    REQUIRE(sql.find("LIMIT 10 OFFSET 20") != std::string::npos);

    requireSameParameterized(assigned.buildParameterized(LazyOrm::LazyOrm::MariaDB),
                             source.buildParameterized());
}

TEST_CASE("copy assignment transfers state into LazyOrm for PostgreSQL", "[copy][assign][lazyorm]")
{
    LazyOrm::PostgreLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    source.setCountType({"age"});
    source[LazyOrm::WHERE] = {
        {
            {"name", "like", "a%"},
            {"OR"},
            {"name", "like", "%z"}
        },
        {"AND"},
        {
            {"age", ">", "18"},
            {"AND"},
            {"grade", "in", LazyOrm::DbList({1, 5})}
        }
    };
    source[LazyOrm::GROUPBY] = {{"group1"}};
    source[LazyOrm::ORDERBY_DESC] = {{"num1"}};
    source[LazyOrm::LIMIT] = "5,10";

    LazyOrm::LazyOrm assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.queryString(LazyOrm::LazyOrm::Postgres) == source.queryString());

    const std::string sql = assigned.queryString(LazyOrm::LazyOrm::Postgres);
    REQUIRE(sql.find("COUNT(\"age\")") != std::string::npos);
    REQUIRE(sql.find("\"name\" like 'a%'") != std::string::npos);
    REQUIRE(sql.find("\"grade\" in (1,5)") != std::string::npos);
    REQUIRE(sql.find("GROUP BY \"group1\"") != std::string::npos);
    REQUIRE(sql.find("ORDER BY \"num1\" DESC") != std::string::npos);
    REQUIRE(sql.find("LIMIT 5 OFFSET 10") != std::string::npos);
    REQUIRE(sql.find('`') == std::string::npos);

    requireSameParameterized(assigned.buildParameterized(LazyOrm::LazyOrm::Postgres),
                             source.buildParameterized());
}

TEST_CASE("copy assignment transfers state into LazyOrm for SQLite", "[copy][assign][lazyorm]")
{
    LazyOrm::SqliteLazy source;
    source[LazyOrm::UPDATE] = "student";
    source["name"] = "anya";
    source["age"] = 6;
    source[LazyOrm::WHERE] = {{"id", 3}, {"AND"}, {"hair", "pink"}};
    source[LazyOrm::ORDERBY_ASC] = {{"num1"}};
    source[LazyOrm::LIMIT] = {{4, 5}};

    LazyOrm::LazyOrm assigned;
    assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(assigned.queryString(LazyOrm::LazyOrm::Sqlite3) == source.queryString());

    const std::string sql = assigned.queryString(LazyOrm::LazyOrm::Sqlite3);
    REQUIRE(sql.find("UPDATE student SET") != std::string::npos);
    REQUIRE(sql.find("`age`='6'") != std::string::npos);
    REQUIRE(sql.find("WHERE ((`id` = '3') AND (`hair` = 'pink'))") != std::string::npos);
    REQUIRE(sql.find("ORDER BY `num1` ASC") != std::string::npos);
    REQUIRE(sql.find("LIMIT 4 OFFSET 5") != std::string::npos);

    requireSameParameterized(assigned.buildParameterized(LazyOrm::LazyOrm::Sqlite3),
                             source.buildParameterized());
}

TEST_CASE("copy assignment clears destination filters when source has none", "[copy][assign][stale][filters]")
{
    LazyOrm::MariadbLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    REQUIRE(source.whereFilter().empty());
    REQUIRE(source.havingFilter().empty());

    LazyOrm::MariadbLazy destination;
    destination[LazyOrm::SELECT] = "student";
    destination << "name";
    destination[LazyOrm::WHERE] = staleWhereFilter();
    destination[LazyOrm::Having::HAVING] = staleHavingFilter();

    REQUIRE_FALSE(destination.whereFilter().empty());
    REQUIRE_FALSE(destination.havingFilter().empty());
    REQUIRE(destination.query_with_trim_consecutive_spaces().find("WHERE") != std::string::npos);
    REQUIRE(destination.query_with_trim_consecutive_spaces().find("HAVING") != std::string::npos);

    destination = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(destination.whereFilter().empty());
    REQUIRE(destination.havingFilter().empty());

    const std::string sql = destination.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("WHERE") == std::string::npos);
    REQUIRE(sql.find("HAVING") == std::string::npos);
    REQUIRE(sql.find("stale") == std::string::npos);
    REQUIRE(Catch::trim(sql) == R"(SELECT `name` FROM student;)");
}

TEST_CASE("parameter comparison distinguishes identical text with different types", "[parameterized][types]")
{
    const DbVariant intSeven = 7;
    const DbVariant uintSeven = 7u;
    const DbVariant stringSeven = std::string("7");

    // identical textual representation ...
    REQUIRE(intSeven.toString() == stringSeven.toString());
    REQUIRE(uintSeven.toString() == stringSeven.toString());
    // ... but different underlying types, so they must not compare equal
    REQUIRE(intSeven.typeName() != stringSeven.typeName());
    REQUIRE(uintSeven.typeName() != intSeven.typeName());
    REQUIRE_FALSE(sameVariantValue(intSeven, stringSeven));
    REQUIRE_FALSE(sameVariantValue(uintSeven, intSeven));
    REQUIRE(sameVariantValue(intSeven, DbVariant(7)));
    REQUIRE(sameVariantValue(stringSeven, DbVariant(std::string("7"))));

    // end-to-end: queries whose only difference is the parameter type
    LazyOrm::PostgreLazy intQuery;
    intQuery[LazyOrm::INSERT] = "users";
    intQuery.setProperties({{"id", 7}});

    LazyOrm::PostgreLazy stringQuery;
    stringQuery[LazyOrm::INSERT] = "users";
    stringQuery.setProperties({{"id", std::string("7")}});

    const auto intParams = intQuery.buildParameterized();
    const auto stringParams = stringQuery.buildParameterized();
    REQUIRE(intParams.params.size() == 1);
    REQUIRE(stringParams.params.size() == 1);
    REQUIRE(intParams.params[0].toString() == stringParams.params[0].toString());
    REQUIRE(intParams.params[0].typeName() != stringParams.params[0].typeName());
    REQUIRE_FALSE(sameVariantValue(intParams.params[0], stringParams.params[0]));
}

TEST_CASE("parameter comparison handles every DbVariant alternative recursively", "[parameterized][types]")
{
    REQUIRE(sameVariantValue(DbVariant(std::string("value")), DbVariant(std::string("value"))));
    REQUIRE_FALSE(sameVariantValue(DbVariant(std::string("value")), DbVariant(std::string("other"))));
    REQUIRE(sameVariantValue(DbVariant(-7), DbVariant(-7)));
    REQUIRE_FALSE(sameVariantValue(DbVariant(-7), DbVariant(-8)));
    REQUIRE(sameVariantValue(DbVariant(7u), DbVariant(7u)));
    REQUIRE_FALSE(sameVariantValue(DbVariant(7u), DbVariant(8u)));
    REQUIRE(sameVariantValue(DbVariant(9.5), DbVariant(9.5)));
    REQUIRE_FALSE(sameVariantValue(DbVariant(9.5), DbVariant(9.25)));
    REQUIRE(sameVariantValue(DbVariant(true), DbVariant(true)));
    REQUIRE_FALSE(sameVariantValue(DbVariant(true), DbVariant(false)));

    const LazyOrm::BlobType blob{LazyOrm::ByteType{0x00}, LazyOrm::ByteType{0x7f}};
    const LazyOrm::BlobType otherBlob{LazyOrm::ByteType{0x00}, LazyOrm::ByteType{0x01}};
    REQUIRE(sameVariantValue(DbVariant(blob), DbVariant(blob)));
    REQUIRE_FALSE(sameVariantValue(DbVariant(blob), DbVariant(otherBlob)));
    REQUIRE(sameVariantValue(DbVariant(std::monostate{}), DbVariant(std::monostate{})));
    REQUIRE(sameVariantValue(DbVariant(LazyOrm::DbList({})), DbVariant(LazyOrm::DbList({}))));

    const DbVariant nested = LazyOrm::DbList({
        1,
        LazyOrm::DbList({std::string("two"), true, LazyOrm::DbList({3u, 4.5})}),
        std::monostate{}
    });
    const DbVariant sameNested = LazyOrm::DbList({
        1,
        LazyOrm::DbList({std::string("two"), true, LazyOrm::DbList({3u, 4.5})}),
        std::monostate{}
    });
    const DbVariant differentNestedValue = LazyOrm::DbList({
        1,
        LazyOrm::DbList({std::string("two"), false, LazyOrm::DbList({3u, 4.5})}),
        std::monostate{}
    });
    const DbVariant reordered = LazyOrm::DbList({
        std::monostate{},
        LazyOrm::DbList({std::string("two"), true, LazyOrm::DbList({3u, 4.5})}),
        1
    });

    REQUIRE(sameVariantValue(nested, sameNested));
    REQUIRE_FALSE(sameVariantValue(nested, differentNestedValue));
    REQUIRE_FALSE(sameVariantValue(nested, reordered));
}

TEST_CASE("same-type copy assignment replaces stale destination state", "[copy][assign][same_type]")
{
    LazyOrm::MariadbLazy mariaSource;
    mariaSource[LazyOrm::SELECT] = "student";
    mariaSource << "name";
    mariaSource[LazyOrm::WHERE] = {{"name", "like", "a%"}};

    LazyOrm::MariadbLazy mariaDest;
    mariaDest[LazyOrm::UPDATE] = "old_table";
    mariaDest["stale_column"] = "stale_value";
    mariaDest[LazyOrm::WHERE] = {{"stale", "=", 1}};
    mariaDest[LazyOrm::LIMIT] = {{9, 9}};

    mariaDest = mariaSource;

    REQUIRE(Catch::trim(mariaDest.query_with_trim_consecutive_spaces()) ==
            Catch::trim(mariaSource.query_with_trim_consecutive_spaces()));
    REQUIRE(mariaDest.query_with_trim_consecutive_spaces().find("stale") == std::string::npos);

    LazyOrm::PostgreLazy postgreSource;
    postgreSource[LazyOrm::DELETE] = "users";
    postgreSource[LazyOrm::WHERE] = {{"id", "in", LazyOrm::DbList({7, 9})}};

    LazyOrm::PostgreLazy postgreDest;
    postgreDest[LazyOrm::UPDATE] = "old_table";
    postgreDest["stale_column"] = "stale_value";
    postgreDest[LazyOrm::WHERE] = {{"stale", "=", 1}};

    postgreDest = postgreSource;

    REQUIRE(Catch::trim(postgreDest.query_with_trim_consecutive_spaces()) ==
            Catch::trim(postgreSource.query_with_trim_consecutive_spaces()));
    REQUIRE(postgreDest.query_with_trim_consecutive_spaces().find("stale") == std::string::npos);

    LazyOrm::SqliteLazy sqliteSource;
    sqliteSource[LazyOrm::UPDATE] = "student";
    sqliteSource["name"] = "anya";
    sqliteSource[LazyOrm::WHERE] = {{"id", 3}};

    LazyOrm::SqliteLazy sqliteDest;
    sqliteDest[LazyOrm::SELECT] = "old_table";
    sqliteDest << "stale_column";
    sqliteDest[LazyOrm::WHERE] = {{"stale", "=", 1}};

    sqliteDest = sqliteSource;

    REQUIRE(Catch::trim(sqliteDest.query_with_trim_consecutive_spaces()) ==
            Catch::trim(sqliteSource.query_with_trim_consecutive_spaces()));
    REQUIRE(sqliteDest.query_with_trim_consecutive_spaces().find("stale") == std::string::npos);

    LazyOrm::LazyOrm lazyOrmSource;
    lazyOrmSource[LazyOrm::SELECT] = "student";
    lazyOrmSource << "name";
    lazyOrmSource[LazyOrm::WHERE] = {{"name", "like", "a%"}};

    LazyOrm::LazyOrm lazyOrmDest;
    lazyOrmDest[LazyOrm::UPDATE] = "old_table";
    lazyOrmDest["stale_column"] = "stale_value";
    lazyOrmDest[LazyOrm::WHERE] = {{"stale", "=", 1}};

    lazyOrmDest = lazyOrmSource;

    REQUIRE(lazyOrmDest.queryString(LazyOrm::LazyOrm::MariaDB) ==
            lazyOrmSource.queryString(LazyOrm::LazyOrm::MariaDB));
    REQUIRE(lazyOrmDest.queryString(LazyOrm::LazyOrm::MariaDB).find("stale") == std::string::npos);
}

TEST_CASE("copy assignment clears destination grouping ordering and limit when source has none",
          "[copy][assign][stale][filters]")
{
    LazyOrm::PostgreLazy source;
    source[LazyOrm::SELECT] = "student";
    source << "name";
    REQUIRE(source.getFilter().groupConditions().empty());
    REQUIRE(source.getFilter().orderConditions().empty());
    REQUIRE(source.getFilter().limitConditions().empty());

    LazyOrm::PostgreLazy destination;
    destination[LazyOrm::SELECT] = "student";
    destination << "name";
    destination[LazyOrm::GROUPBY] = {{"stale_group"}};
    destination[LazyOrm::ORDERBY_DESC] = {{"stale_order"}};
    destination[LazyOrm::LIMIT] = {{5, 6}};

    REQUIRE(destination.query_with_trim_consecutive_spaces().find("GROUP BY") != std::string::npos);
    REQUIRE(destination.query_with_trim_consecutive_spaces().find("ORDER BY") != std::string::npos);
    REQUIRE(destination.query_with_trim_consecutive_spaces().find("LIMIT") != std::string::npos);

    destination = static_cast<const LazyOrm::AbstractLazy &>(source);

    REQUIRE(destination.getFilter().groupConditions().empty());
    REQUIRE(destination.getFilter().orderConditions().empty());
    REQUIRE(destination.getFilter().limitConditions().empty());

    const std::string sql = destination.query_with_trim_consecutive_spaces();
    REQUIRE(sql.find("stale") == std::string::npos);
    REQUIRE(sql.find("GROUP BY") == std::string::npos);
    REQUIRE(sql.find("ORDER BY") == std::string::npos);
    REQUIRE(sql.find("LIMIT") == std::string::npos);
    REQUIRE(Catch::trim(sql) == R"(SELECT "name" FROM student;)");
}

namespace {

template<typename Lazy>
Lazy crossDialectInsertIgnore()
{
    Lazy query;
    query[LazyOrm::INSERT_IGNORE] = "users";
    query.setProperties({{"id", 7}, {"name", "ignored"}});
    query.setPrimaryKey("id");
    return query;
}

template<typename Lazy>
Lazy crossDialectSelect()
{
    Lazy query;
    query[LazyOrm::SELECT] = "student";
    query << "name";
    query.setCountType({"age"});
    query[LazyOrm::WHERE] = {
        {"grade", "in", LazyOrm::DbList({1, 5})},
        {"AND"},
        {"name", "like", "a%"}
    };
    query[LazyOrm::GROUPBY] = {{"group1"}};
    query[LazyOrm::ORDERBY_DESC] = {{"num1"}};
    query[LazyOrm::LIMIT] = {{10, 20}};
    return query;
}

// Cross-dialect assignment is supported API (every dialect class assigns from
// const AbstractLazy&). The destination must keep its own dialect behavior:
// assigning a Source into a Dest must produce exactly what a natively built
// Dest produces, both as plain SQL and as a parameterized query.
template<typename Source, typename Dest>
void requireCrossDialectAssignment()
{
    const auto requireAssignment = [](const Source &source, const Dest &native) {
        Dest assigned;
        assigned = static_cast<const LazyOrm::AbstractLazy &>(source);

        REQUIRE(Catch::trim(assigned.query_with_trim_consecutive_spaces()) ==
                Catch::trim(native.query_with_trim_consecutive_spaces()));
        requireSameParameterized(assigned.buildParameterized(), native.buildParameterized());
    };

    requireAssignment(insertQuery<Source>(), insertQuery<Dest>());
    requireAssignment(updateQuery<Source>(), updateQuery<Dest>());
    requireAssignment(deleteQuery<Source>(), deleteQuery<Dest>());
    requireAssignment(crossDialectInsertIgnore<Source>(), crossDialectInsertIgnore<Dest>());
    requireAssignment(bulkInsertQuery<Source>(), bulkInsertQuery<Dest>());
    requireAssignment(bulkUpdateQuery<Source>(), bulkUpdateQuery<Dest>());
    requireAssignment(upsertQuery<Source>(), upsertQuery<Dest>());
    requireAssignment(crossDialectSelect<Source>(), crossDialectSelect<Dest>());
}

}

TEST_CASE("cross-dialect assignment retains destination dialect behavior", "[copy][assign][cross_dialect]")
{
    requireCrossDialectAssignment<LazyOrm::PostgreLazy, LazyOrm::MariadbLazy>();
    requireCrossDialectAssignment<LazyOrm::PostgreLazy, LazyOrm::SqliteLazy>();
    requireCrossDialectAssignment<LazyOrm::MariadbLazy, LazyOrm::PostgreLazy>();
    requireCrossDialectAssignment<LazyOrm::MariadbLazy, LazyOrm::SqliteLazy>();
    requireCrossDialectAssignment<LazyOrm::SqliteLazy, LazyOrm::PostgreLazy>();
    requireCrossDialectAssignment<LazyOrm::SqliteLazy, LazyOrm::MariadbLazy>();
}
