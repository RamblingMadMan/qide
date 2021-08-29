cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(qide VERSION 0.0.1 LANGUAGES CXX)

################
# Dependencies #
################

add_subdirectory(deps/fmt)
add_subdirectory(deps/utfcpp)
add_subdirectory(deps/gmqcc)

###################
# Project subdirs #
###################

add_subdirectory(qide)
