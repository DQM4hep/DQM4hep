########################################################
# Top-level CMakeLists.txt to build DQM4hep
# @author Remi Ete, DESY
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
########################################################

# ----- Project name and version -----
project( DQM4hep )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_PATCH 4 )

# ----- CMake settings -----
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( dqm4hep_settings )
include( dqm4hep_check_package_dependencies )
include( ExternalProject )
list( APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake )
list( APPEND DQM4HEP_INSTALL_PACKAGES xdrstream DQMCore DQMNet DQMOnline )
macro( set_if_not )
  if( NOT ${ARGV0} )
    set( ${ARGN} )
  endif()
endmacro()

# ----- Check dependencies and various settings -----
dqm4hep_check_package_dependencies()
dqm4hep_require_cpp_standard()

# ----- Package options -----------------------------------------------------------------------
option( DQM4HEP_USE_MASTER       "Whether to use master version of subpackages" ON )
option( DQM4HEP_BUILD_OFFLINE    "Build offline" OFF )
option( DQM4HEP_BUILD_MKDOCS     "Download and build mkdocs documentation" OFF )

# ----- Versions of DQM4HEP subpackages -----
# Don't forget to update when releasing DQM4hep
# e.g. set( DQMCore_version "v01-00-00" )
if( DQM4HEP_USE_MASTER )
	set_if_not( xdrstream_version            "master" )
	set_if_not( DQMCore_version              "master" )
  set_if_not( DQMNet_version               "master" )
  set_if_not( DQMOnline_version            "master" )
  set_if_not( DQMDoc_version               "master" )
else()
	set_if_not( xdrstream_version            "v01-01-00" )
  set_if_not( DQMCore_version              "master" )
  set_if_not( DQMNet_version               "master" )
  set_if_not( DQMOnline_version            "master" )
  set_if_not( DQMDoc_version               "master" )
endif()

# ----- Download settings -----
# The external project in used are set gradually along the CMakeLists.txt
# When a project is added with ExternalProject_Add(...) these variables are set to the package name
# Mandatory packages are directly set
set_if_not( xdrstream_repository           "https://github.com/dqm4hep/xdrstream.git" )
set_if_not( DQMCore_repository             "https://github.com/dqm4hep/dqm4hep-core.git" )
set_if_not( DQMNet_repository              "https://github.com/dqm4hep/dqm4hep-net.git" )
set_if_not( DQMOnline_repository           "https://github.com/dqm4hep/dqm4hep-online.git" )
set_if_not( DQMDoc_repository              "https://github.com/dqm4hep/dqm4hep-doc.git" )

# In order to pass semicolon-separated lists over the command line to all packages,
# we need to separate list elements with '%' instead of the standard cmake list separator ';'
# The list separator needs also to be redefined in the ExternalProject_Add calls by setting
# the variable LIST_SEPARATOR
foreach( _path ${CMAKE_PREFIX_PATH} )
  set( CMAKE_PREFIX_PATH_FIXED ${CMAKE_PREFIX_PATH_FIXED}%${_path} )
endforeach()

foreach( _path ${CMAKE_MODULE_PATH} )
  set( CMAKE_MODULE_PATH_FIXED ${CMAKE_MODULE_PATH_FIXED}%${_path} )
endforeach()

# ----- cmake arguments common to ALL cmake packages -------------------------
set( DQM4HEP_COMMON_CMAKE_ARGS
  "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
  "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
  "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
  "-DDQM4HEP_DOXYGEN_DOC=${DQM4HEP_DOXYGEN_DOC}"
  "-DDQM4HEP_WARNING_AS_ERROR=${DQM4HEP_WARNING_AS_ERROR}"
  "-DDQM4HEP_EXTRA_WARNINGS=${DQM4HEP_EXTRA_WARNINGS}"
  "-DDQM4HEP_DEV_WARNINGS=${DQM4HEP_EXTRA_WARNINGS}"
  "-DDQM4HEP_TESTING=${DQM4HEP_TESTING}"
)

if( DQM4HEP_BUILD_OFFLINE )
  message( WARNING "Building framework offline - No update will be performed !")
  set_property( DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY EP_UPDATE_DISCONNECTED 1 )
endif()

if( DQM4HEP_TESTING )
  set( DQM4HEP_TEST_COMMAND "ctest" )
endif()

# TODO create an example package
# option( BUILD_EXAMPLES "Build DQM4hep examples" OFF )
# message( STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLES}" )

# ----- xdrstream package -----
ExternalProject_Add( xdrstream
    GIT_REPOSITORY ${xdrstream_repository}
    GIT_TAG ${xdrstream_version}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DINSTALL_DOC=${DQM4HEP_DOXYGEN_DOC}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xdrstream-prefix
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/xdrstream
    LIST_SEPARATOR %
    UPDATE_COMMAND ""
)

# ----- DQMCore package -----
ExternalProject_Add( DQMCore
    DEPENDS xdrstream
    GIT_REPOSITORY ${DQMCore_repository}
    GIT_TAG ${DQMCore_version}
    CMAKE_ARGS ${DQM4HEP_COMMON_CMAKE_ARGS}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/DQMCore-prefix
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/DQMCore
    UPDATE_COMMAND ""
    TEST_COMMAND ${DQM4HEP_TEST_COMMAND}
)

# ----- DQMNet package -----
ExternalProject_Add( DQMNet
    DEPENDS xdrstream DQMCore
    GIT_REPOSITORY ${DQMNet_repository}
    GIT_TAG ${DQMNet_version}
    CMAKE_ARGS ${DQM4HEP_COMMON_CMAKE_ARGS}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/DQMNet-prefix
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/DQMNet
    UPDATE_COMMAND ""
)

# ----- DQMOnline package -----
ExternalProject_Add( DQMOnline
    DEPENDS xdrstream DQMCore DQMNet
    GIT_REPOSITORY ${DQMOnline_repository}
    GIT_TAG ${DQMOnline_version}
    CMAKE_ARGS ${DQM4HEP_COMMON_CMAKE_ARGS}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/DQMOnline-prefix
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/DQMOnline
    UPDATE_COMMAND ""
    TEST_COMMAND ${DQM4HEP_TEST_COMMAND}
)

if( DQM4HEP_BUILD_MKDOCS )
  # ----- DQMDoc package -----
  ExternalProject_Add( DQMDoc
      GIT_REPOSITORY ${DQMDoc_repository}
      GIT_TAG ${DQMDoc_version}
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/DQMDoc-prefix
      LIST_SEPARATOR %
      SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/DQMDoc
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ${MkDocs_EXECUTABLE} build
      UPDATE_COMMAND ""
      INSTALL_COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/doc/mkdocs/ COMMAND cp -r LICENCE site mkdocs.yml docs cinder ${CMAKE_INSTALL_PREFIX}/doc/mkdocs/
      BUILD_IN_SOURCE 1 
  )
  list( APPEND DQM4HEP_INSTALL_PACKAGES DQMDoc )
endif()

# ----- Generate DQM4hep package config files -----
dqm4hep_generate_master_config()

# ----- display some variables and write them to cache -----
display_std_variables()
message( STATUS "The following packages will be installed :" )
foreach( pkg ${DQM4HEP_INSTALL_PACKAGES} )
  message( STATUS " -> ${pkg}: ${${pkg}_repository} (${${pkg}_version})" )
endforeach()
message( STATUS "-------------------------------------------------------------------------------" )
