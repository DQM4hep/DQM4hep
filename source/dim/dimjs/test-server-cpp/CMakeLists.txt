#################################################
# cmake file for building DQMCore package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
PROJECT( test-servers )

SET( ${PROJECT_NAME}_VERSION_MAJOR 2 )
SET( ${PROJECT_NAME}_VERSION_MINOR 3 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
	MESSAGE( SEND_ERROR "DQMCore requires C++11 support. Please upgrade your compiler !" )
ENDIF()

###############################
# Dependencies
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
INCLUDE( default_settings )

# ----- Find Threads for multithreading -----
FIND_PACKAGE( Threads REQUIRED )

# ----- Find DIM package -----
FIND_PACKAGE( DIM REQUIRED SHARED )

# ----- Find JSONCPP package -----
FIND_PACKAGE( JSONCPP REQUIRED SHARED )

# ----- Include package properties -----
FOREACH( pkg DIM JSONCPP )

  INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
  ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )

ENDFOREACH()

set(BOOST_COMPONENTS system coroutine context thread)
# Late 2017 TODO: remove the following checks and always use std::regex
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX")
    endif()
endif()
find_package(Boost 1.54.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

INCLUDE_DIRECTORIES( BEFORE src )

ADD_EXECUTABLE( test-server-1 src/test-server-1.cpp )

INSTALL (
    TARGETS test-server-1
    RUNTIME DESTINATION bin
)

target_link_libraries(test-server-1 ${Boost_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})


###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
