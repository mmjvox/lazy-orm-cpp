# Quick Start

A minimal Hello World example to get started with LazyOrm-CPP.

## Hello World

```cpp
#include "MariadbLazy.h"
#include <iostream>

int main() {
    LazyOrm::MariadbLazy lazyOrm;
    lazyOrm[LazyOrm::INSERT] = "student";
    lazyOrm["name"] = "anya";
    lazyOrm["age"] = 6;
    lazyOrm["hair"] = "pink";
    lazyOrm["cute"] = true;

    std::cout << lazyOrm.query_with_trim_consecutive_spaces() << std::endl;
    // Output: INSERT INTO student (`age`,`cute`,`hair`,`name`) VALUES ('6',true,'pink','anya') ;
    return 0;
}
```

## Build

```bash
g++ -std=c++20 main.cpp -o myapp -I/path/to/lazy-orm-cpp/include -L/path/to/lazy-orm-cpp/build -llazyorm
```

For more examples, see [Use Cases](usecases.md).