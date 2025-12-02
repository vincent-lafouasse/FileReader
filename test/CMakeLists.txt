set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME FileReaderTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./FileReaderTest.cpp
	./TakeLineTest.cpp
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	gtest
	FileReader
)
