set(libName FileReader)
add_library(${libName})

target_sources(${libName}
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

