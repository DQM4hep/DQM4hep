#################################################
# cmake file for building DQMCore component
# @author Ete Remi, DESY
#################################################

dqm4hep_package( DQMCore
  USES [ROOT COMPONENTS Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree REQUIRED]
  xdrstream MySQL spdlog tclap json 
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/dqm4hep
)

# -------------------------------------------------
# build the DQMCore dictionary
dqm4hep_add_dictionary( G__DQMCore
  SOURCES include/dqm4hep/MonitorElement.h
  LINKDEF include/dqm4hep/LinkDef.h
  INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include
  OPTIONS -Wno-unsequenced
)

# -------------------------------------------------
# build the DQMCore library
dqm4hep_add_package_library( DQMCore
  SOURCES src/*.cc src/plugins/*.cc ${DQMCore_DICTIONARY}
  GENERATED G__DQMCore
)

# -------------------------------------------------
# build the DQMCore binaries
dqm4hep_add_executable( dqm4hep-cpustats              SOURCES main/dqm4hep-cpustats.cc )
dqm4hep_add_executable( dqm4hep-dump-plugins          SOURCES main/dqm4hep-dump-plugins.cc )
dqm4hep_add_executable( dqm4hep-generate-qtest-file   SOURCES main/dqm4hep-generate-qtest-file.cc )
dqm4hep_add_executable( dqm4hep-memstats              SOURCES main/dqm4hep-memstats.cc )
dqm4hep_add_executable( dqm4hep-mysql-create-db       SOURCES main/dqm4hep-mysql-create-db.cc )
dqm4hep_add_executable( dqm4hep-mysql-create-user     SOURCES main/dqm4hep-mysql-create-user.cc )
dqm4hep_add_executable( dqm4hep-mysql-dump-parameters SOURCES main/dqm4hep-mysql-dump-parameters.cc )
dqm4hep_add_executable( dqm4hep-mysql-push-parameters SOURCES main/dqm4hep-mysql-push-parameters.cc )
dqm4hep_add_executable( dqm4hep-mysql-rm-user         SOURCES main/dqm4hep-mysql-rm-user.cc )
dqm4hep_add_executable( dqm4hep-mysql-set-parameter   SOURCES main/dqm4hep-mysql-set-parameter.cc )
dqm4hep_add_executable( dqm4hep-netstats              SOURCES main/dqm4hep-netstats.cc )
dqm4hep_add_executable( dqm4hep-run-qtests            SOURCES main/dqm4hep-run-qtests.cc )


