# TODO: document....

MACRO( EXPORT_PACKAGE_DEPENDENCIES _pkgname )

    # this sets PKG_COMPONENT_LIB_DEPENDS variables
    SET( ${_pkgname}_LIB_DEPENDENCIES_FILE "${${_pkgname}_ROOT}/lib/cmake/${_pkgname}LibDeps.cmake" )
    IF( EXISTS "${${_pkgname}_LIB_DEPENDENCIES_FILE}" )
        INCLUDE( "${${_pkgname}_LIB_DEPENDENCIES_FILE}" )
    ENDIF()


    IF( NOT FIND_PACKAGE_SKIP_DEPENDENCIES )

        # INCLUDE_DIRS
        IF( ${_pkgname}_INCLUDE_DIRS AND ${_pkgname}_DEPENDS_INCLUDE_DIRS )
            LIST( APPEND ${_pkgname}_INCLUDE_DIRS ${${_pkgname}_DEPENDS_INCLUDE_DIRS} )
        ENDIF()


        # LIBRARY_DIRS
        IF( ${_pkgname}_LIBRARY_DIRS AND ${_pkgname}_DEPENDS_LIBRARY_DIRS )
            LIST( APPEND ${_pkgname}_LIBRARY_DIRS ${${_pkgname}_DEPENDS_LIBRARY_DIRS} )
        ENDIF()
        

        # LIBRARIES
        IF( ${_pkgname}_LIBRARIES AND ${_pkgname}_DEPENDS_LIBRARIES )
            LIST( APPEND ${_pkgname}_LIBRARIES ${${_pkgname}_DEPENDS_LIBRARIES} )
        ENDIF()

    ENDIF()

ENDMACRO( EXPORT_PACKAGE_DEPENDENCIES )
