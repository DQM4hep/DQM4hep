


mark_as_advanced( CMAKE_BACKWARDS_COMPATIBILITY )
set( DQM4hep_CMAKE_MODULES_ROOT ${CMAKE_CURRENT_LIST_DIR} )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
include( CMakeParseArguments )

# include helper macros
include( MacroAddSharedLibrary )
include( MacroInstallSharedLibrary )
include( MacroInstallDirectory )
include( MacroDisplayStandardVariables )
include( MacroGeneratePackageConfigFiles )

# set default install prefix to project root directory
# instead of the cmake default /usr/local
if( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
  set( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
endif()

# write this variable to cache
set( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Where to install ${PROJECT_NAME}" FORCE )


# set default cmake build type to RelWithDebInfo
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "RelWithDebInfo" )
endif()

# write this variable to cache
set( 
  CMAKE_BUILD_TYPE 
  "${CMAKE_BUILD_TYPE}" 
  CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." 
  FORCE
)

# enable ctest
if( DQM4hep_TESTING )
  set( BUILD_TESTING ON )
  enable_testing()
  include( CTest )
  mark_as_advanced( DART_TESTING_TIMEOUT )
endif()

# output directories
set( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" )
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )
mark_as_advanced( EXECUTABLE_OUTPUT_PATH )
mark_as_advanced( LIBRARY_OUTPUT_PATH )

# add library install path to the rpath list
set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
mark_as_advanced( CMAKE_INSTALL_RPATH )

# add install path to the rpath list (apple)
if( APPLE )
  set( CMAKE_MACOSX_RPATH ON )
  set( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
  mark_as_advanced( CMAKE_INSTALL_NAME_DIR )
endif()

# append link pathes to rpath list
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
mark_as_advanced( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# look for /proc directory
set( DQM4hep_WITH_PROC_FS 0 )
message( STATUS "Check for proc fs directory" )

if( EXISTS "/proc" )
  set( DQM4hep_WITH_PROC_FS 1 )
  add_definitions( -DDQM4HEP_WITH_PROC_FS )
  message( STATUS "Check for proc fs directory: found" )
else()
  message( STATUS "Check for proc fs directory: not found" )
endif()

add_definitions( -DASIO_STANDALONE )

macro( dqm4hep_to_parent_scope val )
  set ( ${val} ${${val}} PARENT_SCOPE )
endmacro()

function( dqm4hep_require_cxx_standard )
  enable_language( CXX )

  # Set C++ standard
  set( CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard used for compiling" )
  set( CMAKE_CXX_STANDARD_REQUIRED ON )
  set( CMAKE_CXX_EXTENSIONS OFF )
endfunction()


function( dqm4hep_set_cxx_flags )
  
  include( CheckCXXCompilerFlag )
  
  set( COMPILER_FLAGS -Wunused-value -Wall -pedantic -Wshadow -Wformat-security -Wno-long-long -Wreturn-type -Wuseless-cast -Wlogical-op -Wredundant-decls -Weffc++ -Wno-unsequenced -Wno-deprecated-declarations -fdiagnostics-color=auto  )
  
  # APPLECLANG 9.1.0 add new warning for hardcoded include nonportable-include-path
  # This is not compatible with the way root generate its dictionary. So we disable it
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(COMPILER_FLAGS ${COMPILER_FLAGS} -Wno-error=nonportable-include-path)
  endif()
  
  if( DQM4hep_DEV_WARNINGS )
    set( COMPILER_FLAGS ${COMPILER_FLAGS} -Wsuggest-override -Wno-comments -Wparentheses )
  endif()
  
  if( DQM4hep_WARNING_AS_ERROR )
    set( COMPILER_FLAGS ${COMPILER_FLAGS} -Werror )
  endif()
  
  if( DQM4hep_EXTRA_WARNINGS )
    set( COMPILER_FLAGS ${COMPILER_FLAGS} -Wextra )
  endif()
  
  foreach( FLAG ${COMPILER_FLAGS} )
    ## need to replace the minus or plus signs from the variables, because it is passed
    ## as a macro to g++ which causes a warning about no whitespace after macro
    ## definition
    string( REPLACE "-" "_" FLAG_WORD ${FLAG} )
    string( REPLACE "+" "P" FLAG_WORD ${FLAG_WORD} )

    check_cxx_compiler_flag( "${FLAG}" CXX_FLAG_WORKS_${FLAG_WORD} )
    
    if( ${CXX_FLAG_WORKS_${FLAG_WORD}} )
      message( STATUS "Adding ${FLAG} to CXX_FLAGS" )
      set( CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS} ")
    else()
      message( STATUS "NOT Adding ${FLAG} to CXX_FLAGS" )
    endif()
  endforeach()
  
  check_cxx_compiler_flag( "-std=c++11" COMPILER_SUPPORTS_CXX11 )
  
  if( COMPILER_SUPPORTS_CXX11 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
  else()
    message( SEND_ERROR "${PROJECT_NAME} requires C++11 support. Please upgrade your compiler !" )
  endif()
endfunction()

# Standard macro to export a DQM4hep package as a CMake project
macro( dqm4hep_generate_package_configuration )
  
  # parse the macro arguments
  set( FLAG_OPTIONS "" )
  set( SINGLE_OPTIONS PACKAGE_NAME HEADER_OUTPUT_PATH )
  set( MULTI_OPTIONS LIBRARIES )
  cmake_parse_arguments( PACKAGE_CONFIG "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )

  if( NOT PACKAGE_CONFIG_PACKAGE_NAME )
    set( PACKAGE_CONFIG_PACKAGE_NAME ${PROJECT_NAME} )
    message( STATUS "Package config: package name set to ${PACKAGE_CONFIG_PACKAGE_NAME}" )
  endif()
  
  if( NOT PACKAGE_CONFIG_HEADER_OUTPUT_PATH )
    set( PACKAGE_CONFIG_HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/source/include )
    message( STATUS "Package config: package config header output path set to ${PACKAGE_CONFIG_HEADER_OUTPUT_PATH}" )
  endif()
  
  if( NOT PACKAGE_CONFIG_LIBRARIES )
    set( PACKAGE_CONFIG_PACKAGE_LIBRARIES ${PROJECT_NAME} )
  else()
    foreach( lib ${PACKAGE_CONFIG_LIBRARIES} )
      set( PACKAGE_CONFIG_PACKAGE_LIBRARIES "${PACKAGE_CONFIG_PACKAGE_LIBRARIES} ${lib}" )      
    endforeach()
  endif()
  
  # package dependencies
  set( PACKAGE_CONFIG_DEPENDS_INCLUDE_DIRS ${${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS} )
  set( PACKAGE_CONFIG_DEPENDS_LIBRARY_DIRS ${${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS} )
  set( PACKAGE_CONFIG_DEPENDS_LIBRARIES ${${PROJECT_NAME}_DEPENDS_LIBRARIES} )
  
  # package version
  set( PACKAGE_CONFIG_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR} )
  set( PACKAGE_CONFIG_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR} )
  set( PACKAGE_CONFIG_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH} )
  set( PACKAGE_CONFIG_PACKAGE_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )
  
  # package definitions
  set( PACKAGE_CONFIG_PACKAGE_DEFINITIONS "" )
  if( ${PROJECT_NAME}_DEFINITIONS )
    set( PACKAGE_CONFIG_PACKAGE_DEFINITIONS ${${PROJECT_NAME}_DEFINITIONS} )
  endif()
      
  # generate config header
  configure_file( 
    "${DQM4hep_CMAKE_MODULES_ROOT}/PackageConfig.h.cmake.in"
    "${PACKAGE_CONFIG_HEADER_OUTPUT_PATH}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.h" 
    @ONLY
  )
  
  # generate Config.cmake file
  configure_file( 
    "${DQM4hep_CMAKE_MODULES_ROOT}/PackageConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.cmake" 
    @ONLY
  )
  
  # generate ConfigVersion.cmake file
  configure_file( 
    "${DQM4hep_CMAKE_MODULES_ROOT}/PackageConfigVersion.cmake.in"
    "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}ConfigVersion.cmake" 
    @ONLY
  )
  
  # install all generate config cmake files
  install( FILES "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}ConfigVersion.cmake" DESTINATION lib/cmake )
  install( FILES "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.cmake" DESTINATION lib/cmake )
  
endmacro()


function ( dqm4hep_set_version packageName )
  cmake_parse_arguments ( ARG "" "MAJOR;MINOR;PATCH" "" ${ARGN} )
  if ( NOT "${packageName}" STREQUAL "" )
    project ( ${packageName} )
  else()
    message( FATAL_ERROR "${packageName}: !!! Attempt to define a DQM4hep project without a name !!!" )
  endif()
  set ( major ${ARG_MAJOR} )
  set ( minor ${ARG_MINOR} )
  set ( patch ${ARG_PATCH} )
  
  if ( "${major}" STREQUAL "" ) 
    set ( major ${DQM4hep_VERSION_MAJOR} )
  endif()
  if ( "${minor}" STREQUAL "" ) 
    set ( minor ${DQM4hep_VERSION_MINOR} )
  endif()
  if ( "${patch}" STREQUAL "" ) 
    set ( patch ${DQM4hep_VERSION_PATCH} )
  endif()

  if ( NOT ("${major}" STREQUAL "" OR "${minor}" STREQUAL "" OR "${patch}" STREQUAL "") )
    set( ${packageName}_VERSION_MAJOR ${major} PARENT_SCOPE )
    set( ${packageName}_VERSION_MINOR ${minor} PARENT_SCOPE )
    set( ${packageName}_VERSION_PATCH ${patch} PARENT_SCOPE )
    set( ${packageName}_VERSION       "${major}.${minor}" PARENT_SCOPE )
    set( ${packageName}_SOVERSION     "${major}.${minor}" PARENT_SCOPE )
  else()
    message( FATAL_ERROR "${packageName}: No Package versions specified.....->  ( ${major}.${minor}.${patch} )" )
  endif()
endfunction()

function ( dqm4hep_include_directories pkg_incs )
  FOREACH( INCDIR ${pkg_incs} )
    string(FIND "${INCDIR}" "${CMAKE_SOURCE_DIR}" out)
    IF("${out}" EQUAL 0)
      INCLUDE_DIRECTORIES( ${INCDIR} )
    ELSE()
      INCLUDE_DIRECTORIES( SYSTEM ${INCDIR} )
    ENDIF()
  ENDFOREACH()
endfunction()

function ( dqm4hep_make_unique_list name )
  cmake_parse_arguments ( ARG "" "" "VALUES" ${ARGN} )
  set ( vals )
  foreach( v ${ARG_VALUES} )
    set ( vals "${vals} ${v}" )
  endforeach()
  string( REGEX REPLACE " " ";" vals "${vals}" )
  if( NOT "${vals}" STREQUAL "" )
    list ( REMOVE_DUPLICATES vals )
  endif()
  set( ${name} ${vals} PARENT_SCOPE )
endfunction()


function( dqm4hep_install_dir )
  cmake_parse_arguments ( ARG "" "DESTINATION" "" ${ARGN} )
  if( NOT "${ARG_UNPARSED_ARGUMENTS}" STREQUAL "" )
    foreach ( d ${ARG_UNPARSED_ARGUMENTS} )
      install ( DIRECTORY ${d}
        DESTINATION ${ARG_DESTINATION}
        PATTERN ".svn" EXCLUDE )
    endforeach()
  endif()
endfunction()


function ( dqm4hep_install_includes package )
  message ( "DQM4hep_install_includes[${package}]: ${ARGN}" )
  dqm4hep_install_dir( ${ARGN} DESTINATION include )
endfunction()


function ( dqm4hep_unpack_package_opts name opt )
  set ( nam_pkg  )
  set ( req_pkg  )
  set ( typ_pkg  )
  set ( com_pkg )
  set ( comp_pkg )
  set ( srcs_pkg )
  set ( defs_pkg )
  if ( "${opt}" MATCHES "\\[" )
    string ( REGEX REPLACE "\\[;" "" opt "${opt}" )
    string ( REGEX REPLACE "\\["  "" opt "${opt}" )
    string ( REGEX REPLACE ";\\]" "" opt "${opt}" )
    string ( REGEX REPLACE "\\]"  "" opt "${opt}" )
    string ( REPLACE ";" ";" all_opt "${opt}" )
    foreach( e ${all_opt} )
      if( "${nam_pkg}" STREQUAL "" )
        set ( nam_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "REQUIRED" )
        set ( req_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "INTERNAL" )
        set ( typ_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "EXTERNAL" )
        set ( typ_pkg ${e} )
        unset ( act_pkg )
      elseif ( "${com_pkg}" STREQUAL "" AND ${e} STREQUAL "COMPONENTS" )
        set ( com_pkg ${e} )
        set ( act_pkg ${e} )
      elseif ( "${src_pkg}" STREQUAL "" AND ${e} STREQUAL "SOURCES" )
        set ( act_pkg ${e} )
      elseif ( "${def_pkg}" STREQUAL "" AND ${e} STREQUAL "DEFINITIONS" )
        set ( act_pkg ${e} )
      elseif ( "${act_pkg}" STREQUAL "COMPONENTS" )
        set ( comp_pkg ${comp_pkg} ${e} )
      elseif ( "${act_pkg}" STREQUAL "SOURCES" )
        set ( srcs_pkg ${srcs_pkg} ${e} )
      elseif ( "${act_pkg}" STREQUAL "DEFINITIONS" )
        set ( defs_pkg ${defs_pkg} ${e} )
      endif()
    endforeach()
  else()
    set ( nam_pkg ${opt} )
    set ( req_pkg REQUIRED )
  endif()
  string ( TOUPPER "${nam_pkg}" nam_pkg )
  set ( ${name}_NAME        ${nam_pkg}   PARENT_SCOPE )
  set ( ${name}_REQUIRED    ${req_pkg}   PARENT_SCOPE )
  set ( ${name}_TYPE        ${typ_pkg}   PARENT_SCOPE )
  set ( ${name}_COMPONENT   ${com_pkg}   PARENT_SCOPE )
  set ( ${name}_COMPONENTS  ${comp_pkg}  PARENT_SCOPE )
  set ( ${name}_SOURCES     ${srcs_pkg}  PARENT_SCOPE )
  set ( ${name}_DEFINITIONS ${defs_pkg}  PARENT_SCOPE )
endfunction ()

function ( dqm4hep_get_dependency_opts local_incs local_libs local_uses pkg )
  #
  #  If the variables <package>_INCLUDE_DIRS and <package>_LIBRARIES were not set
  #  at the parent level, check if a corresponding property exists....
  #
  string ( TOUPPER "${pkg}" pkg )
  if ( "${${pkg}_INCLUDE_DIRS}" STREQUAL "" )
    get_property ( ${pkg}_INCLUDE_DIRS GLOBAL PROPERTY ${pkg}_INCLUDE_DIRS )
  endif()
  if ( "${${pkg}_LIBRARIES}" STREQUAL "" )
    get_property(${pkg}_LIBRARIES    GLOBAL PROPERTY ${pkg}_LIBRARIES )
  endif()
  set ( libs "${${pkg}_LIBRARIES}" )
  string ( REGEX REPLACE "  " " " libs "${libs}" )
  string ( REGEX REPLACE " " ";"  libs "${libs}" )

  set ( incs "${${pkg}_INCLUDE_DIRS}" )
  string ( REGEX REPLACE "  " " " incs "${incs}" )
  string ( REGEX REPLACE " " ";"  incs "${incs}" )
  
  set ( uses "${${pkg}_USES}" )
  string ( REGEX REPLACE "  " " " uses "${uses}" )
  string ( REGEX REPLACE " " ";"  uses "${uses}" )
  
  set ( ${local_incs} ${incs} PARENT_SCOPE )
  set ( ${local_libs} ${libs} PARENT_SCOPE )
  set ( ${local_uses} ${uses} PARENT_SCOPE )
endfunction ()

function ( dqm4hep_find_external_package PKG_NAME )
  set ( pkg ${PKG_NAME} )
  if ( "${pkg}" STREQUAL "MYSQL" )
    set (pkg "MySQL" )
  elseif ( "${pkg}" STREQUAL "XDRSTREAM" )
    set (pkg "xdrstream" )
  endif()
  if ( "${${pkg}_LIBRARIES}" STREQUAL "" )
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} ${ARG_ARGS} )
  else()
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} ${ARG_ARGS} )
  endif()
  # Propagate values to caller
  string ( TOUPPER "${pkg}" PKG )
  set ( libs ${${pkg}_LIBRARY}     ${${pkg}_LIBRARIES}   ${${pkg}_COMPONENT_LIBRARIES} )
  set ( incs ${${pkg}_INCLUDE_DIR} ${${pkg}_INCLUDE_DIRS} )
  if ( NOT "${pkg}" STREQUAL "${PKG}" )
    set ( libs ${libs}  ${${PKG}_LIBRARIES} ${${PKG}_LIBRARY} ${${PKG}_COMPONENT_LIBRARIES} )
    set ( incs ${incs}  ${${PKG}_INCLUDE_DIRS} ${${PKG}_INCLUDE_DIR} )
  endif()
  ##MESSAGE(STATUS "Call find_package: ${pkg}/${PKG_NAME} -> ${libs}" )
  dqm4hep_make_unique_list ( libs VALUES ${libs} )
  dqm4hep_make_unique_list ( incs VALUES ${incs} )

  set ( ${PKG}_EXISTS       "ON"       PARENT_SCOPE )
  set ( ${PKG}_LIBRARIES    ${libs}    PARENT_SCOPE )
  set ( ${PKG}_INCLUDE_DIRS ${incs}    PARENT_SCOPE )
  if ( "${incs}" STREQUAL "" )
    message ( FATAL_ERROR "Unknown package ${pkg}" )
  endif()
endfunction()

function( dqm4hep_add_library binary building )
  cmake_parse_arguments ( ARG "NODEFAULTS;NOINSTALL" "" "SOURCES;GENERATED;LINK_LIBRARIES;INCLUDE_DIRS;USES;OPTIONAL;DEFINITIONS;PRINT" ${ARGN} )
  dqm4hep_package_properties( pkg PKG_NAME enabled )
  set ( tag "Library[${pkg}] -> ${binary}" )
  if ( NOT "${ARG_PRINT}" STREQUAL "" )
    set ( tag ${ARG_PRINT} )
  endif()

  if ( "${enabled}" STREQUAL "OFF" )
    message ( "${tag} DISBALED -- package is not built!" )
  else()
    set ( building_binary "OFF" )

    if ( NOT "${ARG_OPTIONAL}" STREQUAL "" )
      dqm4hep_handle_optional_sources ( ${tag} "${ARG_OPTIONAL}" optional_missing optional_uses optional_sources )
    endif()

    if ( NOT "${optional_missing}" STREQUAL "" )
      message ( "|++> ${tag} (optional) skipped. Missing dependency: ${optional_missing}" )
    else()
      if ( ${ARG_NODEFAULTS} )
        set ( LOCAL_LINK_LIBRARIES )
        set ( LOCAL_LINK_LIBRARIES )
        set ( LOCAL_DEFINITIONS )
      else()
        dqm4hep_use_package( "${tag}" PACKAGE LOCAL 
          USES     ${ARG_USES} ${optional_uses}
          OPTIONAL ${ARG_OPTIONAL} )
      endif()
      if ( NOT "${LOCAL_MISSING}" STREQUAL "" )
        message ( "|++> ${tag} skipped. Missing dependency: ${missing}" )
      endif()
      #
      dqm4hep_make_unique_list( pkg_incs VALUES ${LOCAL_INCLUDE_DIRS}   ${ARG_INCLUDE_DIRS} )
      dqm4hep_make_unique_list( pkg_libs VALUES ${LOCAL_LINK_LIBRARIES} ${ARG_LINK_LIBRARIES} )
      dqm4hep_make_unique_list( pkg_defs VALUES ${LOCAL_DEFINITIONS}    ${ARG_DEFINITIONS} )
      #
      file ( GLOB   sources ${ARG_SOURCES} )
      list ( APPEND sources ${optional_sources} )
      if ( NOT "${ARG_GENERATED}" STREQUAL "" )
        #
        # root-cint produces warnings of type 'unused-function' disable them on generated files
        foreach ( f in  ${ARG_GENERATED} )
          set_source_files_properties( ${f} PROPERTIES COMPILE_FLAGS "-Wno-unused-function -Wno-overlength-strings" GENERATED TRUE )
        endforeach()
        list ( APPEND sources ${ARG_GENERATED} )
      endif()
      #
      #
      if ( NOT "${sources}" STREQUAL "" )
        dqm4hep_make_unique_list ( sources  VALUES ${sources} )
        #
        dqm4hep_include_directories( "${pkg_incs}" )
        add_definitions ( ${pkg_defs} )
        #
        add_library ( ${binary} SHARED ${sources} )
        target_link_libraries ( ${binary} ${pkg_libs} )
        if ( "${${pkg}_VERSION}" STREQUAL "" OR "${${pkg}_SOVERSION}" STREQUAL "" )
          message ( FATAL_ERROR "BAD Package versions: VERSION[${pkg}_VERSION] ${${pkg}_VERSION} SOVERSION[${pkg}_SOVERSION] ${${pkg}_SOVERSION} " )
        endif()
        set_target_properties ( ${binary} PROPERTIES VERSION ${${pkg}_VERSION} SOVERSION ${${pkg}_SOVERSION})
        #
        if ( NOT ${ARG_NOINSTALL} )
          install ( TARGETS ${binary}  
            LIBRARY DESTINATION lib 
            RUNTIME DESTINATION bin)
        endif()
        set ( building_binary "ON" )
      else()
        message ( "|++> ${tag} Skipped. No sources to be compiled [Use constraint]" )
      endif()
    endif()
  endif()
  set ( ${building} ${building_binary} PARENT_SCOPE )
endfunction()

function ( dqm4hep_package_properties name upper enabled )
  get_property(n DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
  string ( TOUPPER "${n}" N )
  get_property(e GLOBAL PROPERTY DQM4hep_USE_${N} )
  set ( ${name} ${n} PARENT_SCOPE )
  set ( ${upper} ${N} PARENT_SCOPE )
  set ( ${enabled} ${e} PARENT_SCOPE )
endfunction()

function( dqm4hep_add_package_library library )
  dqm4hep_package_properties( pkg PKG enabled )
  set ( tag "Package library[${pkg}] -> ${library}" )
  if ( "${enabled}" STREQUAL "OFF" )
    message ( "${tag} DISBALED -- package is not built!" )
  else()
    dqm4hep_add_library( ${library} building ${ARGN} PRINT ${tag} )
    if ( "${building}" STREQUAL "ON" )
      get_property(pkg_libs GLOBAL PROPERTY PROPERTY ${PKG}_LIBRARIES )
      dqm4hep_make_unique_list ( pkg_libs VALUES ${pkg_libs} ${library} )
      set_property(GLOBAL PROPERTY ${PKG}_LIBRARIES ${pkg_libs} )
      # Test and print if correct
      get_property(pkg_libs GLOBAL PROPERTY PROPERTY ${PKG}_LIBRARIES )
    else()
      message ( FATAL_ERROR "Package library[${pkg}] -> ${binary} Cannot be built! This is an ERROR condition." )    
    endif()
  endif()
endfunction()

function ( dqm4hep_find_package name found )
  cmake_parse_arguments(ARG "" "TYPE" "" ${ARGN} )
  string ( TOUPPER ${name} NAME )
  get_property ( all_packages GLOBAL PROPERTY DQM4hep_ALL_PACKAGES )
  set ( found_package -1 )

  if ( NOT "${all_packages}" STREQUAL "" )
    list ( FIND all_packages "${NAME}" found_package )
  endif()

  get_property( use_pkg   GLOBAL PROPERTY DQM4hep_USE_${NAME} )

  # For internal dependencies we rely on the correct order of the included directories
  if ( NOT found_package EQUAL -1 )
    # Resolve dependency from local packages
    get_property(used GLOBAL PROPERTY DQM4hep_USE_${NAME} )
    get_property(incs GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS )
    get_property(libs GLOBAL PROPERTY ${NAME}_LIBRARIES )
    set ( DQM4hep_USE_${NAME}   "ON"    PARENT_SCOPE )
    set ( ${NAME}_LIBRARIES    ${libs} PARENT_SCOPE )
    set ( ${NAME}_INCLUDE_DIRS ${incs} PARENT_SCOPE )
    set ( ${found} "ON" PARENT_SCOPE )
  elseif ( found_package EQUAL -1 AND "${ARG_TYPE}" STREQUAL "INTERNAL" )
    set_property( GLOBAL PROPERTY DQM4hep_USE_${NAME} "OFF" )
    set ( ${found} "OFF" PARENT_SCOPE )
  elseif ( "${DQM4hep_USE_${name}}" STREQUAL "OFF" OR "${DQM4hep_USE_${NAME}}" STREQUAL "OFF" )
    set ( ${found} "OFF" PARENT_SCOPE )
  elseif ( "${use_pkg}" STREQUAL "OFF" )
    set ( ${found} "OFF" PARENT_SCOPE )
  else()
    #
    # 3 possibilities left:
    # 1) Either use external package cache from previous call (same call args!) or
    # 2) call findPackage again/first time....or
    # 3) package does not exist!
    #
    get_property( pkg_setup GLOBAL PROPERTY ${NAME}_COMPONENTS )
    set ( ARGN ${ARG_UNPARSED_ARGUMENTS} )
    set ( arguments ${NAME} ${ARGN} )
    list ( REMOVE_DUPLICATES arguments )
    list ( REMOVE_ITEM arguments "REQUIRED" "QUIET" )

    if ( "${pkg_setup}" STREQUAL "${arguments}" )
      get_property ( incs   GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS )
      get_property ( libs   GLOBAL PROPERTY ${NAME}_LIBRARIES    )
      get_property ( exists GLOBAL PROPERTY ${NAME}_EXISTS       )
      set ( DQM4hep_USE_${NAME}   "ON"       PARENT_SCOPE )
      set ( ${NAME}_EXISTS       ${exists}  PARENT_SCOPE )
      set ( ${NAME}_LIBRARIES    ${libs}    PARENT_SCOPE )
      set ( ${NAME}_INCLUDE_DIRS ${incs}    PARENT_SCOPE )
      set ( ${found}             "ON"       PARENT_SCOPE )	
    else()
      dqm4hep_find_external_package( ${name} ${ARGN} )
      if ( "${${NAME}_EXISTS}" STREQUAL "ON" )
      	set_property ( GLOBAL PROPERTY ${NAME}_COMPONENTS   ${arguments} )
      	set_property ( GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS ${${NAME}_INCLUDE_DIRS} )
      	set_property ( GLOBAL PROPERTY ${NAME}_LIBRARIES    ${${NAME}_LIBRARIES} )
      	set_property ( GLOBAL PROPERTY ${NAME}_EXISTS       ${${NAME}_EXISTS} )
      	dqm4hep_to_parent_scope ( DQM4hep_USE_${NAME} )
      	dqm4hep_to_parent_scope ( ${NAME}_EXISTS )
      	dqm4hep_to_parent_scope ( ${NAME}_LIBRARIES )
      	dqm4hep_to_parent_scope ( ${NAME}_INCLUDE_DIRS )
      	set ( ${found} "ON" PARENT_SCOPE )
      else()
	       set ( ${found} "OFF" PARENT_SCOPE )
      endif()
    endif()
  endif()
endfunction()


function( dqm4hep_use_package print_prefix inName outName )
  cmake_parse_arguments( ARG "" "NAME" "USES;OPTIONAL" ${ARGN} )
  get_property ( pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME )
  #
  get_property ( pkg_incs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_INCLUDE_DIRS   )
  get_property ( pkg_libs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_LINK_LIBRARIES )
  get_property ( pkg_uses DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_USES )
  #
  set ( missing )
  set ( used_uses ${pkg_uses} )
  set ( used_incs ${pkg_incs} )
  set ( used_libs ${pkg_libs} )
  #
  foreach( use ${ARG_USES} )
    if( "${use}" MATCHES "\\[" )
      dqm4hep_unpack_package_opts ( USE ${use} )
      dqm4hep_find_package( ${USE_NAME} pkg_found ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} )
      set ( use ${USE_NAME} )
    else()
      dqm4hep_find_package( ${use} pkg_found )
    endif()
    if ( "${pkg_found}" STREQUAL "ON" )
      set ( use ${use} )
    elseif ( "{pkg_found}" STREQUAL "OFF" )
      message ( "|++> ${print_prefix} ...Missing package: ${use} [Ignored]" )
      unset ( use )
    elseif ( "${USE_REQUIRED}" STREQUAL "REQUIRED" )
      message( FATAL_ERROR "${print_prefix} ...Missing package: ${use} [Fatal REQUIRED]" )
      set ( missing ${missing} ${use} )
      unset ( use )
    else()
      message ( "|    ${print_prefix}  ...optional: Skip sources ${USE_SOURCES} [Usage ${use} not defined]" )
      unset ( use )
    endif()
    set ( used_uses ${used_uses} ${use} )
  endforeach()
  #
  #
  #
  foreach ( use ${ARG_UNPARSED_ARGUMENTS} ${ARG_OPTIONAL} )
    if ( "${use}" MATCHES "\\[" )
      dqm4hep_unpack_package_opts ( USE ${use} )
      dqm4hep_find_package ( ${USE_NAME} pkg_found ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} )
      set ( use ${USE_NAME} )
      set ( src "sources ${USE_SOURCES}" )
    else()
      dqm4hep_find_package ( ${use} pkg_found )
      set ( src ${use} )
    endif()
    if ( NOT "${pkg_found}" STREQUAL "ON" )
      message ( "|    ${print_prefix}  ...optional: Skip optional ${src} [${use} Not defined]" )
      unset ( use )
    endif()
    set ( used_uses ${used_uses} ${use} )
  endforeach()
  #
  set (all_used ${used_uses} )
  foreach ( use ${used_uses} )
    get_property ( dependent_uses GLOBAL PROPERTY ${use}_USES )
    foreach ( dependent ${dependent_uses} )
      list ( FIND used_uses ${dependent} already_used )
      if ( ${already_used} EQUAL -1 )
	       set ( all_used ${all_used} ${dependent} )
      else()
	       set ( all_used ${all_used} ${dependent} )
      endif()
    endforeach()
  endforeach()
  #
  set ( used_uses ${all_used} )
  foreach ( use ${used_uses} )
    dqm4hep_get_dependency_opts( local_incs local_libs local_uses ${use} )
    set ( used_incs ${used_incs} ${local_incs} )
    set ( used_libs ${used_libs} ${local_libs} )
    dqm4hep_make_unique_list ( used_incs VALUES ${used_incs} )
    dqm4hep_make_unique_list ( used_libs VALUES ${used_libs} )
  endforeach()
  #
  dqm4hep_make_unique_list ( used_incs VALUES ${used_incs} )
  dqm4hep_make_unique_list ( used_libs VALUES ${used_libs} )
  dqm4hep_make_unique_list ( used_uses VALUES ${used_uses} )
  dqm4hep_make_unique_list ( missing   VALUES ${missing} )
  #
  if ( "${used_uses}" STREQUAL "" AND "${missing}" STREQUAL "" )
    message ( "|++> ${print_prefix} Uses  DEFAULTS" )
  elseif ( "${missing}" STREQUAL "" )
    message ( "|++> ${print_prefix} Uses: ${used_uses}" )
  endif()
  #
  set ( ${outName}_MISSING        ${missing}   PARENT_SCOPE )
  set ( ${outName}_INCLUDE_DIRS   ${used_incs} PARENT_SCOPE )
  set ( ${outName}_LINK_LIBRARIES ${used_libs} PARENT_SCOPE )
  set ( ${outName}_USES           ${used_uses} PARENT_SCOPE )
endfunction()


function( dqm4hep_package packageName )
  
  cmake_parse_arguments(ARG "" "LIBRARY;MAJOR;MINOR;PATCH" "USES;OPTIONAL;LINK_LIBRARIES;INCLUDE_DIRS;INSTALL_INCLUDES" ${ARGN} )
  set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME ${packageName} )
  string ( TOUPPER "${packageName}" PKG_NAME )
  
  set ( missing_uses )
  foreach ( use ${ARG_USES} )
    dqm4hep_unpack_package_opts ( TEST ${use} )
    dqm4hep_find_package ( ${TEST_NAME} pkg_found )
    if ( "${pkg_found}" STREQUAL "OFF" )
      message( FATAL_ERROR "++ Package ${PKG_NAME}: missing dependency ${TEST_NAME}" )
    endif()
  endforeach()

  dqm4hep_set_version ( ${packageName} MAJOR ${ARG_MAJOR} MINOR ${ARG_MINOR} PATCH ${ARG_PATCH} )
  dqm4hep_to_parent_scope ( ${packageName}_VERSION_MAJOR )
  dqm4hep_to_parent_scope ( ${packageName}_VERSION_MINOR )
  dqm4hep_to_parent_scope ( ${packageName}_VERSION_PATCH )
  dqm4hep_to_parent_scope ( ${packageName}_VERSION )
  dqm4hep_to_parent_scope ( ${packageName}_SOVERSION )
  
  set ( vsn "Version: ( ${${packageName}_VERSION_MAJOR}.${${packageName}_VERSION_MINOR}.${${packageName}_VERSION_PATCH} )" )
  message ( "<<<< PACKAGE ${packageName} >>>> ${vsn}" )

  dqm4hep_use_package ( "Package[${packageName}]" PACKAGE PACKAGE 
    USES     ${ARG_USES} 
    OPTIONAL ${ARG_OPTIONAL} )

  if ( "${PACKAGE_MISSING}" STREQUAL "" )
    set (used_incs  ${PACKAGE_INCLUDE_DIRS} )
    set (used_libs  ${PACKAGE_LINK_LIBRARIES} )
    #
    #  Define the include directories including dependent packages
    #
    list(APPEND used_incs ${CMAKE_CURRENT_SOURCE_DIR}/include )
    foreach( inc ${ARG_INCLUDE_DIRS} )
      list( APPEND used_incs ${CMAKE_CURRENT_SOURCE_DIR}/${inc} )
    endforeach()
    #
    #  Build the list of link libraries required to build the package library and plugins etc.
    #
    dqm4hep_make_unique_list ( used_libs VALUES ${used_libs} ${ARG_LINK_LIBRARIES} )
    dqm4hep_make_unique_list ( used_incs VALUES ${used_incs} )
    #
    #  Save the variables in the context of the current source directory (ie. THIS package)
    #
    set ( use "ON" )
    set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_INCLUDE_DIRS   ${used_incs} )
    set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_LINK_LIBRARIES ${used_libs} )

    set_property ( GLOBAL PROPERTY ${PKG_NAME}_LIBRARIES    ${used_libs} )
    set_property ( GLOBAL PROPERTY DQM4hep_USE_${PKG_NAME}   ${use} )
    set_property ( GLOBAL PROPERTY ${PKG_NAME}_INCLUDE_DIRS ${used_incs} )
    set_property ( GLOBAL PROPERTY ${PKG_NAME}_USES         ${PACKAGE_USES} )
    #
    #  Add package to 'internal' package list
    #
    get_property ( all_packages GLOBAL PROPERTY DQM4hep_ALL_PACKAGES )
    set ( all_packages ${PKG_NAME} ${all_packages} )
    set_property ( GLOBAL PROPERTY DQM4hep_ALL_PACKAGES ${all_packages} )
    get_property ( use GLOBAL PROPERTY DQM4hep_USE_${PKG_NAME} )
    #
    #  Define the installation pathes of the headers.
    #
    dqm4hep_install_dir ( ${ARG_INSTALL_INCLUDES} DESTINATION include )
  else()
    message( FATAL_ERROR "Missing package dependencies: ${PACKAGE_MISSING}" )
  endif()
  
