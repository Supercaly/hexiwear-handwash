# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_sources(${PROJECT_NAME} 
    PRIVATE
        config.cpp
        haptic_feedback.cpp
        data_recorder.cpp
        data_exporter.cpp
)