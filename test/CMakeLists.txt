# Copyright 2018, 2019 Peter Dimov
# Copyright 2023 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTestJamfile OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(HAVE_BOOST_TEST)

    enable_testing()

    if(BOOST_INT128_ENABLE_CUDA)

        message(STATUS "Building Boost.int128 with CUDA")

        enable_language(CUDA)
        find_package(CUDAToolkit REQUIRED)
        set(CMAKE_CUDA_EXTENSIONS OFF)

        enable_testing()

        boost_test_jamfile(FILE cuda_jamfile LINK_LIBRARIES Boost::int128 Boost::random Boost::charconv CUDA::cudart COMPILE_DEFINITIONS BOOST_INT128_ENABLE_CUDA=1  )
        
    else()

        boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::int128 Boost::core Boost::random Boost::multiprecision Boost::mp11 Boost::charconv)

    endif()

endif()
