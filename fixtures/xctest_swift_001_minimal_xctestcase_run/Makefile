# Self-Documented Makefile
# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help
help: ## Show this help message.
	@grep -E '^[0-9a-zA-Z_\.-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

SWIFTCC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/swift
# CC=/opt/llvm-5.0.0/bin/clang
SYSROOT=$(shell xcrun --sdk macosx --show-sdk-path)

MACOS_FRAMEWORKS_DIR=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks
# XCTEST_PATH=$(MACOS_FRAMEWORKS_DIR)/XCTest.framework/XCTest
# XCTEST_FLAGS=-framework XCTest

CCFLAGS=$(SYSROOT) -F$(MACOS_FRAMEWORKS_DIR)

OUTPUT_PATH=$(realpath $(PWD)/../bitcode)

FIXTURES_OUT_BC=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.bc)
FIXTURES_OUT_LL=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.ll)
FIXTURES_OUT_O=$(FIXTURES_IN:%=$(OUTPUT_PATH)/%.o)

compile: ## Compile MinimalSwiftXCTestCase.swift
	mkdir -p $(OUTPUT_PATH)
	$(SWIFTCC) \
		  -frontend \
		  -sdk $(SYSROOT) \
          -emit-bc \
          -enable-objc-interop \
          -Onone \
          -swift-version 4 \
          -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks \
          -F/opt/SwiftTestCase/Build/Products/Release \
          -target x86_64-apple-macosx10.13 \
          -o $(OUTPUT_PATH)/swift_001_minimal_xctestcase_run.bc \
	  MinimalSwiftXCTestCase.swift

	$(SWIFTCC) \
		  -frontend \
          -sdk $(SYSROOT) \
          -emit-ir \
          -enable-objc-interop \
          -Onone \
          -g \
          -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks \
          -F/opt/SwiftTestCase/Build/Products/Release \
          -target x86_64-apple-macosx10.13 \
          -o $(OUTPUT_PATH)/swift_001_minimal_xctestcase_run.ll \
	  MinimalSwiftXCTestCase.swift

	$(SWIFTCC) \
          -frontend \
          -sdk $(SYSROOT) \
          -emit-object \
          -sdk $(SYSROOT) \
          -g \
          -Onone \
          -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks \
          -F/opt/SwiftTestCase/Build/Products/Release \
          -target x86_64-apple-macosx10.13 \
          -o $(OUTPUT_PATH)/swift_001_minimal_xctestcase_run.Onone.o \
	  MinimalSwiftXCTestCase.swift

	$(SWIFTCC) \
          -frontend \
          -sdk $(SYSROOT) \
          -emit-object \
          -O \
          -g \
          -F/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks \
          -F/opt/SwiftTestCase/Build/Products/Release \
          -target x86_64-apple-macosx10.13 \
          -o $(OUTPUT_PATH)/swift_001_minimal_xctestcase_run.o \
	  MinimalSwiftXCTestCase.swift

clean: ## Cleans the build folder.
	rm -rfv $(OUTPUT_PATH)

