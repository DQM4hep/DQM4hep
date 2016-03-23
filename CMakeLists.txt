#################################################
# cmake file for building DQMCore package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
PROJECT( DQMCore )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
	LIST( APPEND CMAKE_CXX_FLAGS "-std=c++11" )
ELSE()
	MESSAGE( SEND_ERROR "DQMCore requires C++11 support. Please upgrade your compiler !" )
ENDIF()

###############################
# Dependencies
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
INCLUDE( default_settings )

###############################
# Define options

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
OPTION( BUILD_EXAMPLES "Whether to build DQM4HEP examples" OFF )

###############################
FIND_PROGRAM( ELOG_EXECUTABLE elog HINTS /usr/local/bin /usr/bin )

IF( ELOG_EXECUTABLE )
    MESSAGE( STATUS "Check for ELOG_EXECUTABLE: ${ELOG_EXECUTABLE} -- ok" )
    SET( DQM4HEP_USE_ELOG 1 )
    ADD_DEFINITIONS( -DDQM4HEP_USE_ELOG )
ELSE()
    MESSAGE( STATUS "Check for ELOG_EXECUTABLE: -- not found !" )
    SET( DQM4HEP_USE_ELOG 0 )
ENDIF()

# ----- Find ROOT package -----
LIST( APPEND ROOT_COMPONENT_LIST Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree )
FIND_PACKAGE( ROOT COMPONENTS ${ROOT_COMPONENT_LIST} REQUIRED )

# ----- Find Threads for multithreading -----
FIND_PACKAGE( Threads REQUIRED )

# ----- Find MySQL package -----
FIND_PACKAGE( MySQL REQUIRED )

# ----- Find DIM package -----
FIND_PACKAGE( DIM REQUIRED SHARED )

# ----- Find xdrstream package -----
FIND_PACKAGE( xdrstream REQUIRED )

# ----- Find LOG4CXX package -----
FIND_PACKAGE( LOG4CXX REQUIRED )

# ----- Include package properties -----
FOREACH( pkg ROOT DIM MySQL xdrstream LOG4CXX )

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

###############################
# Generate DQMCoreConfig.h 
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )
    
###############################
# Add source directory
ADD_SUBDIRECTORY( ./source )

###############################
# Install scripts files
FILE ( GLOB SCRIPTS_FILES scripts/* 
	PATTERN "*~" EXCLUDE
    PATTERN "*#*" EXCLUDE
    PATTERN ".#*" EXCLUDE
    PATTERN "*CVS" EXCLUDE
    PATTERN "*.svn" EXCLUDE
)

INSTALL( 
	FILES ${SCRIPTS_FILES} 
	DESTINATION bin 
    PERMISSIONS 
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

###############################
# Install conf files
FILE ( GLOB CONF_FILES conf/* 
	PATTERN "*~" EXCLUDE
    PATTERN "*#*" EXCLUDE
    PATTERN ".#*" EXCLUDE
    PATTERN "*CVS" EXCLUDE
    PATTERN "*.svn" EXCLUDE
)

INSTALL( 
	FILES ${CONF_FILES} 
	DESTINATION conf
)

###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

###############################
# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

