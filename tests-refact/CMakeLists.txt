include_directories(..)
include_directories(../../faslib)
include_directories(../../iow)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WFC_TEST_DIR})

enable_testing()
add_subdirectory(module)

#add_subdirectory(logger)
#add_subdirectory(wfc)
#add_subdirectory(pubsub)
#add_subdirectory(io)
#add_subdirectory(provider)
#add_subdirectory(jsonrpc)
#add_subdirectory(service)
