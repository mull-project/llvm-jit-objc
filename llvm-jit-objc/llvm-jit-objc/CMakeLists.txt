cmake_minimum_required(VERSION 3.5.1)

include(LLVM.toolchain.macos.cmake)
include(LLVM.cmake)

# This enables assertions for Release builds.
# https://stackoverflow.com/questions/22140520/how-to-enable-assert-in-cmake-release-mode
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Setting THREADS_PREFER_PTHREAD_FLAG and finding package Threads resolves:
# /usr/bin/ld: ../../gtest-1.7.0/libgtest.a(gtest-all.cc.o): undefined reference to symbol     'pthread_key_delete@@GLIBC_2.2.5'
# /lib/x86_64-linux-gnu/libpthread.so.0: error adding symbols: DSO missing from command line
# https://stackoverflow.com/questions/25617839/undefined-reference-to-symbol-pthread-key-deleteglibc-2-2-5
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

if (NOT LLVM_ROOT)
  message(FATAL_ERROR "This CMakeLists.txt file expects cmake to be called with a
    CMake toolchain file which provides LLVM_ROOT:\n \
    -DCMAKE_TOOLCHAIN_FILE=path_to_your_toolchain")
endif()

## LLVM CXX flags
llvm_get_cxx_flags(LLVM_CXX_FLAGS)

## LLVM lib directory
llvm_get_lib_dir(LLVM_LIB_PATH)

## LLVM linker flags.
llvm_get_link_flags(LLVM_LINK_FLAGS)
# Without -undefined,dynamic_lookup flags there is a problem with order in which
# the libraries are linked and that causes missing symbols.
# https://github.com/eliben/llvm-clang-samples/pull/11
set(LLVM_LINK_FLAGS "${LLVM_LINK_FLAGS} -Wl,-undefined,dynamic_lookup")

# LLVM libraries
set(llvm_components
  "asmparser"
  "orcjit"
  "support"
  "x86")
llvm_get_libs(LLVMJITLAB_DEPENDENCY_LLVM_LIBRARIES "${llvm_components}")

# https://cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" "${LLVM_LIB_PATH}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(LLVMJITLAB_SOURCE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})
set(LLVMJITLAB_INCLUDE_DIR  ${LLVMJITLAB_SOURCE_DIR}/include)

include_directories(${LLVM_ROOT}/include)

## LLVM's dependencies.
find_library(LLVMJITLAB_DEPENDENCY_LIBZ z HINTS ${LLVM_TOOLCHAIN_LIBZ_PATH})
find_library(LLVMJITLAB_DEPENDENCY_NCURSES ncurses)
## Mull's own dependencies:
find_library(LLVMJITLAB_DEPENDENCY_SQLITE sqlite3 HINTS ${LLVMJITLAB_TOOLCHAIN_SQLITE_PATH})

include_directories(${LLVMJITLAB_INCLUDE_DIR})

## LLVM
set(LLVM_DEPENDENCIES
  ${LLVMJITLAB_DEPENDENCY_LLVM_LIBRARIES}

  ${LLVMJITLAB_DEPENDENCY_LIBZ}
  ${LLVMJITLAB_DEPENDENCY_NCURSES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LLVM_LINK_FLAGS}")
# # Link flags
# get_target_property(default_link_flags llvm-jit-objc LINK_FLAGS)
# if(NOT ${default_link_flags})
# set(default_link_flags "")
# endif()
# set(link_flags
#   "${default_link_flags} "
# )
# set_target_properties(llvm-jit-objc PROPERTIES LINK_FLAGS "${link_flags}")

add_subdirectory(src)
add_subdirectory(tests)
