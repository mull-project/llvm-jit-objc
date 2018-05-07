set(sources
  ObjCResolver.cpp
  ObjCRuntime.mm
  ObjCRuntimeHelpers.mm
  ObjCEnabledMemoryManager.cpp
  ObjCType.mm
  DebugUtils.cpp
  Tests/ObjCTest.cpp
  Tests/SandboxTest.cpp
  Tests/XCTestObjCTest.cpp
  Tests/XCTestSwiftTest.cpp
  Tests/TestHelpers.cpp
)

# Globbing is not recommended by CMake so maybe consider listing headers
# explicitly in include/CMakeLists.txt file. However LLVM goes find(GLOB) under
# the hood so probably we are also good for now.
# "We do not recommend using GLOB to collect a list of source files from your
# source tree. If no CMakeLists.txt file changes when a source is added or
# removed then the generated build system cannot know when to ask
# CMake to regenerate. https://cmake.org/cmake/help/v3.0/command/file.html
file(GLOB_RECURSE headers *.h)

set(cs_link_flags "") # to be filled later
set(cs_rpath "")      # to be filled later

add_executable(llvm-jit-lab ${sources} ${headers})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(llvm-jit-lab google-test)
include_directories("${CMAKE_SOURCE_DIR}/googletest/include")

# The following is needed to make CMake to generate a folder structure in Xcode
# that corresponds to a physical folder structure.
get_target_property(HERE_TARGET_SOURCES llvm-jit-lab SOURCES)
foreach(source_file ${HERE_TARGET_SOURCES})
  if (source_file MATCHES "(.*)\.h$")
    file(RELATIVE_PATH relative_source_file ${CMAKE_CURRENT_SOURCE_DIR} ${source_file})
    get_filename_component(relative_path ${relative_source_file} DIRECTORY)
    source_group("${relative_path}" FILES ${source_file})
  else()
    get_filename_component(relative_path ${source_file} DIRECTORY)
    source_group("${relative_path}" FILES ${source_file})
  endif()
endforeach()

## LLVM
set(additional_libraries
  ${LLVMJITLAB_DEPENDENCY_LLVM_LIBRARIES}

  ${LLVMJITLAB_DEPENDENCY_LIBZ}
  ${LLVMJITLAB_DEPENDENCY_NCURSES}
)

target_link_libraries(llvm-jit-lab ${additional_libraries})

# compile flags
get_target_property(default_compile_flags llvm-jit-lab COMPILE_FLAGS)
if(NOT default_compile_flags)
  set(default_compile_flags "")
endif()
set(compileflags ${default_compile_flags} ${LLVM_CXX_FLAGS})
set_target_properties(llvm-jit-lab
  PROPERTIES COMPILE_FLAGS
  "${compileflags}"
)

# Link flags
get_target_property(default_link_flags llvm-jit-lab LINK_FLAGS)
if(NOT ${default_link_flags})
set(default_link_flags "")
endif()
set(link_flags
  "${default_link_flags} ${LLVM_LINK_FLAGS}"
)
set_target_properties(llvm-jit-lab PROPERTIES LINK_FLAGS "${link_flags}")
