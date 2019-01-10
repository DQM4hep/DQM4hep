########################################################
# Top-level CMakeLists.txt to build DQM4hep
# @author Remi Ete, DESY
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
########################################################

# ----- Project name and version -----
project( DQM4hep )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_PATCH 4 )

# ----- CMake settings -----
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

#############################################################
# Enable CXX as project language to perform compiler checks #
#############################################################

ENABLE_LANGUAGE(CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard used for compiling")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Package options
option( DQM4hep_TESTING             "Whether to build unit tests (ctest)" ON )
option( DQM4hep_WARNING_AS_ERROR    "Whether to add -Werror flag to cxx flags" OFF )
option( DQM4hep_EXTRA_WARNINGS      "Whether to add -Wextra flag to cxx flags" ON )
option( DQM4hep_DEV_WARNINGS        "Whether to add extra warning for developpers to cxx flags" OFF )
option( DQM4hep_DOXYGEN_DOC         "Set to OFF to skip build/install Documentation" OFF )
option( DQM4hep_BUILD_STATIC_LIBS        "Set to ON to build with static libraries (required on slc6) " OFF )

if ( CMAKE_SYSTEM MATCHES ".el6.")
  message (WARNING "DQM4hep: Running on redhat6 - Setting DQM4hep_BUILD_STATIC_LIBS to true")
  set( DQM4hep_BUILD_STATIC_LIBS ON )
endif()
  
include( DQM4hepBuild )
dqm4hep_set_version( DQM4hep
  MAJOR ${DQM4hep_VERSION_MAJOR}
  MINOR ${DQM4hep_VERSION_MINOR}
  PATCH ${DQM4hep_VERSION_PATCH} 
)

configure_file(
  "${DQM4hep_CMAKE_MODULES_ROOT}/DQM4hepConfig.h.in"
  "${PROJECT_SOURCE_DIR}/DQMCore/include/dqm4hep/DQM4hepConfig.h" 
  @ONLY
)

dqm4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )

# ----- Check dependencies and various settings -----
find_package( Threads REQUIRED )
find_package( ROOT 6.08 REQUIRED COMPONENTS Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree  )
include( ${ROOT_USE_FILE} )
find_package( MySQL REQUIRED )

# ----- Compile third party libraries -----
add_subdirectory( 3rdparty )

# ----- Set global cxx flags -----
dqm4hep_set_cxx_flags()

include( DQM4hep ) 
include( DQM4hepMacros ) 

# ----- Compile our software -----
add_subdirectory( DQMCore )
add_subdirectory( DQMNet )
add_subdirectory( DQMOnline )
if( DQM4hep_TESTING )
  set( BUILD_TESTING ON )
  enable_testing()
  include( CTest )
  mark_as_advanced( DART_TESTING_TIMEOUT )
  add_subdirectory( DQMTest )
endif()

# ----- Build and install doxygen documentation -----
dqm4hep_build_doxygen( OUTPUT doc/Doxygen )

# ----- Install DQM4hep cmake macros in lib/cmake -----
install(DIRECTORY cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.cmake"
  PATTERN "*.in" EXCLUDE
  PATTERN "Doxyfile" EXCLUDE
)
  

# ----- display some variables and write them to cache -----
dqm4hep_display_std_variables()

######################################################
# generate and install following configuration files #
######################################################

dqm4hep_generate_package_configuration_files( DQM4hepConfig.cmake )

get_property( DQM4hep_PLUGIN_DLL GLOBAL PROPERTY DQM4hep_PLUGIN_DLL )
string( REPLACE ";" ":" DQM4hep_PLUGIN_DLL "${DQM4hep_PLUGIN_DLL}" )
configure_file(
  "${DQM4hep_CMAKE_MODULES_ROOT}/thisdqm4hep.sh.in"
  "${DQM4hep_CMAKE_MODULES_ROOT}/thisdqm4hep.sh" 
  @ONLY
)
