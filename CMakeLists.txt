cmake_minimum_required(VERSION 2.8)
project(AtomAlign)

# cmake --build . --config RelWithDebInfo

set(CMAKE_CXX_FLAGS "--std=c++11 -g -O2 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-g -O2 -Wall ${CMAKE_C_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR}/lua-5.3.4/src)
include_directories(${PROJECT_SOURCE_DIR}/dlib-19.4)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

# compiling lua into the application for simplicity (removing standalone interpreter and compiler)
file(GLOB lua_src_file ${PROJECT_SOURCE_DIR}/lua-5.3.4/src/*.c)
list(REMOVE_ITEM lua_src_file "${PROJECT_SOURCE_DIR}/lua-5.3.4/src/lua.c")
list(REMOVE_ITEM lua_src_file "${PROJECT_SOURCE_DIR}/lua-5.3.4/src/luac.c")
add_definitions(-DLUA_USE_POSIX)
add_library(embedded_lua ${lua_src_file})

file(GLOB ALIGN_SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB ALIGN_HDR_FILES ${PROJECT_SOURCE_DIR}/src/*.h)

set(FAKE interactive_code.h)

add_custom_command(
  OUTPUT interactive_code.h
  DEPENDS ${PROJECT_BINARY_DIR}/embedfile ${PROJECT_SOURCE_DIR}/src/interactive.lua
  COMMAND ${PROJECT_BINARY_DIR}/embedfile ${PROJECT_SOURCE_DIR}/src/interactive.lua ${PROJECT_BINARY_DIR}/interactive_code.h interactive_code
  COMMENT "Embedding interactive.lua")

add_executable(atom_align
                    ${PROJECT_BINARY_DIR}/interactive_code.h
                    ${ALIGN_SRC_FILES}
                    ${ALIGN_HDR_FILES}
                    interactive_code.h)

add_dependencies(atom_align interactive_code.h embedfile)

add_executable(embedfile embedfile/main.cpp)

target_link_libraries(atom_align readline embedded_lua)
