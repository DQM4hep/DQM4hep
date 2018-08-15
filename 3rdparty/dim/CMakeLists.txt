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

# install all includes
dqm4hep_package( dim
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/
)

###############################
# dim compilation flags
set( DIM_CFLAGS "" )
set( DIM_LIBFLAGS "-lpthread" )
set( DIM_EXCFLAGS "" )
set( DIM_ARCH "Linux" ) # default arch
set( DIM_EXPORT_DEFINITIONS )

# FIXME Do we really need this ???
if( DIM_32BIT )
  set( DIM_CFLAGS "${DIM_CFLAGS} -m32" )
  set( DIM_LIBFLAGS "${DIM_LIBFLAGS} -m32" )
  set( DIM_EXCFLAGS "${DIM_EXCFLAGS} -m32" )
  set( DIM_EXPORT_DEFINITIONS ${DIM_EXPORT_DEFINITIONS} -m32 )
endif()

if( UNIX )
  if(NOT APPLE) # linux case
    set( DIM_CFLAGS "${DIM_CFLAGS} -Wall -Wextra -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux -fPIC" )
    set( DIM_EXCFLAGS "${DIM_EXCFLAGS} -Wall -Wextra" )
    set( DIM_EXPORT_DEFINITIONS ${DIM_EXPORT_DEFINITIONS} -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux )
  else() # MACOSX case
    set( DIM_ARCH "Darwin" )
    set( DIM_CFLAGS "${DIM_CFLAGS} -DMIPSEL -DPROTOCOL=1 -Dunix -Ddarwin -fPIC" )
    set( DIM_EXPORT_DEFINITIONS ${DIM_EXPORT_DEFINITIONS} -DMIPSEL -DPROTOCOL=1 -Dunix -Ddarwin )
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

# -------------------------------------------------
# build the DQMCore library
dqm4hep_add_package_library( dim
  SOURCES ${DIM_SRCS}
  LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
)



###############################
# dns executable
dqm4hep_add_executable( dns SOURCES src/dns.c )
# add_executable( dns src/dns.c )
# target_link_libraries( dns dim_shared )
# install( TARGETS dns RUNTIME DESTINATION bin )

###############################
# dim utilities
dqm4hep_add_executable( dim_get_service SOURCES src/util/dim_get_service.c )
dqm4hep_add_executable( dim_send_command SOURCES src/util/dim_send_command.c )
dqm4hep_add_executable( DimBridge SOURCES src/util/dimbridge.cxx )
dqm4hep_add_executable( checkDns SOURCES src/util/check_dns.cxx )
dqm4hep_add_executable( checkDimServers SOURCES src/util/check_dim_servers.cxx )

###############################
# dim examples
if( DIM_EXAMPLES )
  dqm4hep_add_executable( test_server SOURCES src/examples/test_server.c )
  dqm4hep_add_executable( test_client SOURCES src/examples/test_client.c )
  dqm4hep_add_executable( testServer SOURCES src/examples/test_server.cxx )
  dqm4hep_add_executable( testClient SOURCES src/examples/test_client.cxx )
  dqm4hep_add_executable( demo_server SOURCES src/examples/demo_server.c )
  dqm4hep_add_executable( demo_client SOURCES src/examples/demo_client.c )
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

  dqm4hep_add_executable( did 
    SOURCES src/did/dui_util.c src/did/did.c
    INCLUDE_DIRS ${MOTIF_INCLUDE_DIR} ${X11_INCLUDE_DIR} src/did
    LINK_LIBRARIES dim ${MOTIF_LIBRARIES} ${X11_LIBRARIES} ${MRM_LIBRARY} ${XT_LIBRARY}
  )

  ###############################
  # WebDid executable
  dqm4hep_add_executable( webDid 
    SOURCES src/webDid/webServer.c src/webDid/webTcpip.c src/webDid/webDid.c
    INCLUDE_DIRS ${MOTIF_INCLUDE_DIR} ${X11_INCLUDE_DIR} src/did
    LINK_LIBRARIES dim
  )

  # install webDid js and html sources with the binary in a dedicated dir
  install( DIRECTORY ./WebDID DESTINATION ./bin )
endif()

set( ${PROJECT_NAME}_DEFINITIONS ${${PROJECT_NAME}_DEFINITIONS} ${DIM_EXPORT_DEFINITIONS} CACHE STRING "" FORCE )

#
