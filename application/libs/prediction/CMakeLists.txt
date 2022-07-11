# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(prediction INTERFACE)

target_link_libraries(prediction INTERFACE
    tensorflow
    utils
)

target_include_directories(prediction INTERFACE 
    .
    model
)

target_sources(prediction INTERFACE
    predictor.cpp
    model/handwash_model.cc
)

