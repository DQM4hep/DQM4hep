
##############################################################################
# macro for extracting a numeric version from a string, e.g.
# EXTRACT_NUMERIC_VERSION( "v01-00-03" )
# message( "NUMERIC VERSION: ${NUMERIC_VERSION}" ) --> NUMERIC VERSION: 01.00.03
#
# if a version is found, the following varibles are returned:
#
#   NUMERIC_VERSION : contains the whole extracted version (as shown above)
#   VERSION_MAJOR
#   VERSION_MINOR
#   VERSION_PATCH (if found)
#   VERSION_TWEAK (if found)
#
# Jan Engels, Desy - IT
##############################################################################

macro( EXTRACT_NUMERIC_VERSION _version )

    unset( NUMERIC_VERSION )
    unset( VERSION_MAJOR )
    unset( VERSION_MINOR )
    unset( VERSION_PATCH )
    unset( VERSION_TWEAK )

    if( "${_version}" MATCHES "[0-9]+[^0-9][0-9]+" ) # version found

        string( REGEX REPLACE "[^0-9]*([0-9]+).*" "\\1" VERSION_MAJOR "${_version}" )
        string( REGEX REPLACE "[^0-9]*[0-9]+[^0-9]+([0-9]+).*" "\\1" VERSION_MINOR "${_version}" )

        #message( "INPUT VERSION: ${_version}" )
        #message( "VERSION_MAJOR: ${VERSION_MAJOR}" )
        #message( "VERSION_MINOR: ${VERSION_MINOR}" )

        set( NUMERIC_VERSION ${VERSION_MAJOR}.${VERSION_MINOR} )

        if( "${_version}" MATCHES "[^0-9]*[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+" ) # version has a patch level
            string( REGEX REPLACE "[^0-9]*[0-9]+[^0-9]+[0-9]+[^0-9]+([0-9]+).*" "\\1" VERSION_PATCH "${_version}" )
            set( NUMERIC_VERSION ${NUMERIC_VERSION}.${VERSION_PATCH} )
            #message( "VERSION_PATCH: ${VERSION_PATCH}" )
        endif()

        if( "${_version}" MATCHES "[^0-9]*[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+" ) # version has a tweak level
            string( REGEX REPLACE "[^0-9]*[0-9]+[^0-9]+[0-9]+[^0-9]+[0-9]+[^0-9]+([0-9]+).*" "\\1" VERSION_TWEAK "${_version}" )
            set( NUMERIC_VERSION ${NUMERIC_VERSION}.${VERSION_TWEAK} )
            #message( "VERSION_TWEAK: ${VERSION_TWEAK}" )
        endif()

        #message( "NUMERIC VERSION: ${NUMERIC_VERSION}" )

    endif()
    
endmacro()

