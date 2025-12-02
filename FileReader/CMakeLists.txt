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

