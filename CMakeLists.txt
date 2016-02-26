#################################################
# cmake file for building DQMCore package
# @author Eté Rémi
# Copyright (c) CNRS / IPNL
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( DQMCore )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# ----- dependencies -----
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# load default settings
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES QUIET )

IF( ILCUTIL_FOUND )
	INCLUDE( ilcsoft_default_settings )
ELSE()
	INCLUDE( default_settings )
ENDIF()


###############################
# Define options

# Generate DOXYGEN documentation (optionnal)
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# Compile the LCIO streamer plugins to send lcio data through the network
OPTION( DQM4HEP_LCIO "LCIO option to build lcio streamer for network transfer" OFF )

# Compile module examples 
OPTION( DQM4HEP_EXAMPLES "Build examples" OFF )

# Compile the elog interface
OPTION( DQM4HEP_ELOG        "ELOG option to build Elog interface" OFF )


###############################
# Build elog interface
IF( DQM4HEP_ELOG )

    MESSAGE( STATUS "Check for ELOG" )

    FIND_PROGRAM( ELOG_EXECUTABLE elog HINTS /usr/local/bin /usr/bin )
    
    IF( ELOG_EXECUTABLE )
        MESSAGE( STATUS "Check for ELOG_EXECUTABLE: ${ELOG_EXECUTABLE} -- ok" )
        SET( DQM4HEP_USE_ELOG 1 )
        ADD_DEFINITIONS( -DDQM4HEP_USE_ELOG )
    ELSE()
        SET( DQM4HEP_USE_ELOG 0 )
        MESSAGE( SEND_ERROR "ELOG executable not found !" )
        MESSAGE( "Please unset the ELOG option or install it !" )
        MESSAGE( "" )
    ENDIF()
    
ELSE()
    SET( DQM4HEP_USE_ELOG 0 )
ENDIF()


# ----- Find lcio package -----
IF( DQM4HEP_LCIO )

    FIND_PACKAGE( LCIO REQUIRED )
    
    INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${LCIO_LIBRARIES} )
    ADD_DEFINITIONS ( ${LCIO_DEFINITIONS} )
    
    ADD_DEFINITIONS( -DDQM4HEP_USE_LCIO )
    
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

# ----- Find streamlog package -----
FIND_PACKAGE( streamlog REQUIRED )

# ----- Find xdrstream package -----
FIND_PACKAGE( xdrstream REQUIRED )

# ----- Include package properties -----
FOREACH( pkg streamlog ROOT DIM MySQL xdrstream )

  INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
  ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )

ENDFOREACH()


# ----- Export ${PROJECT_NAME}_DEPENDS* to ${PROJECT_NAME}Config.cmake
SET( ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}  ${DIM_INCLUDE_DIRS} ${streamlog_INCLUDE_DIRS})
SET( ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${ROOT_LIBRARY_DIR}   ${DIM_LIBRARY_DIRS} ${streamlog_LIBRARY_DIRS})
SET( ${PROJECT_NAME}_DEPENDS_LIBRARIES    ${ROOT_LIBRARIES}     ${DIM_LIBRARIES}    ${streamlog_LIBRARIES})

IF( DQM4HEP_LCIO )
    LIST( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${LCIO_INCLUDE_DIRS} )
    LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${LCIO_LIBRARY_DIRS} )
    LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES    ${LCIO_LIBRARIES}    )
ENDIF()


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
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

###############################
# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

