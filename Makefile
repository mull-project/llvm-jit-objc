MAKEFLAGS += --warn-undefined-variables

BUILD_DIR_XCODE?=$(PWD)/BuildXcode

# Self-Documented Makefile
# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help
help: ## Show this help message.
	@grep -E '^[a-zA-Z_\.-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

## Xcode

xcode.build: ## Build Xcode project with CMake.
	mkdir -p $(BUILD_DIR_XCODE)

	# rm -rfv $(BUILD_DIR_XCODE)/CMakeCache.txt
	cd $(BUILD_DIR_XCODE) && cmake ../ -G Xcode

xcode.open: ## Open Xcode project
	open BuildXcode/llvm-jit-objc.xcodeproj

clean: ## Delete Xcode CMake build cache.
	rm -rfv $(BUILD_DIR_XCODE)
