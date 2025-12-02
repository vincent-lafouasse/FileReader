# a minimal buffered reader for POSIX file descriptors

who needs `stdio`

## configure buffer size
either
- add `set(FILE_READER_BUFFER_SIZE 4096 CACHE STRING "" FORCE)` to your CMakeLists.txt before adding the lib
- build with `cmake -D FILE_READER_BUFFER_SIZE=4096 # -B build etc`

the default buffer size of 4096 should be fine on most desktop computers
