
# macro to add a single executable 
macro( DQM4HEP_ADD_EXECUTABLE _dir _name _install_loc )
	add_executable( ${_name} ${PROJECT_SOURCE_DIR}/source/${_dir}/${_name}.cc )
	target_link_libraries( ${_name} ${PROJECT_NAME} )
	install (
	    TARGETS ${_name}
	    RUNTIME DESTINATION ${_install_loc}
      PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
	)
endmacro()


# macro to add a single test 
macro( DQM4HEP_ADD_TEST )
  
  # parse the macro arguments
  set( FLAG_OPTIONS DEBUG )
  set( SINGLE_OPTIONS NAME )
  set( MULTI_OPTIONS ARGS )
  cmake_parse_arguments( TEST_BIN "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  if( NOT TEST_BIN_NAME )
    message( SEND_ERROR "Add test: no test name specified !" )
  endif()
  
  dqm4hep_add_executable( tests ${TEST_BIN_NAME} bin/tests )
  add_test( ${TEST_BIN_NAME}-test ${PROJECT_BINARY_DIR}/bin/${TEST_BIN_NAME} ${TEST_BIN_ARGS} )
  
  if( TEST_BIN_DEBUG )
    message( STATUS "Adding unit test '${TEST_BIN_NAME}'" )
  endif()
  
endmacro()

# macro to add all executable in the main directory
macro( DQM4HEP_ADD_MAIN_EXECUTABLES )
  # parse the macro arguments
  set( FLAG_OPTIONS "" )
  set( SINGLE_OPTIONS "" )
  set( MULTI_OPTIONS EXCEPT )
  cmake_parse_arguments( MAIN_BINS "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  file( GLOB MAIN_SRCS "${PROJECT_SOURCE_DIR}/source/main/*.cc" )

  foreach( MAIN_SRC ${MAIN_SRCS} )
    get_filename_component( MAIN_SRC_NO_EXT ${MAIN_SRC} NAME_WE )
    # check if you should skip this executable
    list( FIND MAIN_BINS_EXCEPT ${MAIN_SRC_NO_EXT} SKIP_BIN )
    if( NOT ${SKIP_BIN} GREATER -1 )
      # add executable
      dqm4hep_add_executable( main ${MAIN_SRC_NO_EXT} bin )
    endif()
  endforeach()
endmacro()




# # macro to add all executable in the test directory
macro( DQM4HEP_ADD_TESTS )
  
  # parse the macro arguments
  set( FLAG_OPTIONS "" )
  set( SINGLE_OPTIONS "" )
  set( MULTI_OPTIONS EXCEPT )
  cmake_parse_arguments( TEST_BINS "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  file( GLOB TEST_SRCS "${PROJECT_SOURCE_DIR}/source/tests/*.cc" )

  foreach( TEST_SRC ${TEST_SRCS} )
    get_filename_component( TEST_SRC_NO_EXT ${TEST_SRC} NAME_WE )
    # check if you should skip this test
    list( FIND TEST_BINS_EXCEPT ${TEST_SRC_NO_EXT} SKIP_TEST )
    if( NOT ${SKIP_TEST} GREATER -1 )
      # add test
      dqm4hep_add_test( NAME ${TEST_SRC_NO_EXT} )
    endif()
  endforeach()
endmacro()

