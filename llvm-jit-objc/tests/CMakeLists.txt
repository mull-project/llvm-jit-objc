set(sources
  ObjCTest.cpp
  SandboxTest.cpp
  XCTestObjCTest.cpp
  XCTestSwiftTest.cpp
  TestHelpers.cpp
  TestHelpers.h
  ${CMAKE_CURRENT_BINARY_DIR}/CustomXCTestRunnerBinding.cpp
)

add_executable(llvm-jit-objc-tests ${sources} ${headers})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_dependencies(llvm-jit-objc-tests CustomXCTestRunner)
target_link_libraries(llvm-jit-objc-tests llvm-jit-objc google-test)
include_directories("${CMAKE_SOURCE_DIR}/googletest/include")

target_link_libraries(llvm-jit-objc-tests ${LLVM_DEPENDENCIES})

configure_file (
  ${CMAKE_CURRENT_LIST_DIR}/CustomXCTestRunnerBinding.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/CustomXCTestRunnerBinding.cpp
  @ONLY
)
