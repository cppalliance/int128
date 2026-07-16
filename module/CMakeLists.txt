# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# Builds Boost.Int128 as a C++ module (boost.int128) and runs the module-aware
# tests in ../test against it, consuming the standard library module (import std).
# This file is included from ../test/CMakeLists.txt when the CMake option
# BOOST_INT128_BUILD_MODULE is set and expects the Boost superproject (for the
# Boost:: dependency targets). import std is required (see the check below).

if(NOT TARGET tests)
    add_custom_target(tests)
endif()

# The module interface unit.
add_library(boost_int128_module)
target_sources(boost_int128_module
        PUBLIC
        FILE_SET int128_module TYPE CXX_MODULES FILES int128.cppm
)
target_link_libraries(boost_int128_module PUBLIC Boost::int128)
target_compile_features(boost_int128_module PUBLIC cxx_std_23)

# The purview includes the library's angled <boost/int128...> headers by design.
# clang-scan-deps warns on that and ignores -Wno-include-angled-in-module-purview,
# so -w keeps the dependency-scan output clean (this glue unit's code is fully
# warned on in the header builds).
target_compile_options(boost_int128_module PRIVATE $<$<CXX_COMPILER_ID:Clang>:-w>)

# int128.cppm defines BOOST_INT128_BUILD_MODULE itself, but the tests link this
# target and need the macro too so they import the module instead of including
# the headers directly.
target_compile_definitions(boost_int128_module PUBLIC BOOST_INT128_BUILD_MODULE)

# Export the handful of detail-namespace entities that the tests exercise directly.
# Only the interface unit needs this, so keep it private to the module target.
target_compile_definitions(boost_int128_module PRIVATE BOOST_INT128_EXPORT_TESTING)

# The tests introspect standard-library facilities (e.g. numeric_limits members) that
# a module consumer only sees reliably through the standard library module, so import
# std is required for the test build. CMAKE_CXX_COMPILER_IMPORT_STD lists the standards
# it is available for; it is only populated when CMAKE_EXPERIMENTAL_CXX_IMPORT_STD is
# set before the project() call.
if(NOT "23" IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    message(FATAL_ERROR
            "Building the Boost.Int128 module tests requires 'import std' (the C++23 "
            "standard library module). Use a toolchain that supports it and pass "
            "-DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD=<uuid> before the project() call. "
            "CMAKE_CXX_COMPILER_IMPORT_STD='${CMAKE_CXX_COMPILER_IMPORT_STD}'")
endif()

set_target_properties(boost_int128_module PROPERTIES CXX_MODULE_STD ON)
target_compile_definitions(boost_int128_module PRIVATE BOOST_INT128_USE_STD_MODULE)
message(STATUS "Boost.Int128 module: using 'import std;'")

# Tests that switch to `import boost.int128;` when BOOST_INT128_BUILD_MODULE is set.
# test_i128 and test_i128_no_sign_conv are intentionally omitted: they exercise the
# arithmetic operators with the raw builtin 128-bit types (__int128), and clang
# resolves those overloads differently across the module boundary, selecting a
# generic path that narrows. That is a library-level concern independent of module
# support, so those two remain covered by the header-based test suite.
set(BOOST_INT128_MODULE_TESTS
        test_bit
        test_div
        test_from_chars_bases
        test_gcd_lcm
        test_hash
        test_ipow
        test_isqrt
        test_limits_i128
        test_limits_u128
        test_literals
        test_midpoint
        test_powm
)

# GCC's libstdc++ cannot yet mix the test sources' textual standard library
# includes with import std in one consumer translation unit, so only the quick
# test runs there. MSVC is treated the same way until its full consumer suite has
# been validated. The full list runs under clang/libc++.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Boost.Int128 module: ${CMAKE_CXX_COMPILER_ID} consumer tests limited to module_quick_test")
    set(BOOST_INT128_MODULE_TESTS)
endif()

add_executable(module_quick_test "${CMAKE_CURRENT_SOURCE_DIR}/quick_test.cpp")
target_link_libraries(module_quick_test PRIVATE boost_int128_module)
# The superproject pins an older policy version (CMP0155), so C++ sources are not
# scanned for module imports by default. Force scanning so `import boost.int128;`
# is resolved to the module built above.
set_target_properties(module_quick_test PROPERTIES CXX_SCAN_FOR_MODULES ON CXX_MODULE_STD ON)
add_test(NAME module_quick_test COMMAND module_quick_test)
add_dependencies(tests module_quick_test)

foreach(test ${BOOST_INT128_MODULE_TESTS})
    add_executable(module_${test} "${CMAKE_CURRENT_SOURCE_DIR}/../test/${test}.cpp")
    target_link_libraries(module_${test}
            PRIVATE
            boost_int128_module
            Boost::core
            Boost::mp11
            Boost::random
    )
    set_target_properties(module_${test} PROPERTIES CXX_SCAN_FOR_MODULES ON CXX_MODULE_STD ON)
    add_test(NAME module_${test} COMMAND module_${test})
    add_dependencies(tests module_${test})
endforeach()
