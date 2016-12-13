########################################################
# cmake file for building DQM4HEP
# @author Remi Ete
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################

# project name
PROJECT( DQM4HEP )

### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 4 )
SET( ${PROJECT_NAME}_VERSION_MINOR 3 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
	MESSAGE( SEND_ERROR "DQM4HEP requires C++11 support. Please upgrade your compiler !" )
ENDIF()

OPTION( USE_MASTER "Whether to use master version of subpackages" OFF )

###############################
# versions of DQM4HEP subpackages
# don't forget to update when releasing DQM4HEP
# e.g. SET( DQMCore_version "v01-00-00" )

IF( USE_MASTER )
	SET( xdrstream_version            "master" )
	SET( xdrlcio_version              "master" )
	SET( DQMCore_version              "master" )
	SET( DQMViz_version               "master" )
	SET( DQM4ILC_version              "master" )
	SET( JsonCpp_version              "0.10.5" )
	SET( DIMJC_version                "master" )
	SET( levbdim_version              "master" )
	SET( Mongoose_version             "master" )
ELSE()
	SET( xdrstream_version            "v01-00-01" )
	SET( xdrlcio_version              "v01-00-00" )
	SET( DQMCore_version              "v02-03-00" )
	SET( DQMViz_version               "v02-01-00" )
	SET( DQM4ILC_version              "v01-02-00" )
	SET( JsonCpp_version              "0.10.5" )
	SET( DIMJC_version                "v01-02-00" )
	SET( levbdim_version              "v01-00-00" )
	SET( Mongoose_version             "master" )
ENDIF()

    # SET( LOG4CXX_version              "v0_10_0") # Last available release...doesn't compile on all system due to unfixed bug
    SET( LOG4CXX_version              "trunk")
    SET( APR_version                  "1.5.2")
    SET( APRUTIL_version              "1.5.4")

# ----- download settings -----
SET( xdrstream_repository           "https://github.com/DQM4HEP/xdrstream.git" )
SET( xdrlcio_repository             "https://github.com/DQM4HEP/xdrlcio.git" )
SET( DQMCore_repository             "https://github.com/DQM4HEP/DQMCore.git" )
SET( DQMViz_repository              "https://github.com/DQM4HEP/DQMViz.git" )
SET( DQM4ILC_repository             "https://github.com/DQM4HEP/DQM4ILC.git" )
SET( JsonCpp_repository             "https://github.com/DQM4HEP/jsoncpp.git" )
SET( DIM_repository                 "https://github.com/DQM4HEP/dim.git" )
SET( DIMJC_repository               "https://github.com/DQM4HEP/dimjc.git" )
SET( levbdim_repository             "https://github.com/DQM4HEP/levbdim.git" )
SET( Mongoose_repository            "https://github.com/DQM4HEP/mongoose-cpp.git" )
# SET( LOG4CXX_repository             "https://github.com/apache/log4cxx.git" ) # Do not compile on OSX
SET( LOG4CXX_repository             "https://github.com/DQM4HEP/log4cxx.git" )
SET( APR_repository                 "https://github.com/apache/apr.git" )
SET( APRUTIL_repository             "https://github.com/apache/apr-util.git" )

# the external project in used are
# set gradually along the CMakeLists.txt
# When a project is added with ExternalProject_Add(...)
# these variables are set to the package name
# Mandatory packages are directly set
SET( xdrstream_package_depends "xdrstream" )
SET( DQMCore_package_depends "DQMCore" )
SET( DQMViz_package_depends "" )
SET( jsoncpp_package_depends "" )
SET( dim_package_depends "" )
SET( dimjc_package_depends "" )
SET( Mongoose_package_depends "" )
SET( LOG4CXX_package_depends "APR APRUTIL" )
SET( APR_package_depends "" )
SET( APRUTIL_package_depends "" )

# ----- additional cmake args -----
IF ( NOT APPLE )
	SET( JsonCpp_additional_cmake_args  -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON )
ENDIF ( NOT APPLE )

SET( JsonCpp_additional_cmake_args ${JsonCpp_additional_cmake_args} -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_WARNING_AS_ERROR=OFF )


# ----- CLANG 7.0+ gives an error for #define private public -----
# ----- error: keyword is hidden by macro definition [-Werror,-Wkeyword-macro] -----
# ----- Also complains about multi-line // comment [-Werror,-Wcomment]
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET( DQMCore_additional_cmake_args
    ${DQMCore_additional_cmake_args}
    "-DCMAKE_CXX_FLAGS=-Wno-keyword-macro -Wno-comment"
	)
SET( levbdim_additional_cmake_args
    ${levbdim_additional_cmake_args}
    "-DCMAKE_CXX_FLAGS= -Wno-macro-redefined -Wno-format"
  )

ENDIF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")


# ----- dim os detection -----
IF( APPLE )
	SET( DIM_OS "Darwin" )
ELSE()
	SET( DIM_OS "Linux" )
ENDIF()

# ----- dependencies -----
FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES QUIET )

LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
INCLUDE( default_settings )

# ----------------------------------------------------------------------------

INCLUDE( ExternalProject )

# In order to pass semicolon-separated lists over the command line to all packages,
# we need to separate list elements with '%' instead of the standard cmake list separator ';'
# The list separator needs also to be redefined in the ExternalProject_Add calls by setting
# the variable LIST_SEPARATOR
FOREACH( _path ${CMAKE_PREFIX_PATH} )
    SET( CMAKE_PREFIX_PATH_FIXED ${CMAKE_PREFIX_PATH_FIXED}%${_path} )
ENDFOREACH()

FOREACH( _path ${CMAKE_MODULE_PATH} )
    SET( CMAKE_MODULE_PATH_FIXED ${CMAKE_MODULE_PATH_FIXED}%${_path} )
ENDFOREACH()

# ----- cmake arguments common to ALL cmake packages -------------------------
SET( common_cmake_args
     "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}"
     "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}"
     "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
     "-DINSTALL_DOC=${INSTALL_DOC}"
)

# ----- package options -----------------------------------------------------------------------

OPTION( BUILD_DQMVIZ "Build DQMViz library and binaries (requires Qt and ROOT built with Qt support (--enable-qt))" OFF )
MESSAGE( STATUS "BUILD_DQMVIZ: ${BUILD_DQMVIZ}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
MESSAGE( STATUS "INSTALL_DOC: ${INSTALL_DOC}" )

OPTION( BUILD_DQM4ILC "Build DQM4ILC package (requires ilcsoft installed)" OFF )
MESSAGE( STATUS "BUILD_DQM4ILC: ${BUILD_DQM4ILC}" )

OPTION( BUILD_EXAMPLES "Build DQMCore (resp DQM4ILC) with examples" OFF )
MESSAGE( STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLES}" )

OPTION( DIM_GUI "Whether to build dim gui part (did, webDid)" OFF )
MESSAGE( STATUS "DIM_GUI: ${DIM_GUI}" )

OPTION( BUILD_EVB "Whether to compile the evb sources of DQMCore" OFF )
MESSAGE( STATUS "BUILD_EVB: ${BUILD_EVB}" )

OPTION( BUILD_HTTP "Whether to compile the http supprt of DQMCore" OFF )
MESSAGE( STATUS "BUILD_HTTP: ${BUILD_HTTP}" )

# ----------------------------------------------------------------------------

IF( DIM_GUI )
	SET( DIM_GUI_FLAG "GUI=yes" )
ELSE()
	SET( DIM_GUI_FLAG "GUI=no" )
ENDIF()

SET( DQMCore_additional_cmake_args
	${DQMCore_additional_cmake_args}
	-DBUILD_EXAMPLES=${BUILD_EXAMPLES}
	-DBUILD_EVB=${BUILD_EVB}
)

# fixme : problems on lyoserv.
# Couldn't find mysql and log4cxx because install path is not standard

IF( MySQL_DIR )
	SET( common_cmake_args
		${common_cmake_args}
		-DMySQL_DIR=${MySQL_DIR}
	)
ENDIF()

IF( LOG4CXX_DIR )
	SET( LOG4CXX_ROOT ${LOG4CXX_DIR} )

	SET( common_cmake_args
		${common_cmake_args}
		-DLOG4CXX_ROOT=${LOG4CXX_DIR}
	)
ENDIF()


# ----- xdrstream package -----
ExternalProject_Add( xdrstream
    GIT_REPOSITORY ${xdrstream_repository}
    GIT_TAG ${xdrstream_version}
    CMAKE_ARGS ${common_cmake_args}
    PREFIX xdrstream
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xdrstream
    LIST_SEPARATOR %
)

# ----- DIM package -----
FIND_PACKAGE( DIM QUIET )

IF ( NOT DIM_FOUND )
	SET ( DIMDIR ${CMAKE_CURRENT_SOURCE_DIR}/dim )

	ExternalProject_Add( dim
	    GIT_REPOSITORY ${DIM_repository}
	    GIT_TAG ${DIM_version}
	    CMAKE_ARGS ${common_cmake_args}
	    PREFIX dim
	    LIST_SEPARATOR %
	    SOURCE_DIR ${DIMDIR}
	    BINARY_DIR ${DIMDIR}
	   	PATCH_COMMAND ${DIMDIR}/external-project/patch-command ${DIM_OS}
	    CONFIGURE_COMMAND ""
	    BUILD_COMMAND ${DIMDIR}/external-project/build-command ${DIM_OS} ${DIM_GUI_FLAG}
	    INSTALL_COMMAND ${DIMDIR}/external-project/install-command ${CMAKE_INSTALL_PREFIX}/bin ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/include
	)

	SET( dim_package_depends "dim" )
ENDIF ()


# ----- LOG4CXX package -----
FIND_PACKAGE( LOG4CXX REQUIRED )

IF( NOT LOG4CXX_FOUND )
    # ----- apr-util package ----- 
    FIND_PACKAGE ( APRUTIL REQUIRED )
    IF( NOT APRUTIL_FOUND )
        # ----- apr package -----
        # Force new install of apr to unsure compatibility with apr-util version 
        # FIND_PACKAGE ( APR REQUIRED )
        # IF( NOT APR_FOUND )
            SET ( APR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apr )
            ExternalProject_Add( APR
                GIT_REPOSITORY ${APR_repository}
                GIT_TAG ${APR_version}
                CMAKE_ARGS ${common_cmake_args}
                PREFIX apr
                LIST_SEPARATOR %
                SOURCE_DIR ${APR_DIR}
                BINARY_DIR ${APR_DIR}
                UPDATE_COMMAND ${APR_DIR}/buildconf
                CONFIGURE_COMMAND ${APR_DIR}/configure --prefix=${APR_DIR}
            )
        	SET( APR_package_depends "apr" )
        # ENDIF( NOT APR_FOUND)

    	SET ( APRUTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apr-util )
    	ExternalProject_Add( APRUTIL
    	    GIT_REPOSITORY ${APRUTIL_repository}
    	    GIT_TAG ${APRUTIL_version}
    	    CMAKE_ARGS ${common_cmake_args}
    	    PREFIX apr-util
    	    LIST_SEPARATOR %
    	    SOURCE_DIR ${APRUTIL_DIR}
    	    BINARY_DIR ${APRUTIL_DIR}
            UPDATE_COMMAND ${APRUTIL_DIR}/buildconf
            CONFIGURE_COMMAND ${APRUTIL_DIR}/configure --prefix=${APRUTIL_DIR} --with-apr=${APR_DIR}
        )
        SET( APRUTIL_package_depends "aprutil" )
    ENDIF( NOT APRUTIL_FOUND)

    SET ( LOG4CXX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apache-log4cxx )
    ExternalProject_Add( LOG4CXX
        GIT_REPOSITORY ${LOG4CXX_repository}
        GIT_TAG ${LOG4CXX_version}
        CMAKE_ARGS ${common_cmake_args}
        PREFIX log4cxx
        LIST_SEPARATOR %
        SOURCE_DIR ${LOG4CXX_DIR}
        BINARY_DIR ${LOG4CXX_DIR}
        UPDATE_COMMAND ${LOG4CXX_DIR}/autogen.sh
	    CONFIGURE_COMMAND ${LOG4CXX_DIR}/configure --prefix=${LOG4CXX_DIR} --with-apr=${APR_DIR} --with-apr-util=${APRUTIL_DIR}
	)

	SET( LOG4CXX_package_depends "log4cxx" )
ENDIF()


SET( DQMCore_additional_cmake_args
	${DQMCore_additional_cmake_args}
	-Dxdrstream_DIR=${CMAKE_CURRENT_SOURCE_DIR}/xdrstream
	-DBUILD_EXAMPLES=${BUILD_EXAMPLES}
	-DDIMDIR=${DIMDIR}
)

SET( xdrlcio_additional_cmake_args
	${xdrlcio_additional_cmake_args}
	-Dxdrstream_DIR=${CMAKE_CURRENT_SOURCE_DIR}/xdrstream
)

SET( DIMJC_additional_cmake_args
	${DIMJC_additional_cmake_args}
	-DDIMDIR=${DIMDIR}
)

SET( DQMVis_additional_cmake_args
	${DQMVis_additional_cmake_args}
	-DDIMDIR=${DIMDIR}
)

SET( DQM4ILC_additional_cmake_args
	${DQM4ILC_additional_cmake_args}
	-Dxdrlcio_DIR=${CMAKE_CURRENT_SOURCE_DIR}/xdrlcio
	-DDQMCore_DIR=${CMAKE_CURRENT_SOURCE_DIR}/DQMCore
	-DBUILD_EXAMPLES=${BUILD_EXAMPLES}
)

SET( levbdim_additional_cmake_args
    ${levbdim_additional_cmake_args}
    -DDIMDIR=${DIMDIR}
  )

# ----- JsonCpp package -----
FIND_PACKAGE( JSONCPP QUIET )

IF ( NOT JSONCPP_FOUND )
	SET ( JSONCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp )

	ExternalProject_Add( jsoncpp
	    GIT_REPOSITORY ${JsonCpp_repository}
	    GIT_TAG ${JsonCpp_version}
	    CMAKE_ARGS ${common_cmake_args} ${JsonCpp_additional_cmake_args}
		PREFIX jsoncpp
	    LIST_SEPARATOR %
	    SOURCE_DIR ${JSONCPP_DIR}
	)

	SET( jsoncpp_package_depends "jsoncpp" )

ENDIF()

# ----- levbdim package -----
IF ( BUILD_EVB )
	SET ( levbdim_DIR ${CMAKE_CURRENT_SOURCE_DIR}/levbdim )

	ExternalProject_Add( levbdim
		DEPENDS ${jsoncpp_package_depends} ${dim_package_depends}
	    GIT_REPOSITORY ${levbdim_repository}
	    GIT_TAG ${levbdim_version}
	    CMAKE_ARGS ${common_cmake_args} ${levbdim_additional_cmake_args}
		PREFIX levbdim
	    LIST_SEPARATOR %
	    SOURCE_DIR ${levbdim_DIR}
	)

	SET( levbdim_package_depends "levbdim" )

ENDIF()

# ----- Mongoose package -----
IF ( BUILD_HTTP )
	SET ( Mongoose_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mongoose-cpp )

	ExternalProject_Add( Mongoose
	    GIT_REPOSITORY ${Mongoose_repository}
	    GIT_TAG ${Mongoose_version}
	    CMAKE_ARGS ${common_cmake_args} ${Mongoose_additional_cmake_args}
		PREFIX mongoose-cpp
	    LIST_SEPARATOR %
	    SOURCE_DIR ${Mongoose_DIR}
	)

      SET( Mongoose_package_depends "Mongoose" )

      LIST( APPEND DQMCore_additional_cmake_args "-DMongoose_DIR=${CMAKE_INSTALL_PREFIX}" )

ENDIF()

# ----- DQMCore package -----
ExternalProject_Add( DQMCore
    DEPENDS ${dim_package_depends} ${xdrstream_package_depends} ${jsoncpp_package_depends} ${levbdim_package_depends} ${Mongoose_package_depends}
    GIT_REPOSITORY ${DQMCore_repository}
    GIT_TAG ${DQMCore_version}
    CMAKE_ARGS ${common_cmake_args} ${DQMCore_additional_cmake_args}
    PREFIX DQMCore
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQMCore
)

# ----- DIMJC package -----
FIND_PACKAGE( DIMJC QUIET )

IF ( NOT DIMJC_FOUND )
	SET ( DIMJC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dimjc )

	ExternalProject_Add( dimjc
		DEPENDS ${jsoncpp_package_depends} ${dim_package_depends}
	    GIT_REPOSITORY ${DIMJC_repository}
	    GIT_TAG ${DIMJC_version}
	    CMAKE_ARGS ${common_cmake_args} ${DIMJC_additional_cmake_args}
	    PREFIX dimjc
	    LIST_SEPARATOR %
	    SOURCE_DIR ${DIMJC_DIR}
	)

	SET( dimjc_package_depends "dimjc" )

ENDIF ()

SET( DQMVis_additional_cmake_args
	${DQMVis_additional_cmake_args}
	-DDIMJC_DIR=${DIMJC_DIR}
)



IF( BUILD_DQM4ILC )

	# ----- xdrlcio package -----
    ExternalProject_Add( xdrlcio
        DEPENDS ${xdrstream_package_depends}
        GIT_REPOSITORY ${xdrlcio_repository}
        GIT_TAG ${xdrlcio_version}
        CMAKE_ARGS ${common_cmake_args} ${xdrlcio_additional_cmake_args}
	    PREFIX xdrlcio
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xdrlcio
    )

    SET( xdrlcio_package_depends "xdrlcio" )

	# ----- DQM4ILC package -----
    ExternalProject_Add( DQM4ILC
        DEPENDS
        	${xdrstream_package_depends}
        	${xdrlcio_package_depends}
        	${DQMCore_package_depends}
        GIT_REPOSITORY ${DQM4ILC_repository}
        GIT_TAG ${DQM4ILC_version}
        CMAKE_ARGS ${common_cmake_args} ${DQM4ILC_additional_cmake_args}
	    PREFIX DQM4ILC
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQM4ILC
    )

ENDIF()



IF( BUILD_DQMVIZ )

	# ----- DIMViz package -----
    ExternalProject_Add( DQMViz
        DEPENDS
        	${DQMCore_package_depends}
        	${dim_package_depends}
        	${jsoncpp_package_depends}
        	${dimjc_package_depends}
        	${xdrstream_package_depends}
        GIT_REPOSITORY ${DQMViz_repository}
        GIT_TAG ${DQMViz_version}
        CMAKE_ARGS ${common_cmake_args} ${DQMVis_additional_cmake_args}
	    PREFIX DQMViz
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQMViz
    )

ENDIF()



# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
