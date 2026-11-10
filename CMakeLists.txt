cmake_minimum_required(VERSION 3.10.0)
project(meBash VERSION 0.1.0 LANGUAGES C)

include_directories("include")

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(meBash ${SOURCES})