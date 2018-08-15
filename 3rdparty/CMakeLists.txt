
foreach( package asio dim json mongoose spdlog tclap websocketpp )
  add_subdirectory( ${package} )
endforeach()
