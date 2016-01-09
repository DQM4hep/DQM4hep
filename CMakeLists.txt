########################################################
# cmake file for building DQM4HEP
# @author Remi Ete
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( DQM4HEP )

### PACKAGE VERSIONS ########################################################
SET( ${PROJECT_NAME}_VERSION_MAJOR 3 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# versions of DQM4HEP subpackages
# don't forget to update when releasing DQM4HEP
# e.g. SET( DQMCore_version "v01-00-00" )
SET( DQMCore_version              "v01-00-00" )
SET( DQMViz_version               "v01-00-00" )
SET( JsonCpp_version              "0.10.5" )
SET( DIMJC_version                "v01-00-00" )
SET( LCIO_version                 "v02-06")

# ----- download settings -----
SET( DQMCore_repository             "https://github.com/DQM4HEP/DQMCore.git" )
SET( DQMViz_repository              "https://github.com/DQM4HEP/DQMViz.git" )
SET( JsonCpp_repository             "https://github.com/open-source-parsers/jsoncpp.git" )
SET( DIM_repository                 "https://github.com/DQM4HEP/dim.git" )
SET( DIMJC_repository               "https://github.com/mirabitl/dimjc.git" )
SET( streamlog_repository           "https://github.com/DQM4HEP/streamlog" )  # public (no authentication)
SET( LCIO_repository                "svn://svn.freehep.org/lcio/tags" ) # public (no authentication)

# ----- additional cmake args -----
SET( JsonCpp_additional_cmake_args  "-DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF" )
SET( DQMCore_additional_cmake_args  "" )

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

OPTION( DQM4HEP_VIZ "Build DQM4HEPViz library and binaries (requires ROOT and Qt)" OFF )
MESSAGE( STATUS "DQM4HEP_VIZ: ${DQM4HEP_VIZ}" )

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

OPTION( DQM4HEP_LCIO "Build DQMCore with LCIO (requires LCIO installed)" OFF )
MESSAGE( STATUS "DQM4HEP_LCIO: ${DQM4HEP_LCIO}" )

OPTION( DQM4HEP_ELOG "Build DQMCore with ELog (requires ELog binary installed)" OFF )
MESSAGE( STATUS "DQM4HEP_ELOG: ${DQM4HEP_ELOG}" )

OPTION( DQM4HEP_EXAMPLES "Build DQMCore with examples" OFF )
MESSAGE( STATUS "DQM4HEP_EXAMPLES: ${DQM4HEP_EXAMPLES}" )

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
		    PREFIX LCIO-${LCIO_version}
		    LIST_SEPARATOR %
		    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/LCIO-${LCIO_version}
		)
		
		LIST( APPEND DQMCore_package_depends "LCIO" )
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
	
	LIST( APPEND DQMCore_package_depends "streamlog" )
	
ENDIF()

# ----- DIM package -----
ExternalProject_Add( DIM
    GIT_REPOSITORY ${DIM_repository}
    GIT_TAG ${DIM_version}
    CMAKE_ARGS ${common_cmake_args}
    PREFIX dim
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dim
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dim
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dim/external-project/build-command ${DIM_OS}
    INSTALL_COMMAND ""
)


SET( DQMCore_additional_cmake_args 
	${DQMCore_additional_cmake_args} 
	"-DDIMDIR=${CMAKE_CURRENT_SOURCE_DIR}/dim"
)

SET( DIMJC_additional_cmake_args 
	${DIMJC_additional_cmake_args} 
	"-DDIMDIR=${CMAKE_CURRENT_SOURCE_DIR}/dim"
)

SET( DQMVis_additional_cmake_args 
	${DQMVis_additional_cmake_args} 
	"-DDIMDIR=${CMAKE_CURRENT_SOURCE_DIR}/dim"
)


# ----- DQMCore package -----
ExternalProject_Add( DQMCore
    DEPENDS DIM ${DQMCore_package_depends}
    #GIT_REPOSITORY ${DQMCore_repository}/${DQMCore_version}
    #GIT_TAG ${DQMCore_version}
    URL /home/rete/soft/DQMCore
    CMAKE_ARGS ${common_cmake_args} ${DQMCore_additional_cmake_args}
    PREFIX DQMCore-${DQMCore_version}
    LIST_SEPARATOR %
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQMCore-${DQMCore_version}
)


IF( DQM4HEP_VIZ )

	# ----- JsonCpp package -----
    ExternalProject_Add( JsonCpp
        GIT_REPOSITORY ${JsonCpp_repository}
        GIT_TAG ${JsonCpp_version}
        CMAKE_ARGS ${common_cmake_args} ${JsonCpp_additional_cmake_args}
	    PREFIX JsonCpp-${JsonCpp_version}
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/JsonCpp-${JsonCpp_version}
    )
    
    # ----- DIMJC package -----
    ExternalProject_Add( DIMJC
    	DEPENDS DIM JsonCpp
	    GIT_REPOSITORY ${DIMJC_repository}
	    GIT_TAG ${DIMJC_version}
	    CMAKE_ARGS ${common_cmake_args} ${DIMJC_additional_cmake_args}
	    PREFIX dim-${DIMJC_version}
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DIMJC-${DIMJC_version}
	)
	
	SET( DQMVis_additional_cmake_args 
	${DQMVis_additional_cmake_args} 
	"-DDIMJC_DIR=${CMAKE_CURRENT_SOURCE_DIR}/DIMJC-${DIMJC_version}"
)

	# ----- DIMViz package -----
    ExternalProject_Add( DQMViz
        DEPENDS DQMCore ${DQMCore_package_depends} DIM JsonCpp DIMJC 
        #GIT_REPOSITORY ${DQMViz_repository}
        #GIT_TAG ${DQMViz_version}
        URL /home/rete/soft/DQMViz
        CMAKE_ARGS ${common_cmake_args} ${DQMVis_additional_cmake_args}
	    PREFIX DQMViz-${DQMViz_version}
	    LIST_SEPARATOR %
	    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DQMViz-${DQMViz_version}
    )

ENDIF()

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
