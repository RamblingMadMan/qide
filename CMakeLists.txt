cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(
	qide
	DESCRIPTION "The Quake Mod IDE"
	VERSION 0.0.0
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

################
# Project Defs #
################

set(QIDE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

set(
	QIDE_HEADERS
	${QIDE_INCLUDE_DIR}/qide/types.hpp
	${QIDE_INCLUDE_DIR}/qide/shapes.hpp
	${QIDE_INCLUDE_DIR}/qide/Camera.hpp
	${QIDE_INCLUDE_DIR}/qide/Renderer.hpp
	${QIDE_INCLUDE_DIR}/qide/RendererGL.hpp
)

################
# Dependencies #
################

add_subdirectory(deps/fmt)
add_subdirectory(deps/rapidfuzz-cpp)
add_subdirectory(deps/gmqcc)
add_subdirectory(deps/glm)

function(add_glbinding)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
	set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "")
	add_subdirectory(deps/glbinding)
endfunction()

add_glbinding()

###################
# Project subdirs #
###################

add_subdirectory(base)
add_subdirectory(qide)

install(
	TARGETS qide
	RUNTIME DESTINATION bin
)

# Install icon and .desktop file on linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	install(FILES res/QIDE.desktop DESTINATION share/applications)
	install(FILES qide/img/qide.png DESTINATION share/icons/hicolor/256x256/apps)
endif()
