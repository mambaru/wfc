add_executable(reader0_test reader0_test.cpp logger.cpp)
add_test(reader0_test reader0_test)
target_link_libraries(reader0_test wfc boost_system pthread)

add_executable(reader1_test reader1_test.cpp logger.cpp)
add_test(reader1_test reader1_test)
target_link_libraries(reader1_test wfc boost_system pthread)

add_executable(reader2_test reader2_test.cpp logger.cpp)
add_test(reader2_test reader2_test)
target_link_libraries(reader2_test wfc boost_system pthread)

add_executable(reader3_test reader3_test.cpp logger.cpp)
add_test(reader3_test reader3_test)
target_link_libraries(reader3_test wfc boost_system pthread)

add_executable(reader4_test reader4_test.cpp logger.cpp)
add_test(reader4_test reader4_test)
target_link_libraries(reader4_test wfc boost_system pthread)

add_executable(reader5_test reader5_test.cpp logger.cpp)
add_test(reader5_test reader5_test)
target_link_libraries(reader5_test wfc boost_system pthread)

add_executable(reader_rn0_test reader_rn0_test.cpp logger.cpp)
add_test(reader_rn0_test reader_rn0_test)
target_link_libraries(reader_rn0_test wfc boost_system pthread)

add_executable(reader_rn1_test reader_rn1_test.cpp logger.cpp)
add_test(reader_rn1_test reader_rn1_test)
target_link_libraries(reader_rn1_test wfc boost_system pthread)

add_executable(reader_rn2_test reader_rn2_test.cpp logger.cpp)
add_test(reader_rn2_test reader_rn2_test)
target_link_libraries(reader_rn2_test wfc boost_system pthread)

add_executable(reader_rn3_test reader_rn3_test.cpp logger.cpp)
add_test(reader_rn3_test reader_rn3_test)
target_link_libraries(reader_rn3_test wfc boost_system pthread)

add_executable(reader_rn4_test reader_rn4_test.cpp logger.cpp)
add_test(reader_rn4_test reader_rn4_test)
target_link_libraries(reader_rn4_test wfc boost_system pthread)

add_executable(reader_rn5_test reader_rn5_test.cpp logger.cpp)
add_test(reader_rn5_test reader_rn5_test)
target_link_libraries(reader_rn5_test wfc boost_system pthread)

#
# WRITER
#

add_executable(writer0_test writer0_test.cpp logger.cpp)
add_test(writer0_test writer0_test)
target_link_libraries(writer0_test wfc boost_system pthread)

add_executable(writer5_test writer5_test.cpp logger.cpp)
add_test(writer5_test writer5_test)
target_link_libraries(writer5_test wfc boost_system pthread)

add_executable(writer_rn5_test writer_rn5_test.cpp logger.cpp)
add_test(writer_rn5_test writer_rn5_test)
target_link_libraries(writer_rn5_test wfc boost_system pthread)


add_executable(connection1_test connection1_test.cpp logger.cpp)
add_test(connection1_test connection1_test)
target_link_libraries(connection1_test wfc boost_system pthread)

add_executable(accept1_test accept1_test.cpp logger.cpp)
add_test(accept1_test accept1_test)
target_link_libraries(accept1_test wfc boost_system pthread)
