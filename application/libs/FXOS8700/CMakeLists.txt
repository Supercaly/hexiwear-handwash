# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(hexiwear_FXOS8700 INTERFACE)

target_include_directories(hexiwear_FXOS8700 INTERFACE .)

target_sources(hexiwear_FXOS8700 INTERFACE FXOS8700.cpp)
