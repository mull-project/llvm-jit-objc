add_library(CustomXCTestRunner SHARED CustomXCTestRunner.m)

set(SYSROOT_VALUE "")
execute_process(
  COMMAND xcrun --sdk macosx --show-sdk-path
  OUTPUT_VARIABLE SYSROOT_VALUE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(MACOS_FRAMEWORKS_DIR "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks")

set(XCTEST_FLAGS -F${MACOS_FRAMEWORKS_DIR} -framework XCTest)

#message(FATAL_ERROR "foo ${SYSROOT}")

target_compile_options(CustomXCTestRunner
  PRIVATE
  -fobjc-arc
  -isysroot ${SYSROOT_VALUE}
  ${XCTEST_FLAGS}
)

target_link_options(CustomXCTestRunner
  PRIVATE
  -shared -undefined dynamic_lookup
)
