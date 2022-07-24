# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

# Path Variables
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MBED_PATH ${ROOT_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(LIBS_DIR ${ROOT_DIR}/libs)

include(${MBED_PATH}/tools/cmake/app.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_app_subdirectory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_all_targets.cmake)

# Extract ML_MODE
if ("$ENV{MODE}" STREQUAL "")
    message(FATAL_ERROR "MODE was not specified")
else()
    set(ML_MODE $ENV{MODE})
endif()

# Enable log in debug mode
if ((CMAKE_BUILD_TYPE STREQUAL "develop") OR (CMAKE_BUILD_TYPE STREQUAL "debug"))
    set(APP_LOG_ENABLED TRUE)
endif()

# Project Name
project(application)
add_executable(${PROJECT_NAME})

# Extract WINDOW_SIZE
if ("$ENV{WS}" STREQUAL "")
    message(FATAL_ERROR "WS was not specified")
else()
    if(NOT $ENV{WS} MATCHES "^[0-9]+$")
        message(FATAL_ERROR "WS must be a number ($ENV{WS})")
    endif()
    add_compile_definitions(WINDOW_SIZE=$ENV{WS})
    add_compile_definitions(FULL_WINDOW_SIZE=$ENV{WS}00)
endif()

# Include mbed-os and disable warnings
add_subdirectory(${MBED_PATH})
target_compile_options(mbed-os INTERFACE -w) # Disable all warnings from mbed-os core code
target_compile_options(mbed-events INTERFACE -w) # Disable all warnings from mbed-os events code

# Include Libraries
add_subdirectory(${LIBS_DIR}/csv)
add_subdirectory(${LIBS_DIR}/tensorflow)
add_subdirectory(${LIBS_DIR}/utils)
add_subdirectory(${LIBS_DIR}/prediction)
# Include ML libraries
if(${ML_MODE} STREQUAL "svm")
    add_subdirectory(${LIBS_DIR}/svm)
elseif(${ML_MODE} STREQUAL "knn")
elseif(${ML_MODE} STREQUAL "cnn")
    add_subdirectory(${LIBS_DIR}/ml)
elseif(${ML_MODE} STREQUAL "lstm")
else()
    message(FATAL_ERROR "unsupported ml mode ${ML_MODE}!")
endif()

# Ensure APP was specified
if ("$ENV{APP}" STREQUAL "")
    message(FATAL_ERROR "APP was not specified")
endif()

# Include apps
add_app_subdirectory(app1)
add_app_subdirectory(app2)

# Ensure that a target exists for the APP
get_all_targets(ALL_BUILDSYSTEM_TARGETS)
if ($ENV{APP} IN_LIST ALL_BUILDSYSTEM_TARGETS)
    message(STATUS "App '$ENV{APP}' is being built with mode '$ENV{MODE}' and window size '$ENV{WS}'")
else()
    message(FATAL_ERROR "App '$ENV{APP}' is not an app target found in '${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt'")
endif()

# Execute mbed post build
mbed_set_post_build(${PROJECT_NAME})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()