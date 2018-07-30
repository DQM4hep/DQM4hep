# dqm4hep_clang_tools.cmake file
# Defines targets for running clang-tidy and clang-format
# author: rete, DESY

# -------------------------------------------------
# dqm4hep_clang_tidy macro
macro( DQM4HEP_CLANG_TIDY )
  
  if( NOT CLANG_TIDY_PROGRAM )
    message( SEND_ERROR "CLANG_TIDY - Couldn't create cland-tidy target: the clang-tidy program is not defined" )
  endif()
  
  # parse the macro arguments
  cmake_parse_arguments( CLANG_TIDY "DEBUG" "CHECKS" "SOURCES" ${ARGN} )

  if( NOT CLANG_TIDY_SOURCES )
    message( SEND_ERROR "CLANG_TIDY - Couldn't create cland-tidy target: no source file provided" )
  endif()
  
  if( NOT CLANG_TIDY_CHECKS )
    message( SEND_ERROR "CLANG_TIDY - Couldn't create cland-tidy target: no check provided" )
  endif()
   
  if( CLANG_TIDY_DEBUG )
    message( "CLANG_TIDY_SOURCES: ${CLANG_TIDY_SOURCES}" )
    message( "CLANG_TIDY_PROGRAM: ${CLANG_TIDY_PROGRAM}" )
  endif()
  
  add_custom_target(
    clang-tidy
    COMMAND 
    ${DQM4HEP_CMAKE_MODULES_ROOT}/run-clang-tidy.py
    -clang-tidy-binary ${CLANG_TIDY_PROGRAM}
    -header-filter='./include/dqm4hep/*'
    -checks='${CLANG_TIDY_CHECKS}'
    ${CLANG_TIDY_SOURCES}
    COMMENT "Running clang-tidy ..."
  )
  
  add_custom_target(
    clang-tidy-fix
    COMMAND 
    ${DQM4HEP_CMAKE_MODULES_ROOT}/run-clang-tidy.py
    -clang-tidy-binary ${CLANG_TIDY_PROGRAM}
    -header-filter='.*'
    -checks='${CLANG_TIDY_CHECKS}'
    -fix
    ${CLANG_TIDY_SOURCES}
    COMMENT "Running clang-tidy and fixing warnings ..."
  )
  
  # mandatory to run clang tidy python script
  if( NOT CMAKE_EXPORT_COMPILE_COMMANDS )
    set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
  endif()
endmacro()

# -------------------------------------------------
# dqm4hep_run_clang_tidy macro
macro( DQM4HEP_RUN_CLANG_TIDY )
  
  if( NOT CLANG_TIDY_PROGRAM )
      message( WARNING "clang-tidy program not found ! Target will not be created !" )
  else()
    if( NOT DQM4HEP_CLANG_TIDY_SOURCES )
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/src" )
        file( GLOB_RECURSE DQM4HEP_CLANG_TIDY_SOURCES_SRC "${PROJECT_SOURCE_DIR}/source/src/*.cc" )
        list( APPEND DQM4HEP_CLANG_TIDY_SOURCES ${DQM4HEP_CLANG_TIDY_SOURCES_SRC} )
      endif()
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/main" )
        file( GLOB DQM4HEP_CLANG_TIDY_SOURCES_MAIN "${PROJECT_SOURCE_DIR}/source/main/*.cc" )
        list( APPEND DQM4HEP_CLANG_TIDY_SOURCES ${DQM4HEP_CLANG_TIDY_SOURCES_MAIN} )
      endif()
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/tests" )
        file( GLOB DQM4HEP_CLANG_TIDY_SOURCES_TESTS "${PROJECT_SOURCE_DIR}/source/tests/*.cc" )
        list( APPEND DQM4HEP_CLANG_TIDY_SOURCES ${DQM4HEP_CLANG_TIDY_SOURCES_TESTS} )
      endif()
    endif()
    
    # reformat checks list
    string( REPLACE ";" "," DQM4HEP_CLANG_TIDY_CHECKS "${DQM4HEP_CLANG_TIDY_CHECKS}" )
    
    dqm4hep_clang_tidy( SOURCES ${DQM4HEP_CLANG_TIDY_SOURCES} CHECKS ${DQM4HEP_CLANG_TIDY_CHECKS} )
  endif()
  
endmacro()

