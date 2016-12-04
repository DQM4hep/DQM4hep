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


# -------------------------------------------------
# build test-json-binary executable


ADD_EXECUTABLE( test-json-binary main/test-json-binary.cc )
TARGET_LINK_LIBRARIES( test-json-binary ${PROJECT_NAME} )

INSTALL (
    TARGETS test-json-binary
    RUNTIME DESTINATION bin
)

# -------------------------------------------------
# build test-server executable


ADD_EXECUTABLE( test-server main/test-server.cc )
TARGET_LINK_LIBRARIES( test-server ${PROJECT_NAME} )

INSTALL (
    TARGETS test-server
    RUNTIME DESTINATION bin
)

# -------------------------------------------------
# build dqm4hep-server-info executable


ADD_EXECUTABLE( dqm4hep-server-info main/dqm4hep-server-info.cc )
TARGET_LINK_LIBRARIES( dqm4hep-server-info ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep-server-info
    RUNTIME DESTINATION bin
)


# -------------------------------------------------
# build dqm4hep-send-request executable


ADD_EXECUTABLE( dqm4hep-send-request main/dqm4hep-send-request.cc )
TARGET_LINK_LIBRARIES( dqm4hep-send-request ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep-send-request
    RUNTIME DESTINATION bin
)

# -------------------------------------------------
# build dqm4hep-send-request executable


ADD_EXECUTABLE( dqm4hep-send-request-response main/dqm4hep-send-request-response.cc )
TARGET_LINK_LIBRARIES( dqm4hep-send-request-response ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep-send-request-response
    RUNTIME DESTINATION bin
)

# -------------------------------------------------
# build dqm4hep-subscribe-service executable


ADD_EXECUTABLE( dqm4hep-subscribe-service main/dqm4hep-subscribe-service.cc )
TARGET_LINK_LIBRARIES( dqm4hep-subscribe-service ${PROJECT_NAME} )

INSTALL (
    TARGETS dqm4hep-subscribe-service
    RUNTIME DESTINATION bin
)



INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )


