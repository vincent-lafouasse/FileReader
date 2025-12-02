# Fetch gtest through git
#
# Targets : `libgtest` `libgmock`

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

find_package(Threads REQUIRED)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        DOWNLOAD_NO_PROGRESS TRUE
      )
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
