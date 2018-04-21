

macro( DQM4HEP_FIND_PACKAGES )
  
  # parse the macro arguments
  set( FLAG_OPTIONS REQUIRED QUIET ADD_DEPENDS )
  set( SINGLE_OPTIONS "" )
  set( MULTI_OPTIONS PACKAGES )
  cmake_parse_arguments( FIND_PACKAGES "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  if( FIND_PACKAGES_REQUIRED )
    set( FIND_PACKAGES_PKG_MODE ${FIND_PACKAGES_PKG_MODE} REQUIRED )
  endif()
  
  if( FIND_PACKAGES_QUIET )
    set( FIND_PACKAGES_PKG_MODE ${FIND_PACKAGES_PKG_MODE} QUIET )
  endif()
  
  foreach( pkg ${FIND_PACKAGES_PACKAGES} )
    # if required, it should send error here
    find_package( ${pkg} ${FIND_PACKAGES_PKG_MODE} )
    
    if( ${pkg}_FOUND )
      include_directories( SYSTEM ${${pkg}_INCLUDE_DIRS} )
      link_libraries( ${${pkg}_LIBRARIES} )
      add_definitions ( ${${pkg}_DEFINITIONS} )

      if( FIND_PACKAGES_ADD_DEPENDS )
        list( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${pkg}_INCLUDE_DIRS} )
        list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${pkg}_LIBRARY_DIRS} )
        list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${pkg}_LIBRARIES} )
      endif()
    elseif( NOT FIND_PACKAGES_REQUIRED AND NOT FIND_PACKAGES_QUIET )
      message( WARNING "find_package( ${pkg} ${FIND_PACKAGES_PKG_MODE} ): package not found !" )
    endif()
  endforeach()
  
endmacro()


macro( DQM4HEP_FIND_PACKAGE )
  
  # parse the macro arguments
  set( FLAG_OPTIONS REQUIRED QUIET ADD_DEPENDS )
  set( SINGLE_OPTIONS PACKAGE VERSION )
  set( MULTI_OPTIONS COMPONENTS )
  cmake_parse_arguments( FIND_PKG "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  if( NOT FIND_PKG_PACKAGE )
    message( SEND_ERROR "dqm4hep_find_package(): no PACKAGE argument provided !" )
  endif()
  
  if( FIND_PKG_REQUIRED )
    set( FIND_PKG_PKG_MODE ${FIND_PKG_PKG_MODE} REQUIRED )
  endif()
  
  if( FIND_PKG_QUIET )
    set( FIND_PKG_PKG_MODE ${FIND_PKG_PKG_MODE} QUIET )
  endif()
  
  if( FIND_PKG_COMPONENTS )
    find_package( ${FIND_PKG_PACKAGE} ${FIND_PKG_VERSION} ${FIND_PKG_PKG_MODE} COMPONENTS ${FIND_PKG_COMPONENTS} )
  else()
    find_package( ${FIND_PKG_PACKAGE} ${FIND_PKG_VERSION} ${FIND_PKG_PKG_MODE} )
  endif()
  
  include_directories( SYSTEM ${${FIND_PKG_PACKAGE}_INCLUDE_DIRS} )
  link_libraries( ${${FIND_PKG_PACKAGE}_LIBRARIES} )
  add_definitions ( ${${FIND_PKG_PACKAGE}_DEFINITIONS} )

  if( FIND_PKG_ADD_DEPENDS )
    list( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${FIND_PKG_PACKAGE}_INCLUDE_DIRS} )
    list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${FIND_PKG_PACKAGE}_LIBRARY_DIRS} )
    list( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${FIND_PKG_PACKAGE}_LIBRARIES} )
  endif()
  
endmacro()
  