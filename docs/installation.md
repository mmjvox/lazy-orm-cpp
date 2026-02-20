# Installation

This guide explains how to install and build LazyOrm-CPP in your project.

## Requirements

- **CMake**: Version 3.15 or higher
- **C++ Compiler**: C++20 standard support required
  - GCC 9.0+
  - Clang 10.0+
  - MSVC 19.28+

## Installation Methods

### CMake

#### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/your-repo/lazy-orm-cpp.git
cd lazy-orm-cpp
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake ..
```

4. Build the library:
```bash
cmake --build .
```

5. Install (optional):
```bash
cmake --install .
```

#### Using in Your CMake Project

Add LazyOrm-CPP to your CMake project using `add_subdirectory`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject)

# Add LazyOrm-CPP
add_subdirectory(path/to/lazy-orm-cpp)

# Link to your target
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE lazyorm)
```

Or use CMake's `find_package` after installation:

```cmake
find_package(lazyorm REQUIRED)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE lazyorm)
```


## Build Options

The following CMake options are available:

| Option | Default | Description |
|--------|---------|-------------|
| `tests` | `OFF` | Build test suite |

To build tests:

```bash
cmake -Dtests=ON ..
cmake --build .
```

## Including Headers

After installation, include LazyOrm-CPP headers in your code:

```cpp
#include "LazyOrm.h"
#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"
```

## Database Drivers

LazyOrm-CPP is a query builder and does not include database drivers.
Query builder of LazyOrm-CPP supprots:

- **MariaDB/MySQL**`
- **PostgreSQL**
- **SQLite**


## Installation Paths

By default, LazyOrm-CPP installs to:

- **Headers**: `/usr/local/include/LazyOrm/`
- **Library**: `/usr/local/lib/`
- **CMake config**: `/usr/local/lib/cmake/lazyorm/`

To customize installation paths:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

## Verifying Installation

To verify your installation, compile a simple test program:

```cpp
#include <iostream>
#include "LazyOrm.h"

int main() {
    LazyOrm::LazyOrm lazyOrm;
    std::cout << "LazyOrm-CPP installed successfully!" << std::endl;
    return 0;
}
```

Compile:
```bash
g++ -std=c++20 test.cpp -o test -I/usr/local/include -L/usr/local/lib -llazyorm
```

## Troubleshooting

### CMake Version Error

If you encounter a CMake version error, upgrade your CMake:
```bash
# Ubuntu/Debian
sudo apt-get install --only-upgrade cmake

# macOS with Homebrew
brew upgrade cmake
```

### C++20 Support

Ensure your compiler supports C++20:
```bash
g++ --version  # Should be 9.0+
clang++ --version  # Should be 10.0+
```

### Linking Errors

If you encounter linking errors, verify the library path is in your linker search path:
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

Or add the path to `/etc/ld.so.conf.d/` and run `sudo ldconfig`.
