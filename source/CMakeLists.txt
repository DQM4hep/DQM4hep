#######################################################
# cmake file for building DQMNet package
# @author Ete Remi
# Copyright (c) 
#######################################################

add_subdirectory( dim )

# include directories
include_directories( BEFORE dim/include )
include_directories( BEFORE include )

# require proper c++
add_definitions( "-pedantic -Wunused-value -O2" )
add_definitions("-Wno-long-long -Wreturn-type -Werror")

# -------------------------------------------------
# build the project library
aux_source_directory( src SRC_FILES )
add_shared_library( ${PROJECT_NAME} ${SRC_FILES} )
target_link_libraries( ${PROJECT_NAME} dim_shared )
install( TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib )

# -------------------------------------------------
# main executable
dqm4hep_add_main_executables( EXCEPT dqm4hep-ws-server )

# -------------------------------------------------
# clang tidy and format rules
dqm4hep_run_clang_tidy()
dqm4hep_run_clang_format()

# -------------------------------------------------
# web socket bridge process
if( DQMNET_WEBSOCKETS )
  dqm4hep_add_executable( main dqm4hep-ws-server bin )
  target_link_libraries( dqm4hep-ws-server ${Boost_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )
else()
  set( EXCLUDE_INSTALL ${EXCLUDE_INSTALL} "include/dqm4hep/ws" )
endif()

# -------------------------------------------------
# install includes
if( EXCLUDE_INSTALL )
  install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" PATTERN ${EXCLUDE_INSTALL} EXCLUDE )
else()
  install_directory( include DESTINATION . FILES_MATCHING PATTERN "*.h" )
endif()
