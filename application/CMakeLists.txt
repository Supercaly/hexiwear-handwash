# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

# Path Variables
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MBED_PATH ${ROOT_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

include(${MBED_PATH}/tools/cmake/app.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_app_subdirectory.cmake)

# Enable log in debug mode
if ((CMAKE_BUILD_TYPE STREQUAL "develop") OR (CMAKE_BUILD_TYPE STREQUAL "debug"))
    set(APP_LOG_ENABLED TRUE)
endif()

# Project Name
project(application)

# Include mbed-os and disable warnings
add_subdirectory(${MBED_PATH})
target_compile_options(mbed-os INTERFACE -w) # Disable all warnings from mbed-os core code
target_compile_options(mbed-events INTERFACE -w) # Disable all warnings from mbed-os events code

# Include Libraries
add_subdirectory(${ROOT_DIR}/tensorflow)

# Ensure APP was specified
if ("$ENV{APP}" STREQUAL "")
    message(FATAL_ERROR "APP was not specified")
else()
    message(STATUS "App $ENV{APP} is being built")
endif()

# Include apps
add_app_subdirectory(app1)
add_app_subdirectory(app2)

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()