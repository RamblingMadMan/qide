cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(
	qide
	DESCRIPTION "The Quake Mod IDE"
	VERSION 0.0.0
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

################
# Dependencies #
################

add_subdirectory(deps/fmt)
add_subdirectory(deps/rapidfuzz-cpp)
add_subdirectory(deps/gmqcc)

###################
# Project subdirs #
###################

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
