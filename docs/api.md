# API Reference

Complete API reference for LazyOrm-CPP.

## Overview

LazyOrm-CPP provides a fluent, type-safe API for building SQL queries. The library follows a lazy evaluation model where queries are built incrementally and only generated when requested.

### Core Classes

| Class | Description |
|-------|-------------|
| `AbstractLazy` | Base abstract class providing common query building functionality |
| `LazyOrm` | Generic ORM class supporting multiple database backends |
| `MariadbLazy` | MariaDB/MySQL specific implementation |
| `PostgreLazy` | PostgreSQL specific implementation |
| `SqliteLazy` | SQLite specific implementation |
| `FilteringLazy` | Filtering operations for generic queries |
| `Transaction` | Transaction support for batching queries |
| `DbVariant` | Variant type supporting multiple data types |
| `FilterVariant` | Variant for filter conditions |
| `WhereFilter` | WHERE clause builder |
| `HavingFilter` | HAVING clause builder |
| `Result` | Result container for query results |
| `ResultRow` | Single row result container |
| `DbList` | List variant for IN clauses |

---

## AbstractLazy

Base class providing common query building functionality.

### Methods

#### Table and Query Type

```cpp
void setTabeName(const std::string &name);
```
Sets the table name for the query.

**Example:**
```cpp
LazyOrm::AbstractLazy *lazy = &mariadbLazy;
lazy->setTabeName("users");
```

---

```cpp
void setQueryType(LazyOrm::Query queryType);
```
Sets the query type (INSERT, SELECT, UPDATE, DELETE, etc.).

**Example:**
```cpp
lazy->setQueryType(LazyOrm::Query::SELECT);
```

---

```cpp
std::string & operator[](const LazyOrm::Query &queryType);
```
Sets both query type and table name using operator syntax.

**Example:**
```cpp
lazy->operator[](LazyOrm::SELECT) = "users";
```

---

#### Properties

```cpp
void setProperty(const std::string key, const DbVariant value);
```
Sets a single property (column name and value).

**Example:**
```cpp
lazy->setProperty("name", "John");
lazy->setProperty("age", 30);
```

---

```cpp
DbVariant & operator[](const std::string &key);
```
Get or set a property using operator syntax.

**Example:**
```cpp
lazy->operator[]("name") = "John";
DbVariant name = lazy->operator[]("name");
```

---

```cpp
void setProperties(const std::vector<std::string> &keys);
```
Sets SELECT columns as a vector.

**Example:**
```cpp
lazy->setProperties({"name", "age", "email"});
```

---

```cpp
AbstractLazy & operator<<(const std::string &key);
```
Stream operator for setting SELECT columns.

**Example:**
```cpp
lazy->operator<<("name") << "age" << "email";
```

---

```cpp
void setProperties(const std::initializer_list<std::pair<DbVariant,DbVariant>> items);
```
Sets multiple properties as key-value pairs.

**Example:**
```cpp
lazy->setProperties({
    {"name", "John"},
    {"age", 30},
    {"email", "john@example.com"}
});
```

---

```cpp
AbstractLazy & operator<<(const std::pair<DbVariant,DbVariant> &key_value);
```
Stream operator for setting properties.

**Example:**
```cpp
lazy->operator<<(LazyOrm::pair{"name", "John"})
      << LazyOrm::pair{"age", 30};
```

---

```cpp
void setProperties(const std::initializer_list<std::map<DbVariant, DbVariant>> list);
void setProperties(const std::list<std::map<DbVariant, LazyOrm::DbVariant>> &list);
```
Sets bulk properties for bulk operations.

**Example:**
```cpp
lazy->setProperties({
    {{"id", "1"}, {"name", "John"}},
    {{"id", "2"}, {"name", "Jane"}}
});
```

---

#### Filters

```cpp
FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
```
Access filter by type.

**Example:**
```cpp
auto& orderBy = lazy->operator[](LazyOrm::ORDERBY);
```

---

```cpp
WhereFilter& operator[](const LazyOrm::NestedWhere &nestedWhere);
```
Access WHERE filter.

**Example:**
```cpp
auto& where = lazy->operator[](LazyOrm::WHERE);
where = {{"age", ">", "18"}};
```

---

```cpp
void setNestedWhere(LazyOrm::WhereFilter nestedWhere);
```
Sets nested WHERE conditions.

**Example:**
```cpp
LazyOrm::WhereFilter wf = {{"age", ">", "18"}};
lazy->setNestedWhere(wf);
```

---

```cpp
HavingFilter& operator[](const LazyOrm::Having &nestedHaving);
```
Access HAVING filter.

**Example:**
```cpp
auto& having = lazy->operator[](LazyOrm::Having::HAVING);
having = {{"COUNT(*)", ">", "2"}};
```

---

```cpp
void setNestedHaving(LazyOrm::HavingFilter nestedHaving);
```
Sets nested HAVING conditions.

**Example:**
```cpp
LazyOrm::HavingFilter hf = {{"COUNT(*)", ">", "2"}};
lazy->setNestedHaving(hf);
```

---

#### Primary Key

```cpp
void setPrimaryKey(const std::string &primaryKey);
```
Sets the primary key column name.

**Example:**
```cpp
lazy->setPrimaryKey("user_id");
```

---

```cpp
std::string & operator[](const LazyOrm::Primary_Key &primaryKey);
```
Sets primary key using operator syntax.

**Example:**
```cpp
lazy->operator[](LazyOrm::PrimaryKey) = "id";
```

---

#### Count

```cpp
void setCountType(std::initializer_list<DbVariant> countFields);
```
Sets COUNT fields for aggregation.

**Example:**
```cpp
lazy->setCountType({"*", "name", "age as age_group"});
```

---

```cpp
std::list<DbVariant> & operator[](const LazyOrm::Count &count);
```
Access COUNT fields.

**Example:**
```cpp
auto& counts = lazy->operator[](LazyOrm::COUNT_PROPS);
counts = {"*", "name"};
```

---

#### Distinct

```cpp
void enableDistinctSelect(bool distinct=true);
```
Enables or disables DISTINCT in SELECT queries.

**Example:**
```cpp
lazy->enableDistinctSelect(true);
```

---

#### Getters

```cpp
std::string tabeName() const;
```
Gets the table name.

---

```cpp
std::map<DbVariant, DbVariant> properties() const;
```
Gets all properties.

---

```cpp
std::list<std::map<DbVariant, DbVariant>> batchProperties() const;
```
Gets batch properties for bulk operations.

---

```cpp
WhereFilter whereFilter() const;
```
Gets WHERE filter.

---

```cpp
HavingFilter havingFilter() const;
```
Gets HAVING filter.

---

```cpp
Query queryType() const;
```
Gets query type.

---

```cpp
virtual const FilteringAbstractLazy& getFilter() const = 0;
```
Gets filter object (pure virtual).

---

```cpp
std::string primaryKey() const;
```
Gets primary key.

---

#### Query Generation

```cpp
virtual std::string queryString() const;
```
Generates the SQL query string.

**Example:**
```cpp
std::string sql = lazy->queryString();
```

---

```cpp
static void trim_consecutive_spaces(std::string& str);
```
Static method to trim consecutive spaces.

**Example:**
```cpp
std::string sql = "SELECT  *  FROM  users";
LazyOrm::AbstractLazy::trim_consecutive_spaces(sql);
// sql becomes "SELECT * FROM users"
```

---

```cpp
virtual std::string query_with_trim_consecutive_spaces();
```
Generates query with trimmed spaces.

**Example:**
```cpp
std::string sql = lazy->query_with_trim_consecutive_spaces();
```

---

## LazyOrm

Generic ORM class supporting multiple database backends.

### Methods

#### Query Generation

```cpp
std::string queryString(const DBMS_TYPE &dbms) const;
```
Generates query for specific database type.

**Example:**
```cpp
LazyOrm::LazyOrm lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
lazyOrm["name"] = "John";

// Generate for different databases
std::string mariaSql = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::MariaDB);
std::string pgSql = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::Postgres);
std::string sqliteSql = lazyOrm.queryString(LazyOrm::LazyOrm::DBMS_TYPE::Sqlite3);
```

---

#### Copy Operations

```cpp
LazyOrm(const AbstractLazy& abstractLaz);
void operator=(const AbstractLazy& abstractLaz);
```
Copy constructor and assignment operator.

**Example:**
```cpp
LazyOrm::MariadbLazy mariaLazy;
mariaLazy[LazyOrm::INSERT] = "users";

LazyOrm::LazyOrm lazyOrm(mariaLazy);
```

---

### Types

#### DBMS_TYPE

```cpp
enum DBMS_TYPE {
    Postgres = 0,
    MariaDB,
    Sqlite3
};
```

Supported database management systems.

---

## Database-Specific Classes

### MariadbLazy

MariaDB/MySQL specific implementation.

#### Constructor

```cpp
MariadbLazy();
MariadbLazy(const std::string &table, const Query &queryType);
```

**Example:**
```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";

// or
LazyOrm::MariadbLazy lazyOrm("users", LazyOrm::INSERT);
```

---

### PostgreLazy

PostgreSQL specific implementation.

#### Constructor

```cpp
PostgreLazy();
PostgreLazy(const std::string &table, const Query &queryType);
```

**Example:**
```cpp
LazyOrm::PostgreLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
```

---

### SqliteLazy

SQLite specific implementation.

#### Constructor

```cpp
SqliteLazy();
SqliteLazy(const std::string &table, const Query &queryType);
```

**Example:**
```cpp
LazyOrm::SqliteLazy lazyOrm;
lazyOrm[LazyOrm::INSERT] = "users";
```

---

## FilteringAbstractLazy

Base class for filtering operations.

### Methods

#### Set Filters

```cpp
void setFilterForReserved(const LazyOrm::FilterVariant &variant);
void setFilterForReserved(const std::vector<LazyOrm::FilterVariant> &variantList);
void setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
void setFilter(const Filters &filter, LazyOrm::FilterVariant filterVariant);
```

**Example:**
```cpp
LazyOrm::MariadbFilteringLazy filters;
filters.setFilter(LazyOrm::ORDERBY, {{"name", "age"}});
filters.setFilter(LazyOrm::LIMIT, "10,10");
```

---

#### Operator Overloads

```cpp
FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
void operator=(const LazyOrm::FilterVariant &variant);
```

**Example:**
```cpp
filters[LazyOrm::ORDERBY] = {{"name"}};
filters[LazyOrm::LIMIT] = "10";
```

---

#### Direction

```cpp
void setDESC(bool desc=true);
void setASC(bool asc=true);
```
Sets ORDER BY direction.

**Example:**
```cpp
filters.setDESC();  // DESC
filters.setASC();   // ASC
```

---

#### Getters

```cpp
Filters reservedFilter() const;
FilterVariant limitConditions() const;
FilterVariant orderConditions() const;
FilterVariant groupConditions() const;
```

---

#### Query String

```cpp
std::string filter_conditions() const;
std::string filter_conditions_with_trim_consecutive_spaces();
```

---

## FilteringLazy

Filtering operations for generic queries.

### Methods

```cpp
std::string orderbyString() const override;
std::string limitString() const override;
std::string groupString() const override;
```

Generate filter clauses for specific databases.

---

## WhereFilter

WHERE clause builder supporting nested conditions.

### Constructors

```cpp
WhereFilter();
WhereFilter(std::string str);
WhereFilter(DbVariant variant);
WhereFilter(std::initializer_list<WhereFilter> wfs);
WhereFilter(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> variants);
WhereFilter(std::vector<DbVariant> variants);
WhereFilter(std::vector<WhereFilter> wfs);
```

**Examples:**

```cpp
// Simple condition
LazyOrm::WhereFilter wf1 = {{"age", ">", "18"}};

// String condition
LazyOrm::WhereFilter wf2 = {"age > 18"};

// Nested conditions
LazyOrm::WhereFilter wf3 = {
    {
        {"name", "like", "John%"},
        {"OR"},
        {"name", "like", "Jane%"}
    },
    {"AND"},
    {"age", ">", "18"}
};
```

---

### Methods

#### Append

```cpp
void append(std::vector<DbVariant> variants);
void append(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> variants);
void append(std::vector<WhereFilter> wfs);
```

**Example:**
```cpp
LazyOrm::WhereFilter wf;
wf.append({"age", ">", "18"});
wf.append({"AND"});
wf.append({"name", "like", "John%"});
```

---

#### Query String

```cpp
std::string toString() const;
```

**Example:**
```cpp
LazyOrm::WhereFilter wf = {{"age", ">", "18"}};
std::string whereClause = wf.toString();
// Output: WHERE `age` > '18'
```

---

#### Getters

```cpp
bool empty() const;
const std::vector<WhereFilter>& nestedWhereFilters() const;
const std::vector<std::vector<DbVariant>>& nestedDbVariant() const;
```

---

## HavingFilter

HAVING clause builder (extends WhereFilter).

### Constructors

```cpp
HavingFilter();
HavingFilter(std::string str);
HavingFilter(DbVariant variant);
// Inherits all WhereFilter constructors
```

**Example:**
```cpp
LazyOrm::HavingFilter hf = {{"COUNT(*)", ">", "2"}};
```

---

## DbVariant

Variant type supporting multiple data types.

### Constructors

```cpp
DbVariant();  // monostate
DbVariant(std::string value);
DbVariant(const char* value);
DbVariant(unsigned short value);
DbVariant(unsigned int value);
DbVariant(unsigned long value);
DbVariant(unsigned long long value);
DbVariant(short value);
DbVariant(int value);
DbVariant(long value);
DbVariant(long long value);
DbVariant(float value);
DbVariant(double value);
DbVariant(long double value);
DbVariant(bool value);
```

**Example:**
```cpp
LazyOrm::DbVariant v1 = "Hello";
LazyOrm::DbVariant v2 = 42;
LazyOrm::DbVariant v3 = 3.14;
LazyOrm::DbVariant v4 = true;
```

---

### Methods

#### Conversion

```cpp
std::string typeName() const;
std::string toString() const;
unsigned long long toULongLong() const;
long long toLongLong() const;
long double toLongDouble() const;
bool toBool() const;
```

**Example:**
```cpp
LazyOrm::DbVariant v = "123";
std::string type = v.typeName();     // "string"
std::string str = v.toString();      // "123"
long long num = v.toLongLong();      // 123
```

---

#### String Operations

```cpp
std::string toLowerString() const;
std::string toUpperString() const;
bool empty();
const size_t lenght() const;
bool contains(std::string substr) const;
bool startsWith(std::string prefix, bool caseSensitive=true) const;
std::string trim() const;
```

**Example:**
```cpp
LazyOrm::DbVariant v = "Hello World";
std::string lower = v.toLowerString();  // "hello world"
std::string upper = v.toUpperString();  // "HELLO WORLD"
bool hasHello = v.contains("Hello");    // true
```

---

#### SQL String Generation

```cpp
std::string setQuote() const;
std::string setBackTick() const;
std::string toCleanString() const;
bool isUpdate() const;
```

**Example:**
```cpp
LazyOrm::DbVariant v = "John";
std::string quoted = v.setQuote();        // "'John'"
std::string backticked = v.setBackTick();  // "`John`"
```

---

#### Type Detection

```cpp
bool isNumeric() const;
DbVariant alterStringToBestMatchType();
DbVariant alterStringToNumber();
```

**Example:**
```cpp
LazyOrm::DbVariant v = "123";
bool numeric = v.isNumeric();  // true
LazyOrm::DbVariant num = v.alterStringToBestMatchType();  // Converts to number
```

---

#### Set

```cpp
DbVariant& set(DbVariant newVariant);
```

**Example:**
```cpp
LazyOrm::DbVariant v = "Hello";
v.set("World");  // Now v is "World"
```

---

## FilterVariant

Variant for filter conditions.

### Constructors

```cpp
FilterVariant();
FilterVariant(DbVariant v);
FilterVariant(std::vector<DbVariant> v);
```

**Example:**
```cpp
LazyOrm::FilterVariant f1 = "name";
LazyOrm::FilterVariant f2 = {{"name", "age", "email"}};
```

---

### Methods

```cpp
bool empty() const;
void append(DbVariant &dbVariant);
std::vector<DbVariant> toDbVariants() const;
```

---

## DbList

List variant for IN clauses.

