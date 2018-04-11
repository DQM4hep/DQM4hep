
# Create a root dictionary using rootcint. The command is 
# attached to a target and run before the target is built
macro( dqm4hep_create_dictionary )
  
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
  
endmacro()