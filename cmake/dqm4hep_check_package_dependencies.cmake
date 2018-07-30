
set( DQM4HEP_ROOT_COMPONENTS Core Hist Rint HistPainter Graf Graf3d MathCore Net RIO Tree )
set( DQM4HEP_ROOT_VERSION 6.08 )
set( DQM4HEP_MySQL_VERSION "" )

macro( DQM4HEP_CHECK_PACKAGE_DEPENDENCIES )
  # ----- check ROOT package -----
  find_package( ROOT ${DQM4HEP_ROOT_VERSION} COMPONENTS ${DQM4HEP_ROOT_COMPONENTS} REQUIRED )
  # ----- check MySQL package -----
  find_package( MySQL ${DQM4HEP_MySQL_VERSION} REQUIRED )
  # ----- check Doxygen package -----
  if( DQM4HEP_DOXYGEN_DOC )
    find_package( Doxygen REQUIRED )
  endif()
  # ----- check MkDocs package -----
  if( DQM4HEP_BUILD_MKDOCS )
    find_package( MkDocs REQUIRED )
  endif()
endmacro()