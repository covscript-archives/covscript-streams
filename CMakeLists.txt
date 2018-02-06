cmake_minimum_required(VERSION 3.4)
project(covscript-streams)
include_directories(include)
include_directories(../covscript/include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")

add_library(streams SHARED streams.cpp)

set_target_properties(streams PROPERTIES PREFIX "")
set_target_properties(streams PROPERTIES SUFFIX ".cse")

