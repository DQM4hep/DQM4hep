#######################################################
# cmake file for building DQMNet package
# @author Eté Rémi , IPNL
# Copyright (c) CNRS / IPNL
#######################################################

add_subdirectory( ./dim )

# include directories
include_directories( BEFORE dim/include )
include_directories( BEFORE include )

# require proper c++
add_definitions( "-pedantic -Wunused-value -O2" )
add_definitions("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the global library
aux_source_directory( src SRC_FILES )

add_shared_library( ${PROJECT_NAME} ${SRC_FILES} )
target_link_libraries( ${PROJECT_NAME} dim_shared )

install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
)

# add executable macro
macro( BUILD_EXECUTABLE _dir _name _install_loc)
	add_executable( ${_name} ${_dir}/${_name}.cc )
	target_link_libraries( ${_name} ${PROJECT_NAME} )
	install (
	    TARGETS ${_name}
	    RUNTIME DESTINATION ${_install_loc}
	)
endmacro()

# tests
build_executable( main test-server bin/tests )

# main binaries
build_executable( main dqm4hep-server-info bin )
build_executable( main dqm4hep-server-list bin )
build_executable( main dqm4hep-server-running bin )
build_executable( main dqm4hep-send-request bin )
build_executable( main dqm4hep-send-command bin )
build_executable( main dqm4hep-send-request-response bin )
build_executable( main dqm4hep-subscribe-service bin )

dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()

if( DQMNET_WEBSOCKETS )
  build_executable( main dqm4hep-ws-server bin )
  target_link_libraries( dqm4hep-ws-server ${Boost_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
endif()

if( NOT DQMNET_WEBSOCKETS )
  set( EXCLUDE_INSTALL ${EXCLUDE_INSTALL} "include/dqm4hep/ws" )
endif()

if( EXCLUDE_INSTALL )
  install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN ${EXCLUDE_INSTALL} EXCLUDE )
else()
  install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" )
endif()
