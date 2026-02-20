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

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::BULK_UPDATE] = "fruits";
lazyOrm.setProperties({
    {{"id", "1"}, {"name", "anya"}, {"family", "forger"}, {"age", 6}},
    {{"id", "2"}, {"name", "damian"}, {"family", "Desmond"}, {"age", 8}},
    {{"id", "3"}, {"name", "loid"}, {"family", "forger"}, {"age", 30}}
});
lazyOrm[LazyOrm::WHERE] = {{"userid", "20"}};

std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
```

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