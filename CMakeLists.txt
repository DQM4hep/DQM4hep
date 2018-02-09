#################################################
# cmake file for building DQMCore package
# @author Eté Rémi
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMCore )

set( ${PROJECT_NAME}_VERSION_MAJOR 2 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
include( CheckCXXCompilerFlag )

check_cxx_compiler_flag( "-std=c++11" COMPILER_SUPPORTS_CXX11 )

if( COMPILER_SUPPORTS_CXX11 )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
else()
	message( SEND_ERROR "DQMCore requires C++11 support. Please upgrade your compiler !" )
endif()

###############################
# Dependencies
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( dqm4hep_settings )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

###############################
# Define options
option( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- Find ROOT package -----
list( APPEND ROOT_COMPONENT_LIST Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree )
find_package( ROOT COMPONENTS ${ROOT_COMPONENT_LIST} REQUIRED )

# ----- Find Threads for multithreading -----
find_package( Threads REQUIRED )

# ----- Find MySQL package -----
find_package( MySQL REQUIRED )

# ----- Find xdrstream package -----
find_package( xdrstream REQUIRED )

# ----- Include package properties -----
foreach( pkg ROOT MySQL xdrstream )
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

###############################
# Generate DQMCoreConfig.h
configure_file( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )

###############################
# Add source directory
add_subdirectory( ./source )

###############################
# Install scripts files
file( 
  GLOB SCRIPTS_FILES scripts/*
	PATTERN "*~" EXCLUDE
  PATTERN "*#*" EXCLUDE
  PATTERN ".#*" EXCLUDE
  PATTERN "*CVS" EXCLUDE
  PATTERN "*.svn" EXCLUDE
)

install(
	FILES ${SCRIPTS_FILES}
	DESTINATION bin
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
)

###############################
# Install conf files
file( 
  GLOB CONF_FILES conf/*
	PATTERN "*~" EXCLUDE
  PATTERN "*#*" EXCLUDE
  PATTERN ".#*" EXCLUDE
  PATTERN "*CVS" EXCLUDE
  PATTERN "*.svn" EXCLUDE
)

install(
	FILES ${CONF_FILES}
	DESTINATION conf
)

install( 
  DIRECTORY cmake 
  DESTINATION . 
  PATTERN "*.in" EXCLUDE 
)

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
generate_package_configuration_files( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )
