cmake_minimum_required(VERSION 3.8)
project(trantor CXX)
set(PROJECT_VERSION 1.0.1)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (${CMAKE_CXX_STANDARD} LESS 20)
    message(FATAL_ERROR "trantor requires C++20 or later")
endif ()

find_package(SQLite3 REQUIRED)

add_library(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(SQLite3 REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE SQLite::SQLite3)
