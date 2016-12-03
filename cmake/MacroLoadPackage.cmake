#######################################################################
# wrapper macro for calling FIND_PACKAGE
#
# this macro is DEPRECATED. Please do not use it!
#
# @author Jan Engels, DESY
#######################################################################

MESSAGE( "MacroLoadPackage.cmake IS DEPRECATED !!! Please use the standard cmake FIND_PACKAGE" )

MACRO( LOAD_PACKAGE _pkgname )

    IF( NOT ${_pkgname}_CHECKED )
        SET( ${_pkgname}_CHECKED TRUE INTERNAL "Resolve cyclic dependencies" )

        IF( NOT ${_pkgname} STREQUAL ${PROJECT_NAME} )

            SET( args "${ARGN}" )
            LIST( REMOVE_ITEM args "LINK_ONLY" "NOACTION" )

            IF( NOT ${_pkgname}_DIR AND ${_pkgname}_HOME )
                SET( ${_pkgname}_DIR "${${_pkgname}_HOME}" )
            ENDIF()

            FIND_PACKAGE( ${_pkgname} ${args} )

            IF( ${_pkgname}_FOUND )
                INCLUDE_DIRECTORIES( ${${_pkgname}_INCLUDE_DIRS} )
                LINK_LIBRARIES( ${${_pkgname}_LIBRARIES} )
                ADD_DEFINITIONS( ${${_pkgname}_DEFINITIONS} )
            ENDIF()

        ENDIF()
    ENDIF()

ENDMACRO( LOAD_PACKAGE )

