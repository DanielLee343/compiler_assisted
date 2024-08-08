cmake_minimum_required(VERSION 3.10)
project(SharedModuleChecker)

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)
# Set LLVM directories
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
# list(APPEND TVM_LINKER_LIBS LLVM)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Define the LLVM pass
add_library(SharedModuleChecker SHARED SharedModuleChecker.cpp)

# Ensure LLVM libraries are linked correctly
llvm_map_components_to_libnames(llvm_libs support core irreader passes)
target_link_libraries(SharedModuleChecker PRIVATE ${llvm_libs})