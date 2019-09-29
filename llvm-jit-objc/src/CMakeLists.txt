
add_library(llvm-jit-objc SHARED dummy.cpp)

target_link_libraries(llvm-jit-objc ${LLVM_DEPENDENCIES})

add_subdirectory(include)
add_subdirectory(lib)

