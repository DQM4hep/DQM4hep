# - Try to find DIM include dirs and libraries
# This module finds the DIM library and includes. 
# Author: Barthelemy von Haller
#
# There are two components : STATIC and SHARED (uppercase!). 
# If none is indicated, we return the shared library. 
# Same thing if both are indicated.
# Note that the first call to find_package(DIM) will decide what
# component to choose. The subsequent calls are ignored. This is
# especially important if you need a package that itself needs DIM. 
# Be sure to call find_package(DIM xxx) before calling 
# find_package(package_depending_on_dim). 
#
# Users can overwrite DIM_LIBRARY in ccmake to force the use of 
# a different DIM library than the one found by this script.
# 
# This script will set the following variables:
#     DIM_FOUND - System has DIM
#     DIM_INCLUDE_DIRS - The DIM include directories
#     DIM_LIBRARIES - The libraries needed to use DIM
#     DIM_DEFINITIONS - Compiler switches required for using DIM
#
# Usage examples: 
#  find_package(DIM REQUIRED SHARED)
#  find_package(DIM)

# Initialization
include(FindPackageHandleStandardArgs)
UNSET(DIM_INCLUDE_DIRS)
UNSET(DIM_LIBRARIES)
SET(DIM_FOUND FALSE) 

# If DIM_DIR is available, set up our hints
IF( DIM_DIR )
    SET (DIM_INCLUDE_HINTS HINTS "${DIM_DIR}/include" "${DIM_DIR}")
    SET (DIM_LIBRARY_HINTS HINTS "${DIM_DIR}/lib")
ELSE()
  SET (DIM_INCLUDE_HINTS HINTS "/opt/dim" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/dim")
  SET (DIM_LIBRARY_HINTS HINTS "/opt/dim" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/dim")
ENDIF()

# find includes
find_path(DIM_INCLUDE_DIR dim.h
          PATHS ${DIM_INCLUDE_HINTS}  
          PATH_SUFFIXES dim)
LIST(APPEND DIM_INCLUDE_DIRS ${DIM_INCLUDE_DIR} )
          
# Warn or bail out if "SHARED" and "STATIC" have both been requested, and choose shared.
# Default to shared if no component specified
if ( DIM_FIND_REQUIRED_STATIC AND DIM_FIND_REQUIRED_SHARED)
    message(WARNING "Two incompatible components specified : static and shared. We are going to ignore the static component.")
    LIST(REMOVE_ITEM DIM_FIND_COMPONENTS static)
    UNSET(DIM_FIND_REQUIRED_STATIC) 
endif ( DIM_FIND_REQUIRED_STATIC AND DIM_FIND_REQUIRED_SHARED)
IF(NOT DIM_FIND_COMPONENTS)
    LIST(APPEND DIM_FIND_COMPONENTS shared)
    SET(DIM_FIND_REQUIRED_SHARED TRUE)
ENDIF(NOT DIM_FIND_COMPONENTS)

# DIM_FIND_REQUIRED_STATIC and DIM_FIND_REQUIRED_SHARED decide which flavour
# of the library to find. 
SET( DIMLIB_DIR linux)
if( APPLE )
 set( DIMLIB_DIR darwin ) 
endif()
if( DIM_FIND_REQUIRED_STATIC) 
    find_library(DIM_STATIC_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}dim${CMAKE_STATIC_LIBRARY_SUFFIX} PATHS ${DIM_LIBRARY_HINTS}  PATH_SUFFIXES ${DIMLIB_DIR})
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(DIM_STATIC DEFAULT_MSG DIM_STATIC_LIBRARY DIM_INCLUDE_DIR )
    IF(DIM_STATIC_FOUND)
        SET(DIM_LIBRARY ${DIM_STATIC_LIBRARY} CACHE FILEPATH "Path to the DIM library") 
    ENDIF() 
    LIST(REMOVE_ITEM DIM_FIND_COMPONENTS static)
    UNSET(DIM_FIND_REQUIRED_STATIC) 
elseif (DIM_FIND_REQUIRED_SHARED)
    find_library(DIM_SHARED_LIBRARY NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}dim${CMAKE_SHARED_LIBRARY_SUFFIX} PATHS ${DIM_LIBRARY_HINTS}  PATH_SUFFIXES ${DIMLIB_DIR})
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(DIM_SHARED DEFAULT_MSG DIM_SHARED_LIBRARY DIM_INCLUDE_DIR )
    IF(DIM_SHARED_FOUND)
        SET(DIM_LIBRARY ${DIM_SHARED_LIBRARY} CACHE FILEPATH "Path to the DIM library") 
    ENDIF() 
    LIST(REMOVE_ITEM DIM_FIND_COMPONENTS shared)
    UNSET(DIM_FIND_REQUIRED_SHARED) 
endif( DIM_FIND_REQUIRED_STATIC) 
set(DIM_LIBRARIES ${DIM_LIBRARY})


MESSAGE( STATUS "Check for DIM_INCLUDE_DIRS: ${DIM_INCLUDE_DIRS}" )
MESSAGE( STATUS "Check for DIM_LIBRARIES: ${DIM_LIBRARIES}" )

# handle the QUIETLY and REQUIRED arguments and set DIM_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DIM DEFAULT_MSG DIM_LIBRARIES DIM_INCLUDE_DIR)

mark_as_advanced(DIM_INCLUDE_DIR DIM_SHARED_LIBRARY DIM_STATIC_LIBRARY DIM_FORCE_STATIC)
