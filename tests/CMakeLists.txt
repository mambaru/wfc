include_directories(..)
include_directories(../../faslib)
include_directories(../../iow)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WFC_TEST_DIR})

enable_testing()
add_subdirectory(module)
#add_subdirectory(jsonrpc)
