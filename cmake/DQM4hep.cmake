
#---------------------------------------------------------------------------------------------------
if ( DQM4hep_DIR )
  set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DQM4hep_DIR}/cmake ) 
endif()
if(CMAKE_INSTALL_PREFIX)
  set ( CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
endif()

#---------------------------------------------------------------------------------------------------
# Main functional include file
if ( "${DQM4hepBuild_included}" STREQUAL "" )
  include ( DQM4hepBuild )
endif()
