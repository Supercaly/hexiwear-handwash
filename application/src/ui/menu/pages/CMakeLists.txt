# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_sources(${PROJECT_NAME} 
    PRIVATE
        wrist_page.cpp
        stats_page.cpp
        recorder_page.cpp
        export_page.cpp
)