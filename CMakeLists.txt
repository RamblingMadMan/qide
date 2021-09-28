if(WIN32)
    cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)
else()
    cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
endif()

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

function(add_physfs)
	set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "")
	set(PHYSFS_BUILD_TEST OFF CACHE BOOL "")
	add_subdirectory(physfs-3.0.2 EXCLUDE_FROM_ALL)
endfunction()

function(add_glbinding)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
	set(OPTION_BUILD_EXAMPLES OFF CACHE BOOL "")
	add_subdirectory(deps/glbinding EXCLUDE_FROM_ALL)
endfunction()

add_physfs()
add_glbinding()

set(PHYSFS_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/physfs-3.0.2/src)

add_subdirectory(deps/fmt EXCLUDE_FROM_ALL)
add_subdirectory(deps/rapidfuzz-cpp EXCLUDE_FROM_ALL)
add_subdirectory(deps/gmqcc EXCLUDE_FROM_ALL)
add_subdirectory(deps/glm EXCLUDE_FROM_ALL)

###################
# Project subdirs #
###################

add_subdirectory(base)
add_subdirectory(qide)

install(
	TARGETS qide-base
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(FILES ${QIDE_HEADERS} DESTINATION include/qide)

install(
	TARGETS qide
	RUNTIME DESTINATION bin
)

if(WIN32 AND NOT MINGW)
	install(FILES $<TARGET_RUNTIME_DLLS:qide> DESTINATION bin)
endif()

# Install icon and .desktop file on linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	install(FILES res/QIDE.desktop DESTINATION share/applications)
	install(FILES qide/img/qide.png DESTINATION share/icons/hicolor/256x256/apps)
endif()
