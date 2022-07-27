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
    ${ROOT_DIR}/../training/models/${ML_MODE}_${ML_WINDOW_SIZE}/model.cc
)

target_sources(ml INTERFACE
    predictor.cpp
    ${ROOT_DIR}/../training/models/${ML_MODE}_${ML_WINDOW_SIZE}/model.cc
)

add_subdirectory(libtf)