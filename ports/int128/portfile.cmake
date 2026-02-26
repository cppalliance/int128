# Copyright 2024 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#
# See: https://devblogs.microsoft.com/cppblog/registries-bring-your-own-libraries-to-vcpkg/

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO cppalliance/int128
        REF v1.6.1
        SHA512 84978c581edd5d4b2b9ecaf9229b552dae4c686387d7e3149a2fd7ff4736a045ba6712611f6560ed58084f0f0cd2a5fc4d32759bb0d4166529f50bc066dc59e0
        HEAD_REF master
)

file(COPY "${SOURCE_PATH}/include" DESTINATION "${CURRENT_PACKAGES_DIR}")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
