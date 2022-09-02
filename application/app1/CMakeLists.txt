# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set(APP_NAME app1)

add_library(${APP_NAME})

# Include Libraries
add_subdirectory(hexiwear_FXAS21002)
add_subdirectory(hexiwear_FXOS8700)
add_subdirectory(hexiwear_kw40z)
add_subdirectory(oled_ssd1351)

# Include sources
add_subdirectory(generated)
add_subdirectory(src)

target_link_libraries(${APP_NAME} 
    mbed-os
    mbed-events
    mbed-storage
    hexiwear_FXAS21002
    hexiwear_FXOS8700
    hexiwear_kw40z
    oled_ssd1351
    prediction
    utils
)

if(${ML_MODE} STREQUAL "svm")
    target_link_libraries(${APP_NAME} svm)
elseif(${ML_MODE} STREQUAL "cnn" OR ${ML_MODE} STREQUAL "lstm")
    target_link_libraries(${APP_NAME} ml)
endif()