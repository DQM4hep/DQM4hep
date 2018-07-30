

option( DQM4HEP_DOXYGEN_DOC "Set to OFF to skip build/install Documentation" OFF )

macro( dqm4hep_build_doxygen )
  # parse the macro arguments
  set( FLAG_OPTIONS "" )
  set( SINGLE_OPTIONS INSTALL_PREFIX )
  set( MULTI_OPTIONS EXCLUDE )
  cmake_parse_arguments( DOXYGEN_DOC "${FLAG_OPTIONS}" "${SINGLE_OPTIONS}" "${MULTI_OPTIONS}" ${ARGN} )
  
  if( DQM4HEP_DOXYGEN_DOC )
      find_package( Doxygen )

      if( NOT DOXYGEN_FOUND )
          message( SEND_ERROR "Could not find doxygen required to build documentation" )
          message( "Please install doxygen or set DQM4HEP_DOXYGEN_DOC to OFF" )
          message( "" )
      endif()

      set( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild/${PROJECT_NAME}" )
      set( DOX_INPUT ${PROJECT_SOURCE_DIR}/source/include/ ${PROJECT_SOURCE_DIR}/source/src )
      file( GLOB_RECURSE DOX_SOURCE_FILES ${PROJECT_SOURCE_DIR}/source/include/* ${PROJECT_SOURCE_DIR}/source/src/* )
      set( DOXYFILE ${DQM4HEP_CMAKE_MODULES_ROOT}/Doxyfile )
      set( DOX_DEPEND ${DOXYFILE} ${DOX_SOURCE_FILES} )
      
      # custom command to build documentation
      add_custom_command(
          OUTPUT  "${DOC_BIN_DIR}/html/index.html"
          COMMAND mkdir -p ${DOC_BIN_DIR}
          COMMAND DOX_PROJECT_NAME=${PROJECT_NAME}
                  DOX_PROJECT_NUMBER="${${PROJECT_NAME}_VERSION}"
                  DOX_OUTPUT_DIRECTORY="${DOC_BIN_DIR}"
                  DOX_INPUT="${DOX_INPUT}"
                  DOX_EXLUDE_PATTERNS="${DOXYGEN_DOC_EXCLUDE}"
                  ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
          WORKING_DIRECTORY "${DQM4HEP_CMAKE_MODULES_ROOT}"
          COMMENT "Building ${PROJECT_NAME} API Documentation..."
          DEPENDS ${DOX_DEPEND}
      )

      # add doc target
      add_custom_target( doc DEPENDS "${DOC_BIN_DIR}/html/index.html" )
      
      if( NOT DOXYGEN_DOC_INSTALL_PREFIX )
        set( DOXYGEN_DOC_INSTALL_PREFIX doc/${PROJECT_NAME} )
      endif()
      
      install( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )
      install( DIRECTORY "${DOC_BIN_DIR}/html" DESTINATION ${DOXYGEN_DOC_INSTALL_PREFIX} )
      install( DIRECTORY "${DOC_BIN_DIR}/latex" DESTINATION ${DOXYGEN_DOC_INSTALL_PREFIX} )
  endif()
endmacro()


