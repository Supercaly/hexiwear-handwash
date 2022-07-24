# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(prediction INTERFACE)

target_link_libraries(prediction INTERFACE
    utils
)

target_include_directories(prediction INTERFACE 
    .
)

target_sources(prediction INTERFACE
    features.cpp
)

