cmake_minimum_required(VERSION 3.1)
project(FunctionInlining)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(function-inlining)  # Use your pass name here.
