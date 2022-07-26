# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(ml INTERFACE)

target_compile_definitions(ml INTERFACE HAS_ML)

target_link_libraries(ml INTERFACE
    prediction
)

target_include_directories(ml INTERFACE
    .
    model
)

target_sources(ml INTERFACE
    predictor.cpp
    model/model.cc
)

add_subdirectory(libtf)