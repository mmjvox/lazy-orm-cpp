# Use Cases

Practical examples and common use cases for LazyOrm-CPP.

## CRUD Operations

### CREATE - Insert Records

#### Simple INSERT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm["name"] = "John Doe";
lazyOrm["email"] = "john@example.com";
lazyOrm["age"] = 30;

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: INSERT INTO users (`age`,`email`,`name`) VALUES ('30','john@example.com','John Doe') ;
```

#### INSERT Multiple Properties

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm.setProperties({
    {"name", "Jane Smith"},
    {"email", "jane@example.com"},
    {"age", 25},
    {"active", true}
});

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

#### INSERT with Stream Operator

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm << LazyOrm::pair{"name", "Bob"}
        << LazyOrm::pair{"email", "bob@example.com"}
        << LazyOrm::pair{"age", 35};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

#### INSERT ON DUPLICATE KEY UPDATE (MariaDB/MySQL)

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT_UPDATE] = "users";
lazyOrm["name"] = "John";
lazyOrm["email"] = "john@example.com";
lazyOrm["[update]email"] = "newemail@example.com";
lazyOrm["[update]updated_at"] = "NOW()";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: INSERT INTO users (...) VALUES (...) ON DUPLICATE KEY UPDATE `email`='newemail@example.com', `updated_at`='NOW()';
```

#### INSERT IGNORE (MariaDB/MySQL)

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT_IGNORE] = "users";
lazyOrm["name"] = "John";
lazyOrm["email"] = "john@example.com";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

### READ - Query Records

#### Simple SELECT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`email`,`name` FROM users;
```

#### SELECT All Columns

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "*";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT * FROM users;
```

