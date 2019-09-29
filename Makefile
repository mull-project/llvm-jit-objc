MAKEFLAGS += --warn-undefined-variables

BUILD_DIR_XCODE?=$(PWD)/BuildXcode
BUILD_DIR_NINJA?=$(PWD)/BuildNinja

# Self-Documented Makefile
# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help
help: ## Show this help message.
	@grep -E '^[a-zA-Z_\.-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

## Xcode

.PHONY: build xcode.build ninja.build

xcode.build: ## Build Xcode project with CMake.
	mkdir -p $(BUILD_DIR_XCODE)

	# rm -rfv $(BUILD_DIR_XCODE)/CMakeCache.txt
	cd $(BUILD_DIR_XCODE) && cmake ../ -G Xcode

xcode.rebuild: xcode.build xcode.reopen ## Build Xcode project with CMake, kill Xcode, reopen the project in Xcode

ninja.build: ## Build Ninja project with CMake.
	mkdir -p $(BUILD_DIR_NINJA)

	rm -rfv $(BUILD_DIR_NINJA)/CMakeCache.txt
	cd $(BUILD_DIR_NINJA) && cmake -G Ninja \
    -DCMAKE_INSTALL_PREFIX=/usr/local/mull/mull-install \
    $(CMAKE_COMMAND_LINE_DEBUG_FLAGS) \
    -DCMAKE_TOOLCHAIN_FILE=$(MULL_CMAKE_TOOLCHAIN) \
    -DMULL_SUPPORT_RUST=0 \
    ../

## Xcode-specific tools.
## TODO: maybe extract to Makefile.Xcode?
xcode.open: ## Open Mull.xcodeproj in Xcode
	open BuildXcode/llvm-jit-objc.xcodeproj

# This reopen task is mostly needed to do a work that involves serious
# modifications of CMake's files: **/CMakeLists.txt and toolchain files.
# Xcode does not pickup all of the changes in CMake without being reopened.
xcode.reopen: ## Kill Xcode and open Mull.xcodeproj in Xcode.
	killall Xcode || true
	open BuildXcode/llvm-jit-objc.xcodeproj

clean: ninja.clean xcode.clean ## Delete CMake build caches: Xcode and Ninja.

xcode.clean: ## Delete Xcode CMake build cache.
	rm -rfv $(BUILD_DIR_XCODE)

ninja.clean: ## Delete Ninja CMake build cache.
	rm -rfv $(BUILD_DIR_NINJA)

