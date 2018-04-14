#################################################
# cmake file for building DQMCore package
# @author Ete Remi, DESY
# Copyright (c)
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMCore )

set( ${PROJECT_NAME}_VERSION_MAJOR 2 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Central settings
include( dqm4hep_settings )

###############################
# Define options
option( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

# ----- Find ROOT package -----
dqm4hep_find_package(
  PACKAGE ROOT
  COMPONENTS Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree
  REQUIRED ADD_DEPENDS  
)

# ----- Find MySQL and xdrstream packages -----
dqm4hep_find_packages(
  PACKAGES MySQL xdrstream
  REQUIRED ADD_DEPENDS
)

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
# Add source directory
add_subdirectory( source )

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
dqm4hep_generate_package_confguration( PACKAGE_NAME ${PROJECT_NAME} )