endfunction()


function( dqm4hep_generate_master_config )
  # generate DQM4hepConfig.cmake file
  configure_file( 
    "${DQM4hep_CMAKE_MODULES_ROOT}/DQM4hepConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/DQM4hepConfig.cmake" 
    @ONLY
  )
  # generate DQM4hepConfigVersion.cmake file
  configure_file( 
    "${DQM4hep_CMAKE_MODULES_ROOT}/DQM4hepConfigVersion.cmake.in"
    "${PROJECT_BINARY_DIR}/DQM4hepConfigVersion.cmake" 
    @ONLY
  )
  install( FILES "${PROJECT_BINARY_DIR}/DQM4hepConfigVersion.cmake" DESTINATION lib/cmake )
  install( FILES "${PROJECT_BINARY_DIR}/DQM4hepConfig.cmake" DESTINATION lib/cmake )
endfunction()


function ( dqm4hep_handle_optional_sources tag optionals missing uses sources )
  get_property ( pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
  set (miss)
  set (src)
  set (use)
  foreach ( opt ${optionals} )
    dqm4hep_unpack_package_opts ( USE ${opt} )
    dqm4hep_find_package( ${USE_NAME} pkg_found 
      ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} 
      TYPE ${USE_TYPE} )
    if ( "${pkg_found}" STREQUAL "OFF" )
      message ( "|    ${tag}  ...optional: Skip sources ${USE_SOURCES} [requires ${USE_NAME}]" )
    elseif ( "${pkg_found}" STREQUAL "ON" )
      message ( "|    ${tag}  ...optional: ADD sources ${USE_SOURCES} [dependent on ${USE_NAME}]" )
      file ( GLOB opt_sources ${USE_SOURCES} )
      set ( src ${src} ${opt_sources} )
      set ( use ${use} ${opt} )
    elseif ( "${USE_REQUIRED}" STREQUAL "REQUIRED" )
      set ( miss ${miss} ${USE_NAME} )
    else()
      message ( "|    ${tag}  ...optional: Skip sources ${USE_SOURCES} [requires ${USE_NAME}]" )
    endif()
  endforeach()
  set ( ${missing} ${miss} PARENT_SCOPE )
  set ( ${uses}    ${use}  PARENT_SCOPE )
  set ( ${sources} ${src}  PARENT_SCOPE )
