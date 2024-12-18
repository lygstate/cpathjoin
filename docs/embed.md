---
title: Embedding
description: A guide on how to embed the cpj path library for C/C++.
---


In order to embed **cpj**, you will have to download it.
You can do so using git (or download it from [here](https://github.com/likle/cpj/archive/stable.zip)).  

```bash
git clone -b stable git@github.com:likle/cpj.git
```

**Note**: The *stable* branch points to the latest stable version. You should
always use a stable version in production code.

## Using CMake to embed cpj

If you are using CMake it is fairly easy to embed **cpj**.
This only requires two lines, you don't even have to specify the include directories.
The following example shows how to do so:

```cmake
# Some basics you will need in your cmake file.
cmake_minimum_required(VERSION 3.9.2)
project(example C)
add_executable(example_target main.c)

# Replace your_path_to_cpj with the path to your cpj copy. 
# This could be something like "${CMAKE_CURRENT_SOURCE_DIR}/lib/cpj".
add_subdirectory(your_path_to_cpj) 

# Replace example_target with the target name which requires cpj.
# After this, there is no need to specify any include directories.
target_link_libraries(example_target cpj)
```

After that, you should be able to use cpj in your source code:

```c
#include <cpj.h>
```

## Directly embed cpj in your source

If you don't use CMake and would like to embed **cpj** directly, you could
just add the two files ``src/cpj.c`` and ``ìnclude/cpj.h`` to your project.
The folder containing ``cpj.h`` has to be in your include directories
([Visual Studio](https://docs.microsoft.com/en-us/cpp/ide/vcpp-directories-property-page?view=vs-2017),
[Eclipse](https://help.eclipse.org/mars/index.jsp?topic=%2Forg.eclipse.cdt.doc.user%2Freference%2Fcdt_u_prop_general_pns_inc.htm),
[gcc](https://www.rapidtables.com/code/linux/gcc/gcc-i.html),
[clang](https://clang.llvm.org/docs/ClangCommandLineReference.html#include-path-management)).
