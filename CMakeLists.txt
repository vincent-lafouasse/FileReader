cmake_minimum_required(VERSION 3.16)

project(FileReader VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(FILE_READER_BUILD_TESTS "Build and run tests using CTest" ON)

if(NOT DEFINED FILE_READER_BUFFER_SIZE)
  set(FILE_READER_BUFFER_SIZE
      "4096"
      CACHE STRING "Internal stack buffer size for FileReader")
endif()

add_library(${PROJECT_NAME})

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC FILE_READER_BUFFER_SIZE=${FILE_READER_BUFFER_SIZE})

target_sources(${PROJECT_NAME} PRIVATE src/FileReader.c src/takeLine.c)

target_include_directories(${PROJECT_NAME} PUBLIC ./include)

if(FILE_READER_BUILD_TESTS)
    include(FetchContent)
    include(cmake/fetch_gtest.cmake)

    include(CTest)
    enable_testing()

    add_subdirectory(test)
endif()
