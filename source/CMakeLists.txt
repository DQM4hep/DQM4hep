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
# build start shm driver executable


ADD_EXECUTABLE( test-json-binary main/test-json-binary.cc )
TARGET_LINK_LIBRARIES( test-json-binary ${PROJECT_NAME} )

INSTALL (
    TARGETS test-json-binary
    RUNTIME DESTINATION bin
)



INSTALL_DIRECTORY( include DESTINATION . FILES_MATCHING PATTERN "*.h" )


