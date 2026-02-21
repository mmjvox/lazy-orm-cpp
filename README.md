
> **⚠️ Warning: Under Active Development**  
> This library is in the early stages of development. The API is unstable and may change significantly without notice until version 1.0.0 is released. Expect bugs, incomplete documentation, and rough edges.
>
> **🚧 Not recommended for production use.**

<p align="center">
  <img src="https://img.shields.io/badge/C++-20-blue?logo=c%2B%2B" alt="C++20"/>
  <img src="https://img.shields.io/badge/License-MIT-green" alt="License"/>
  <img src="https://img.shields.io/badge/CMake-3.15+-blue?logo=cmake" alt="CMake"/>
</p>

<h1 align="center">LazyOrm-CPP</h1>
<p align="center">A lazy ORM for C++ that makes SQL query building simple and elegant.</p>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#quick-start">Quick Start</a> •
  <a href="#installation">Installation</a> •
  <a href="#examples">Examples</a> •
  <a href="#documentation">Documentation</a>
</p>

---

## Features

- **Simple API**: Build SQL queries with intuitive operator syntax
- **Multi-Database**: Support for MariaDB/MySQL, PostgreSQL, and SQLite
- **CRUD Operations**: INSERT, SELECT, UPDATE, DELETE with ease
- **Advanced Filters**: WHERE, ORDER BY, GROUP BY, LIMIT, HAVING
- **Bulk Operations**: Efficient bulk insert and update
- **Transactions**: Built-in transaction support
- **Type-Safe**: Strong typing with DbVariant for multiple data types
- **C++20**: Modern C++ features and best practices

---

## Quick Start

```cpp
#include "MariadbLazy.h"
#include <iostream>

int main() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::INSERT] = "users";
    lazyOrm["name"] = "John Doe";
    lazyOrm["email"] = "john@example.com";
    lazyOrm["age"] = 30;

    std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
    // Output: INSERT INTO users (`age`,`email`,`name`) VALUES ('30','john@example.com','John Doe') ;

    return 0;
}
```

---

## Installation

### Requirements

- CMake 3.15 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19.28+)

### Build from Source

```bash
git clone https://github.com/your-repo/lazy-orm-cpp.git
cd lazy-orm-cpp
mkdir build && cd build
cmake ..
cmake --build .
```

### Use in Your Project

```cmake
add_subdirectory(path/to/lazy-orm-cpp)
target_link_libraries(your_app PRIVATE lazyorm)
```

---

## Examples

### SELECT with WHERE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "email" << "age";
lazyOrm[LazyOrm::WHERE] = {{"age", ">=", "18"}};
lazyOrm[LazyOrm::ORDERBY_DESC] = {{"created_at"}};
lazyOrm[LazyOrm::LIMIT] = "0,10";
```

### UPDATE with Conditions

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::UPDATE] = "users";
lazyOrm["name"] = "John Doe";
lazyOrm["email"] = "newemail@example.com";
lazyOrm[LazyOrm::WHERE] = {{"id", "=", "1"}};
```

### DELETE with Filter

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::DELETE] = "users";
lazyOrm[LazyOrm::WHERE] = {{"active", false}};
```

### Bulk INSERT

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::BULK_INSERT] = "users";
lazyOrm.setProperties({
    {{"name", "John"}, {"email", "john@example.com"}, {"age", 30}},
    {{"name", "Jane"}, {"email", "jane@example.com"}, {"age", 25}},
    {{"name", "Bob"}, {"email", "bob@example.com"}, {"age", 35}}
});
```

### Transaction

```cpp
#include "Transaction.h"

LazyOrm::MariadbLazy q1, q2;
q1[LazyOrm::INSERT] = "users";
q1["name"] = "John";

q2[LazyOrm::INSERT] = "logs";
q2["action"] = "user_created";

LazyOrm::Transaction<LazyOrm::MariadbLazy> transaction({q1, q2});
// Output: START TRANSACTION; INSERT ... ; INSERT ... ; COMMIT;
```

### Complex WHERE

```cpp
LazyOrm::MariadbLazy lazyOrm;
lazyOrm[LazyOrm::SELECT] = "users";
lazyOrm << "name" << "age";

lazyOrm[LazyOrm::WHERE] = {
    {
        {"name", "like", "John%"},
        {"OR"},
        {"name", "like", "Jane%"}
    },
    {"AND"},
    {"age", "between", "18,65"}
};
```

---

## Database Support

| Database | Class | Example |
|----------|-------|---------|
| MariaDB/MySQL | `MariadbLazy` | `LazyOrm::MariadbLazy` |
| PostgreSQL | `PostgreLazy` | `LazyOrm::PostgreLazy` |
| SQLite | `SqliteLazy` | `LazyOrm::SqliteLazy` |

---

## Documentation

- [Installation Guide](docs/installation.md)
- [Quick Start](docs/quick-start.md)
- [Use Cases](docs/usecases.md)
- [API Reference](docs/api.md)

---

## License

Licensed under the [MIT License](LICENSE).

---

## Author

**mmjvox** - [GitHub](https://github.com/mmjvox)
