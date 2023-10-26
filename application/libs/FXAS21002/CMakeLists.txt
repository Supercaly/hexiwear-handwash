# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(hexiwear_FXAS21002 INTERFACE)

target_include_directories(hexiwear_FXAS21002 INTERFACE .)

target_sources(hexiwear_FXAS21002 INTERFACE FXAS21002.cpp)
