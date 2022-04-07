# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/generated/resources)
    message(STATUS "Generating resources from assets...")
    execute_process(
        COMMAND python3 tools/generate_asset.py --input asset --output generated/resources --name menu_resources 
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
    message(STATUS "Generating resources done")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_sources(${PROJECT_NAME}
    PRIVATE
        resources/menu_resources.c
        model/handwash_model.cc
)