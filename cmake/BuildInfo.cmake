MACRO(build_info target_name prefix)
  add_custom_target(
    ${prefix}_build_info
    COMMAND
      /bin/bash ${CMAKE_SOURCE_DIR}/comet/build_info.sh ${prefix}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Create build info files"
    VERBATIM
  )
  add_dependencies(${target_name} ${prefix}_build_info)
  target_link_libraries(${target_name} /tmp/${prefix}_build_info.o)
ENDMACRO(build_info)