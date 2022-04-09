# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET application)

include(${MBED_PATH}/tools/cmake/app.cmake)

project(${APP_TARGET})

add_executable(${PROJECT_NAME})

add_subdirectory(${MBED_PATH})
add_subdirectory(hexiwear_FXAS21002)
add_subdirectory(hexiwear_FXOS8700)
add_subdirectory(hexiwear_kw40z)
add_subdirectory(oled_ssd1351)

target_link_libraries(${PROJECT_NAME} 
    mbed-os
    mbed-events
    hexiwear_FXAS21002
    hexiwear_FXOS8700
    hexiwear_kw40z
    oled_ssd1351
)

add_subdirectory(generated)
add_subdirectory(tensorflow)
add_subdirectory(src)

mbed_set_post_build(${PROJECT_NAME})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()