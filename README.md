# a minimal buffered reader for POSIX file descriptors

who needs `stdio`

## configure buffer size
either
- add `set(FILE_READER_BUFFER_SIZE 4096 CACHE STRING "" FORCE)` to your CMakeLists.txt before adding the lib
- build with `cmake -B build -DFILE_READER_BUFFER_SIZE=4096`

the default buffer size of 4096 should be fine on most desktop computers
