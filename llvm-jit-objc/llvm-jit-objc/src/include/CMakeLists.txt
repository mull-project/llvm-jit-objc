
target_include_directories(llvm-jit-objc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(headers
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/ObjCResolver.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/ObjCRuntime.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/ObjCRuntimeHelpers.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/ObjCEnabledMemoryManager.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/ObjCType.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/SwiftRuntimeSetup.h"
  "${CMAKE_CURRENT_LIST_DIR}/llvm-jit-objc/DebugUtils.h"
)
target_sources(llvm-jit-objc PRIVATE ${headers})