### Constructors

```cpp
DbList();
DbList(std::initializer_list<DbVariant> input);
```

**Example:**
```cpp
LazyOrm::DbList list = {1, 2, 3, 4, 5};
```

---

### Methods

```cpp
void setList(std::vector<DbVariant> list);
```

---

## Transaction

Transaction support for batching queries.

### Template

```cpp
template <class DBMStype>
class Transaction : public AbstractLazy
```

### Constructors

```cpp
Transaction();
Transaction(std::initializer_list<DBMStype> queries);
```

**Example:**
```cpp
LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction;
// or
LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({q1, q2, q3});
```

---

### Methods

```cpp
void append(DBMStype query);
std::string queryString() const override;
std::string queryStringWithoutBegining() const;
virtual std::string query_with_trim_consecutive_spaces();
```

**Example:**
```cpp
LazyOrm::MariadbLazy q1, q2;
q1[LazyOrm::INSERT] = "users";
q2[LazyOrm::UPDATE] = "users";

LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction;
transaction.append(q1);
transaction.append(q2);

std::string sql = transaction.query_with_trim_consecutive_spaces();
// Output: START TRANSACTION; INSERT ... ; UPDATE ... ; COMMIT;
```

---

## Result

Result container for query results.

### Methods

#### Error Handling

```cpp
bool hasError() const;
void setError(std::string errorMessage);
std::string errorMessage() const;
```

**Example:**
```cpp
LazyOrm::Result result;
if (result.hasError()) {
    std::cout << result.errorMessage() << std::endl;
}
```

---

#### Metadata

```cpp
size_t affectedRows() const;
void setAffectedRows(size_t affectedRows);
unsigned long long insertId() const;
void setInsertId(unsigned long long insertId);
std::vector<std::string> columnNames();
std::string columnName(size_t index);
size_t columnsSize() const;
void setColumnNames(const std::vector<std::string> &columnNames);
```

**Example:**
```cpp
std::cout << "Affected rows: " << result.affectedRows() << std::endl;
std::cout << "Insert ID: " << result.insertId() << std::endl;
```

---

#### String Representation

```cpp
std::string toString() const;
std::string toIndentedString() const;
```

**Example:**
```cpp
std::cout << result.toString() << std::endl;
std::cout << result.toIndentedString() << std::endl;
```

---

#### Access

```cpp
ResultRow value(unsigned long long index);
```

**Example:**
```cpp
if (result.size() > 0) {
    LazyOrm::ResultRow row = result.value(0);
}
```

---

## ResultRow

Single row result container.

### Methods

#### Access

```cpp
DbVariant value(const std::string key, const DbVariant dbVariant={}) const;
DbVariant operator[](const std::string key) const;
DbVariant at(unsigned long long columnIndex) const;
DbVariant value(unsigned long long columnIndex) const;
```

**Examples:**

```cpp
LazyOrm::ResultRow row;

// By column name
DbVariant name = row.value("name", "unknown");
DbVariant email = row["email"];

// By column index
DbVariant firstCol = row.at(0);
DbVariant secondCol = row.value(1);
```

---

#### Insert

```cpp
void insert(const std::string key, const DbVariant dbVariant);
```

**Example:**
```cpp
LazyOrm::ResultRow row;
row.insert("name", "John");
row.insert("age", 30);
```

---

#### String Representation

```cpp
std::string toString() const;
std::string toIndentedString() const;
```

---

## Enum Types

### Query

```cpp
enum Query {
    UNDEFINED = -1,
    INIT = 0,
    INSERT = 10,
    SELECT,
    UPDATE,
    DELETE,
    INSERT_UPDATE,
    BULK_INSERT,
    INSERT_IGNORE,
    BULK_UPDATE,
    SELECT_DISTINCT,
    CONTAINS,
    COUNT,
    RAW_QUERY
};
```

### Filters

```cpp
enum Filters {
    None = -10,
    ORDERBY = 1,
    ORDERBY_DESC,
    ORDERBY_ASC,
    LIMIT,
    GROUPBY
};
```

### NestedWhere

```cpp
enum NestedWhere {
    NNone = -10,
    WHERE = 0,
    AND = 1,
    OR,
    NOT
};
```