#### SELECT with WHERE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";
lazyOrm[LazyOrm::WHERE] = {{"age", ">=", "18"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`email`,`name` FROM users WHERE (`age` >= '18') ;
```

#### SELECT with Multiple WHERE Conditions

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";
lazyOrm[LazyOrm::WHERE] = {
    {"age", ">=", "18"},
    {"AND"},
    {"active", true}
};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`email`,`name` FROM users WHERE ((`age` >= '18') AND (`active` = true)) ;
```

#### SELECT with IN Clause

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::WHERE] = {{"id", "in", LazyOrm::DbList({1, 2, 3, 4, 5})}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users WHERE (`id` in (1,2,3,4,5)) ;
```

#### SELECT with LIKE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::WHERE] = {{"name", "like", "John%"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users WHERE (`name` like 'John%') ;
```

#### SELECT with BETWEEN

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "age";
lazyOrm[LazyOrm::WHERE] = {{"age", "between", "18,65"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`name` FROM users WHERE (`age` between '18,65') ;
```

#### SELECT with ORDER BY

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";
lazyOrm[LazyOrm::ORDERBY] = {{"name", "age"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`email`,`name` FROM users ORDER BY `name`,`age`;
```

#### SELECT with ORDER BY DESC

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "created_at";
lazyOrm[LazyOrm::ORDERBY_DESC] = {{"created_at"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `created_at`,`name` FROM users ORDER BY `created_at` DESC;
```

#### SELECT with ORDER BY ASC

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "age";
lazyOrm[LazyOrm::ORDERBY_ASC] = {{"age"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`name` FROM users ORDER BY `age` ASC;
```

#### SELECT with Mixed ORDER BY

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "age" << "created_at";
lazyOrm[LazyOrm::ORDERBY] = {{"name", "age[DESC]", "created_at[ASC]"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`created_at`,`name` FROM users ORDER BY `name`,`age` DESC,`created_at` ASC;
```

#### SELECT with LIMIT

```cpp
// MariaDB/MySQL style: offset, count
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::LIMIT] = "10,10";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users LIMIT 10,10 ;
```

```cpp
// SQLite style: count, offset
LazyOrm::SqliteLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::LIMIT] = {{"10", "20"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users LIMIT 10 OFFSET 20;
```

#### SELECT with GROUP BY

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "department" << "COUNT(*)";
lazyOrm[LazyOrm::GROUPBY] = {{"department"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `department`,COUNT(*) FROM users GROUP BY `department`;
```

#### SELECT with HAVING

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "department" << "COUNT(*)";
lazyOrm[LazyOrm::GROUPBY] = {{"department"}};

LazyOrm::HavingFilter having = {{"COUNT(*)", ">", "2"}};
lazyOrm[LazyOrm::Having::HAVING] = having;

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `department`,COUNT(*) FROM users GROUP BY `department` HAVING ((`COUNT(*)` > '2')) ;
```

#### SELECT DISTINCT

```cpp
// Method 1: Using SELECT_DISTINCT
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT_DISTINCT] = "users";
lazyOrm << "department";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT DISTINCT `department` FROM users;
```

```cpp
// Method 2: Enable distinct
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "department";
lazyOrm.enableDistinctSelect(true);

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT DISTINCT `department` FROM users;
```

#### SELECT with COUNT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::COUNT_PROPS] = {"*", "COUNT(*) as total"};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name`,COUNT(*),COUNT(*) as total FROM users;
```

#### Complex SELECT with All Filters

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "student";
lazyOrm << "name" << "age" << "hair";

lazyOrm[LazyOrm::WHERE] = {{"grade", "in", LazyOrm::DbList({1, 5, 7, 9})}};
lazyOrm[LazyOrm::GROUPBY] = {{"group1", "group2"}};
lazyOrm[LazyOrm::ORDERBY] = {{"num1", "num2"}};
lazyOrm[LazyOrm::LIMIT] = "11,11";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`hair`,`name` FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY `group1`,`group2` ORDER BY `num1`,`num2` LIMIT 11,11 ;
```

### UPDATE - Modify Records

#### Simple UPDATE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::UPDATE] = "users";
lazyOrm["name"] = "John Doe";
lazyOrm["email"] = "newemail@example.com";
lazyOrm["age"] = 31;

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: UPDATE users SET `age`='31',`email`='newemail@example.com',`name`='John Doe' ;
```

#### UPDATE with WHERE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::UPDATE] = "users";
lazyOrm["name"] = "John Doe";
lazyOrm[LazyOrm::WHERE] = {{"id", "=", "1"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: UPDATE users SET `name`='John Doe' WHERE (`id` = '1') ;
```

#### UPDATE with Multiple Conditions

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::UPDATE] = "users";
lazyOrm["name"] = "John Doe";
lazyOrm["email"] = "newemail@example.com";
lazyOrm[LazyOrm::WHERE] = {
    {"id", "=", "1"},
    {"AND"},
    {"active", true}
};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

#### UPDATE with ORDER BY and LIMIT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::UPDATE] = "student";
lazyOrm["name"] = "anya";
lazyOrm["age"] = 6;
lazyOrm["hair"] = "pink";

lazyOrm[LazyOrm::WHERE] = {{"grade", "in", LazyOrm::DbList({1, 5, 7, 9})}};
lazyOrm[LazyOrm::GROUPBY] = {{"group1", "group2"}};
lazyOrm[LazyOrm::ORDERBY] = {{"num1", "num2"}};
lazyOrm[LazyOrm::LIMIT] = "11,11";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: UPDATE student SET `age`='6',`hair`='pink',`name`='anya' WHERE (`grade` in (1,5,7,9)) GROUP BY `group1`,`group2` ORDER BY `num1`,`num2` LIMIT 11,11 ;
```

### DELETE - Remove Records

#### Simple DELETE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::DELETE] = "users";
lazyOrm[LazyOrm::WHERE] = {{"id", "=", "1"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: DELETE FROM users WHERE (`id` = '1') ;
```

#### DELETE with Multiple Conditions

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::DELETE] = "users";
lazyOrm[LazyOrm::WHERE] = {
    {"active", false},
    {"AND"},
    {"created_at", "<", "2023-01-01"}
};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

#### DELETE with ORDER BY and LIMIT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::DELETE] = "student";
lazyOrm[LazyOrm::WHERE] = {{"grade", "in", LazyOrm::DbList({1, 5, 7, 9})}};
lazyOrm[LazyOrm::GROUPBY] = {{"group1", "group2"}};
lazyOrm[LazyOrm::ORDERBY] = {{"num1", "num2"}};
lazyOrm[LazyOrm::LIMIT] = "11,11";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: DELETE FROM student WHERE (`grade` in (1,5,7,9)) GROUP BY `group1`,`group2` ORDER BY `num1`,`num2` LIMIT 11,11 ;
```

## Bulk Operations

### Bulk INSERT

#### Bulk INSERT with Initializer List

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::BULK_INSERT] = "student";
lazyOrm.setProperties({
    {{"name", "anya"}, {"family", "forger"}, {"age", 6}, {"hair", "pink"}, {"cute", true}},
    {{"name", "damian"}, {"family", "Desmond"}, {"age", 8}, {"hair", "black"}, {"cute", false}},
    {{"name", "loid"}, {"family", "forger"}, {"age", 30}, {"hair", "blond"}, {"cute", false}}
});

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: INSERT INTO student (...) VALUES (...),(...),(...) ;
```

#### Bulk INSERT with List

```cpp
std::list<std::map<LazyOrm::DbVariant, LazyOrm::DbVariant>> batchProperties;
batchProperties.push_back({{"name", "anya"}, {"family", "forger"}, {"age", 6}});
batchProperties.push_back({{"name", "damian"}, {"family", "Desmond"}, {"age", 8}});

LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::BULK_INSERT] = "student";
lazyOrm.setProperties(batchProperties);

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

### Bulk UPDATE

Bulk UPDATE applies a batch of row updates to a table in a single generated
statement sequence. Rows are staged in a temporary table that is joined back
to the target table by the configured primary key.

**Required configuration**

- `LazyOrm::BULK_UPDATE` selects the target table (supports `table` and
  `schema.table` identifiers; malformed identifiers are rejected).
- `LazyOrm::PrimaryKey` must name the column that identifies the target row
  of each batch row. Without it (or with an invalid identifier) no query is
  generated.
- `setProperties()` supplies the batch: every row is a
  `std::map<DbVariant, DbVariant>` from column name to value.

**Contract**

- Every batch row must contain the primary-key column; the primary key
  selects the target row and is never part of the `SET` assignments.
- Every batch column name must be a valid `[A-Za-z_][A-Za-z0-9_]*`
  identifier; raw-expression markers (`[no']`, `[no`]`, `[func]`,
  `[update]`) and SQL fragments are rejected.
- `std::monostate{}` represents SQL `NULL` in both regular and parameterized
  Bulk UPDATE; ordinary strings such as `"NULL"` remain string values
  (`'NULL'` in regular SQL and bound `"NULL"` in parameterized SQL).
- Raw-expression markers (`[no']`, `[no`]`) in batch data values or Bulk
  UPDATE `WHERE` filters are rejected so values cannot bypass parameter
  binding.
- NULL primary-key values (`std::monostate` or `"NULL"`) are rejected.
- All rows must have exactly the same column set; values are matched to
  columns by name, not by position in the source container.
- The remaining columns are the columns updated, in one `SET` clause.
- Primary-key values in a batch are validated on the C++ side across five
  `DbVariant` categories (`Integer` [signed/unsigned], `Float`, `String`,
  `Boolean`, or `Blob`), without database schema introspection or
  engine-specific type normalization:
  - Duplicate primary keys that are equivalent under the library's C++-side
    type-aware comparison are rejected.
  - Values with different C++ types may be treated as distinct or rejected
    according to the existing normalization and validation rules: signed and
    unsigned integers share the `Integer` category and are compared by
    numeric value (so `int(1)`, `long long(1)`, and `unsigned int(1)` are
    treated as duplicates, whereas `int(1)` and `unsigned int(2)` are
    distinct), while mixing different categories in one batch (such as
    `int(1)` with `string("1")`, or `int(1)` with `double(1.0)`) is
    conservatively rejected.
  - Distinct values of the same C++ type may still become equivalent after
    database type conversion. For a numeric database primary-key column,
    values such as `1`, `"1"`, and `"01"` may be converted by the database
    into equivalent numeric values depending on the database engine and
    column type: although mixed `1` and `"1"` are rejected by the category
    check, distinct strings such as `"1"` and `"01"` pass C++-side
    validation and can still collapse into the same key in the database.
- `LazyOrm::WHERE` filters which target rows are updated; it never restricts
  the staging-table insert.
- When the input violates the contract (empty batch, empty row, missing or
  unconfigured primary key, NULL or mixed-type primary key, inconsistent
  column sets, duplicate primary keys under the C++-side rules above,
  primary-key-only rows, invalid column/table identifiers, or disallowed
  raw-expression markers), `queryString()` and `buildParameterized()` return
  an empty query instead of generating a partial or misleading statement.

**Duplicate primary-key limitations and caller responsibility**: C++-side
duplicate detection is an early validation check, not a replacement for
database constraints, and does not guarantee detection of all database-level
equivalences caused by implicit type conversion. Conversion behavior depends on
the database engine, target column type, and staging-table schema (for
example, MariaDB and PostgreSQL create staging tables using `LIKE`, while
SQLite creates staging columns without declared types). SQLite may still apply
its own type-affinity and comparison rules. Therefore, distinct C++ primary-key
values may become equivalent in the staging table or join condition, depending
on the database's conversion and comparison semantics, causing `UPDATE ... JOIN`
/ `UPDATE ... FROM` / correlated subqueries to behave nondeterministically. The
caller must supply primary-key values that remain unique after conversion to
the target database column's type.

**Generated statement sequence** (same staging-table name in every
statement):

1. `DROP` the staging table if it exists in the temporary namespace
   (`pg_temp.` in PostgreSQL, `temp.` in SQLite, `DROP TEMPORARY TABLE` in
   MariaDB) for safe re-runs on one connection.
2. `CREATE` a temporary staging table shaped like the batch.
3. `INSERT` all batch rows into the staging table.
4. `UPDATE` the target table from the staging table, matched by primary
   key, with the configured `WHERE` filter applied to target rows.
5. `DROP` the staging table again for cleanup in the temporary namespace.

**Transaction and atomicity expectations**: the library generates SQL text
only; it does not wrap the sequence in `BEGIN`/`COMMIT`, so a caller-managed
transaction (for example `LazyOrm::Transaction`) is not interfered with. All
statements must run on the same connection, because temporary tables are
per-connection. Without an enclosing transaction the sequence is not
atomic: if an intermediate statement fails, the temporary staging table
remains in the current session/connection (and earlier target updates remain
committed if not in a transaction). The next Bulk UPDATE on the same
connection for the same target table recovers cleanly because step 1
(`DROP ... IF EXISTS` in the temporary namespace) removes any leftover
staging table before recreating it.

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::PrimaryKey] = "id";          // required
lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
lazyOrm.setProperties({
    {{"id", "1"}, {"name", "anya"}, {"family", "forger"}, {"age", 6}},
    {{"id", "2"}, {"name", "damian"}, {"family", "Desmond"}, {"age", 8}},
    {{"id", "3"}, {"name", "loid"}, {"family", "forger"}, {"age", 30}}
});
lazyOrm[LazyOrm::WHERE] = {{"active", true}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output:
// DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`;
// CREATE TEMPORARY TABLE `lazyormtemp_fruits` LIKE `fruits`;
// INSERT INTO `lazyormtemp_fruits` (`age`,`family`,`id`,`name`) VALUES ('6','forger','1','anya'),('8','Desmond','2','damian'),('30','forger','3','loid') ;
// UPDATE `fruits` t JOIN `lazyormtemp_fruits` u ON t.`id` = u.`id` SET t.`age` = u.`age`,t.`family` = u.`family`,t.`name` = u.`name` WHERE (t.`active` = true) ;
// DROP TEMPORARY TABLE IF EXISTS `lazyormtemp_fruits`;
```

#### Dialect-specific behavior

- **MariaDB** uses `DROP TEMPORARY TABLE IF EXISTS`, `CREATE TEMPORARY TABLE ... LIKE`,
  inserts the batch into the staging table, and updates with
  `UPDATE ... JOIN ... ON t.pk = u.pk`. The `WHERE` filter is qualified with
  the target alias `t` because the staging table has the same columns as
  the target.
- **PostgreSQL** uses `DROP TABLE IF EXISTS pg_temp."..."`,
  `CREATE TEMPORARY TABLE ... (LIKE ...)`, and updates with
  `UPDATE ... SET ... FROM ... WHERE t.pk = u.pk AND (<filter>)`.
- **SQLite** uses `DROP TABLE IF EXISTS temp.`...``, has no `UPDATE ... JOIN`,
  and `UPDATE ... FROM` requires SQLite 3.33 or newer, so each updated column
  gets a correlated subquery (`SET col = (SELECT u.col FROM staging u WHERE u.pk = table.pk)`),
  which works on every SQLite 3.x version. An outer `WHERE EXISTS (SELECT 1 FROM staging u WHERE u.pk = table.pk)`
  guard (with any user `WHERE` filter appended via `AND (...)`) ensures that
  target rows absent from the batch are never overwritten with `NULL`.

#### Parameterized Bulk UPDATE

`buildParameterized()` generates the same statement sequence with every
batch value and every `WHERE` value bound as a parameter — no user data is
interpolated into the SQL text. Batch parameters are ordered row by row,
column by column, and the `WHERE` parameters follow all batch parameters.
PostgreSQL placeholders (`$1`, `$2`, ...) are numbered across all statements;
MariaDB and SQLite use `?` placeholders in binding order.

```cpp
LazyOrm::PostgreLazy lazyOrm;
lazyOrm[LazyOrm::PrimaryKey] = "id";
lazyOrm[LazyOrm::BULK_UPDATE] = "users";
lazyOrm.setProperties({
    {{"id", 1}, {"name", "Alice"}, {"age", 25}},
    {{"id", 2}, {"name", "Bob"}, {"age", 30}}
});
lazyOrm[LazyOrm::WHERE] = {{"active", true}};

const auto query = lazyOrm.buildParameterized();
// query.sql:
// DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ;
// CREATE TEMPORARY TABLE "lazyormtemp_users" (LIKE "users") ;
// INSERT INTO "lazyormtemp_users" ("age","id","name") VALUES ($1,$2,$3),($4,$5,$6) ;
// UPDATE "users" t SET "age" = u."age","name" = u."name" FROM "lazyormtemp_users" u WHERE t."id" = u."id" AND (t."active" = $7) ;
// DROP TABLE IF EXISTS pg_temp."lazyormtemp_users" ;
// query.params: 25, 1, "Alice", 30, 2, "Bob", true
```

Note: the generated parameterized text contains several statements.
Drivers such as `sqlite3_prepare`, MySQL prepared statements, or PostgreSQL
extended-protocol `Parse` accept a single statement per prepare/bind cycle,
so the consumer must split the sequence and bind the parameters that belong
to each statement in order.

## Transactions

### Basic Transaction

```cpp
#include "Transaction.h"

LazyOrm::MariadbLazy insert;
insert[LazyOrm::INSERT] = "users";
insert["name"] = "John";
insert["email"] = "john@example.com";

LazyOrm::MariadbLazy update;
update[LazyOrm::UPDATE] = "logs";
update["action"] = "user_created";

LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({insert, update});

std::cout << transaction.query_with_trim_consecutive_spaces() << std::endl;
// Output: START TRANSACTION; INSERT ... ; UPDATE ... ; COMMIT;
```

### Transaction with Append

```cpp
LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction;

LazyOrm::MariadbLazy q1, q2, q3;
q1[LazyOrm::INSERT] = "users";
q1["name"] = "John";

q2[LazyOrm::INSERT] = "users";
q2["name"] = "Jane";

q3[LazyOrm::INSERT] = "users";
q3["name"] = "Bob";

transaction.append(q1);
transaction.append(q2);
transaction.append(q3);

std::cout << transaction.query_with_trim_consecutive_spaces() << std::endl;
```

## Complex WHERE Clauses

### Nested AND/OR Conditions

```cpp
LazyOrm::SqliteLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "student";
lazyOrm << "name" << "age" << "hair";

lazyOrm[LazyOrm::WHERE] = {
    {"field1", "NULL"},
    {"OR"},
    {"field1", ""},
    {"OR"},
    {"field1", "IS", "NULL"},
    {"AND"},
    {"field1", "BETWEEN", "1111", "AND", "2222"}
};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`hair`,`name` FROM student WHERE ((`field1` = NULL) OR (`field1` = '') OR (`field1` IS NULL) AND (`field1` BETWEEN '1111' AND '2222')) ;
```

### Complex Nested Conditions

```cpp
LazyOrm::WhereFilter whereFilter = {
    {
        {"name", "like", "asqar"},
        {"OR"},
        {"name", "like", "mamad"}
    },
    {"AND"},
    {"age", ">=", "18"}
};

LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "age";
lazyOrm[LazyOrm::WHERE] = whereFilter;

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `age`,`name` FROM users WHERE (((`name` like 'asqar') OR (`name` like 'mamad')) AND (`age` >= '18')) ;
```

## Raw Queries

### Raw Query with Parameters

```cpp
LazyOrm::LazyOrm lazyOrm;
lazyOrm[LazyOrm::RAW_QUERY] = "INSERT INTO student (`age`,`hair`,`name`) VALUES (?,?,?) returning idx;";
lazyOrm << "56" << "black" << "jack";

std::cout << lazyOrm.queryString(LazyOrm::LazyOrm::MariaDB) << std::endl;
// Output: INSERT INTO student (`age`,`hair`,`name`) VALUES ('56','black','jack') returning idx;
```

### Raw Query Contract

#### Property and Parameter Resolution

Parameters are supplied through the property map, in one of two forms:

- **Key-only shorthand** (`lazyOrm << "value"` or
  `setProperties(std::vector<std::string>)`): the string is stored as the
  property *key* in `mProperties` (with an empty value for compatibility with
  `properties()` and `SELECT`), and tracked internally as a shorthand entry so
  the key itself becomes the parameter.
- **Key/value form** (`setProperties({{"label", value}})`,
  `setProperty("label", value)`, `lazyOrm["label"] = value`, or
  `lazyOrm << std::pair{...}`): the property *value* is the parameter and the
  key is only a label.

An explicitly supplied empty-string value (such as `{{"name", ""}}` or
`lazyOrm["name"] = ""`) is **not** treated as shorthand; it resolves to the
empty string `""`. Assigning or inserting a key via a key/value method clears
any prior shorthand status for that key, and `lazyOrm << "key"` marks that key
as shorthand.

Parameters are ordered by ascending property key (`std::map` order,
lexicographic for string keys) — **not** by insertion order. Choose labels
accordingly when order matters:

```cpp
LazyOrm::PostgreLazy lazyOrm;
lazyOrm[LazyOrm::RAW_QUERY] = "INSERT INTO users (id,name) VALUES ($1,$2)";
lazyOrm.setProperties({{"name", "raw"}, {"id", 12}});
// key order: "id" < "name", so $1 <- 12 and $2 <- "raw"
```

#### Regular Query Rendering (`queryString()`)

`queryString()` substitutes resolved parameter values directly into the SQL
text:

- **Quoting and wildcard/`[no']` rules**: Ordinary values are formatted with
  `DbVariant::setQuote()` (strings are single-quoted and escaped, booleans
  render as `true`/`false`, `"null"` / `std::monostate{}` render as `NULL`).
  An ordinary keyed parameter whose value is `"*"` (e.g. `{{"col", "*"}}`)
  is quoted as `'*'`. Unquoted wildcard/raw SQL rendering is preserved only for
  intentional shorthand `"*"` (`lazyOrm << "*"` / `setProperties({"*"})`) or
  explicit `"[no']"` / `"[no`]"` prefixes handled by `DbVariant::setQuote()`.
- **Dialect placeholder conventions**:
  - **PostgreSQL** (`PostgreLazy` / `LazyOrm::Postgres`): uses numbered `$N`
    placeholders only (`1`-based). `?`, `?|`, and `?&` are treated as
    PostgreSQL JSONB operators, not placeholders.
  - **MariaDB and SQLite**: accept positional `?` placeholders or numbered
    `$N` placeholders, but not mixed together in the same query.
- **Numbered `$N` validation**: Every index from `1` through `N` (where `N` is
  the supplied parameter count) must appear at least once. Repeated
  (`$1, $1, $2`) and out-of-order (`$2, $1`) references are valid, while gaps
  (`$2` with 2 params, `$1, $3` with 3 params), missing parameters, and extra
  unused parameters are rejected.
- **Rejection behavior**: Any contract violation or malformed placeholder
  (`$0`, `$1abc`, bare `$`, `$abc`, numeric overflow, or non-PostgreSQL
  dollar-quoting) causes `queryString()` to return an empty string.

#### Parameterized Query Building (`buildParameterized()`)

`buildParameterized()` passes the SQL text through unchanged and returns bound
parameters in `params` (in ascending property key order):

- **PostgreSQL**: uses numbered `$N` placeholders only. Each `$N` binds
  `params[N - 1]`. Repeated references (`$1, $1`) reuse `params[0]` without
  duplicating it; out-of-order references (`$2, $1`) keep `params` in property
  key order; every index `1..N` must be referenced at least once (no gaps,
  missing parameters, or extra unused parameters). PostgreSQL JSONB operators
  `#>`, `#>>`, `?`, `?|`, and `?&` are preserved untouched and never mistaken
  for comments or placeholders.
- **MariaDB and SQLite**: use positional `?` placeholders only, bound in
  parameter order (`params.size()` must equal the number of `?` placeholders).
  Numbered `$N` placeholders (`$1`, `$1, $2`), mixed `$N`/`?` queries, and
  `$tag$` dollar-quoted syntax are **rejected**.
- **Bound values**: Values are never interpolated into `sql`; native types
  (`int`, `double`, `bool`, `std::monostate`, `BlobType`, `std::string`
  including `""` and `"*"`) are preserved in `params`.
- **Rejection behavior**: Rejected queries return empty `sql` and empty
  `params`.

#### Dialect-Aware SQL Scanning and Limitations

`RawQuery::scan()` skips non-placeholder SQL constructs according to the active
dialect:

- **PostgreSQL**:
  - Single-quoted strings `'...'` use `''` doubling only (`standard_conforming_strings = on`);
    backslash escapes apply only inside `E'...'` / `e'...'` escape strings.
  - Double-quoted identifiers `"..."` use `""` doubling only.
  - Dollar-quoted strings (`$$...$$`, `$tag$...$tag$`) are skipped.
  - Comments: `--` to end of line (no trailing whitespace required) and nested
    `/* ... */` block comments. `#` is not a comment (`#>`, `#>>` stay valid).
  - Unquoted identifiers containing `$` (e.g. `col$1`) are skipped as whole
    identifiers.
- **MariaDB**:
  - Single-quoted `'...'` and double-quoted `"..."` strings honor both
    backslash escapes and doubled quotes; backticks `` `...` `` use `` `` ``
    doubling only.
  - Comments: `#` to end of line, `--` only when followed by whitespace, a
    control character, or EOF (so `1--2` or `1--?` is not treated as a comment),
    and non-nested `/* ... */` block comments.
- **SQLite**:
  - Single-quoted `'...'`, double-quoted `"..."`, backtick `` `...` ``, and
    bracketed `[...]` tokens use quote doubling (`''`, `""`, `` `` ``) with no
    backslash escapes.
  - Comments: `--` to end of line (no trailing whitespace required) and
    non-nested `/* ... */` block comments (`#` is not a comment).

**Scanner limitations** (inherent to stateless scanning without a live server
connection):

- PostgreSQL scanning assumes `standard_conforming_strings = on` (the default
  since PostgreSQL 9.1); legacy `standard_conforming_strings = off` is not
  detected.
- MariaDB/MySQL scanning assumes default SQL mode (backslash escapes enabled in
  `'...'` and `"..."`); server modes `NO_BACKSLASH_ESCAPES` and `ANSI_QUOTES`
  cannot be queried without a connection. MariaDB versioned executable comments
  (`/*! ... */`) are treated as block comments.
- In PostgreSQL, `?` is treated solely as an operator rather than a placeholder;
  a standalone stray `?` with zero parameters cannot be distinguished from
  operator syntax without schema/type information.
- In SQLite, only `?` positional placeholders are supported by LazyOrm;
  SQLite-native `?NNN`, `:name`, and `@name` host parameter syntaxes are not
  recognized as LazyOrm placeholders (while `$`-prefixed tokens such as `$1`
  and `$name` are rejected).
- Unterminated quotes, comments, or dollar-quoted strings swallow through the
  end of the SQL string.

### Raw Query with Numbered Placeholders

```cpp
LazyOrm::PostgreLazy lazyOrm;
lazyOrm[LazyOrm::RAW_QUERY] = "SELECT $2, $1";
lazyOrm << "a" << "b";

const auto parameterized = lazyOrm.buildParameterized();
// parameterized.sql    == "SELECT $2, $1"
// parameterized.params == ["a", "b"]  ($1 -> "a", $2 -> "b")
```

## Multi-Database Support

### Using LazyOrm for Multiple Databases

```cpp
LazyOrm::LazyOrm lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm["name"] = "John";
lazyOrm["email"] = "john@example.com";

// Generate for different databases
std::string mariaQuery = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::MariaDB);
std::string pgQuery = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::Postgres);
std::string sqliteQuery = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::Sqlite3);
```

## Aggregation and Grouping

### COUNT with GROUP BY

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "department";
lazyOrm[LazyOrm::COUNT_PROPS] = {"*", "COUNT(*) as total"};
lazyOrm[LazyOrm::GROUPBY] = {{"department"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `department`,COUNT(*),COUNT(*) as total FROM users GROUP BY `department`;
```

### COUNT with HAVING

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "department";
lazyOrm[LazyOrm::COUNT_PROPS] = {"COUNT(*)"};
lazyOrm[LazyOrm::GROUPBY] = {{"department"}};

LazyOrm::HavingFilter having = {{"COUNT(*)", ">", "2"}};
lazyOrm[LazyOrm::Having::HAVING] = having;

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `department`,COUNT(*) FROM users GROUP BY `department` HAVING ((`COUNT(*)` > '2')) ;
```

## Database-Specific Examples

### PostgreSQL

```cpp
LazyOrm::PostgreLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm["name"] = "John";
lazyOrm["email"] = "john@example.com";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Uses PostgreSQL-specific syntax (double quotes for identifiers, different LIMIT format)
```

### SQLite

```cpp
LazyOrm::SqliteLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::LIMIT] = {{"10", "20"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users LIMIT 10 OFFSET 20;
```

### MariaDB/MySQL

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::LIMIT] = "10,20";

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
// Output: SELECT `email`,`name` FROM users LIMIT 10,20 ;
```

## Common Patterns

### Pagination

```cpp
// MariaDB/MySQL
int page = 2;
int pageSize = 10;
int offset = (page - 1) * pageSize;

LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::ORDERBY_DESC] = {{"created_at"}};
lazyOrm[LazyOrm::LIMIT] = std::to_string(offset) + "," + std::to_string(pageSize);
```

```cpp
// SQLite/PostgreSQL
int page = 2;
int pageSize = 10;
int offset = (page - 1) * pageSize;

LazyOrm::SqliteLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::ORDERBY_DESC] = {{"created_at"}};
lazyOrm[LazyOrm::LIMIT] = {{std::to_string(pageSize), std::to_string(offset)}};
```

### Search with Multiple Conditions

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";

// Search by name or email, and age range
lazyOrm[LazyOrm::WHERE] = {
    {
        {"name", "like", "%John%"},
        {"OR"},
        {"email", "like", "%John%"}
    },
    {"AND"},
    {"age", "between", "18,65"}
};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

### Soft Delete Pattern

```cpp
// Include deleted records
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email";
lazyOrm[LazyOrm::WHERE] = {{"deleted_at", "NULL"}};

// Only active records
lazyOrm[LazyOrm::WHERE] = {
    {"deleted_at", "NULL"},
    {"AND"},
    {"active", true}
};
```

### Audit Log with Transaction

```cpp
LazyOrm::MariadbLazy updateUser;
updateUser[LazyOrm::UPDATE] = "users";
updateUser["name"] = "New Name";
updateUser[LazyOrm::WHERE] = {{"id", "=", "1"}};

LazyOrm::MariadbLazy logAction;
logAction[LazyOrm::INSERT] = "audit_log";
logAction["user_id"] = "1";
logAction["action"] = "update";
logAction["old_name"] = "Old Name";
logAction["new_name"] = "New Name";

LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({updateUser, logAction});
std::cout << transaction.query_with_trim_consecutive_spaces() << std::endl;
```