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
MACRO(BUILD_EXECUTABLE _dir _name _install_loc)
	ADD_EXECUTABLE( ${_name} ${_dir}/${_name}.cc )
	TARGET_LINK_LIBRARIES( ${_name} ${PROJECT_NAME} )
	INSTALL (
	    TARGETS ${_name}
	    RUNTIME DESTINATION ${_install_loc}
	)
ENDMACRO()

# tests
BUILD_EXECUTABLE( main test-server bin/tests )

# main binaries
BUILD_EXECUTABLE( main dqm4hep-server-info bin )
BUILD_EXECUTABLE( main dqm4hep-server-list bin )
BUILD_EXECUTABLE( main dqm4hep-server-running bin )
BUILD_EXECUTABLE( main dqm4hep-send-request bin )
BUILD_EXECUTABLE( main dqm4hep-send-command bin )
BUILD_EXECUTABLE( main dqm4hep-send-request-response bin )
BUILD_EXECUTABLE( main dqm4hep-subscribe-service bin )

INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )
