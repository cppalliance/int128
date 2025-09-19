# Copyright 2023 - 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.10...3.20)

# Set version explicitly if not part of Boost superproject
if(NOT BOOST_SUPERPROJECT_VERSION)
    set(BOOST_INT128_VERSION 1.3.0)
else()
    set(BOOST_INT128_VERSION ${BOOST_SUPERPROJECT_VERSION})
endif()

project(boost_int128 VERSION ${BOOST_INT128_VERSION} LANGUAGES CXX)

add_library(boost_int128 INTERFACE)

add_library(Boost::int128 ALIAS boost_int128)

if(NOT BOOST_SUPERPROJECT_VERSION)

    target_include_directories(boost_int128 INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

else()

    target_include_directories(boost_int128 INTERFACE include)

endif()

target_compile_features(boost_int128 INTERFACE cxx_std_14)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")

    add_subdirectory(test)

endif()

# Installation and config-file package generation
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(DIRECTORY "include/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    # Install targets
    install(TARGETS boost_int128
            EXPORT boost_int128Targets
            INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    # Install export targets
    install(EXPORT boost_int128Targets
            FILE boost_int128Targets.cmake
            NAMESPACE Boost::
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost_int128"
    )

    # Generate config files
    configure_package_config_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost_int128Config.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128Config.cmake"
            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost_int128"
    )

    write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
    )

    # Install config files
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128ConfigVersion.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost_int128"
    )

    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost_int128.pc.in"
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128.pc"
            @ONLY
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/boost_int128.pc"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    )

endif()
