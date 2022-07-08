# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# This script provides add_app_subdirectory(), a function that lets you add a subdirectory
# for an application, like add_subdirectory(), but only if the app currently being built 
# is the one specifyed.
function(add_app_subdirectory APP_SUBDIR)
    string(FIND ${APP_SUBDIR} / INDEX REVERSE)
    math(EXPR INDEX "${INDEX} + 1")
    string(SUBSTRING ${APP_SUBDIR} ${INDEX} -1 APP_NAME)
    
    if ("${APP_NAME}" STREQUAL "")
        message(FATAL_ERROR "add_app_subdirectory could not extract an app name from ${APP_SUBDIR}")
    elseif ("$ENV{APP}" STREQUAL ${APP_NAME})
        add_subdirectory(${APP_SUBDIR})
        target_link_libraries(${PROJECT_NAME} ${APP_NAME})
    endif ()
endfunction()