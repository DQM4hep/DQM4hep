MARK_AS_ADVANCED( CMAKE_BACKWARDS_COMPATIBILITY )
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE ) # default in cmake 2.6

SET( DQM4HEP_CMAKE_MODULES_ROOT ${CMAKE_CURRENT_LIST_DIR} )

# include helper macros
INCLUDE( MacroAddSharedLibrary )
INCLUDE( MacroInstallSharedLibrary )
INCLUDE( MacroInstallDirectory )
INCLUDE( MacroDisplayStandardVariables )
INCLUDE( MacroGeneratePackageConfigFiles )


# include dqm4hep settings
INCLUDE( dqm4hep_install_prefix )
INCLUDE( dqm4hep_build_type )
INCLUDE( dqm4hep_enable_ctest )
INCLUDE( dqm4hep_library_versioning )
INCLUDE( dqm4hep_build_output_directories )
INCLUDE( dqm4hep_rpath_settings )
INCLUDE( dqm4hep_clang_tools )
#INCLUDE( build_32bit_compatible )

# uninstall target may only be created once per project
# otherwise problems occur due to duplicate targets. even
# setting CMAKE_POLICY(SET CMP0002 OLD) causes an error
# calling 'make uninstall'
IF( NOT _dqm4hep_settings_loaded )

    INCLUDE( dqm4hep_uninstall_target )

    SET( _dqm4hep_settings_loaded TRUE )

ENDIF()
