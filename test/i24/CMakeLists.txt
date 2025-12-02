set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME i24Test)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./i24Test.cpp

	${testedSourceDir}/common/int24.c
)

target_include_directories(${TEST_RUNNER_NAME} PUBLIC
	${testedSourceDir}
	${testedSourceDir}/common/
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	gtest
)
