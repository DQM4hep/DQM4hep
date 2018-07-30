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

