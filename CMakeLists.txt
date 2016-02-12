########################################################
# cmake file for building DQM4HEP
# @author Remi Ete
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( DQM4HEP )

### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 3 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# versions of DQM4HEP subpackages
# don't forget to update when releasing DQM4HEP
# e.g. SET( DQMCore_version "v01-00-00" )
SET( DQMCore_version              "master" )
SET( DQMViz_version               "master" )
SET( JsonCpp_version              "0.10.5" )
SET( DIMJC_version                "master" )
SET( LCIO_version                 "v02-06")

# ----- download settings -----
SET( DQMCore_repository             "https://github.com/DQM4HEP/DQMCore.git" )
SET( DQMViz_repository              "https://github.com/DQM4HEP/DQMViz.git" )
SET( JsonCpp_repository             "https://github.com/open-source-parsers/jsoncpp.git" )
SET( DIM_repository                 "https://github.com/DQM4HEP/dim.git" )
SET( DIMJC_repository               "https://github.com/mirabitl/dimjc.git" )
SET( streamlog_repository           "https://github.com/DQM4HEP/streamlog" )
SET( LCIO_repository                "svn://svn.freehep.org/lcio/tags" ) # public (no authentication)

# the external project in used are 
# set gradually along the CMakeLists.txt
# When a project is added with ExternalProject_Add(...)
# these variables are set to the package name
# Mandatory packages are directly set
SET( DQMCore_package_depends "DQMCore" )
SET( DQMViz_package_depends "" )
SET( jsoncpp_package_depends "" )
SET( dim_package_depends "" )
SET( dimjc_package_depends "" )
SET( streamlog_package_depends "" )
SET( lcio_package_depends "" )

# ----- additional cmake args -----
IF ( NOT APPLE )
	SET( JsonCpp_additional_cmake_args  "-DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON" )
ENDIF ( NOT APPLE )

SET( JsonCpp_additional_cmake_args "${JsonCpp_additional_cmake_args} -DJSONCPP_WITH_TESTS=OFF" )

SET( DQMCore_additional_cmake_args  "" )

# ----- CLANG 7.0+ gives an error for #define private public -----
# ----- error: keyword is hidden by macro definition [-Werror,-Wkeyword-macro] -----
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	SET( DQMCore_additional_cmake_args 
		${DQMCore_additional_cmake_args}
		"-DCMAKE_CXX_FLAGS=-Wno-keyword-macro"
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

IF( ILCUTIL_FOUND )
	INCLUDE( ilcsoft_default_settings )
ELSE()	
	INCLUDE( default_settings )
ENDIF()

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

OPTION( DQM4HEP_VIZ "Build DQM4HEPViz library and binaries (requires Qt and ROOT built with Qt support (--enable-qt))" OFF )
MESSAGE( STATUS "DQM4HEP_VIZ: ${DQM4HEP_VIZ}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

OPTION( DQM4HEP_LCIO "Build DQMCore with LCIO (requires LCIO installed)" OFF )
MESSAGE( STATUS "DQM4HEP_LCIO: ${DQM4HEP_LCIO}" )

OPTION( DQM4HEP_ELOG "Build DQMCore with ELog (requires ELog binary installed)" OFF )
MESSAGE( STATUS "DQM4HEP_ELOG: ${DQM4HEP_ELOG}" )

OPTION( DQM4HEP_EXAMPLES "Build DQMCore with examples" OFF )
MESSAGE( STATUS "DQM4HEP_EXAMPLES: ${DQM4HEP_EXAMPLES}" )

OPTION( DIM_GUI "Whether to build dim gui part (did, webDid)" OFF )
MESSAGE( STATUS "DIM_GUI: ${DIM_GUI}" )

IF( DIM_GUI )
	SET( DIM_GUI_FLAG "GUI=yes" )
ELSE()
	SET( DIM_GUI_FLAG "GUI=no" )
ENDIF()

SET( DQMCore_additional_cmake_args 
	${DQMCore_additional_cmake_args} 
	"-DDQM4HEP_LCIO=${DQM4HEP_LCIO}"
	"-DDQM4HEP_ELOG=${DQM4HEP_ELOG}"
	"-DDQM4HEP_EXAMPLES=${DQM4HEP_EXAMPLES}"
)

# ----------------------------------------------------------------------------

# if ilcsoft is not installed, download the needed
# packages normally provided by ilcsoft (except ROOT and Qt)
IF( NOT ILCUTIL_FOUND )
	
	MESSAGE( STATUS "ILCSOFT not found" )
	
	IF( DQM4HEP_LCIO )
	
		# ----- LCIO package -----
		ExternalProject_Add( LCIO
		    SVN_REPOSITORY "${LCIO_repository}/${LCIO_version}"
		    CMAKE_ARGS ${common_cmake_args}
		    PREFIX lcio
		    LIST_SEPARATOR %
		    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lcio
		)
		
		SET( lcio_package_depends "LCIO" )
		LIST( APPEND DQMCore_additional_cmake_args "-DDQM4HEP_LCIO=ON" )
		
	ENDIF()
	
	# ----- streamlog package -----
	ExternalProject_Add( streamlog
	    GIT_REPOSITORY "${streamlog_repository}"
	    CMAKE_ARGS ${common_cmake_args}
	    PREFIX streamlog
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/streamlog
	)
	
	SET( streamlog_package_depends "streamlog" )
	
ENDIF()

# ----- DIM package -----
FIND_PACKAGE( DIM )

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
	    INSTALL_COMMAND ""
	)
	
	SET( dim_package_depends "dim" )
ENDIF ()

SET( DQMCore_additional_cmake_args 
	${DQMCore_additional_cmake_args} 
	"-DDIMDIR=${DIMDIR}"
)

SET( DIMJC_additional_cmake_args 
	${DIMJC_additional_cmake_args} 
	"-DDIMDIR=${DIMDIR}"
)

SET( DQMVis_additional_cmake_args 
	${DQMVis_additional_cmake_args} 
	"-DDIMDIR=${DIMDIR}"
)

# ----- DQMCore package -----
ExternalProject_Add( DQMCore
    DEPENDS ${dim_package_depends} ${streamlog_package_depends} ${lcio_package_depends}
    GIT_REPOSITORY ${DQMCore_repository}
    GIT_TAG ${DQMCore_version}
    CMAKE_ARGS ${common_cmake_args} ${DQMCore_additional_cmake_args}
    PREFIX DQMCore
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQMCore
)

# ----- JsonCpp package -----
FIND_PACKAGE( JSONCPP )

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

# ----- DIMJC package -----
FIND_PACKAGE( DIMJC )

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
	"-DDIMJC_DIR=${DIMJC_DIR}"
)

IF( DQM4HEP_VIZ )

	# ----- DIMViz package -----
    ExternalProject_Add( DQMViz
        DEPENDS ${DQMCore_package_depends} ${dim_package_depends} ${jsoncpp_package_depends} ${dimjc_package_depends} ${streamlog_package_depends} 
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
