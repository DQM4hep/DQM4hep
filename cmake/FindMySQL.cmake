#############################################################
# cmake module for finding mysqlclient
#
# returns:
#   MySQL_FOUND        : set to TRUE or FALSE
#   MySQL_VERSION      : package version
#   MySQL_INCLUDE_DIRS : paths to mysql includes
#   MySQL_LIBRARY_DIRS : paths to mysql libraries
#   MySQL_LIBRARIES    : list of mysql libraries
#
# @author Jan Engels, DESY
#############################################################


# ------------- mysql_config  --------------------------------
SET( MySQL_CONFIG_EXECUTABLE MySQL_CONFIG_EXECUTABLE-NOTFOUND )
MARK_AS_ADVANCED( MySQL_CONFIG_EXECUTABLE )
FIND_PROGRAM( MySQL_CONFIG_EXECUTABLE mysql_config PATHS ${MySQL_DIR}/bin NO_DEFAULT_PATH )
IF( NOT MySQL_DIR )
    FIND_PROGRAM( MySQL_CONFIG_EXECUTABLE mysql_config )
ENDIF()

IF( MySQL_CONFIG_EXECUTABLE )

    # ==============================================
    # ===          MySQL_VERSION                   ===
    # ==============================================

    EXECUTE_PROCESS( COMMAND "${MySQL_CONFIG_EXECUTABLE}" --version
        OUTPUT_VARIABLE MySQL_VERSION
        RESULT_VARIABLE _exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    IF( _exit_code EQUAL 0 )
        SET( MySQL_VERSION ${MySQL_VERSION} )
    ELSE()
        SET( MySQL_VERSION )
    ENDIF()

ENDIF( MySQL_CONFIG_EXECUTABLE )


# ------------- include dirs ---------------------------------
SET( MySQL_INCLUDE_DIRS MySQL_INCLUDE_DIRS-NOTFOUND )
MARK_AS_ADVANCED( MySQL_INCLUDE_DIRS )

# WARNING: if using CMAKE_FIND_ROOT_PATH to define the installation path of mysql
#           the following FIND_PATH commando finds the wrong mysql installation because PATH_SUFFIXES is used ... !?
#           if PATHS are expanded or CMAKE_PREFIX_PATH is used instead of CMAKE_FIND_ROOT_PATH the problem goes away
#FIND_PATH( MySQL_INCLUDE_DIRS NAMES mysql.h PATHS ${MySQL_DIR} PATH_SUFFIXES include include/mysql include/mysql5/mysql NO_DEFAULT_PATH )
FIND_PATH( MySQL_INCLUDE_DIRS NAMES mysql.h PATHS 
  ${MySQL_DIR}/include 
  ${MySQL_DIR}/include/mysql 
  ${MySQL_DIR}/include/mysql5/mysql 
  NO_DEFAULT_PATH 
)

# if not found in MySQL_DIR / CMAKE_PREFIX_PATH / CMAKE_INCLUDE_PATH or CMAKE_FIND_ROOT_PATH ...
# look into the following locations
IF( NOT MySQL_DIR )
    FIND_PATH( MySQL_INCLUDE_DIRS NAMES mysql.h PATHS
        /usr
        /usr/local
        /opt/local
        /opt/products/mysql/5.0.45
        /opt/products/mysql/5.0.26
        PATH_SUFFIXES include include/mysql include/mysql5/mysql mysql/include
    )
ENDIF()




# ------------- libraries ------------------------------------
SET( MySQL_LIBRARIES MySQL_LIBRARIES-NOTFOUND )
MARK_AS_ADVANCED( MySQL_LIBRARIES )

# WARNING: see WARNING for MySQL_INCLUDE_DIRS above !
#FIND_LIBRARY( MySQL_LIBRARIES NAMES mysqlclient mysqlclient_r PATHS ${MySQL_DIR}
#    PATH_SUFFIXES lib64 lib64/mysql lib64/mysql5/mysql lib lib/mysql lib/mysql5/mysql
#    NO_DEFAULT_PATH
#)

FIND_LIBRARY( MySQL_LIBRARIES NAMES mysqlclient mysqlclient_r PATHS
    ${MySQL_DIR}/lib64
    ${MySQL_DIR}/lib64/mysql
    ${MySQL_DIR}/lib64/mysql5/mysql
    ${MySQL_DIR}/lib
    ${MySQL_DIR}/lib/mysql
    ${MySQL_DIR}/lib/mysql5/mysql
    NO_DEFAULT_PATH
)

# if not found in MySQL_DIR / CMAKE_PREFIX_PATH / CMAKE_LIBRARY_PATH or CMAKE_FIND_ROOT_PATH ...
# look into the following locations:
IF( NOT MySQL_DIR )
    FIND_LIBRARY( MySQL_LIBRARIES NAMES mysqlclient mysqlclient_r PATHS
        /usr
        /usr/local
        /opt/local
        /opt/products/mysql/5.0.45
        /opt/products/mysql/5.0.26
        PATH_SUFFIXES lib64 lib64/mysql lib64/mysql5/mysql lib lib/mysql lib/mysql5/mysql mysql/lib
    )
ENDIF()
IF( MySQL_LIBRARIES )
    GET_FILENAME_COMPONENT( MySQL_LIBRARY_DIRS ${MySQL_LIBRARIES} PATH )
    MARK_AS_ADVANCED( MySQL_LIBRARY_DIRS )
ENDIF( MySQL_LIBRARIES )



# ---------- final checking ---------------------------------------------------
INCLUDE( FindPackageHandleStandardArgs )
SET( PACKAGE_VERSION_COMPATIBLE TRUE )
# set MySQL_FOUND to TRUE if all listed variables are TRUE and not empty
FIND_PACKAGE_HANDLE_STANDARD_ARGS( MySQL DEFAULT_MSG MySQL_INCLUDE_DIRS MySQL_LIBRARIES PACKAGE_VERSION_COMPATIBLE )

SET( MySQL_FOUND ${MYSQL_FOUND} )

