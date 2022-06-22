# Copyright (c) 2022 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        .
        collector
        common
        data
        predictor
        ui
        ui/menu
)

target_sources(${PROJECT_NAME} 
    PRIVATE
        main.cpp
        collection/collector_thread.cpp
        common/haptic_feedback.cpp
        common/data_recorder.cpp
        common/data_exporter.cpp
        prediction/predictor_thread.cpp
        ui/display_thread.cpp 
        ui/menu/navigator.cpp
        ui/menu/menu.cpp
        ui/menu/pages/wrist_page.cpp
        ui/menu/pages/stats_page.cpp
        ui/menu/pages/recorder_page.cpp
        ui/menu/pages/export_page.cpp
)