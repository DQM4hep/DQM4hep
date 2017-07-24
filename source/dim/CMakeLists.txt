#######################################################
# cmake file for building dim software
# @author Eté Rémi, DESY
# Copyright (c) 2017
#######################################################

###############################
# User options for compiling dim
OPTION( DIM_GUI       "Whether to build dim GUI" OFF )
OPTION( DIM_EXAMPLES  "Whether to build dim examples" OFF )
OPTION( DIM_32BIT     "Whether to build dim with 32 bit support" OFF )

INCLUDE_DIRECTORIES( BEFORE include )

###############################
# dim compilation flags
SET( DIM_CFLAGS "" )
SET( DIM_LIBFLAGS "-lpthread" )
SET( DIM_EXCFLAGS "" )
SET( DIM_ARCH "Linux" ) # default arch

# FIXME Do we really need this ???
IF( DIM_32BIT )
  SET( DIM_CFLAGS "${DIM_CFLAGS} -m32" )
  SET( DIM_LIBFLAGS "${DIM_LIBFLAGS} -m32" )
  SET( DIM_EXCFLAGS "${DIM_EXCFLAGS} -m32" )
ENDIF()

IF( UNIX )
  IF(NOT APPLE) # linux case
    SET( DIM_CFLAGS "${DIM_CFLAGS} -Wall -Wextra -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC" )
    SET( DIM_EXCFLAGS "${DIM_EXCFLAGS} -Wall -Wextra" )
  ELSE() # MACOSX case
    SET( DIM_ARCH "Darwin" )
    SET( DIM_CFLAGS "${DIM_CFLAGS} -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -Ddarwin -fPIC" )

    # dim requires a patch on MacOSX
    EXECUTE_PROCESS( COMMAND patch src/tcpip.c dim4mac/tcpip.c.patch )
  ENDIF()
ELSE()
  MESSAGE( SEND_ERROR "UNIX plateform only ! Consider buying a decent OS please ..." )
ENDIF()

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DIM_CFLAGS}" )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DIM_CFLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DIM_EXCFLAGS} ${DIM_LIBFLAGS}" )

###############################
# All dev headers
SET( DIM_HEADERS
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
SET( DIM_SRCS
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
ADD_LIBRARY( dim_shared SHARED ${DIM_SRCS} )
ADD_LIBRARY( dim_static STATIC ${DIM_SRCS} )
SET_TARGET_PROPERTIES( dim_shared PROPERTIES OUTPUT_NAME dim )
SET_TARGET_PROPERTIES( dim_static PROPERTIES OUTPUT_NAME dim )
TARGET_LINK_LIBRARIES( dim_shared ${CMAKE_THREAD_LIBS_INIT} )
TARGET_LINK_LIBRARIES( dim_static ${CMAKE_THREAD_LIBS_INIT} )

INSTALL(
        TARGETS dim_shared dim_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

###############################
# Install dev headers
INSTALL( FILES ${DIM_HEADERS} DESTINATION ./include )

###############################
# dns executable
ADD_EXECUTABLE( dns src/dns.c )
TARGET_LINK_LIBRARIES( dns dim_shared )
INSTALL( TARGETS dns RUNTIME DESTINATION bin )

###############################
# dim utilities
ADD_EXECUTABLE( dim_get_service src/util/dim_get_service.c )
TARGET_LINK_LIBRARIES( dim_get_service dim_shared )

ADD_EXECUTABLE( dim_send_command src/util/dim_send_command.c )
TARGET_LINK_LIBRARIES( dim_send_command dim_shared )

ADD_EXECUTABLE( DimBridge src/util/dimbridge.cxx )
TARGET_LINK_LIBRARIES( DimBridge dim_shared )

ADD_EXECUTABLE( checkDns src/util/check_dns.cxx )
TARGET_LINK_LIBRARIES( checkDns dim_shared )

ADD_EXECUTABLE( checkDimServers src/util/check_dim_servers.cxx )
TARGET_LINK_LIBRARIES( checkDimServers dim_shared )

INSTALL(
  TARGETS dim_get_service dim_send_command DimBridge checkDns checkDimServers
  RUNTIME DESTINATION bin
)

###############################
# dim examples
IF( DIM_EXAMPLES )
  ADD_EXECUTABLE( test_server src/examples/test_server.c )
  TARGET_LINK_LIBRARIES( test_server dim_shared )

  ADD_EXECUTABLE( test_client src/examples/test_client.c )
  TARGET_LINK_LIBRARIES( test_client dim_shared )

  ADD_EXECUTABLE( testServer src/examples/test_server.cxx )
  TARGET_LINK_LIBRARIES( testServer dim_shared )

  ADD_EXECUTABLE( testClient src/examples/test_client.cxx )
  TARGET_LINK_LIBRARIES( testClient dim_shared )

  ADD_EXECUTABLE( demo_server src/examples/demo_server.c )
  TARGET_LINK_LIBRARIES( demo_server dim_shared )

  ADD_EXECUTABLE( demo_client src/examples/demo_client.c )
  TARGET_LINK_LIBRARIES( demo_client dim_shared )

  INSTALL(
    TARGETS test_server test_client testClient testServer demo_client demo_server
    RUNTIME DESTINATION bin
  )
ENDIF()


IF( DIM_GUI )
  ###############################
  # Did executable
  FIND_PACKAGE( Motif REQUIRED )
  FIND_PACKAGE( X11 REQUIRED )

  FIND_LIBRARY( MRM_LIBRARY Mrm )

  IF( NOT MRM_LIBRARY )
    MESSAGE( SEND_ERROR "libMrm.so required !" )
  ELSE()
    MESSAGE( STATUS "Found Mrm : ${MRM_LIBRARY}" )
  ENDIF()

  FIND_LIBRARY( XT_LIBRARY Xt )

  IF( NOT XT_LIBRARY )
    MESSAGE( SEND_ERROR "libXt.so required !" )
  ELSE()
    MESSAGE( STATUS "Found Xt : ${XT_LIBRARY}" )
  ENDIF()

  INCLUDE_DIRECTORIES( ${MOTIF_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( src/did )

  ADD_EXECUTABLE( did src/did/dui_util.c src/did/did.c )
  TARGET_LINK_LIBRARIES( did dim_shared ${MOTIF_LIBRARIES} ${X11_LIBRARIES} ${MRM_LIBRARY} ${XT_LIBRARY} )
  INSTALL(
    TARGETS did
    RUNTIME DESTINATION bin
  )

  ###############################
  # WeDid executable
  ADD_EXECUTABLE( webDid
    src/webDid/webServer.c
    src/webDid/webTcpip.c
    src/webDid/webDid.c
  )
  TARGET_LINK_LIBRARIES( webDid dim_shared )

  # install webDid js and html sources with the binary in a dedicated dir
  INSTALL( DIRECTORY ./WebDID DESTINATION ./bin )
  INSTALL(
    TARGETS webDid
    RUNTIME DESTINATION bin/WebDID
  )

ENDIF()

#
