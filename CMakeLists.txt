#################################################
# cmake file for building DQMNet package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMNet )

set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 0 )
set( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
include( CheckCXXCompilerFlag )

check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

if( COMPILER_SUPPORTS_CXX11 )
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message( SEND_ERROR "DQMNet requires C++11 support. Please upgrade your compiler !" )
endif()

###############################
# Dependencies
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( dqm4hep_settings )

###############################
# Define options

option( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
option( DQMNET_WEBSOCKETS "Set to OFF to skip build/install web socket library" OFF )

# ----- Find Threads for multithreading -----
find_package( Threads REQUIRED )

# ----- Find DQMCore package -----
find_package( DQMCore REQUIRED )

# ----- Include package properties -----
foreach( pkg DQMCore )

  include_directories( ${${pkg}_INCLUDE_DIRS} )
  link_libraries( ${${pkg}_LIBRARIES} )
  add_definitions ( ${${pkg}_DEFINITIONS} )

  list( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${pkg}_INCLUDE_DIRS} )
  list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${pkg}_LIBRARY_DIRS} )
  list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${pkg}_LIBRARIES} )

endforeach()

# ----- Install doc -----
if( INSTALL_DOC )
    find_package( Doxygen )

    if( DOXYGEN_FOUND )
        add_subdirectory( ./doc )
    else()
        message( SEND_ERROR "Could not find doxygen required to build documentation" )
        message( "Please install doxygen or set INSTALL_DOC to OFF" )
        message( "" )
    endif()
endif()

# ----- Build web socket lib -----
if( DQMNET_WEBSOCKETS )
  set( BOOST_COMPONENTS system coroutine context thread )

  # Late 2017 TODO: remove the following checks and always use std::regex
  if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
      if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )
          set( BOOST_COMPONENTS ${BOOST_COMPONENTS} regex )
          set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX" )
      endif()
  endif()

  # ----- Boost package -----
  find_package( Boost 1.54.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED )
  include_directories( SYSTEM ${Boost_INCLUDE_DIR} )

  # ----- OpenSSL package -----
  find_package( OpenSSL REQUIRED )
  include_directories( ${OPENSSL_INCLUDE_DIR} )
endif()

###############################
# Generate DQMNetConfig.h
configure_file( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )

###############################
# Add source directory
add_subdirectory( ./source )

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
generate_package_configuration_files( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )
