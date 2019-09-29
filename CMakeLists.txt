cmake_minimum_required(VERSION 3.5.1)

project(llvm-jit-objc C CXX)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

# https://cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" "${LLVM_LIB_PATH}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(googletest)
add_subdirectory(llvm-jit-objc)
