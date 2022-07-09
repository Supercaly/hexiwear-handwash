# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(csv INTERFACE)

target_include_directories(csv INTERFACE .)

target_sources(csv INTERFACE
    csv_parser.cpp
    getline.cpp
)