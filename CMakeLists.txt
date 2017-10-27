#################################################
# cmake file for building DQMNet package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
PROJECT( DQMNet )

SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
	MESSAGE( SEND_ERROR "DQMNet requires C++11 support. Please upgrade your compiler !" )
ENDIF()

###############################
# Dependencies
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
INCLUDE( dqm4hep_settings )

###############################
# Define options

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
OPTION( DQMNET_WEBSOCKETS "Set to OFF to skip build/install web socket library" OFF )

# ----- Find Threads for multithreading -----
FIND_PACKAGE( Threads REQUIRED )

# ----- Find json cpp package -----
FIND_PACKAGE( JSONCPP REQUIRED )

# ----- Include package properties -----
FOREACH( pkg JSONCPP )

  INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
  ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )

  LIST( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${pkg}_INCLUDE_DIRS} )
  LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${pkg}_LIBRARY_DIRS} )
  LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${pkg}_LIBRARIES} )

ENDFOREACH()

# ----- Install doc -----
IF( INSTALL_DOC )
    FIND_PACKAGE( Doxygen )

    IF( DOXYGEN_FOUND )
        ADD_SUBDIRECTORY( ./doc )
    ELSE()
        MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
        MESSAGE( "Please install doxygen or set INSTALL_DOC to OFF" )
        MESSAGE( "" )
    ENDIF()
ENDIF()

# ----- Build web socket lib -----
IF( DQMNET_WEBSOCKETS )
  SET( BOOST_COMPONENTS system coroutine context thread )

  # Late 2017 TODO: remove the following checks and always use std::regex
  IF( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
      IF( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )
          SET( BOOST_COMPONENTS ${BOOST_COMPONENTS} regex )
          SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX" )
      ENDIF()
  ENDIF()

  # ----- Boost package -----
  FIND_PACKAGE( Boost 1.54.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED )
  INCLUDE_DIRECTORIES( SYSTEM ${Boost_INCLUDE_DIR} )

  # ----- OpenSSL package -----
  FIND_PACKAGE( OpenSSL REQUIRED )
  INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} )
ENDIF()

###############################
# Generate DQMNetConfig.h
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )

###############################
# Add source directory
ADD_SUBDIRECTORY( ./source )

###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

###############################
# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )
