# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(utils INTERFACE)

target_include_directories(utils INTERFACE .)

if (APP_LOG_ENABLED)
    target_compile_definitions(utils INTERFACE LOG_ENABLED)
endif()