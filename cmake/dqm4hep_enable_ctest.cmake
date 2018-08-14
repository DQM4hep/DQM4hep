

option( DQM4HEP_TESTING "Whether to build unit tests (ctest)" ON )

if( DQM4HEP_TESTING )
  # enable ctest
  set( BUILD_TESTING ON )
  enable_testing()
  include( CTest )
  mark_as_advanced( DART_TESTING_TIMEOUT )
endif()

