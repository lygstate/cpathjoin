---
title: Building
description: A guide on how to build the cpj path library for C/C++.
---

In order to build the source, you will have to download it. You can do so using git (or download it from [here](https://github.com/likle/cpj/archive/stable.zip)).

```bash
git clone -b stable git@github.com:likle/cpj.git
```

**Note**: The *stable* branch points to the latest stable version. You should
always use a stable version in production code.

## Using Windows

Visual Studio 2017 is recommended, then you can just open the source using ``File -> Open -> CMake...``. You can use Visual Studio to compile the source and debug the code. Make sure you have the CMake and C/C++ features enabled.

## Using Ubuntu

You will need [CMake](https://cmake.org/download/) and either gcc or clang installed. On Ubuntu you can use the following to compile **cpj**:

```bash
sudo apt-get install build-essential cmake
mkdir cpj/build
cd cpj/build
cmake ..
make
```

## Using MacOS

You will need [CMake](https://cmake.org/download/) and either gcc or clang installed. On MacOS you can use the following to compile **cpj**:

```bash
brew install cmake gcc
mkdir cpj/build
cd cpj/build
cmake ..
make
```

## Running Tests

In order to run tests, cpj needs to be built with tests enabled. There is a ``ENABLE_TESTS`` flag for that. It can be passed to the cmake command like this:

```bash
cmake .. -DENABLE_TESTS=1
```

After building **cpj** you can run tests to ensure everything is fine. In order to do that, make sure that you are in the build folder and then execute the test program:

```bash
./cpjtest
```

That's it!

You can even specify which tests to execute by optionally specifying the category and test name:

```bash
# ./cpjtest [category] [test]
./cpjtest normalize mixed
```
