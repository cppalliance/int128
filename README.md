# Boost.int128 (candidate library)
Portable implementations of signed and unsigned 128-bit integers

# Notice

Decimal is under active development and is not an official boost library.

# How To Use The Library

This library is header only. It contains no other dependencies.
Simply `#include` it and use it.

# Supported Platforms

Boost.Decimal is tested natively on Ubuntu (x86_64, s390x, and aarch64), macOS (x86_64, and Apple Silicon), and Windows (x32 and x64);
as well as emulated PPC64LE and STM32 using QEMU with the following compilers:

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
(e.g. like those found in `<limits>`, `<iostream>`, etc.).

The entire library can be conveniently included with `#include <boost/int128.hpp>`

# Full Documentation

The complete documentation can be found at: https://master.int128.cpp.al

## References

[1] Donald E. Knuth, _The_ _Art_ _of_ _Computer_ _Programming_ _Volume_ _2_ _Seminumerical_ _Algorithms_, 3rd edition, 1998
