MARK_AS_ADVANCED( CMAKE_BACKWARDS_COMPATIBILITY )
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE ) # default in cmake 2.6

# include helper macros
INCLUDE( MacroAddSharedLibrary )
INCLUDE( MacroInstallSharedLibrary )
INCLUDE( MacroInstallDirectory )
INCLUDE( MacroDisplayStandardVariables )
INCLUDE( MacroGeneratePackageConfigFiles )


# include default settings
INCLUDE( default_install_prefix )
INCLUDE( default_build_type )
INCLUDE( default_enable_ctest )
INCLUDE( default_library_versioning )
INCLUDE( default_build_output_directories )
INCLUDE( default_rpath_settings )
#INCLUDE( build_32bit_compatible )


# uninstall target may only be created once per project
# otherwise problems occur due to duplicate targets. even
# setting CMAKE_POLICY(SET CMP0002 OLD) causes an error
# calling 'make uninstall'
IF( NOT _default_settings_loaded )

    INCLUDE( default_uninstall_target )

    SET( _default_settings_loaded TRUE )

ENDIF()
