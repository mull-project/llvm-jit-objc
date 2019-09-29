CC=/opt/llvm-6.0.0/bin/clang
# CC=/opt/llvm-5.0.0/bin/clang
SYSROOT=-isysroot $(shell xcrun --sdk macosx --show-sdk-path)

MACOS_FRAMEWORKS_DIR=/Applications/Xcode-9.2.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks
# XCTEST_PATH=$(MACOS_FRAMEWORKS_DIR)/XCTest.framework/XCTest
XCTEST_FLAGS=-F$(MACOS_FRAMEWORKS_DIR) -framework XCTest

CCFLAGS=$(SYSROOT)

compile: compile.dylib compile.bc

compile.dylib:
	$(CC) \
	  $(CCFLAGS) \
	  $(XCTEST_FLAGS) \
	  -fobjc-arc -shared -undefined dynamic_lookup \
	  -o CustomXCTestRunner.dylib \
	  CustomXCTestRunner.m

compile.bc:
	$(CC) $(CCFLAGS) \
          -F$(MACOS_FRAMEWORKS_DIR) \
          -fobjc-arc \
          -emit-llvm \
	  CustomXCTestRunner.m \
	  -c \
	  -o CustomXCTestRunner.bc

