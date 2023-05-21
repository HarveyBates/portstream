cmake_minimum_required(VERSION 3.24)
project(portstream C)

set(CMAKE_C_STANDARD 90)

# All files in source and include directories
file(GLOB_RECURSE SRC_DIR RELATIVE ${CMAKE_SOURCE_DIR} src/*.c)
file(GLOB_RECURSE INC_DIR RELATIVE ${CMAKE_SOURCE_DIR} include/*.h)

add_executable(${PROJECT_NAME} ${SRC_DIR} ${INC_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
target_compile_options(${PROJECT_NAME} PRIVATE -pedantic-errors)
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-misleading-indentation)
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-shift-negative-value)

install(TARGETS ${PROJECT_NAME})