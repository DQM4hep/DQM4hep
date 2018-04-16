#######################################################
# cmake file for building DQMCore package
# @author Ete Remi, DESY
# Copyright (c)
#######################################################

# -------------------------------------------------
# include directories
include_directories( BEFORE include )

# -------------------------------------------------
# require proper c++
add_definitions( "-pedantic -Wunused-value -O2" )
add_definitions("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the project dictionary
dqm4hep_create_dictionary( 
  DICT_FILE PROJECT_DICTIONARY
  DICTIONARY ${PROJECT_NAME}
  LINK_DEF ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/LinkDef.h
  HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/MonitorElement.h
  FLAGS "-Wno-unsequenced" # to avoid warning from json library
)

# -------------------------------------------------
# build the project library
aux_source_directory( src SRC_FILES )
aux_source_directory( src/plugins SRC_FILES )
add_shared_library( ${PROJECT_NAME} ${SRC_FILES} ${PROJECT_DICTIONARY} )
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib )
install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )

# -------------------------------------------------
# build all binaries in main directory
dqm4hep_add_main_executables()

# -------------------------------------------------
# build unit tests
if( DQM4HEP_TESTING )
  
  # Add all tests
  dqm4hep_add_tests( EXCEPT test-xmlparser )
  
  # Custom test with arguments
  dqm4hep_add_test( NAME test-xmlparser ARGS "${PROJECT_SOURCE_DIR}/tests/test_xmlparser.xml" )
  
  # Super-custom test
  configure_file( "${PROJECT_SOURCE_DIR}/tests/quality-test-test.in"
      "${PROJECT_BINARY_DIR}/bin/quality-test-test" @ONLY )
  
  install(
  	FILES ${PROJECT_BINARY_DIR}/bin/quality-test-test
  	DESTINATION bin/tests
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )
endif()

# -------------------------------------------------
# Add targets for clang-tidy/format
dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()


