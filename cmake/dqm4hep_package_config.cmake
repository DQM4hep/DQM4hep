
# Standard macro to export a DQM4hep package as a CMake project
macro( dqm4hep_generate_package_confguration )
  
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
      
  # generate config header
  configure_file( 
    "${DQM4HEP_CMAKE_MODULES_ROOT}/PackageConfig.h.cmake.in"
    "${PACKAGE_CONFIG_HEADER_OUTPUT_PATH}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.h" 
    @ONLY
  )
  
  # generate Config.cmake file
  configure_file( 
    "${DQM4HEP_CMAKE_MODULES_ROOT}/PackageConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.cmake" 
    @ONLY
  )
  
  # generate ConfigVersion.cmake file
  configure_file( 
    "${DQM4HEP_CMAKE_MODULES_ROOT}/PackageConfigVersion.cmake.in"
    "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}ConfigVersion.cmake" 
    @ONLY
  )
  
  # generate LibDeps.cmake file
  export_library_dependencies( "${PACKAGE_CONFIG_PACKAGE_NAME}LibDeps.cmake" )
  
  # install all generate config cmake files
  install( FILES "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}LibDeps.cmake" DESTINATION lib/cmake )
  install( FILES "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}ConfigVersion.cmake" DESTINATION . )
  install( FILES "${PROJECT_BINARY_DIR}/${PACKAGE_CONFIG_PACKAGE_NAME}Config.cmake" DESTINATION . )
  
endmacro()