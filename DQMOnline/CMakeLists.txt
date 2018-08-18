#################################################
# cmake file for building DQMOnline component
# @author Ete Remi, DESY
#################################################

dqm4hep_package( DQMOnline
  USES DQMCore DQMNet mongoose
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/dqm4hep
)

# -------------------------------------------------
# build the DQMOnline library
dqm4hep_add_package_library( DQMOnline
  SOURCES src/*.cc src/plugins/*.cc
)

# -------------------------------------------------
# build the DQMOnline binaries
dqm4hep_add_executable( dqm4hep-dump-event                  SOURCES main/dqm4hep-dump-event.cc )
dqm4hep_add_executable( dqm4hep-online-logger               SOURCES main/dqm4hep-online-logger.cc )
dqm4hep_add_executable( dqm4hep-start-event-collector       SOURCES main/dqm4hep-start-event-collector.cc )
dqm4hep_add_executable( dqm4hep-start-module                SOURCES main/dqm4hep-start-module.cc )
dqm4hep_add_executable( dqm4hep-start-online-mgr            SOURCES main/dqm4hep-start-online-mgr.cc )
dqm4hep_add_executable( dqm4hep-start-random-event-source   SOURCES main/dqm4hep-start-random-event-source.cc )
dqm4hep_add_executable( dqm4hep-start-rc-server             SOURCES main/dqm4hep-start-rc-server.cc )
