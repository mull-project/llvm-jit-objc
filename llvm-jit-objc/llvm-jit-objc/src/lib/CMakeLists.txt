set(sources
  "${CMAKE_CURRENT_LIST_DIR}/ObjCResolver.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ObjCRuntime.mm"
  "${CMAKE_CURRENT_LIST_DIR}/ObjCRuntimeHelpers.mm"
  "${CMAKE_CURRENT_LIST_DIR}/ObjCEnabledMemoryManager.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/ObjCType.mm"
  "${CMAKE_CURRENT_LIST_DIR}/DebugUtils.cpp"
)

target_sources(llvm-jit-objc PUBLIC ${sources})

# # The following is needed to make CMake to generate a folder structure in Xcode
# # that corresponds to a physical folder structure.
# get_target_property(HERE_TARGET_SOURCES llvm-jit-objc SOURCES)
# foreach(source_file ${HERE_TARGET_SOURCES})
#   if (source_file MATCHES "(.*)\.h$")
#     file(RELATIVE_PATH relative_source_file ${CMAKE_CURRENT_SOURCE_DIR} ${source_file})
#     get_filename_component(relative_path ${relative_source_file} DIRECTORY)
#     source_group("${relative_path}" FILES ${source_file})
#   else()
#     get_filename_component(relative_path ${source_file} DIRECTORY)
#     source_group("${relative_path}" FILES ${source_file})
#   endif()
# endforeach()
