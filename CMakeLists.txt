cmake_minimum_required(VERSION 3.16)
project(MushOS C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE mush_core MushCore/boot/* MushCore/drivers/* MushCore/kernel/* MushCore/mushlib/*)
file(GLOB_RECURSE mush_creator MushCreator/file_system/* MushCreator/utils/* MushCreator/adapter/* main.c)
file(GLOB_RECURSE mush_orbit MushDome/*)
file(GLOB_RECURSE mush_lib MushLib/*)
add_executable(MushOS ${mush_core} ${mush_creator} ${mush_orbit} ${mush_lib})
