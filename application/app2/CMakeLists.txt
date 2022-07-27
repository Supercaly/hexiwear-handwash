# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

set(APP_NAME app2)

add_library(${APP_NAME})

# Include Libraries
add_subdirectory(hexiwear_FXAS21002)
add_subdirectory(hexiwear_FXOS8700)
add_subdirectory(src)

target_link_libraries(${APP_NAME}
    mbed-os
    mbed-storage
    hexiwear_FXAS21002
    hexiwear_FXOS8700
    csv
    prediction
    utils
)

if(${ML_MODE} STREQUAL "svm")
    target_link_libraries(${APP_NAME} svm)
elseif(${ML_MODE} STREQUAL "cnn" OR ${ML_MODE} STREQUAL "lstm")
    target_link_libraries(${APP_NAME} ml)
endif()