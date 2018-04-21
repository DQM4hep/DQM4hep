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
dqm4hep_build_doxygen()

###############################
# Add source directory
add_subdirectory( source )

###############################
# display some variables and write them to cache
display_std_variables()

###############################
# generate and install following configuration files
dqm4hep_generate_package_confguration( PACKAGE_NAME ${PROJECT_NAME} )
