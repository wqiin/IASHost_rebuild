cmake_minimum_required(VERSION 3.16)

project(patroller_log LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(patroller_log SHARED
    PatrolLogger.cpp
    PatrolLogger.h
    GetConfigHelper.h
    GetConfigHelper.cpp
)

target_compile_definitions(patroller_log PRIVATE PATROLLER_LOG_LIBRARY)

#设置库文件生成目录
set_target_properties(patroller_log PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)