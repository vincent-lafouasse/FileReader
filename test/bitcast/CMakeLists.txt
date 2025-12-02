set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME bitcastTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./bitcastTest.cpp
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	gtest
	bitcast
)