# -------------------------------------------------
# dqm4hep_clang_tidy macro
macro( DQM4HEP_CLANG_FORMAT )
  
  if( NOT CLANG_FORMAT_PROGRAM )
    message( SEND_ERROR "CLANG_FORMAT - Couldn't create clang-format target: the clang-format program is not defined" )
  endif()
  
  # parse the macro arguments
  cmake_parse_arguments( CLANG_FORMAT "DEBUG" "STYLE" "SOURCES" ${ARGN} )

  if( NOT CLANG_FORMAT_STYLE )
    message( WARNING "CLANG_FORMAT - No style specified: using llvm style !" )
    set( CLANG_FORMAT_STYLE "llvm" )
  endif()
  
  set( CLANG_FORMAT_STYLE_ARG "-style=${CLANG_FORMAT_STYLE}" )
  set( CLANG_FORMAT_FALLBACK_STYLE_ARG "-fallback-style=llvm" )
  
  if( NOT CLANG_FORMAT_SOURCES )
    message( SEND_ERROR "CLANG_FORMAT - Couldn't create clang-format target: no source file provided" )
  endif()
   
  if( CLANG_TIDY_DEBUG )
    message( "CLANG_FORMAT_SOURCES: ${CLANG_FORMAT_SOURCES}" )
    message( "CLANG_FORMAT_PROGRAM: ${CLANG_FORMAT_PROGRAM}" )
    message( "CLANG_FORMAT_STYLE  : ${CLANG_FORMAT_STYLE}"   )
  endif()
  
  add_custom_target(
    clang-format
    COMMAND 
    ${CLANG_FORMAT_PROGRAM}
    ${CLANG_FORMAT_FALLBACK_STYLE_ARG}
    ${CLANG_FORMAT_STYLE_ARG}
    ${CLANG_FORMAT_SOURCES}
    -i # to replace the formatted code !
    COMMENT "Running clang-format ..."
  )
endmacro()


# -------------------------------------------------
# dqm4hep_run_clang_format macro
macro( DQM4HEP_RUN_CLANG_FORMAT )
  
  if( NOT CLANG_FORMAT_PROGRAM )
      message( WARNING "clang-format program not found ! Target will not be created !" )
  else()
    if( NOT DQM4HEP_CLANG_FORMAT_SOURCES )
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/include/dqm4hep" )
        file( GLOB_RECURSE DQM4HEP_CLANG_FORMAT_SOURCES_INCLUDE "${PROJECT_SOURCE_DIR}/source/include/dqm4hep/*.h" )
        list( APPEND DQM4HEP_CLANG_FORMAT_SOURCES ${DQM4HEP_CLANG_FORMAT_SOURCES_INCLUDE} )
      endif()
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/src" )
        file( GLOB_RECURSE DQM4HEP_CLANG_FORMAT_SOURCES_SRC "${PROJECT_SOURCE_DIR}/source/src/*.cc" )
        list( APPEND DQM4HEP_CLANG_FORMAT_SOURCES ${DQM4HEP_CLANG_FORMAT_SOURCES_SRC} )
      endif()
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/main" )
        file( GLOB DQM4HEP_CLANG_FORMAT_SOURCES_MAIN "${PROJECT_SOURCE_DIR}/source/main/*.cc" )
        list( APPEND DQM4HEP_CLANG_FORMAT_SOURCES ${DQM4HEP_CLANG_FORMAT_SOURCES_MAIN} )
      endif()
      if( EXISTS "${PROJECT_SOURCE_DIR}/source/tests" )
        file( GLOB DQM4HEP_CLANG_FORMAT_SOURCES_TESTS "${PROJECT_SOURCE_DIR}/source/tests/*.cc" )
        list( APPEND DQM4HEP_CLANG_FORMAT_SOURCES ${DQM4HEP_CLANG_FORMAT_SOURCES_TESTS} )
      endif()
    endif()
    
    # copy the clang format config into project source directory
    file( GENERATE OUTPUT ${PROJECT_SOURCE_DIR}/.clang-format INPUT ${DQM4HEP_CMAKE_MODULES_ROOT}/dqm4hep_clang_format_config.yml )
    
    dqm4hep_clang_format( SOURCES ${DQM4HEP_CLANG_FORMAT_SOURCES} STYLE file DEBUG )
  endif()
  
endmacro()


# -------------------------------------------------
# find the clang-tidy program to enable possible related targets
if( NOT CLANG_TIDY_PROGRAM )
  find_program( CLANG_TIDY_PROGRAM "clang-tidy" )
  if( NOT CLANG_TIDY_PROGRAM )
    message( STATUS "Clang tidy: clang-tidy program not found" )
  else()
    message( STATUS "Clang tidy: ${CLANG_TIDY_PROGRAM}" )
  endif()
endif()

# -------------------------------------------------
# find the clang-format program to enable possible related targets
if( NOT CLANG_FORMAT_PROGRAM )
  find_program( CLANG_FORMAT_PROGRAM "clang-format" )
  if( NOT CLANG_FORMAT_PROGRAM )
    message( STATUS "Clang format: clang-format program not found" )
  else()
    message( STATUS "Clang format: ${CLANG_FORMAT_PROGRAM}" )
  endif()
endif()

# -------------------------------------------------
# define list of default checks if not defined
if( NOT DQM4HEP_CLANG_TIDY_CHECKS )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "-*" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "clang-diagnostic-*" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "clang-analyzer-*" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "-clang-analyzer-alpha*" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "modernize-loop-convertmodernize-use-auto,modernize-use-default,modernize-use-nullptr,modernize-use-override" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "readability-else-after-return,misc-unused-parameters" )
  list( APPEND DQM4HEP_CLANG_TIDY_CHECKS "misc-*" )
endif()

message( STATUS "Clang tidy checks: ${DQM4HEP_CLANG_TIDY_CHECKS}" )
