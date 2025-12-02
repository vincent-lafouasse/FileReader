set(testedSourceDir ${CMAKE_SOURCE_DIR}/src)

set(TEST_RUNNER_NAME OptionsTest)

add_executable(${TEST_RUNNER_NAME})
target_sources(${TEST_RUNNER_NAME}
PRIVATE
	./main.cpp
	./OptionsTest.cpp

	${testedSourceDir}/options/Options.c
	${testedSourceDir}/options/Flag.c
)

target_include_directories(${TEST_RUNNER_NAME} PUBLIC
	${testedSourceDir}
	${testedSourceDir}/common/
)

target_link_libraries(${TEST_RUNNER_NAME}
PRIVATE
	gtest
)
