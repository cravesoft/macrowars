# macrowars

add_definitions( -D_CONSOLE -DOBOY_PLATFORM_LINUX -D_UNICODE -DUNICODE )
add_definitions( "/W3 /wd4995 /nologo" )

include_directories( ${OBoy_SOURCE_DIR}/libs )

include_directories( ${CEGUI_INCLUDE_DIR} )

link_directories(
  ${IRRKLANG_LIBRARY_DIR}
  ${CEGUI_LIBRARY_DIR}
  ${GLEW_LIBRARY_DIR}
)

if(WIN32)
  include_directories ( ${DXSDK_LIBRARY_DIR} )
endif()

set(macrowars_bodies_SRCS
  Asteroid.cpp
  Body.cpp
  Core.cpp
  Movable.cpp
  #OrbitStrategy.cpp
  Orbitable.cpp
  OriginAndDestination.cpp
  Planet.cpp
  Satellite.cpp
  Ship.cpp
  #TravelStrategy.cpp
  Traveller.cpp
  Waypoint.cpp
)
set(macrowars_bodies_HDRS
  Asteroid.h
  Body.h
  Core.h
  Movable.h
  #MoveStrategy.h
  #OrbitStrategy.h
  Orbitable.h
  OriginAndDestination.h
  Planet.h
  Satellite.h
  Ship.h
  #TravelStrategy.h
  Traveller.h
  Waypoint.h
)
set(macrowars_player_SRCS
  Player.cpp
)
set(macrowars_player_HDRS
  Player.h
)
set(macrowars_general_SRCS
  GuiSystem.cpp
  Macro.cpp
  main.cpp
)
set(macrowars_general_HDRS
  GuiSystem.h
  Macro.h
)

add_executable(Macrowars
  ${macrowars_general_SRCS}
  ${macrowars_general_HDRS}
  ${macrowars_player_SRCS}
  ${macrowars_player_HDRS}
  ${macrowars_bodies_SRCS}
  ${macrowars_bodies_HDRS}
)

target_link_libraries(Macrowars
  OBoy
  OBoyLib
  tinyxml
  irrKlang
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${CEGUI_LIBRARIES}
  ${CEGUI_OPENGL_LIBRARIES}
  ${GLEW_LIBRARY}
)

if(WIN32)
target_link_libraries(Macrowars
  d3dx9
  DxErr9
  DXUT
  comctl32
  ${CEGUI_DIRECT3D9_LIBRARIES}
)
endif()

if(MSVC)
	# These are used to create visual studio folders.
  source_group(bodies FILES ${macrowars_bodies_SRCS} ${macrowars_bodies_HDRS})
  source_group(player FILES ${macrowars_player_SRCS} ${macrowars_player_HDRS})
	source_group("" FILES ${macrowars_general_SRCS} ${macrowars_general_HDRS})
endif()