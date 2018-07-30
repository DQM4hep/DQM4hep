
# look for /proc directory
set( DQM4HEP_WITH_PROC_FS 0 )
message( STATUS "Check for proc fs directory" )

if( EXISTS "/proc" )
  set( DQM4HEP_WITH_PROC_FS 1 )
  add_definitions( "-DDQM4HEP_WITH_PROC_FS=1" )
  message( STATUS "Check for proc fs directory: found" )
else()
  message( STATUS "Check for proc fs directory: not found" )
endif()