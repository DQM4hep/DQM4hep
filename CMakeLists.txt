#################################################
# cmake file for building DQMNet package
# @author Ete Remi
# Copyright (c)
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMNet )

set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 0 )
set( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Central settings
include( dqm4hep_settings )

###############################
# Define options
option( DQMNET_WEBSOCKETS "Set to OFF to skip build/install web socket library" OFF )

# ----- Find Threads for multithreading -----
find_package( Threads REQUIRED )

# ----- Find DQMCore package -----
dqm4hep_find_packages(
  PACKAGES DQMCore
  REQUIRED ADD_DEPENDS
)

# ----- Install doc -----
dqm4hep_build_doxygen()

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
  dqm4hep_find_package(
    PACKAGE Boost
    VERSION 1.54.0
    COMPONENTS ${BOOST_COMPONENTS}
    REQUIRED ADD_DEPENDS
  )

  # ----- OpenSSL package -----
  dqm4hep_find_package(
    PACKAGE OpenSSL
    REQUIRED ADD_DEPENDS
   )
endif()

###############################
# Add source directory
add_subdirectory( source )

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
dqm4hep_generate_package_configuration(
  PACKAGE_NAME ${PROJECT_NAME}
  LIBRARIES ${PROJECT_NAME} dim
)
