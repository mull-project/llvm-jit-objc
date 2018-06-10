set(sources
  ObjCTest.cpp
  SandboxTest.cpp
  XCTestObjCTest.cpp
  XCTestSwiftTest.cpp
  TestHelpers.cpp
  TestHelpers.h
)

add_executable(llvm-jit-objc-tests ${sources} ${headers})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(llvm-jit-objc-tests llvm-jit-objc google-test)
include_directories("${CMAKE_SOURCE_DIR}/googletest/include")

target_link_libraries(llvm-jit-objc-tests ${LLVM_DEPENDENCIES})
