
dqm4hep_package( Mongoose
  MAJOR 1 MINOR 0 PATCH 0
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/
)

set( 
  MONGOOSE_SOURCES 
  src/mongoose.c 
  src/Utils.cpp
  src/Controller.cpp
  src/Mutex.cpp
  src/Request.cpp
  src/Response.cpp
  src/Server.cpp
  src/Session.cpp
  src/Sessions.cpp
  src/StreamResponse.cpp
  src/UploadFile.cpp
  src/WebController.cpp
  src/WebSocket.cpp
  src/WebSockets.cpp
)

# Adding dl
if (NOT WIN32)
    set (EXTRA_LIBS ${EXTRA_LIBS} dl)
else()    
  set (EXTRA_LIBS ${EXTRA_LIBS} ws2_32)
endif ()

# -------------------------------------------------
# build the DQMCore library
dqm4hep_add_package_library( Mongoose
  SOURCES ${MONGOOSE_SOURCES}
  LINK_LIBRARIES ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT}
  DEFINITIONS -DENABLE_STATS
)

