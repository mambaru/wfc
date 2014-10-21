enable_testing()


include_directories(..)
include_directories(../faslib/)

add_subdirectory(logger)
add_subdirectory(core)
add_subdirectory(wfc)
add_subdirectory(pubsub)
add_subdirectory(io)
add_subdirectory(provider)
add_subdirectory(jsonrpc)
add_subdirectory(service)
