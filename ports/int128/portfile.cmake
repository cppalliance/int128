# Copyright 2024 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# See: https://devblogs.microsoft.com/cppblog/registries-bring-your-own-libraries-to-vcpkg/

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO cppalliance/int128
        REF v1.6.0
        SHA512 cc5a5e065643ee5b790f35441b34cea291bd299f5460278bb3402005c9cdd5a9a552b28e64816cc0ed480662c53959eb7cd5d1774d7e2480fffc65532353dc5c
        HEAD_REF master
)

file(COPY "${SOURCE_PATH}/include" DESTINATION "${CURRENT_PACKAGES_DIR}")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
