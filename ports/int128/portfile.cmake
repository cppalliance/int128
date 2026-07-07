# Copyright 2024 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# See: https://devblogs.microsoft.com/cppblog/registries-bring-your-own-libraries-to-vcpkg/

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO cppalliance/int128
        REF v3.0.0
        SHA512 e65a61782f6b5d99fcbe452b441725410a54b57f0737a87b3a1da4a0d0be90445c146d7b6b6795a5cd1ccef0a573144f4d5df634c35675b9b38365572bf3ff35
        HEAD_REF master
)

file(COPY "${SOURCE_PATH}/include" DESTINATION "${CURRENT_PACKAGES_DIR}")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
