project(ex2)
cmake_minimum_required(VERSION 3.10)
set(LLVM_DIR "/home/yaojq/llvm-project-llvmorg-10.0.1/build/lib/cmake/llvm")

find_package(LLVM 10 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

set(Z3_DIR "/home/yaojq/Tools/z3-4.8.6-x64-ubuntu-16.04")
#set(Z3_DIR $ENV{Z3_DIR})
get_filename_component(Z3_DIR ${Z3_DIR} ABSOLUTE)
set(Z3_CXX_INCLUDE_DIRS ${Z3_DIR}/include)
set(Z3_LIBRARIES ${Z3_DIR}/bin/libz3.so)
message(STATUS "Z3_DIR: ${Z3_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
include_directories(${Z3_CXX_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${Z3_LIBRARIES})

add_executable(dse
        src/DSE.cpp
        src/Strategy.cpp
        include/Strategy.h
)

add_llvm_library(InstrumentPass MODULE
        src/DSEInstrument.cpp
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(dse ${llvm_libs} ${Z3_LIBRARIES})

add_library(runtime MODULE
        src/SymbolicInterpreter.cpp
        src/Runtime.cpp
        include/Strategy.h
)

target_link_libraries(runtime ${llvm_libs} ${Z3_LIBRARIES})
