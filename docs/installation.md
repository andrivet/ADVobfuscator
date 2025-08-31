# Installation & Usage

**ADVobjuscator** is a header-only C++ library that integrates cleanly with CMake.
There are several possibilities to install and use it:

* Option 0: Manual download
* Option 1: Install & Use via `find_package`
* Option 2: Add as a Git Submodule / Subdirectory
* Option 3: Use with `FetchContent` in CMake

## Option 0: Manual download

If you don’t use CMake or prefer to copy files manually:

1. Download

* Click the green “Code” button on GitHub → Download ZIP
* or download only the `include/` folder

2. Copy

Copy the `include/advobfuscator/` directory into your own project’s `include/` folder.

3. Include It in Your Code

```c++
#include "advobfuscator/obfuscate.h"
```

Make sure your compiler includes the path:

```shell
g++ -Iinclude myapp.cpp
```

If you are using CMake, here is an example of `CMakeFiles.txt`:

```cmake
cmake_minimum_required(VERSION 3.14)
project(myproject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myapp src/main.cpp)

# Add the path to the manually downloaded headers
target_include_directories(myapp PRIVATE ${CMAKE_SOURCE_DIR}/include)
```


## Option 1: Install & Use via `find_package`

1. Clone and Install the Library

```shell
git clone https://github.com/yourusername/advobfuscator.git
cd advobfuscator
cmake -B build -DCMAKE_INSTALL_PREFIX=/your/install/prefix
cmake --build build --target install
```

Replace `/your/install/prefix` with the desired install location (e.g., `/usr/local` or a custom path).

2. Link from Your CMake Project

```cmake
cmake_minimum_required(VERSION 3.14)
project(myproject)

# Add path to CMAKE_PREFIX_PATH if not system-installed
list(APPEND CMAKE_PREFIX_PATH "/your/install/prefix")

find_package(advobfuscator REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE advobfuscator::advobfuscator)
```

## Option 2: Add as a Git Submodule / Subdirectory

1. Add the Library to Your Project

```shell
git submodule add https://github.com/andrivet/advobfuscator.git external/advobfuscator
```

2. Link from Your CMake Project

```cmake
add_subdirectory(external/advobfuscator)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE advobfuscator::advobfuscator)
```

## Option 3: Use with FetchContent in CMake

If you want CMake (3.14 or higher) to automatically fetch and integrate **ADVObfuscator**:

```cmake
include(FetchContent)

FetchContent_Declare(
    advobfuscator
    GIT_REPOSITORY https://github.com/andrivet/advobfuscator.git
    GIT_TAG        v2.0  # Or use a branch or commit hash
)

FetchContent_MakeAvailable(advobfuscator)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE advobfuscator::advobfuscator)
```
