
# set default cmake build type to RelWithDebInfo
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "RelWithDebInfo" )
endif()

# write this variable to cache
set( 
  CMAKE_BUILD_TYPE 
  "${CMAKE_BUILD_TYPE}" 
  CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." 
  FORCE
)
