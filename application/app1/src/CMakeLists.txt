# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(collection)
add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(prediction)
add_subdirectory(ui)

target_include_directories(${APP_NAME} PRIVATE .)

target_sources(${APP_NAME} 
    PRIVATE
        main.cpp
)