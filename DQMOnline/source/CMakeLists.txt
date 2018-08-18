#######################################################
# cmake file for building DQMOnline package
# @author Eté Rémi
# Copyright (c)
#######################################################

# include directories
include_directories( BEFORE include )
dqm4hep_set_cxx_flags()

#AUX_SOURCE_DIRECTORY( src SRC_FILES )
SET( SRC_FILES ${SRC_FILES} src/RemoteLogger.cc )
SET( SRC_FILES ${SRC_FILES} src/OnlineManagerServer.cc )
SET( SRC_FILES ${SRC_FILES} src/OnlineRoutes.cc )
SET( SRC_FILES ${SRC_FILES} src/EventSource.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControl.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControlServer.cc )
SET( SRC_FILES ${SRC_FILES} src/RunControlInterface.cc )
SET( SRC_FILES ${SRC_FILES} src/AppEvent.cc )
SET( SRC_FILES ${SRC_FILES} src/AppEventLoop.cc )
SET( SRC_FILES ${SRC_FILES} src/AppTimer.cc )
SET( SRC_FILES ${SRC_FILES} src/Application.cc )
SET( SRC_FILES ${SRC_FILES} src/EventCollector.cc )
SET( SRC_FILES ${SRC_FILES} src/EventCollectorClient.cc )
SET( SRC_FILES ${SRC_FILES} src/Module.cc )
SET( SRC_FILES ${SRC_FILES} src/ModuleApi.cc )
SET( SRC_FILES ${SRC_FILES} src/Cycle.cc )
SET( SRC_FILES ${SRC_FILES} src/ModuleApplication.cc )
SET( SRC_FILES ${SRC_FILES} src/OnlineElement.cc )
set( SRC_FILES ${SRC_FILES} src/plugins/TestAnalysisModule.cc )
set( SRC_FILES ${SRC_FILES} src/plugins/TestStandaloneModule.cc )

# ----- Http run control based on Mongoose library -----
if( BUILD_HTTP_RC )
  dqm4hep_find_package(
    PACKAGE Mongoose
    REQUIRED ADD_DEPENDS  
  )
  set( SRC_FILES ${SRC_FILES} src/plugins/HttpRunControlPlugin.cc )
endif()

# 
add_shared_library( ${PROJECT_NAME} ${SRC_FILES} )
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib )
install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN "*.cc" )

# -------------------------------------------------
# build all binaries in main directory
dqm4hep_add_main_executables()

# -------------------------------------------------
# build unit tests
if( DQM4HEP_TESTING )
  dqm4hep_add_tests()
endif()

# -------- clang tidy and format --------
dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()

