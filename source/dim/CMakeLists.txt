#######################################################
# cmake file for building dim software
# @author Eté Rémi, DESY
# Copyright (c) 2017
#######################################################

###############################
# User options for compiling dim
option( DIM_GUI       "Whether to build dim GUI" OFF )
option( DIM_EXAMPLES  "Whether to build dim examples" OFF )
option( DIM_32BIT     "Whether to build dim with 32 bit support" OFF )
option( DIM_STATIC    "Whether to build dim static library" OFF )

include_directories( BEFORE include )

###############################
# dim compilation flags
set( DIM_CFLAGS "" )
set( DIM_LIBFLAGS "-lpthread" )
set( DIM_EXCFLAGS "" )
set( DIM_ARCH "Linux" ) # default arch

# FIXME Do we really need this ???
if( DIM_32BIT )
  set( DIM_CFLAGS "${DIM_CFLAGS} -m32" )
  set( DIM_LIBFLAGS "${DIM_LIBFLAGS} -m32" )
  set( DIM_EXCFLAGS "${DIM_EXCFLAGS} -m32" )
endif()

if( UNIX )
  if(NOT APPLE) # linux case
    set( DIM_CFLAGS "${DIM_CFLAGS} -Wall -Wextra -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC" )
    set( DIM_EXCFLAGS "${DIM_EXCFLAGS} -Wall -Wextra" )
  else() # MACOSX case
    set( DIM_ARCH "Darwin" )
    set( DIM_CFLAGS "${DIM_CFLAGS} -DMIPSEL -DPROTOCOL=1 -Dunix -Ddarwin -fPIC" )
  endif()
else()
  message( SEND_ERROR "UNIX platform only ! Consider buying a decent OS please ..." )
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DIM_CFLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DIM_CFLAGS}" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DIM_EXCFLAGS} ${DIM_LIBFLAGS}" )

###############################
# All dev headers
set( DIM_HEADERS
  include/dic.h
  include/dic.hxx
  include/dim_common.h
  include/dim_core.hxx
  include/dim.h
  include/dim.hxx
  include/dis.h
  include/dis.hxx
  include/dllist.hxx
  include/sllist.hxx
  include/tokenstring.hxx
)

# All core sources
set( DIM_SRCS
  src/diccpp.cxx
  src/discpp.cxx
  src/dimcpp.cxx
  src/tokenstring.cxx
  src/dic.c
  src/dis.c
  src/dna.c
  src/sll.c
  src/dll.c
  src/hash.c
  src/swap.c
  src/copy_swap.c
  src/open_dns.c
  src/conn_handler.c
  src/tcpip.c
  src/dtq.c
  src/dim_thr.c
  src/utilities.c
)

###############################
# The main library
# FIXME Do we really need the static library ??
if( DIM_STATIC )
  add_library( dim_static STATIC ${DIM_SRCS} )
  set_target_properties( dim_static PROPERTIES OUTPUT_NAME dim )
  target_link_libraries( dim_static ${CMAKE_THREAD_LIBS_INIT} )
  install(
    TARGETS dim_static
    ARCHIVE DESTINATION lib
  )
endif()

add_library( dim_shared SHARED ${DIM_SRCS} )
set_target_properties( dim_shared PROPERTIES OUTPUT_NAME dim )
target_link_libraries( dim_shared ${CMAKE_THREAD_LIBS_INIT} )
install(
  TARGETS dim_shared
  LIBRARY DESTINATION lib
)

###############################
# Install dev headers
install( FILES ${DIM_HEADERS} DESTINATION include )

###############################
# dns executable
add_executable( dns src/dns.c )
target_link_libraries( dns dim_shared )
install( TARGETS dns RUNTIME DESTINATION bin )

###############################
# dim utilities
add_executable( dim_get_service src/util/dim_get_service.c )
target_link_libraries( dim_get_service dim_shared )

add_executable( dim_send_command src/util/dim_send_command.c )
target_link_libraries( dim_send_command dim_shared )

add_executable( DimBridge src/util/dimbridge.cxx )
target_link_libraries( DimBridge dim_shared )

add_executable( checkDns src/util/check_dns.cxx )
target_link_libraries( checkDns dim_shared )

add_executable( checkDimServers src/util/check_dim_servers.cxx )
target_link_libraries( checkDimServers dim_shared )

install(
  TARGETS dim_get_service dim_send_command DimBridge checkDns checkDimServers
  RUNTIME DESTINATION bin
)

###############################
# dim examples
if( DIM_EXAMPLES )
  add_executable( test_server src/examples/test_server.c )
  target_link_libraries( test_server dim_shared )

  add_executable( test_client src/examples/test_client.c )
  target_link_libraries( test_client dim_shared )

  add_executable( testServer src/examples/test_server.cxx )
  target_link_libraries( testServer dim_shared )

  add_executable( testClient src/examples/test_client.cxx )
  target_link_libraries( testClient dim_shared )

  add_executable( demo_server src/examples/demo_server.c )
  target_link_libraries( demo_server dim_shared )

  add_executable( demo_client src/examples/demo_client.c )
  target_link_libraries( demo_client dim_shared )

  install(
    TARGETS test_server test_client testClient testServer demo_client demo_server
    RUNTIME DESTINATION bin
  )
endif()


if( DIM_GUI )
  ###############################
  # Did executable
  find_package( Motif REQUIRED )
  find_package( X11 REQUIRED )

  find_library( MRM_LIBRARY Mrm )

  if( NOT MRM_LIBRARY )
    message( SEND_ERROR "libMrm.so required !" )
  else()
    message( STATUS "Found Mrm : ${MRM_LIBRARY}" )
  endif()

  find_library( XT_LIBRARY Xt )

  if( NOT XT_LIBRARY )
    message( SEND_ERROR "libXt.so required !" )
  else()
    message( STATUS "Found Xt : ${XT_LIBRARY}" )
  endif()

  include_directories( ${MOTIF_INCLUDE_DIR} )
  include_directories( ${X11_INCLUDE_DIR} )
  include_directories( src/did )

  add_executable( did src/did/dui_util.c src/did/did.c )
  target_link_libraries( did dim_shared ${MOTIF_LIBRARIES} ${X11_LIBRARIES} ${MRM_LIBRARY} ${XT_LIBRARY} )
  install(
    TARGETS did
    RUNTIME DESTINATION bin
  )

  ###############################
  # WebDid executable
  add_executable( webDid
    src/webDid/webServer.c
    src/webDid/webTcpip.c
    src/webDid/webDid.c
  )
  target_link_libraries( webDid dim_shared )

  # install webDid js and html sources with the binary in a dedicated dir
  install( DIRECTORY ./WebDID DESTINATION ./bin )
  install(
    TARGETS webDid
    RUNTIME DESTINATION bin/WebDID
  )
endif()

#
