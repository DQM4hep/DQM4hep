#######################################################
# cmake file for building DQMNet package
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

# include directories
INCLUDE_DIRECTORIES( BEFORE include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library

AUX_SOURCE_DIRECTORY( src SRC_FILES )

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} )

INSTALL(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)

# add executable macro 
MACRO(BUILD_EXECUTABLE _name)

	ADD_EXECUTABLE( ${_name} main/${_name}.cc )
	
	TARGET_LINK_LIBRARIES( ${_name} ${PROJECT_NAME} )
	
	INSTALL (
	    TARGETS ${_name}
	    RUNTIME DESTINATION bin
	)
	
ENDMACRO()

# tests
BUILD_EXECUTABLE( test-json-binary )
BUILD_EXECUTABLE( test-server )

# main binaries
BUILD_EXECUTABLE( dqm4hep-server-info )
BUILD_EXECUTABLE( dqm4hep-server-list )
BUILD_EXECUTABLE( dqm4hep-server-running )
BUILD_EXECUTABLE( dqm4hep-send-request )
BUILD_EXECUTABLE( dqm4hep-send-request-response )
BUILD_EXECUTABLE( dqm4hep-subscribe-service )

INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )

