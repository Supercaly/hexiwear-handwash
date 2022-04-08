# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(FXAS21002 INTERFACE)

target_include_directories(FXAS21002 INTERFACE .)

target_sources(FXAS21002 INTERFACE FXAS21002.cpp)
