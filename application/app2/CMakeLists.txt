# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set(APP_NAME app2)

add_executable(${APP_NAME})

target_sources(${APP_NAME} PRIVATE main.cpp)
target_include_directories(${APP_NAME} PRIVATE .)
target_link_libraries(${APP_NAME}
    mbed-os
)

mbed_set_post_build(${APP_NAME})