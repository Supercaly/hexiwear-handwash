# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/resources)
    message(STATUS "Generating resources from assets...")
    execute_process(
        COMMAND python3 ${ROOT_DIR}/tools/generate_asset.py --input app1/asset --output app1/generated/resources --name menu_resources 
        WORKING_DIRECTORY ${ROOT_DIR}
        RESULT_VARIABLE RES
    )
    if (RES AND NOT RES EQUAL 0)
        message(FATAL_ERROR "Error generating resources")
    else()
        message(STATUS "Generating resources done")
    endif()
endif()

target_include_directories(${APP_NAME} PRIVATE .)

target_sources(${APP_NAME}
    PRIVATE
        resources/menu_resources.c
)