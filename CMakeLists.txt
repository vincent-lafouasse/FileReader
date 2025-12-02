cmake_minimum_required(VERSION 3.16)

project(FileReader VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fdiagnostics-color=always -g3")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	src/FileReader.c

PUBLIC
	FILE_SET myHeaders
	TYPE HEADERS
	BASE_DIRS
		include
	FILES
		include/FileReader.h
)

include(FetchContent)
include(cmake/fetch_gtest.cmake)
add_subdirectory(test)

include(CTest)
enable_testing()

add_test(NAME FileReaderTest
    COMMAND FileReaderTest
)
