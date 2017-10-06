#######################################################
# cmake file for building DQMCore package
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

# include directories
INCLUDE_DIRECTORIES( BEFORE include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library

SET (
  ${PROJECT_NAME}DictHeaders
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/MonitorElement.h
)

SET( DICT_INCLUDE_DIRS "" )
GET_DIRECTORY_PROPERTY(IncludeDirs INCLUDE_DIRECTORIES)

FOREACH( dir ${IncludeDirs} )
   SET( DICT_INCLUDE_DIRS ${DICT_INCLUDE_DIRS}\t-I${dir} )
ENDFOREACH(dir)

SET( LIB_HEADERS_PKG ${${PROJECT_NAME}DictHeaders} )
SET( LINK_DEF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/LinkDef.h )
SET( ROOT_DICT_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dict" )
SET( ROOT_PCM_FILE_NAME "${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict_rdict.pcm" )

ADD_CUSTOM_COMMAND( OUTPUT ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.h
                   COMMAND mkdir -p ${ROOT_DICT_OUTPUT_DIR}
                   COMMAND ${ROOT_CINT_EXECUTABLE} -f ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc -c ${DICT_INCLUDE_DIRS} -p ${LIB_HEADERS_PKG} ${LINK_DEF_FILE}
                   COMMENT "Generating ROOT dictionary for DQMCore library."
                   DEPENDS ${LIB_HEADERS_PKG}
)

# AUX_SOURCE_DIRECTORY( src SRC_FILES )
SET( SRC_FILES ${SRC_FILES} src/QualityTest.cc )
SET( SRC_FILES ${SRC_FILES} src/Logger.cc )
SET( SRC_FILES ${SRC_FILES} src/DBInterface.cc )
SET( SRC_FILES ${SRC_FILES} src/tinystr.cc )
SET( SRC_FILES ${SRC_FILES} src/tinyxml.cc )
SET( SRC_FILES ${SRC_FILES} src/tinyxmlerror.cc )
SET( SRC_FILES ${SRC_FILES} src/tinyxmlparser.cc )
SET( SRC_FILES ${SRC_FILES} src/XmlHelper.cc )
SET( SRC_FILES ${SRC_FILES} src/Event.cc )
SET( SRC_FILES ${SRC_FILES} src/Path.cc )
SET( SRC_FILES ${SRC_FILES} src/PluginManager.cc )
SET( SRC_FILES ${SRC_FILES} src/Plugin.cc )
SET( SRC_FILES ${SRC_FILES} src/GenericEvent.cc )
SET( SRC_FILES ${SRC_FILES} src/Run.cc )
SET( SRC_FILES ${SRC_FILES} src/Version.cc )
SET( SRC_FILES ${SRC_FILES} src/MonitorElement.cc )
SET( SRC_FILES ${SRC_FILES} src/MonitorElementManager.cc )

AUX_SOURCE_DIRECTORY( src/qtest SRC_FILES )

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc )

INSTALL(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

INSTALL(
	FILES ${ROOT_PCM_FILE_NAME}
	DESTINATION lib
)

FILE(GLOB MAIN_SRCS "main/*.cc")

FOREACH( MAIN_SRC ${MAIN_SRCS} )
  GET_FILENAME_COMPONENT( MAIN_SRC_WITHOUT_EXT ${MAIN_SRC} NAME_WE )
  ADD_EXECUTABLE( ${MAIN_SRC_WITHOUT_EXT} main/${MAIN_SRC_WITHOUT_EXT}.cc )
  TARGET_LINK_LIBRARIES( ${MAIN_SRC_WITHOUT_EXT} ${PROJECT_NAME} )
  INSTALL (
      TARGETS ${MAIN_SRC_WITHOUT_EXT}
      RUNTIME DESTINATION bin
  )
ENDFOREACH()


IF( BUILD_TESTS )
  ENABLE_TESTING()
  FILE(GLOB TEST_SRCS "tests/*.cc")

  FOREACH( TEST_SRC ${TEST_SRCS} )
    GET_FILENAME_COMPONENT( TEST_SRC_WITHOUT_EXT ${TEST_SRC} NAME_WE )
    ADD_EXECUTABLE( ${TEST_SRC_WITHOUT_EXT} tests/${TEST_SRC_WITHOUT_EXT}.cc )
    TARGET_LINK_LIBRARIES( ${TEST_SRC_WITHOUT_EXT} ${PROJECT_NAME} )
    INSTALL (
        TARGETS ${TEST_SRC_WITHOUT_EXT}
        RUNTIME DESTINATION bin/tests
    )
    ADD_TEST( ${TEST_SRC_WITHOUT_EXT}-test ${PROJECT_BINARY_DIR}/bin/${TEST_SRC_WITHOUT_EXT} )
  ENDFOREACH()
ENDIF()


INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )
