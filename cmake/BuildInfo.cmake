GET_FILENAME_COMPONENT(CURRENT_SCRIPT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)

MACRO(build_info target_name display_name)

  set(build_dir ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}Info)
  set(build_prefix ${build_dir}/${display_name}_build_info)
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" cxx_build_type_flags_name)
  set(cxx_build_type_flags "${CMAKE_CXX_FLAGS} ${${cxx_build_type_flags_name}}")
  #set(cxx_build_type_flags "${CMAKE_CXX_FLAGS}")
  add_custom_target(
    ${display_name}_build_info
    COMMAND
      /bin/bash ${CURRENT_SCRIPT_DIRECTORY}/../build_info.sh
        ${display_name}
        ${build_dir}
        ${CMAKE_BUILD_TYPE}
        ${CMAKE_CXX_COMPILER}
        ${cxx_build_type_flags}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_dependencies(${target_name} ${display_name}_build_info)
  target_link_libraries(${target_name} PRIVATE ${build_prefix}.a)

  set(clean_list "${CMAKE_CURRENT_SOURCE_DIR}/${display_name}_build_info.h;${build_prefix}1.c;${build_prefix}2.c;${build_prefix}1.c~1;${build_prefix}.a;${build_prefix}1.o;${build_prefix}2.o;")
  SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_list}" )

ENDMACRO(build_info)

