# Boost.int128 (candidate library)
Portable implementations of signed and unsigned 128-bit integers

|                  | Master                                                                                                                                                            |   Develop   |
|------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------|
| Drone            | [![Build Status](https://drone.cpp.al/api/badges/cppalliance/int128/status.svg?ref=refs/heads/master)](https://drone.cpp.al/cppalliance/int128)                 | [![Build Status](https://drone.cpp.al/api/badges/cppalliance/int128/status.svg?ref=refs/heads/develop)](https://drone.cpp.al/cppalliance/int128) |
| Github Actions   | [![CI](https://github.com/cppalliance/int128/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/cppalliance/int128/actions/workflows/ci.yml) | [![CI](https://github.com/cppalliance/int128/actions/workflows/ci.yml/badge.svg?branch=develop)](https://github.com/cppalliance/int128/actions/workflows/ci.yml)
| Codecov          | [![codecov](https://codecov.io/gh/cppalliance/int128/branch/master/graph/badge.svg?token=KWY7M64xhh)](https://codecov.io/gh/cppalliance/int128) | [![codecov](https://codecov.io/gh/cppalliance/int128/graph/badge.svg?token=drvY8nnV5S)](https://codecov.io/gh/cppalliance/int128) |
---

# Notice

int128 is under active development and is not an official boost library.

# How To Use The Library

This library is header only. It contains no other dependencies.
Simply `#include` it and use it.

## CMake

```sh
git clone https://github.com/cppalliance/int128
cd int128
mkdir build && cd build
cmake .. OR cmake .. -DCMAKE_INSTALL_PREFIX=/your/custom/path
cmake --build . --target install
```

then you can use `find_package(boost_int128 REQUIRED)`

## vcpkg

```sh
git clone https://github.com/cppalliance/int128
cd int128
vcpkg install int128 --overlay-ports=ports/int128 
```

# Supported Platforms

Boost.Int128 is tested natively on Ubuntu (x86_64, x86_32, s390x, aarch64, ARM32v7), macOS (x86_64, and Apple Silicon), and Windows (x86_64, x86_32, and ARM64);
as well as emulated PPC64LE using QEMU with the following compilers:

* GCC 5 and later
* Clang 5 and later
* Visual Studio 2017 (14.1) and later
* Intel OneAPI DPC++

# Synopsis

Int128 provides 2 types:

```cpp
namespace boost {
namespace int128 {

struct uint128_t;
struct int128_t;

} //namespace int128
} //namespace boost
```

These types operate like built-in integer types.
They have their own implementations of the Standard-Library functions
(e.g. like those found in `<limits>`, `<iostream>`, `<bit>` etc.).

The entire library can be conveniently included with `#include <boost/int128.hpp>`

# Full Documentation

The complete documentation can be found at: https://master.int128.cpp.al

## References

[1] Donald E. Knuth, _The_ _Art_ _of_ _Computer_ _Programming_ _Volume_ _2_ _Seminumerical_ _Algorithms_, 3rd edition, 1998
