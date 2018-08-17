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
list( APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake )

option( DQM4hep_TESTING             "Whether to build unit tests (ctest)" ON )
option( DQM4hep_WARNING_AS_ERROR    "Whether to add -Werror flag to cxx flags" OFF )
option( DQM4hep_EXTRA_WARNINGS      "Whether to add -Wextra flag to cxx flags" ON )
option( DQM4hep_DEV_WARNINGS        "Whether to add extra warning for developpers to cxx flags" OFF )

include( dqm4hep_build_settings )
dqm4hep_require_cxx_standard()

# ----- Check dependencies and various settings -----
find_package( Threads REQUIRED )
find_package( ROOT COMPONENTS Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree REQUIRED )
include( ${ROOT_USE_FILE} )
find_package( MySQL REQUIRED )

# ----- Compile third party libraries -----
add_subdirectory( 3rdparty )

# ----- Set global cxx flags -----
dqm4hep_set_cxx_flags()

# ----- Compile our software -----
add_subdirectory( DQMCore )
add_subdirectory( DQMNet )
if( DQM4hep_TESTING )
  add_subdirectory( DQMTest )
endif()

# ----- Generate DQM4hep package config files -----
dqm4hep_generate_master_config()

# ----- Install DQM4hep cmake macros in lib/cmake -----
file( 
  GLOB DQM4hep_CMAKE_INSTALL_FILES 
  ${PROJECT_SOURCE_DIR}/cmake/*.cmake 
)
set(
  DQM4hep_CMAKE_INSTALL_FILES 
  ${DQM4hep_CMAKE_INSTALL_FILES}
  ${PROJECT_SOURCE_DIR}/cmake/Doxyfile
  ${PROJECT_SOURCE_DIR}/cmake/dqm4hep_clang_format_config.yml
  ${PROJECT_SOURCE_DIR}/cmake/run-clang-tidy.py
)
install( FILES ${DQM4hep_CMAKE_INSTALL_FILES} DESTINATION lib/cmake )

# ----- display some variables and write them to cache -----
dqm4hep_display_std_variables()