endfunction()


function( dqm4hep_add_dictionary dictionary )
  dqm4hep_package_properties( pkg PKG enabled )
  set ( tag "Dictionary[${pkg}] -> ${dictionary}" )
  if ( "${enabled}" STREQUAL "OFF" )
    message ( "${tag} DISBALED -- package is not built!" )
  else()
    cmake_parse_arguments(ARG "" "" "SOURCES;EXCLUDE;LINKDEF;OPTIONS;OPTIONAL;INCLUDES;OUTPUT" ${ARGN} )
    message ( "|++> ${tag} Building dictionary ..." ) 
    #
    if ( NOT "${ARG_OPTIONAL}" STREQUAL "" )
      dqm4hep_handle_optional_sources ( ${tag} "${ARG_OPTIONAL}" optional_missing optional_uses optional_sources )
    endif()
    #
    if ( NOT "${optional_missing}" STREQUAL "" )
      message ( "|++> ${tag} SKIPPED. Missing optional dependencies: ${optional_missing}" )
    else()
      set ( uses ${ARG_USES} ${optional_uses} )
      dqm4hep_use_package ( ${tag} PACKAGE LOCAL 
        USES     "${uses}"
        OPTIONAL "${ARG_OPTIONAL}" )
    endif()
    if ( NOT "${LOCAL_MISSING}" STREQUAL "" )
      message ( "|++> ${tag} skipped. Missing dependency: ${missing}  --> FATAL ERROR. Build should fail!" )
    endif()
    #
    get_property(incs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_INCLUDE_DIRS)
    get_property(defs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
    get_property(opts DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_OPTIONS)
    dqm4hep_make_unique_list( incs VALUES ${LOCAL_INCLUDE_DIRS}   ${incs} )
    dqm4hep_make_unique_list( defs VALUES ${LOCAL_DEFINITIONS}    ${defs} )
    #
    file( GLOB headers ${ARG_SOURCES} )
    file( GLOB excl_headers ${ARG_EXCLUDE} )
    foreach( f ${excl_headers} )
      list( REMOVE_ITEM headers ${f} )
      message ( "|++        exclude: ${f}" )
    endforeach()
    #
    set ( inc_dirs -I${CMAKE_CURRENT_SOURCE_DIR}/include )
    foreach ( inc ${ARG_INCLUDES} )
      set ( inc_dirs ${inc_dirs} -I${inc} )
    endforeach()
    foreach ( inc ${incs} )
      file ( GLOB inc ${inc} )
      set ( inc_dirs ${inc_dirs} -I${inc} )
    endforeach()
    #
    file ( GLOB linkdefs ${ARG_LINKDEF} )
    #
    set ( comp_defs )
    foreach ( def ${defs} )
      set ( comp_defs ${comp_defs} -D${def} )
    endforeach()
    #
    dqm4hep_make_unique_list ( sources   VALUES ${headers} )
    dqm4hep_make_unique_list ( linkdefs  VALUES ${linkdefs} )
    dqm4hep_make_unique_list ( inc_dirs  VALUES ${inc_dirs} )
    dqm4hep_make_unique_list ( comp_defs VALUES ${comp_defs} )
    #
    set ( output_dir ${CMAKE_CURRENT_BINARY_DIR}/../lib )
    if ( NOT "${ARG_OUTPUT}" STREQUAL "" )
      set ( output_dir ${ARG_OUTPUT} )
    endif()
    add_custom_command(OUTPUT ${dictionary}.cxx
      COMMAND ${ROOT_rootcling_CMD} -cint -f ${dictionary}.cxx
      -s ${output_dir}/${dictionary} -inlineInputHeader -c -p ${ARG_OPTIONS} ${comp_defs} -std=c++${CMAKE_CXX_STANDARD} ${inc_dirs} ${headers} ${linkdefs}
      DEPENDS ${headers} ${linkdefs} )
    #  Install the binary to the destination directory
    install(FILES ${output_dir}/${dictionary}_rdict.pcm DESTINATION lib)
    endif()
endfunction()

function ( dqm4hep_add_executable binary )
  #set ( DD4HEP_DEBUG_CMAKE "ON" )
  dqm4hep_package_properties( pkg PKG enabled )
  set ( tag "Executable[${pkg}] -> ${binary}" )
  if ( "${enabled}" STREQUAL "OFF" )
    message ( "${tag} DISBALED -- package is not built!" )
  else()
    cmake_parse_arguments ( ARG "" "" "SOURCES;GENERATED;LINK_LIBRARIES;INCLUDE_DIRS;USES;OPTIONAL" ${ARGN})
    if ( NOT "${ARG_OPTIONAL}" STREQUAL "" )
      dqm4hep_handle_optional_sources ( ${tag} "${ARG_OPTIONAL}" optional_missing optional_uses optional_sources )
    endif()
    #
    if ( NOT "${optional_missing}" STREQUAL "" )
      message ( "|++> ${tag} SKIPPED. Missing optional dependencies: ${optional_missing}" )
    else()
      set ( uses ${ARG_USES} ${optional_uses} )
      dqm4hep_use_package ( ${tag} PACKAGE LOCAL 
        USES     "${uses}"
        OPTIONAL "${ARG_OPTIONAL}" )
      if ( "${LOCAL_MISSING}" STREQUAL "" )
        #
        get_property ( pkg_library GLOBAL PROPERTY ${PKG}_LIBRARIES )
        #
        #  Sources may also be supplied without argument tag:
        if( "${ARG_SOURCES}" STREQUAL "")
	  set ( ARG_SOURCES ${ARG_UNPARSED_ARGUMENTS} )
        endif()
        #  Prepare flags for cint generated sources:
        foreach ( f in  ${ARG_GENERATED} )
          set_source_files_properties( ${f} PROPERTIES COMPILE_FLAGS "-Wno-unused-function -Wno-overlength-strings" GENERATED TRUE )
        endforeach()
        #
        set ( sources ${ARG_GENERATED} ${ARG_SOURCES} ${optional_sources} )
        #
        if( NOT "${sources}" STREQUAL "")
	  set (incs ${LOCAL_INCLUDE_DIRS} ${ARG_INCLUDE_DIRS} )
	  set (libs ${pkg_library} ${LOCAL_LINK_LIBRARIES} ${ARG_LINK_LIBRARIES} )
	  dqm4hep_make_unique_list ( incs    VALUES ${incs} )
	  dqm4hep_make_unique_list ( libs    VALUES ${libs} )
	  dqm4hep_make_unique_list ( sources VALUES ${sources} )
	  #
	  dqm4hep_include_directories( "${incs}" )
	  add_executable( ${binary} ${sources} )
	  target_link_libraries( ${binary} ${libs} )
	  #
	  #  Install the binary to the destination directory
	  install(TARGETS ${binary} 
	    LIBRARY DESTINATION lib 
	    RUNTIME DESTINATION bin )
        else()
	         message ( "|++> ${tag} SKIPPED. No sources to build [Use constraint]" )
        endif()
      else()
        message( FATAL_ERROR "${tag} SKIPPED. Missing dependencies: ${LOCAL_MISSING}" )
      endif()
    endif()
  endif()
endfunction()
  
# Create a root dictionary using rootcint. The command is 
# attached to a target and run before the target is built
function( dqm4hep_create_dictionary )
  
  if( NOT ROOT_CINT_EXECUTABLE )
    message( SEND_ERROR "Create dictionary: ROOT cint executable not defined (ROOT_CINT_EXECUTABLE) !" )
  endif()

  # parse the macro arguments
  set( FLAG_OPTIONS "" )
  set( SINGLE_OPTIONS DICTIONARY LINK_DEF DICT_FILE )
  set( MULTI_OPTIONS HEADERS INCLUDES FLAGS DEPENDS )
  cmake_parse_arguments( ROOT_DICT "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )

  if( NOT ROOT_DICT_DICTIONARY )
    message( SEND_ERROR "Create dictionary: no ROOT dict name specified !" )
  endif()
  
  if( NOT ROOT_DICT_HEADERS )
    message( SEND_ERROR "Create dictionary: no header specified !" )
  endif()
  
  if( NOT ROOT_DICT_LINK_DEF )
    message( SEND_ERROR "Create dictionary: no LinkDef file specified !" )
  endif()

  if( NOT ROOT_DICT_INCLUDES )
    set( ROOT_DICT_INCLUDES "" )

    get_directory_property( IncludeDirs INCLUDE_DIRECTORIES )

    foreach( dir ${IncludeDirs} )
       set( ROOT_DICT_INCLUDES ${ROOT_DICT_INCLUDES}\t-I${dir} )
    endforeach(dir)
  endif()
  
  set( ROOT_DICT_OUTPUT_DIR ${PROJECT_BINARY_DIR}/dict )
  set( ROOT_DICT_DICT_SRC ${ROOT_DICT_OUTPUT_DIR}/${ROOT_DICT_DICTIONARY}Dict.cc )
  set( ROOT_DICT_PCM_FILE ${ROOT_DICT_OUTPUT_DIR}/${ROOT_DICT_DICTIONARY}Dict_rdict.pcm )
  
  add_custom_command(
    OUTPUT ${ROOT_DICT_DICT_SRC} ${ROOT_DICT_PCM_FILE}
    COMMAND mkdir -p ${ROOT_DICT_OUTPUT_DIR}
    COMMAND ${ROOT_CINT_EXECUTABLE} -f ${ROOT_DICT_DICT_SRC} ${ROOT_DICT_FLAGS} ${ROOT_DICT_INCLUDES} ${ROOT_DICT_HEADERS} ${ROOT_DICT_LINK_DEF}
    COMMENT "Generating ROOT dictionary ${ROOT_DICT_DICTIONARY} ..."
    DEPENDS ${ROOT_DICT_HEADERS} ${ROOT_DICT_DEPENDS}
  )
  
  # Return the name of created file
  if( ROOT_DICT_DICT_FILE )
    set( ${ROOT_DICT_DICT_FILE} ${ROOT_DICT_DICT_SRC} )
  endif()
  
  install( FILES ${ROOT_DICT_PCM_FILE} DESTINATION lib )
  
endfunction()

function ( dqm4hep_add_test_reg test_name )
  cmake_parse_arguments(ARG "BUILD_EXEC" "OUTPUT" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED;REQUIRES" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( ${ARG_BUILD_EXEC} )
    dqm4hep_add_executable ( ${test_name} SOURCES src/${test_name}.cc )
  endif()

  set ( cmd ${ARG_COMMAND} )
  if ( "${cmd}" STREQUAL "" )
    set ( cmd ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${test_name} )
  endif()

  set ( passed ${ARG_REGEX_PASS} ${ARG_REGEX_PASSED} )
  if ( "${passed}" STREQUAL "NONE" )
    unset ( passed )
  elseif ( "${passed}" STREQUAL "" )
    set ( passed "TEST_PASSED" )
  endif()

  set ( failed ${ARG_REGEX_FAIL} ${ARG_REGEX_FAILED} )
  if ( "${failed}" STREQUAL "NONE" )
    unset ( failed )
  endif()
  set ( output ${ARG_OUTPUT} )

  set ( args ${ARG_EXEC_ARGS} )
  if ( "${args}" STREQUAL "" )
    set ( args ${test_name} )
  endif()
  add_test(NAME t_${test_name} COMMAND ${cmd} ${output} ${args} ${output} )
  if ( NOT "${passed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${passed}" )
  endif()
  if ( NOT "${failed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${failed}" )
  endif()
  # Set test dependencies if present
  foreach ( _dep ${ARG_DEPENDS} )
    set_tests_properties( t_${test_name} PROPERTIES DEPENDS t_${_dep} )
  endforeach()
endfunction()
