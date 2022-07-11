# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set(APP_NAME app2)

add_library(${APP_NAME})

add_subdirectory(src)

target_link_libraries(${APP_NAME}
    mbed-os
    mbed-storage
    csv
    prediction
    utils
)