### Having

```cpp
enum class Having {
    NNone = -10,
    HAVING = 0,
    AND = 1,
    OR,
    NOT
};
```

### Count

```cpp
enum Count {
    COUNT_PROPS = 0
};
```

### Primary_Key

```cpp
enum Primary_Key {
    PrimaryKey = 0
};
```

---

## Complete Examples

### Full CRUD Example

```cpp
#include "MariadbLazy.h"
#include "Transaction.h"

void CRUDExample() {
    // INSERT
    LazyOrm::MariadbLazy insert;
    insert[LazyOrm::INSERT] = "users";
    insert["name"] = "John Doe";
    insert["email"] = "john@example.com";
    insert["age"] = 30;
    std::cout << insert.query_with_trim_consecutive_spaces() << std::endl;

    // SELECT
    LazyOrm::MariadbLazy select;
    select[LazyOrm::SELECT] = "users";
    select << "name" << "email" << "age";
    select[LazyOrm::WHERE] = {{"age", ">=", "18"}};
    select[LazyOrm::ORDERBY_DESC] = {{"created_at"}};
    select[LazyOrm::LIMIT] = "0,10";
    std::cout << select.query_with_trim_consecutive_spaces() << std::endl;

    // UPDATE
    LazyOrm::MariadbLazy update;
    update[LazyOrm::UPDATE] = "users";
    update["email"] = "newemail@example.com";
    update[LazyOrm::WHERE] = {{"id", "=", "1"}};
    std::cout << update.query_with_trim_consecutive_spaces() << std::endl;

    // DELETE
    LazyOrm::MariadbLazy del;
    del[LazyOrm::DELETE] = "users";
    del[LazyOrm::WHERE] = {{"id", "=", "1"}};
    std::cout << del.query_with_trim_consecutive_spaces() << std::endl;
}
```

### Bulk Operations Example

```cpp
void BulkOperationsExample() {
    // Bulk INSERT
    LazyOrm::MariadbLazy bulkInsert;
    bulkInsert[LazyOrm::BULK_INSERT] = "users";
    bulkInsert.setProperties({
        {{"name", "John"}, {"email", "john@example.com"}, {"age", 30}},
        {{"name", "Jane"}, {"email", "jane@example.com"}, {"age", 25}},
        {{"name", "Bob"}, {"email", "bob@example.com"}, {"age", 35}}
    });
    std::cout << bulkInsert.query_with_trim_consecutive_spaces() << std::endl;
}
```

### Complex WHERE Example

```cpp
void ComplexWhereExample() {
    LazyOrm::MariadbLazy select;
    select[LazyOrm::SELECT] = "users";
    select << "name" << "email" << "age";

    LazyOrm::WhereFilter where = {
        {
            {"name", "like", "John%"},
            {"OR"},
            {"name", "like", "Jane%"}
        },
        {"AND"},
        {"age", "between", "18,65"},
        {"AND"},
        {"active", true}
    };

    select[LazyOrm::WHERE] = where;
    std::cout << select.query_with_trim_consecutive_spaces() << std::endl;
}
```

### Transaction Example

```cpp
void TransactionExample() {
    LazyOrm::MariadbLazy q1, q2, q3;
    q1[LazyOrm::INSERT] = "users";
    q1["name"] = "John";

    q2[LazyOrm::INSERT] = "users";
    q2["name"] = "Jane";

    q3[LazyOrm::INSERT] = "users";
    q3["name"] = "Bob";

    LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({q1, q2, q3});
    std::cout << transaction.query_with_trim_consecutive_spaces() << std::endl;
}
```

### Aggregation Example

```cpp
void AggregationExample() {
    LazyOrm::MariadbLazy select;
    select[LazyOrm::SELECT] = "users";
    select << "department";
    select[LazyOrm::COUNT_PROPS] = {"*", "COUNT(*) as total"};
    select[LazyOrm::GROUPBY] = {{"department"}};

    LazyOrm::HavingFilter having = {{"COUNT(*)", ">", "2"}};
    select[LazyOrm::Having::HAVING] = having;

    std::cout << select.query_with_trim_consecutive_spaces() << std::endl;
}
```
