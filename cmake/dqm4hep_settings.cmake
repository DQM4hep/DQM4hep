mark_as_advanced( CMAKE_BACKWARDS_COMPATIBILITY )
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE ) # default in cmake 2.6

set( DQM4HEP_CMAKE_MODULES_ROOT ${CMAKE_CURRENT_LIST_DIR} )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

include( CMakeParseArguments )

# include helper macros
include( MacroAddSharedLibrary )
include( MacroInstallSharedLibrary )
include( MacroInstallDirectory )
include( MacroDisplayStandardVariables )
include( MacroGeneratePackageConfigFiles )

# include dqm4hep settings
include( dqm4hep_install_prefix )
include( dqm4hep_build_type )
include( dqm4hep_enable_ctest )
include( dqm4hep_library_versioning )
include( dqm4hep_build_output_directories )
include( dqm4hep_rpath_settings )
include( dqm4hep_clang_tools )
include( dqm4hep_with_proc_fs )
include( dqm4hep_package_config )
include( dqm4hep_compiler_settings )
include( dqm4hep_root_dictionary )
include( dqm4hep_make_executables )
include( dqm4hep_find_packages )

