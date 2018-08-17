#################################################
# cmake file for building DQMNet component
# @author Ete Remi, DESY
#################################################

dqm4hep_package( DQMNet
  USES DQMCore dim asio websocketpp
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/dqm4hep
)

# -------------------------------------------------
# build the DQMCore library
dqm4hep_add_package_library( DQMNet
  SOURCES src/*.cc
)

# -------------------------------------------------
# build the DQMNet binaries
dqm4hep_add_executable( dqm4hep-send-command            SOURCES main/dqm4hep-send-command.cc )
dqm4hep_add_executable( dqm4hep-send-request-response   SOURCES main/dqm4hep-send-request-response.cc )
dqm4hep_add_executable( dqm4hep-send-request            SOURCES main/dqm4hep-send-request.cc )
dqm4hep_add_executable( dqm4hep-server-info             SOURCES main/dqm4hep-server-info.cc )
dqm4hep_add_executable( dqm4hep-server-list             SOURCES main/dqm4hep-server-list.cc )
dqm4hep_add_executable( dqm4hep-server-running          SOURCES main/dqm4hep-server-running.cc )
dqm4hep_add_executable( dqm4hep-subscribe-service       SOURCES main/dqm4hep-subscribe-service.cc )
dqm4hep_add_executable( dqm4hep-test-ws-server          SOURCES main/test-ws-server.cc )
dqm4hep_add_executable( dqm4hep-test-server             SOURCES main/test-server.cc )

# -------------------------------------------------
# generate config headers and config files
# TODO remove this and replace by:
# - a single header DQM4hepConfig.h
# - single configuration files DQM4hepConfig.cmake and DQM4hepConfigVersion.cmake
dqm4hep_generate_package_configuration( 
  PACKAGE_NAME DQMNet
  HEADER_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include
)

