
option( DQM4HEP_WARNING_AS_ERROR    "Whether to add -Werror flag to cxx flags" OFF )
option( DQM4HEP_EXTRA_WARNINGS      "Whether to add -Wextra flag to cxx flags" OFF )

macro( DQM4HEP_SET_CXX_FLAGS )
  # Require C++11
  include( CheckCXXCompilerFlag )
  
  set( COMPILER_FLAGS -Wunused-value -Wall -pedantic -Wshadow -Wformat-security -Wno-long-long -Wdeprecated -Wreturn-type -Wsuggest-attribute=const -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -Wno-comments -Wzero-as-null-pointer-constant -Wparentheses -Wuseless-cast -Wlogical-op -Wredundant-decls )
  
  if( DQM4HEP_WARNING_AS_ERROR )
    set( COMPILER_FLAGS ${COMPILER_FLAGS} -Werror )
  endif()
  
  if( DQM4HEP_EXTRA_WARNINGS )
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
endmacro()