GET_FILENAME_COMPONENT(CURRENT_SCRIPT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
set(ENV{BUILD_INFO_SCRIPT} ${CURRENT_SCRIPT_DIRECTORY}/build_info.sh)

MACRO(build_info)

  set(target_name ${ARGV0})

  if ( "${ARGV1}" STREQUAL "" )
    set(display_name ${target_name})
  else()
    set(display_name ${ARGV1})
  endif()

  set(build_dir ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}Info)
  set(build_prefix ${build_dir}/${display_name}_build_info)

  get_target_property( target_opt ${target_name} COMPILE_OPTIONS)
  string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" cxx_build_type_flags_name)
  set(cxx_build_type_flags "${CMAKE_CXX_FLAGS};${${cxx_build_type_flags_name}};${target_opt}")
  list(REMOVE_DUPLICATES cxx_build_type_flags)
  string (REPLACE ";" " " cxx_build_type_flags_str "${cxx_build_type_flags}")

  add_custom_target(
    ${display_name}_build_info
    COMMAND
      /bin/bash $ENV{BUILD_INFO_SCRIPT}
        "${display_name}"
        "${build_dir}"
        "${CMAKE_BUILD_TYPE}"
        "${CMAKE_CXX_COMPILER}"
        "${cxx_build_type_flags_str}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_dependencies(${target_name} ${display_name}_build_info)
  target_link_libraries(${target_name} PRIVATE ${build_prefix}.a)

  set(clean_list "${CMAKE_CURRENT_SOURCE_DIR}/${display_name}_build_info.h;${build_prefix}1.c;${build_prefix}2.c;${build_prefix}1.c~1;${build_prefix}.a;${build_prefix}1.o;${build_prefix}2.o;")
  SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${clean_list}" )
ENDMACRO(build_info)


