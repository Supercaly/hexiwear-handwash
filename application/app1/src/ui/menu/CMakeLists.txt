# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(pages)

target_include_directories(${APP_NAME} PRIVATE .)

target_sources(${APP_NAME} 
    PRIVATE
        navigator.cpp
        menu.cpp
)