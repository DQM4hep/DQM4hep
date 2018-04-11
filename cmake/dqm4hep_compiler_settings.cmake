
# Require C++11
include( CheckCXXCompilerFlag )

check_cxx_compiler_flag( "-std=c++11" COMPILER_SUPPORTS_CXX11 )

if( COMPILER_SUPPORTS_CXX11 )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
else()
	message( SEND_ERROR "${PROJECT_NAME} requires C++11 support. Please upgrade your compiler !" )
endif()