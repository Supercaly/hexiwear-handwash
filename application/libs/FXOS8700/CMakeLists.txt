# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(FXOS8700 INTERFACE)

target_include_directories(FXOS8700 INTERFACE .)

target_sources(FXOS8700 INTERFACE FXOS8700.cpp)
