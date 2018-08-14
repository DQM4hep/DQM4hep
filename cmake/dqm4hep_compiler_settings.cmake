
option( DQM4HEP_WARNING_AS_ERROR    "Whether to add -Werror flag to cxx flags" OFF )
option( DQM4HEP_EXTRA_WARNINGS      "Whether to add -Wextra flag to cxx flags" ON )
option( DQM4HEP_DEV_WARNINGS        "Whether to add extra warning for developpers to cxx flags" OFF )

macro( DQM4HEP_REQUIRE_CPP_STANDARD )
  enable_language( CXX )

  # Set C++ standard
  set( CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard used for compiling" )
  set( CMAKE_CXX_STANDARD_REQUIRED ON )
  set( CMAKE_CXX_EXTENSIONS OFF )
endmacro()

macro( DQM4HEP_SET_CXX_FLAGS )
  
  include( CheckCXXCompilerFlag )
  
  set( COMPILER_FLAGS -Wunused-value -Wall -pedantic -Wshadow -Wformat-security -Wno-long-long -Wreturn-type -Wuseless-cast -Wlogical-op -Wredundant-decls -Weffc++ -Wno-unsequenced -Wno-deprecated-declarations -fdiagnostics-color=auto  )
  
  # APPLECLANG 9.1.0 add new warning for hardcoded include nonportable-include-path
  # This is not compatible with the way root generate its dictionary. So we disable it
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(COMPILER_FLAGS ${COMPILER_FLAGS} -Wno-error=nonportable-include-path)
  endif()
  
  if( DQM4HEP_DEV_WARNINGS )
    set( COMPILER_FLAGS ${COMPILER_FLAGS} -Wsuggest-override -Wno-comments -Wparentheses )
  endif()
  
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