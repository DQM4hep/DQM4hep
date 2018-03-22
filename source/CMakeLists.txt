#######################################################
# cmake file for building DQMCore package
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

# include directories
include_directories( BEFORE include )

# require proper c++
add_definitions( "-pedantic -Wunused-value -O2" )
add_definitions("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library

set(
  ${PROJECT_NAME}DictHeaders
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/MonitorElement.h
)

set( DICT_INCLUDE_DIRS "" )

get_directory_property( IncludeDirs INCLUDE_DIRECTORIES )

foreach( dir ${IncludeDirs} )
   set( DICT_INCLUDE_DIRS ${DICT_INCLUDE_DIRS}\t-I${dir} )
endforeach(dir)

set( LIB_HEADERS_PKG ${${PROJECT_NAME}DictHeaders} )
set( LINK_DEF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/LinkDef.h )
set( ROOT_DICT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/dict" )
set( ROOT_PCM_FILE_NAME "${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict_rdict.pcm" )

add_custom_command( 
  OUTPUT ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict_rdict.pcm
  COMMAND mkdir -p ${ROOT_DICT_OUTPUT_DIR}
  COMMAND ${ROOT_CINT_EXECUTABLE} -f ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc -Wno-unsequenced  ${DICT_INCLUDE_DIRS} ${LIB_HEADERS_PKG} ${LINK_DEF_FILE}
  COMMENT "Generating ROOT dictionary for DQMCore library ..."
  DEPENDS ${LIB_HEADERS_PKG}
)

aux_source_directory( src SRC_FILES )
aux_source_directory( src/qtest SRC_FILES )

add_shared_library( ${PROJECT_NAME} ${SRC_FILES} ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc )
target_link_libraries( ${PROJECT_NAME} ${MySQL_LIBRARIES} ${JSONCPP_LIBRARIES} ${xdrstream_LIBRARIES} )
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib )
install( FILES ${ROOT_PCM_FILE_NAME} DESTINATION lib )

# -------------------------------------------------
# build all binaries in main directory
file( GLOB MAIN_SRCS "main/*.cc" )

foreach( MAIN_SRC ${MAIN_SRCS} )
  get_filename_component( MAIN_SRC_WITHOUT_EXT ${MAIN_SRC} NAME_WE )
  add_executable( ${MAIN_SRC_WITHOUT_EXT} main/${MAIN_SRC_WITHOUT_EXT}.cc )
  target_link_libraries( ${MAIN_SRC_WITHOUT_EXT} ${PROJECT_NAME} )
  install (
      TARGETS ${MAIN_SRC_WITHOUT_EXT}
      RUNTIME DESTINATION bin
  )
endforeach()


# -------------------------------------------------
# build unit tests
if( BUILD_TESTS )
  enable_testing()
  
  file( GLOB TEST_SRCS "tests/*.cc" )

  foreach( TEST_SRC ${TEST_SRCS} )
    get_filename_component( TEST_SRC_WITHOUT_EXT ${TEST_SRC} NAME_WE )
    add_executable( ${TEST_SRC_WITHOUT_EXT} tests/${TEST_SRC_WITHOUT_EXT}.cc )
    target_link_libraries( ${TEST_SRC_WITHOUT_EXT} ${PROJECT_NAME} )
    install (
        TARGETS ${TEST_SRC_WITHOUT_EXT}
        RUNTIME DESTINATION bin/tests
    )
    
    set( TEST_ARGS "" )
    
    if( ${TEST_SRC_WITHOUT_EXT} STREQUAL "test-xmlparser" )
      set( TEST_ARGS "${PROJECT_SOURCE_DIR}/tests/test_xmlparser.xml" )
    endif()
    
    add_test( ${TEST_SRC_WITHOUT_EXT}-test ${PROJECT_BINARY_DIR}/bin/${TEST_SRC_WITHOUT_EXT} ${TEST_ARGS} )
  endforeach()

  configure_file( "${PROJECT_SOURCE_DIR}/cmake/quality-test-test.in"
      "${PROJECT_BINARY_DIR}/bin/quality-test-test" @ONLY )

  install(
  	FILES ${PROJECT_BINARY_DIR}/bin/quality-test-test
  	DESTINATION bin/tests
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

  add_test( quality-test-test ${CMAKE_INSTALL_PREFIX}/bin/tests/quality-test-test )
endif()

dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()

# -------------------------------------------------
# install headers
install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )
