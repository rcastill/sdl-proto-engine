cmake_minimum_required(VERSION 3.7)
project(sdl-proto-engine)

find_package(SDL2 REQUIRED PATHS)
include_directories(${SDL2_INCLUDE_DIRS} include)

set(libname SDLProtoEngine)

add_library(${libname} src/sdlprotoengine.cc)
target_link_libraries(${libname} ${SDL2_LIBRARIES})

set_target_properties(${libname} PROPERTIES PUBLIC_HEADER "include/sdlprotoengine.h")

install(TARGETS ${libname}
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include)