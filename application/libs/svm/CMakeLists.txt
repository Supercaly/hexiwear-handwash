# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(svm INTERFACE)

target_link_libraries(svm INTERFACE
    utils
    prediction
)

target_include_directories(svm INTERFACE
    .
    libsvm
)

target_sources(svm INTERFACE
    svm.cpp
    libsvm/svm.cpp
)

