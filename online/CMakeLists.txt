#################################################
# cmake file for building DQMOnline package
# @author Eté Rémi
# Copyright (c)
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
#################################################

###############################
# project name
project( DQMOnline )

set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_PATCH 3 )

###############################
# Central settings
include( dqm4hep_settings )

###############################
# Define options
option( BUILD_HTTP_RC  "Set to OFF to skip build/install http run control plugin" OFF )

# ----- Find DQMCore package -----
dqm4hep_find_packages( 
  PACKAGES DQMCore DQMNet
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
dqm4hep_generate_package_configuration( PACKAGE ${PROJECT_NAME} )
