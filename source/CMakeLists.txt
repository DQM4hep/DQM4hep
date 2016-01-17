#######################################################
# cmake file for building DQMCore package
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

# include directories
INCLUDE_DIRECTORIES( include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library

SET (
  ${PROJECT_NAME}DictHeaders
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/DQMMonitorElement.h
)

SET( DICT_INCLUDE_DIRS "" )
GET_DIRECTORY_PROPERTY(IncludeDirs INCLUDE_DIRECTORIES)

FOREACH( dir ${IncludeDirs} )
   SET( DICT_INCLUDE_DIRS ${DICT_INCLUDE_DIRS}\t-I${dir} )
ENDFOREACH(dir)

SET( LIB_HEADERS_PKG ${${PROJECT_NAME}DictHeaders} )
SET( LINK_DEF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/dqm4hep/LinkDef.h )
SET( ROOT_DICT_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dict" )

ADD_CUSTOM_COMMAND( OUTPUT ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.h
                   COMMAND mkdir -p ${ROOT_DICT_OUTPUT_DIR}
                   COMMAND ${ROOT_CINT_EXECUTABLE} -f ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc -c ${DICT_INCLUDE_DIRS} -p ${LIB_HEADERS_PKG} ${LINK_DEF_FILE}
                   COMMENT "Generating ROOT dictionary for DQMCore library."
                   DEPENDS ${LIB_HEADERS_PKG}
)

AUX_SOURCE_DIRECTORY( src SRC_FILES )

IF( DQM4HEP_LCIO )
    AUX_SOURCE_DIRECTORY( src/lcio SRC_FILES )
ENDIF()

IF( DQM4HEP_ELOG )
    AUX_SOURCE_DIRECTORY( src/elog SRC_FILES )
ENDIF()

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} ${ROOT_DICT_OUTPUT_DIR}/${PROJECT_NAME}Dict.cc )

INSTALL(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

# -------------------------------------------------
# build start external application executable

ADD_EXECUTABLE( dqm4hep_start_external_application main/dqm4hep_start_external_application.cc )
TARGET_LINK_LIBRARIES( dqm4hep_start_external_application ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_start_external_application
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build start event collector executable

ADD_EXECUTABLE( dqm4hep_start_event_collector main/dqm4hep_start_event_collector.cc )
TARGET_LINK_LIBRARIES( dqm4hep_start_event_collector ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_start_event_collector
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build start monitor element collector executable

ADD_EXECUTABLE( dqm4hep_start_monitor_element_collector main/dqm4hep_start_monitor_element_collector.cc )
TARGET_LINK_LIBRARIES( dqm4hep_start_monitor_element_collector ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_start_monitor_element_collector
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build start analysis module executable

ADD_EXECUTABLE( dqm4hep_start_analysis_module main/dqm4hep_start_analysis_module.cc )
TARGET_LINK_LIBRARIES( dqm4hep_start_analysis_module ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_start_analysis_module
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build start standalone module executable

ADD_EXECUTABLE( dqm4hep_start_standalone_module main/dqm4hep_start_standalone_module.cc )
TARGET_LINK_LIBRARIES( dqm4hep_start_standalone_module ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_start_standalone_module
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build open root canvas executable

ADD_EXECUTABLE( dqm4hep_open_root_canvas main/dqm4hep_open_root_canvas.cc )
TARGET_LINK_LIBRARIES( dqm4hep_open_root_canvas ${PROJECT_NAME} ${ROOT_GUI_LIBRARY} ${ROOT_GRAPH_LIBRARY} ${ROOT_HIST_LIBRARY} ${ROOT_CINT_LIBRARY} )

INSTALL (
    TARGETS dqm4hep_open_root_canvas
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build db insert config file executable

ADD_EXECUTABLE( dqm4hep_db_insert_config_file main/dqm4hep_db_insert_config_file.cc )
TARGET_LINK_LIBRARIES( dqm4hep_db_insert_config_file ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep_db_insert_config_file
    RUNTIME DESTINATION bin
)

# -------------------------------------------------
# build examples

IF( DQM4HEP_EXAMPLES )

    INCLUDE_DIRECTORIES( examples/module )

    ADD_SHARED_LIBRARY( RandomModuleExample examples/module/random/RandomModule.cc )
    TARGET_LINK_LIBRARIES( RandomModuleExample ${PROJECT_NAME} )

    INSTALL(
        TARGETS RandomModuleExample
        LIBRARY DESTINATION lib
    )
    
    
    ADD_SHARED_LIBRARY( StandaloneRandomModuleExample examples/module/standaloneRandom/StandaloneRandomModule.cc )
    TARGET_LINK_LIBRARIES( StandaloneRandomModuleExample ${PROJECT_NAME} )

    INSTALL(
        TARGETS StandaloneRandomModuleExample
        LIBRARY DESTINATION lib
    )
    
    IF( DQM4HEP_LCIO )
    
	    ADD_SHARED_LIBRARY( CaloHitModuleExample examples/module/lcio/CaloHitModule.cc )
	    TARGET_LINK_LIBRARIES( CaloHitModuleExample ${PROJECT_NAME} )
	
	    INSTALL(
	        TARGETS CaloHitModuleExample
	        LIBRARY DESTINATION lib
	    )
	    
	ENDIF()
    
ENDIF()

# Build lcio file service
IF( DQM4HEP_LCIO )

    ADD_EXECUTABLE( dqm4hep_start_lcio_file_service main/dqm4hep_start_lcio_file_service.cc )

    TARGET_LINK_LIBRARIES( dqm4hep_start_lcio_file_service ${PROJECT_NAME} )

    INSTALL (
        TARGETS dqm4hep_start_lcio_file_service
        RUNTIME DESTINATION bin
    )

ENDIF()

INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )


