# Generated by `boostdep --cmake int128`
# Copyright 2020, 2021 Peter Dimov
# Copyright 2023 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.8...3.20)

project(boost_int128 VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

add_library(boost_int128 INTERFACE)

add_library(Boost::int128 ALIAS boost_int128)

target_include_directories(boost_int128 INTERFACE include)

target_compile_features(boost_int128 INTERFACE cxx_std_14)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")

    add_subdirectory(test)

endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

    include(GNUInstallDirs)
    install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

endif()
