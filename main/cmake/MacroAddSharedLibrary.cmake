# create symbolic lib target for calling library targets
#ADD_CUSTOM_TARGET( lib )

macro( ADD_SHARED_LIBRARY _name )
    add_library( ${_name} SHARED ${ARGN} )
       
    # change lib_target properties
    set_target_properties( ${_name} PROPERTIES
        # create *nix style library versions + symbolic links
        VERSION ${${PROJECT_NAME}_VERSION}
        SOVERSION ${${PROJECT_NAME}_SOVERSION}
    )
endmacro()

