# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(menu)

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_sources(${PROJECT_NAME} 
    PRIVATE
        display_thread.cpp 
)