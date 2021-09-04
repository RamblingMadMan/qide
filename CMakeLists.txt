cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(
	qide
	DESCRIPTION "The Quake Mod IDE"
	VERSION 0.0.1
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
